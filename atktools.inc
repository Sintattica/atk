<?php

 /**
  *
  * A collection of functions for general use in ATK
  *
  * @author Ivo Jansch <ivo@ibuildings.nl>

  * @version $Revision$
  *
  * $Id$
  *
  */

  /**
   * Function atkErrorHandler
   * This function catches PHP parse errors etc, and passes
   * them to atkerror(), so errors can be mailed and output
   * can be regulated.
   * This funtion must be registered with set_error_handler("atkErrorHandler");
   *
   * @param $errtype: One of the PHP errortypes (E_PARSE, E_USER_ERROR, etc)
                      (See http://www.php.net/manual/en/function.error-reporting.php)
   * @param $errstr: Error text
   * @param $errfile: The php file in which the error occured.
   * @param $errline: The line in the file on which the error occured.
   */
  function atkErrorHandler($errtype, $errstr, $errfile, $errline)
  {
    // Don't handle notices, and don't do anything if @ was u sed
    if ($errtype==E_NOTICE || error_reporting()==0)
    {
      return;
    }
    else if (($errtype & (E_WARNING|E_USER_WARNING)) > 0)
    {
      // This is something we should pay attention to, but we don't need to die.
      atkerror("[$errtype] $errstr in $errfile (line $errline)");
      return;
    }
    else
    {
      atkerror("[$errtype] $errstr in $errfile (line $errline)");

      // we must die. we can't even output anything anymore.. :(
      // we can mail though.
      mailreport();
      die;
    }
  }

  /**
  * Function halt
  * Halts on critical errors and also on warnings if specified in the config file.
  *
  */
  function halt($msg,$level="warning")
  {
    if ($level == $GLOBALS['config_halt_on_error']||$level == "critical")
    {
      if($level == "warning")
      {
        $level_color="#0000ff";
      }
      else
      {
        // critical
        $level_color="#ff0000";
      }

      echo "<html>";
      echo '<body bgcolor="#ffffff" color="#000000">';
      echo "<font color=\"$level_color\"><b>$level</b></font>: $msg<br>\n";
      die("<b>Halted.</b>");
    }
    else
    {
      atkerror("$msg");
    }
    return false;
  }

  function getmicrotime()
  {
    list($usec, $sec) = explode(" ",microtime());
    return ((float)$usec + (float)$sec);
  }

  function elapsed()
  {
    static $offset="";
    if ($offset=="")
    {
      $offset = getmicrotime();
      return "@".strftime("%H:%M:%S")."h";
    }
    else
    {
      return "+".sprintf("%02.05f",getmicrotime()-$offset)."s";
    }

  }



  /**
    * Function atkdebug
    *
    * Adds the debug txt to the debug log
    */
  function atkdebug($txt)
  {
     global $g_debug_msg;

     $g_debug_msg[]="[".elapsed()."] ".$txt;
  }

  /**
   * Like atkdebug, this displays a message at the bottom of the screen.
   * The difference is, that this is also displayed when debugging is turned
   * off.
   *
   * If errorreporting by email is turned on, the errormessages are also mailed
   */
  function atkerror($txt)
  {
    global $g_error_msg;
    $g_error_msg[]=$txt;
  }

  /**
   * Function atkconfig
   *
   * Return the value of a config variable
   */
  function atkconfig($tag)
  {
    return $GLOBALS["config_".$tag];
  }

   /**
   * Function Text
   *
   * Replaces the txt_[vars] with the values from the language files
   */
  function text($var,$node="")
  {
    if($node!="") $result = $GLOBALS["txt_".$node."_".$var];
    if($result=="") $result =  $GLOBALS["txt_".$var];
    if ($result=="") return ucfirst(str_replace("_"," ",str_replace('title_','',$var)));
    else return $result;
  }

  /**
   * Calculate a new session level based on current level and
   * a passed sessionstatus.
   */
  function session_level($sessionstatus=SESSION_DEFAULT, $levelskip=1)
  {
    $currentlevel = atkLevel();

    $newlevel = -1;

    switch ($sessionstatus)
    {
      case SESSION_NEW:
      {
        $newlevel = -1;
        break;
      }
      case SESSION_REPLACE:
      {
        $newlevel = -2;
        break;
      }
      case SESSION_NESTED:
      {
        $newlevel = $currentlevel+1;
        break;
      }
      case SESSION_BACK:
      {
        $newlevel = max(0,$currentlevel-$levelskip);
        break;
      }
      default:
      {
        $newlevel = $currentlevel;
      }
    }
    return $newlevel;
  }

  /**
   * Function session_form
   *
   * Adds session information to a form
   */
  function session_form($sessionstatus=SESSION_DEFAULT)
  {
    global $g_stickyurl;

    $res = "";

    $newlevel = session_level($sessionstatus);

    if ($newlevel!=0)
    {
      $res= '<input type="hidden" name="atklevel" value="'.$newlevel.'">';
    }
    $res.='<input type="hidden" name="atkprevlevel" value="'.atkLevel().'">';

    if ($sessionstatus!=SESSION_NEW)
    {
      $res.='<input type="hidden" name="atkstackid" value="'.atkStackID().'">';
    }

    for ($i=0;$i<count($g_stickyurl);$i++)
    {
      $value = $GLOBALS[$g_stickyurl[$i]];
      if ($value!="")
      {
        $res.="\n".'<input type="hidden" name="'.$g_stickyurl[$i].'" value="'.$value.'">';
      }
    }

    return $res;

  }

  /**
   * Function session_url
   *
   *  Makes a session-aware url thingee...
   */
  function session_url($url,$sessionstatus=SESSION_DEFAULT, $levelskip=1)
  {
    global $g_stickyurl;

    $newlevel = session_level($sessionstatus, $levelskip);

    if ($name=="") $name=$url;
    if (strpos($url,"?")>0)
    {
      $start = "&";
    }
    else
    {
      $start = "?";
    }

    $url.=$start;
    // atklevel is already set manually, we don't append it..
    if ($newlevel!=0 && !strpos($url,"atklevel=")>0)
    {
      $url.= "atklevel=".$newlevel."&";
    }
    $url.= "atkprevlevel=".atkLevel();
    if ($sessionstatus!=SESSION_NEW)
    {
      $url.="&atkstackid=".atkStackID();
    }
    $url.= "&".SID;

    for ($i=0;$i<count($g_stickyurl);$i++)
    {
      $value = $GLOBALS[$g_stickyurl[$i]];
      if ($value!="")
      {
        if (substr($url,-1)!="&") $url.="&";
        $url.=$g_stickyurl[$i]."=".$value;
      }
    }
    return $url;
  }

  /**
   * Function href
   *
   * Makes a session-aware href thingee...
   *
   * When using hrefs in the editform, you can set saveform to true. This will save your
   * form variables in the session and restore them whenever you come back.
   *
   */
  function href($url,$name="",$sessionstatus=SESSION_DEFAULT, $saveform=false, $extraprops="")
  {
    if ($saveform)
    {
      //$str = "javascript:atkSubmit('".session_url($url,$sessionstatus)."')";
    //echo $url."-".base64_encode(session_url($url,$sessionstatus))."<br>";
      //$str = 'javascript:atkSubmit("'.session_url($url,$sessionstatus).'")';
    //echo $url."-<br>";
      $str = 'javascript:atkSubmit("'.atkurlencode(session_url($url,$sessionstatus)).'")';
    }
    else
    {
      $str = session_url($url,$sessionstatus);
    }

    // note, we use single quotes instead of doublequotes in the href='',
    // because we otherwise have a problem with internet explorers buggy
    // javascript parser..
  //echo $str;
    return "<a href='".$str."' ".$extraprops.">".$name."</a>";
  }


  /**
   * Function atk_array_merge
   *
   * Same as array_merge from php, but without duplicates..
   */
  function atk_array_merge($array1, $array2)
  {
    $res = Array();
    for ($i=0;$i<count($array1);$i++)
    {
      if (!in_array($array1[$i],$res))
      {
        $res[]=$array1[$i];
      }
    }
    for ($i=0;$i<count($array2);$i++)
    {
      if (!in_array($array2[$i],$res))
      {
        $res[]=$array2[$i];
      }
    }
    return $res;
  }

  /**
   * Since php triggers an error if you perform an in_array on an
   * uninitialised array, we provide a small wrapper that performs
   * an is_array on the haystack first, just to make sure the user
   * doesn't get an error message.
   */
  function atk_in_array($needle, $haystack)
  {
    return (is_array($haystack)&&in_array($needle, $haystack));
  }

  /**
    * Function dataSetContains
    *
    * checks if a value is in a Array
    */
  function dataSetContains($set, $key, $value)
  {
    for ($i=0;$i<count($set);$i++)
    {
      if ($set[$i][$key]==$value) return true;
    }
    return false;
  }

  /**
   * Function stripQuotes
   *
   * strips ' or  " from the begin and end of a string (only if they are
   * on both sides, e.g. foo' remains foo' but 'bar' becomes bar.
   */
  function stripQuotes($string)
  {
    if ($string[0]=="'" && substr($string,-1)=="'") return substr($string,1,-1);
    if ($string[0]=='"' && substr($string,-1)=='"') return substr($string,1,-1);
    return $string;
  }

  /**
   * Function decodeKeyValuePair
   *
   * translates a string like id='3' into Array("id"=>3)
   */
  function decodeKeyValuePair($pair)
  {
    list($key,$value)=split("==|!=|<>|>=|<=|=|<|>",$pair);
    return array($key=>stripQuotes($value));
  }

  /**
    * Function decodeKeyValueSet
    *
    * translates a string like id='3 AND name='joe'' into Array("id"=>3,"name"=>"joe")
    */
  function decodeKeyValueSet($set)
  {
    $result = array();
    $items=split(" AND ",$set);
    for ($i=0;$i<count($items);$i++)
    {
      list($key,$value)=split("=",$items[$i]);

      $result[$key] = stripQuotes($value);
    }
    return $result;
  }


  /**
    * Function encodeKeyValueSet
    *
    * translates Array("id"=>3,"name"=>"joe") into a string like id='3 AND name='joe''
    */
  function encodeKeyValueSet($set)
  {
    reset($set);
    $items = Array();
    while (list($key, $value) = each($set))
    {
      $items[] = $key."=".$value;
    }
    return implode(" AND ",$items);
  }

  /**
   * same as strip_slashes from php, but if the passed value is an array,
   * all elements of the array are stripped.
   */
  function atk_stripslashes(&$var)
  {
    if (is_array($var))
    {
      foreach (array_keys($var) as $key)
      {
        atk_stripslashes($var[$key]);
      }
    }
    else
    {
      // ltrim is a workaround for a php4.0.4pl1 bug
      $var = ltrim(stripslashes($var));
    }
  }


  /**
    * function atkDataDecode
    *
    * performs stripslashes on all vars and translates:
    *                 something_AMDAE_other[] into something[][other]
    *                 something_AE_other into something[other]
    *                 (and a_AE_b_AE_c into a[b][c] and so on...
    */
  function atkDataDecode(&$vars)
  {
    foreach (array_keys($vars) as $varname)
    {
      $value = &$vars[$varname];
      // We must strip all slashes from the input, since php puts slashes
      // in front of quotes that are passed by the url. (magic_quotes_gpc)
      atk_stripslashes($value);

      AE_decode($vars, $varname);

      if (strpos(strtoupper($varname),'_AMDAE_')>0) // Now I *know* that strpos could return 0 if _AMDAE_ *is* found
                                    // at the beginning of the string.. but since that's not a valid
                                    // encoded var, we do nothing with it.
      {
        // This string is encoded.
        list($dimension1,$dimension2) = split("_AMDAE_",strtoupper($varname));
        if (is_array($value))
        {
          // Multidimensional thing
          for ($i=0;$i<count($value);$i++)
          {
            $vars[strtolower($dimension1)][$i][strtolower($dimension2)] = $value[$i];
          }
        }
        else
        {
          $vars[strtolower($dimension1)][strtolower($dimension2)] = $value;
        }
      }

/*    elseif (strpos(strtoupper($varname),'_BMDAE_')>0) // Now I *know* that strpos could return 0 if _AMDAE_ *is* found
                                    // at the beginning of the string.. but since that's not a valid
                                    // encoded var, we do nothing with it.
      {
        // This string is encoded.
        list($dimension1,$dimension2) = split("_BMDAE_",strtoupper($varname));
        // Multidimensional thing
        while (list($key,$val) = each($value))
        {
          $vars[strtolower($dimension1)][strtolower($dimension2)][$key] = $val;
        }
      }*/
    }
  }

  /**
   * Weird function. $dest is an associative array, that may contain stuff
   * like $dest["a_AE_c_AE_b"] = 3.
   * Now if you run this function like this:
   * AE_decode($dest, "a_AE_c_AE_b");
   * then $dest will contain a decoded array:
   * echo $dest["a"]["b"]["c"]; <- this will display 3
   */
  function AE_decode(&$dest, $var)
  {
    $items = explode("_AE_", $var);
    if (count($items) <= 1) return;
    $current = &$dest;
    foreach ($items as $key) $current = &$current[$key];
    $current = $dest[$var];
    unset($dest[$var]);
  }

  /**
   * Function stringfields
   *
   * Get the [ ] Fields out of a String
   */
  function stringfields($string)
  {
    $tmp = "";
    $adding = false;
    for ($i=0;$i<strlen($string);$i++)
    {
      if ($string[$i]=="]")
      {
        $adding = false;
        $fields[] = $tmp;
        $tmp="";
      }
      else if ($string[$i]=="[")
      {
        $adding = true;
      }
      else
      {
        if ($adding) $tmp.=$string[$i];
      }
    }

    return $fields;
  }

  /**
    * Function stringparse
    *
    * Parse strings
    */
  function stringparse($string, $data,$encode=false)
  {
    $fields = stringfields($string);
    for ($i=0;$i<count($fields);$i++)
    {
      $elements = split("\.",$fields[$i]);
      $databin = $data;
      for($j=0;$j<count($elements);$j++)
      {
        $value = $databin[$elements[$j]];
        $databin = $databin[$elements[$j]];
      }
      if ($encode)
      {
        $string = str_replace("[".$fields[$i]."]",rawurlencode($value),$string);
      }
      else
      {
        $string = str_replace("[".$fields[$i]."]",$value,$string);
      }
    }
    return $string;
  }

  /**
   * Safe urlencode function. Note, you can reencode already encoded strings, but
   * not more than 4 times!
   * If you encode a string more than 4 times, you won't be able to decode it
   * anymore
   *
   * An atkurlencoded string is normaly prefixed with '__', so atkurldecode can
   * determine whether the string was encoded or not. Sometimes however, if you
   * need to reencode part of a string (used in recordlist), you don't want the
   * prefix. Pass false as second parameter, and you won't get a prefix. (Note
   * that you can't atkurldecode that string anymore, so only use this on
   * substrings of already encoded strings)
   */
  function atkurlencode($string, $pref=true)
  {
    $string = rawurlencode($string);
    for ($i=3;$i>=1;$i--)
    {
      $string = str_replace("_".$i,"_".($i+1),$string);
    }
    return ($pref?"__":"").str_replace("%","_1",$string);
  }

  function atkurldecode($string)
  {
    if (substr($string,0,2)!="__") return $string;
    else
    {
      $string = str_replace("_1","%",substr($string,2));
      for ($i=1;$i<=3;$i++)
      {
        $string = str_replace("_".($i+1),"_".$i,$string);
      }
      return rawurldecode($string);
    }
  }

  /**
   * Sent a detailed error report to the maintainer.
   */
  function mailreport()
  {
    global $config_mailreport, $g_error_msg, $g_debug_msg, $HTTP_SERVER_VARS;
    global $HTTP_GET_VARS, $txt_app_title, $g_sessionManager, $g_sessionData;

    if ($config_mailreport!="") // only if enabled..
    {
      $subject = "[".$HTTP_SERVER_VARS["SERVER_NAME"]."] $txt_app_title error";
      $from = $txt_app_title;

      $body = "Hello,\n\nAn error seems to have occurred in the atk application named '$txt_app_title'.\n";
      $body.= "\nThe errormessage was:\n\n".implode("\n",$g_error_msg)."\n";
      $body.= "\nA detailed report follows:\n";
      $body.= "\nPHP Version: ".phpversion()."\n\n";

      $body.= "\nDEBUGMESSAGES\n".str_repeat("-",70)."\n";
      $body.= implode("\n",$g_debug_msg);

      $body.= "\n\nHTTP_GET_VARS\n".str_repeat("-",70)."\n";
      foreach ($HTTP_GET_VARS as $key=>$value)
      {
        $body.=$key.str_repeat(" ",max(1,20-strlen($key)))." = ".$value."\n";
      }

      $body.= "\n\nATK CONFIGURATION\n".str_repeat("-",70)."\n";
      foreach ($GLOBALS as $key=>$value)
      {
        if (substr($key,0,7)=="config_")
        {
          $body.=$key.str_repeat(" ",max(1,30-strlen($key)))." = ".$value."\n";
        }
      }

      if (is_object($g_sessionManager))
      {
        $body.= "\n\nATK SESSION\n".str_repeat("-",70);
        $body.= "\nNamespace: ".$g_sessionManager->m_namespace."\n";
        $stack = $g_sessionData[$g_sessionManager->m_namespace]["stack"];
        for($i=0;$i<count($stack);$i++)
        {
          $body.="\nStack level $i:\n";
          $item = $stack[$i];
          if (is_array($stack[$i]))
	  {
	    foreach ($item as $key=>$value)
            {
              $body.=$key.str_repeat(" ",max(1,30-strlen($key)))." = ".$value."\n";
            }
	  }
        }
        $ns_globals = $g_sessionData[$g_sessionManager->m_namespace]["globals"];
        if(count($ns_globals)>0)
        {
          $body.="\nNamespace globals:\n";
          foreach ($ns_globals as $key=>$value)
          {
            $body.=$key.str_repeat(" ",max(1,30-strlen($key)))." = ".$value."\n";
          }
        }
        $globals = $g_sessionData[$g_sessionManager->m_namespace]["globals"];
        if (count($globals)>0)
        {
          $body.="\nGlobals:\n";
          foreach ($globals as $key=>$value)
          {
            $body.=$key.str_repeat(" ",max(1,30-strlen($key)))." = ".$value."\n";
          }
        }
      }

      $body.= "\n\nSERVER INFORMATION\n".str_repeat("-",70)."\n";

      foreach ($HTTP_SERVER_VARS as $key=>$value)
      {
        $body.=$key.str_repeat(" ",max(1,20-strlen($key)))." = $value\n";
      }

      mail($config_mailreport,$subject,$body,"From: $from <atkinfo@ibuildings.nl>");
    }
  }


  /**
   * escapes quotes for use in SQL: ' -> '' (and sometimes % -> %%)
   */
  function escapeSQL($string, $wildcard=false)
  {
    $result = str_replace("'","''",$string);
    $result = str_replace("\\","\\\\",$result);
    if ($wildcard == true) $result = str_replace("%","%%",$result);
    return $result;
  }

  /**
   * Return the atk version number.
   */
  function atkversion()
  {
    global $g_atkversion;
    return $g_atkversion;
  }

  /**
   * Returns a url to open a popup window
   */
  function atkPopup($target,$params,$winName,$width,$height,$scroll='no',$resize='no')
  {
    $url ="javascript:NewWindow('popup.php?target=".$target."&".$params."','".$winName."',".$height.",".$width.",'".$scroll."','".$resize."')";
    return $url;
  }

  /**
   * Adds new element to error array en $record. When
   * $msg is empty the multilange error string is used.
   * params $rec var in which to add element to error array
   *        $attrib attributename
   *        $err multilanguage error string
   *        $msg optinal error string
   */

  function triggerError(&$rec, $attrib, $err, $msg="")
  {
    if($msg=="")  $msg = text($err);
    $rec['atkerror'][] = array( attrib_name=> $attrib, err => $err, msg => $msg);
  }

  /**
   * Does a var dump of an array. Makes use of atkdebug for displaying the values
   *        $a array
   */

  function atk_var_dump($a)
  {
    atkdebug(create_var_dump($a));
  }

  function create_var_dump($a)
  {
    if(!is_array($a) && !is_object($a))
    {
      return $a;
    }
    $dump = "<ul>";
    while(list($key, $val) = @each($a))
    {
      if(is_array($val) || is_object($val))
      {
        $dump .= "<LI>$key => array(".count($val).")";
        $dump .= create_var_dump($val);
      }
      else $dump .= "<LI>$key = '$val'";
    }
    $dump .= "</ul>";
    return $dump;
  }

  /**
   * This function writes data to the browser for download.
   * $data is the data to download.
   * $filename is the name the file will get when the user downloads it.
   * $compression can be "zip", "gzip" or "bzip", which causes the data
   *              to be compressed before transmission.
   */
  function exportData($data, $filename, $compression="")
  {
    $browser = getBrowserInfo();
    if (eregi("ie", $browser["browser"]))
    {
      $mime = "application/octetstream";
      $disp = 'inline';
    }
    else if (eregi("opera",$browser["browser"]))
    {
      $mime = "application/octetstream";
      $disp = 'attachment';
    }
    else
    {
      $mime = "application/octet-stream";
      $disp = 'attachment';
    }

    if($compression=="bzip")
		{
		  $mime='application/x-bzip';
		 $filename.= ".bz2";
		}
		else if($compression=="gzip")
		{
		  $mime_type='application/x-gzip';
		  $filename.= ".gz";
		}
    else if($compression=="zip")
		{
		  $mime_type='application/x-zip';
		  $filename.= ".zip";
		}

    header('Content-Type: '. $mime);
    header('Content-Disposition:  '.$disp.'; filename="'.$filename.'"');
    if(eregi("ie", $browser["browser"])) header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
    header('Pragma: no-cache');
    header('Expires: 0');

		// 1. as a bzipped file
	  if($compression=="bzip")
		{
		  if (@function_exists('bzcompress'))
			{
        echo bzcompress($data);
      }
		}
		// 2. as a gzipped file
		else if ($compression == 'gzip')
		{
      if (@function_exists('gzencode'))
			{
        echo gzencode($data);
      }
    }
    else if ($compression == 'zip')
		{
      if (@function_exists('gzcompress'))
			{
        echo gzcompress($data);
      }
    }
    // 3. on screen
    else
		{
      echo $data;
    }

    exit;
  }

  /**
   * This function writes a binary file to the browser for download.
   * $file is the local filename (the file you want to open on the serverside)
   * $filename is the name the file will get when the user downloads it.
   * $mimetype is the mimetype of the file
   */
  function exportFile($file, $filename,$mimetype="")
  {
    include_once(atkconfig("atkroot")."atk/atkbrowsertools.inc");
    $browser = getBrowserInfo();
    if (eregi("ie", $browser["browser"]))
    {
      $mime = "application/octetstream";
      $disp = 'inline';
    }
    else if (eregi("opera",$browser["browser"]))
    {
      $mime = "application/octetstream";
      $disp = 'inline';
    }
    else
    {
      $mime = "application/octet-stream";
      $disp = 'attachment';
    }
    if($mimetype!="") $mime=$mimetype;

    $fp = @fopen($file,"rb");
    if ($fp!=NULL)
    {
      header('Content-Type: '. $mime);
      header('Content-Disposition:  '.$disp.'; filename="'.$filename.'"');
      if(eregi("ie", $browser["browser"])) header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
      header('Pragma: no-cache');
      header('Expires: 0');
		  fpassthru($fp);
      return true;
    }
    return false;
  }

  function useattrib($attribute)
  {
    global $config_atkroot;
    $a = explode(".", $attribute);
    if (count($a) == 2)
      $include = moduleDir(strtolower($a[0]))."/attributes/class.".strtolower($a[1]).".inc";
    else $include = $config_atkroot."atk/attributes/class.".strtolower($attribute).".inc";
    include_once($include);
  }

  function userelation($relation)
  {
    global $config_atkroot;
    $a = explode(".", $relation);
    if (count($a) == 2)
      $include = $config_atkroot."modules/".strtolower($a[0])."/relations/class.".strtolower($a[1]).".inc";
    else $include = $config_atkroot."atk/relations/class.".strtolower($relation).".inc";
    include_once($include);
  }
  
  /**
   * Returns the (virtual) hostname of the server.
   */
  function atkHost()
  {
    // todo: make this php 4.2 compatible (check version and use $_SERVER if possible.
    global $HTTP_SERVER_VARS;
    return ($HTTP_SERVER_VARS["HTTP_HOST"]!=""?$HTTP_SERVER_VARS["HTTP_HOST"]:$HTTP_SERVER_VARS["SERVER_NAME"]);
  }

  /**
   * Returns the next unique ID for the given sequence.
   * NOTE: ID's are only unique for the script execution!
   * @param $sequence the sequence name
   * @return next unique ID for the given sequence
   */
  function getUniqueID($sequence)
  {
    static $unique = array();
    return ++$unique[$sequence];
  }
?>
