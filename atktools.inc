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
      global $g_debug_msg;
      print implode("<br>", $g_debug_msg);
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
    * @param bool viewhtml: make viewable in html
    */
  function atkdebug($txt, $viewhtml=false)
  {
     global $g_debug_msg;
     if($viewhtml) 
       $txt = htmlentities($txt);
     	
     if (atkconfig("debug")>=0) $g_debug_msg[]="[".elapsed()."] ".$txt;
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
    $g_error_msg[]="[".elapsed()."] ".$txt;   

    if(function_exists("debug_backtrace"))
    {	
      atkdebug("atk_get_trace:".atk_get_trace());
    }
  }

  /**
  * @param string format (html|plaintext)
  *
  * @return void
  */
  function atk_get_trace($format="html")
  {	
    if(!function_exists("debug_backtrace"))
      return "Incorrect php-version for atk_get_trace()";

    $traceArr = debug_backtrace();

    // remove call of atk_get_trace
    array_shift($traceArr);

    if($format=="html")
    {
      $ret = "<br><font face=\"Verdana\" size=-2>";
      $spacer = "&nbsp;";
      $break = "<br/>\n";
      $classCall = "-&gt;";
    }
    else
    {
      $spacer = " ";
      $break = "\n";
      $classCall = "->";

    }
    for($i=0; $i < count($traceArr); $i++)
    {
      $theSpacer = str_repeat($spacer, ($i*2)+3)."|";

      $file = $traceArr[$i]["file"];
      $line = $traceArr[$i]["line"];

      
      $function = $traceArr[$i]["function"];
      $class = $traceArr[$i]["class"];
      if($class)
        $class.=$classCall;

      $tmpArr = $traceArr[$i]["args"];

      $functionParamArr = array();

      foreach($tmpArr as $val)
      {
        if(is_array($val) && count($val))
          $val = "list(".implode(", ", $val).")";

        if(strlen($val))
        {	
          if(strlen($val) > 100)
            $val = substr($val, 0, 97)."...";
            
          $functionParamArr[] = $theSpacer."&nbsp;&nbsp;'".$val."'";
        }
      }

      $functionParams = implode(",".$break."\n", $functionParamArr);

      if(strlen($functionParams))
        $functionParams= $break."\n".$functionParams.$break.$theSpacer;
      
      $ret .= str_repeat("&nbsp;", ($i*2))." |-|".$file." on line ".$line.$break.
                                $theSpacer.$class.$function." with params:(".
                                $functionParams.")".$break;
    }  // end foreach

    if($format=="html")
      $ret .="</font>";

    return $ret;
  }

   /**
   * Function Text
   *
   * Replaces the txt_[vars] with the values from the language files
   */
  function text($var, $node="",$module="")
  {
    if ($module=="")
    {
      // Module is empty, we can only look for a node specific text.      
      if ($node=="")
      {
        // Node is empty too, we can only use the generic string.        
        $key = "txt_".$var;
      }
      else
      {
        // Node is not empty. We try to find a node specific string first. If that doesn't exist,
        // we return the generic string.
        $key = "txt_".$node."_".$var;        
        if (empty($GLOBALS[$key]))
        {
          // No node specific string. We should use the generic string.          
          $key = "txt_".$var;          
        }        
      }
    }
    else
    {
      // Module is set. We try to find a module specific string first.
      // If that doesn't exist, we fall back.
      // NOTE: we don't support passing a module but not a node. You pass
      // both or none at all.
      $key = "txt_".$module."_".$node."_".$var;
      if (empty($GLOBALS[$key]))
      {
        // No module specific string found. We return the non-module
        // specific fallback string.
        return text($var, $node);
      }      
    }            
    return (!empty($GLOBALS[$key])?$GLOBALS[$key]:ucfirst(str_replace("_"," ",str_replace('title_','',$var))));
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
    
    $res .= '<input type="hidden" name="'.session_name().'" value="'.session_id().'">';
    $res .= '<input type="hidden" name="atkescape" value="">';

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

  function session_vars($sessionstatus=SESSION_DEFAULT, $levelskip=1, $url="")
  {
    global $g_stickyurl;
    
    $newlevel = session_level($sessionstatus, $levelskip);
    $vars="";
   // atklevel is already set manually, we don't append it..
    if ($newlevel!=0 && !strpos($url,"atklevel=")>0)
    {
      $vars.= "atklevel=".$newlevel."&";
    }
    $vars.= "atkprevlevel=".atkLevel();
    if ($sessionstatus!=SESSION_NEW)
    {
      $vars.="&atkstackid=".atkStackID();
    }
    $vars.= "&".SID;

    for ($i=0;$i<count($g_stickyurl);$i++)
    {
      $value = $GLOBALS[$g_stickyurl[$i]];
      if ($value!="")
      {
        if (substr($vars,-1)!="&") $vars.="&";
        $vars.=$g_stickyurl[$i]."=".$value;
      }
    }
     return $vars;
  }

  /**
   * Function session_url
   *
   *  Makes a session-aware url thingee...
   */
  function session_url($url,$sessionstatus=SESSION_DEFAULT, $levelskip=1)
  {
    global $g_stickyurl;
    if ($name=="") $name=$url;
    if (strpos($url,"?")!==false)
    {
      $start = "&";
    }
    else
    {
      $start = "?";
    }

    $url.=$start;

    $url.=session_vars($sessionstatus, $levelskip, $url);
   
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
   * wrap lines
   */
  function _wordwrap($line)
  {
    return wordwrap($line,100,"\n",1);
  }

  /**
   * Sent a detailed error report to the maintainer.
   */
  function mailreport()
  {
    global $config_mailreport, $g_error_msg, $g_debug_msg;
    global $txt_app_title, $g_sessionManager, $g_sessionData, $g_user;

    if ($config_mailreport!="") // only if enabled..
    {
      $subject = "[".$_SERVER["SERVER_NAME"]."] $txt_app_title error";
      $from = $txt_app_title;

      $body = "Hello,\n\nAn error seems to have occurred in the atk application named '$txt_app_title'.\n";
      $body.= "\nThe errormessage was:\n\n".implode("\n",$g_error_msg)."\n";
      $body.= "\nA detailed report follows:\n";
      $body.= "\nPHP Version: ".phpversion()."\n\n";

      $body.= "\nDEBUGMESSAGES\n".str_repeat("-",70)."\n";

      for($i=0, $_=count($g_debug_msg);$i<$_;$i++)
      {
        $lines[] = _wordwrap($g_debug_msg[$i]);
      }
      $body.= implode("\n",$lines);

      if (count($_GET)>0)
      {
        $body.= "\n\n_GET\n".str_repeat("-",70)."\n";
        foreach ($_GET as $key=>$value)
        {
          $body.=_wordwrap($key.str_repeat(" ",max(1,20-strlen($key)))." = ".$value)."\n";
        }
      }

      if (count($_GET)>0)
      {
        $body.= "\n\n_GET\n".str_repeat("-",70)."\n";
        foreach ($_GET as $key=>$value)
        {
          $body.=_wordwrap($key.str_repeat(" ",max(1,20-strlen($key)))." = ".$value)."\n";
        }
      }
      
      if (function_exists("getallheaders"))
      {
        $request = getallheaders();
        if (count($request)>0)
        {
          $body.= "\n\nREQUEST INFORMATION\n".str_repeat("-",70)."\n";
          foreach($request as $key=>$value)
          {
            $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".$value)."\n";
          }
        }
      }

      if (count($_POST)>0)
      {
        $body.= "\n\n_POST\n".str_repeat("-",70)."\n";
        foreach ($_POST as $key=>$value)
        { 
          $body.=_wordwrap($key.str_repeat(" ",max(1,20-strlen($key)))." = ".$value)."\n";
        }
      }
      $body.= "\n\nATK CONFIGURATION\n".str_repeat("-",70)."\n";
      foreach ($GLOBALS as $key=>$value)
      {
        if (substr($key,0,7)=="config_")
        {
          $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".$value)."\n";
        }
      }
      
      $body.= "\n\nCurrent User:\n".str_repeat("-",70)."\n";
      if (is_array($g_user) && count($g_user))
      {      
        foreach($g_user as $key => $value)
        {
          $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".$value)."\n";
        }
      }
      else $body.= "Not known\n";

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
              $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".$value)."\n";
            }
          }
        }
        $ns_globals = $g_sessionData[$g_sessionManager->m_namespace]["globals"];
        if(count($ns_globals)>0)
        {
          $body.="\nNamespace globals:\n";
          foreach ($ns_globals as $key=>$value)
          {
            $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".$value)."\n";
          }
        }
        $globals = $g_sessionData["globals"];
        if (count($globals)>0)
        {
          $body.="\nGlobals:\n";
          foreach ($globals as $key=>$value)
          {
            $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".$value)."\n";
          }
        }
      }

      $body.= "\n\nSERVER INFORMATION\n".str_repeat("-",70)."\n";

      foreach ($_SERVER as $key=>$value)
      {
        $body.=_wordwrap($key.str_repeat(" ",max(1,20-strlen($key)))." = ".$value)."\n";
      }

      mail($config_mailreport,$subject,$body,"From: $from <atkinfo@ibuildings.nl>");
    }
  }


  /**
   * wrapper for escapeSQL function
   */
  function escapeSQL($string, $wildcard=false)
  {
    global $g_db;    
    return $g_db->escapeSQL($string, $wildcard);    
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
    $url ="javascript:NewWindow('include.php?file=".$target."&".$params."','".$winName."',".$height.",".$width.",'".$scroll."','".$resize."')";
    return $url;
  }

  /**
   * Adds new element to error array en $record. When
   * $msg is empty the multilange error string is used.
   * params $rec var in which to add element to error array
   *        $attrib attributename || an array with attribute names 
   *        $err multilanguage error string
   *        $msg optinal error string
   */

  function triggerError(&$rec, $attrib, $err, $msg="")
  {
    if($msg=="")  $msg = text($err);

    $rec['atkerror'][] = array( attrib_name=> $attrib, err => $err, msg => $msg);
  }


  /**
  * Adds var_export function to PHP versions older then PHP4.2
  * for documentation about var_export() see
  * http://www.php.net/manual/en/function.var-export.php
  */
  if(!function_exists("var_export"))
  {
    function var_export($a)
    {
      $result = "";
      switch (gettype($a))
      {
         case "array":
           reset($a);
           $result = "array(";
           while (list($k, $v) = each($a))
           {
             $result .= "$k => ".var_export($v).", ";
           }
           $result .= ")";
           break;
         case "string":
           $result = "'$a'";
           break;
         case "boolean":
           $result = ($a) ? "true" : "false";
           break;
         default:
           $result = $a;
           break;
      }
      return $result;
    }
  }

  /**
   * Does a var dump of an array. Makes use of atkdebug for displaying the values
   *        
   * Note, this method uses var_export to output the data. If using a php
   * version older than 4.2, atk_var_dump will output nothing.
   *
   * @param $a data to be displayed
   * @param $d name of the data that's being displayed.
   */
  function atk_var_dump($a, $d="")
  {
    if (atkconfig("debug")>0) // atkdebug checks this, but if we check it here too, it saves us
                              // a call to var_export.
    {
      if (function_exists("var_export"))
      {
        atkdebug("vardump: ".($d!=""?$d." = ":"")."<pre>".var_export($a, true)."</pre>");
      }
    }
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
    return ($_SERVER["HTTP_HOST"]!=""?$_SERVER["HTTP_HOST"]:$_SERVER["SERVER_NAME"]);
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

  /**
   * Checks if the variable $var contains the given flag ($flag).
   * @param $var the variable which might contain flags
   * @param $flag the flag you want to check for
   * @return result of check
   */
  function hasFlag($var, $flag)
  {
    return ($var & $flag) == $flag;
  }
  
  /**
   * Makes an url from the target var and all postvars
   * @param $target the path of the file to open
   */  
  function makeUrlFromPostvars($target)
  {
    global $ATK_VARS;
    
    if(count($ATK_VARS ))
    {
      $url = $target."?";
      foreach ($ATK_VARS as $key => $val)
      {
        $url .= $key."=".rawurlencode($val)."&";
      }
      return $url;
    }
    return "";
    
  }
  
    /**
   * Makes an string with hidden input fields
   * containing all posted vars
   */  
  function makeHiddenPostvars($excludes=array())
  {
    global $ATK_VARS;
    
    if(count($ATK_VARS ))
    {
      foreach ($ATK_VARS as $key => $val)
      {
        if (!in_array($key, $excludes))
          $str .= "<input type='hidden' name=\"$key\" value=\"".$val."\">\n";
      }
      return $str;
    }
    return "";
  }
  
  /** 
   * Returns a string representation of an action status.
   * @param $status Must be one of ACTION_FAILED, ACTION_SUCCESS or ACTION_CANCELLED.
   */
  function atkActionStatus($status)
  {
    switch ($status)
    {
      case ACTION_CANCELLED: return "cancelled";
      case ACTION_FAILED: return "failed";
      case ACTION_SUCCESS: return "success";
    }
  }
  
  /**
   * Generate a dispatch menu URL for use with nodes
   * and their specific actions.
   * @param $node the (module.)node name
   * @param $action the atk action the link will perform
   * param $params: A key/value array with extra options for the url
   * @return url for the node with the action
   */
  function dispatch_url($node, $action, $params="")
  {
    /* create url */
    $url = "dispatch.php?atknodetype=".$node."&atkaction=".$action;
    if ($params!=""&&is_array($params))
    {
      foreach ($params as $key => $value)
      {
        $url .= "&$key=".rawurlencode($value);
      }
    }
    
    /* return url */
    return $url;
  }

  /**
   * Writes trace file to system tmp directory
   * @param $msg message to display in the trace
   */
  function atkTrace($msg="")
  {
    global $HTTP_SERVER_VARS, $HTTP_SESSION_VARS, $HTTP_GET_VARS, $HTTP_COOKIE_VARS, $HTTP_POST_VARS;

    $log = "\n".str_repeat("=", 5)."\n";
    $log.= "Trace triggered: ".$msg."\n";
    $log.= date("r")."\n";
    $log.= $HTTP_SERVER_VARS["REMOTE_ADDR"]."\n";
    $log.= $HTTP_SERVER_VARS["SCRIPT_URL"]."\n";
    $log.= "\nSessioninfo: "."session_name(): ".session_name()." session_id(): ".session_id()." SID: ".SID." REQUEST: ".$_REQUEST[session_name()]." COOKIE: ".$_COOKIE[session_name()]."\n";
    $log.= "\n\nHTTP_SERVER_VARS:\n";
    $log.= var_export($HTTP_SERVER_VARS, true);
    $log.= "\n\nHTTP_SESSION_VARS:\n";
    $log.= var_export($HTTP_SESSION_VARS, true);
    $log.= "\n\nHTTP_COOKIE_VARS:\n";
    $log.= var_export($HTTP_COOKIE_VARS, true);
    $log.= "\n\nHTTP_POST_VARS:\n";
    $log.= var_export($HTTP_POST_VARS, true);
    $log.= "\n\nHTTP_GET_VARS:\n";
    $log.= var_export($HTTP_GET_VARS, true);

    $log.= "\n\nSession file info:\n";
    $log.= var_export(stat(session_save_path()."/sess_".session_id()), true);

    $tmpfile = tempnam("/tmp",atkconfig("identifier")."_trace_");
    $fp = fopen($tmpfile,"a");
    fwrite($fp, $log);
    fclose($fp);
  }
  
  function atkButton($text, $url="", $sessionstatus=SESSION_DEFAULT)
  {
    global $g_layout;
    $g_layout->register_script(atkconfig("atkroot")."atk/javascript/formsubmit.js");
    static $cnt=0;    
    
    $script = 'atkSubmit("'.atkurlencode(session_url($url,$sessionstatus)).'")';
    return '<input type="button" name="atkbtn'.(++$cnt).'" value="'.$text.'" onClick=\''.$script.'\'>';
  }
  
  function atkimport($fullclassname, $failsafe=true)
  {
    $elems = explode(".", strtolower($fullclassname));
    $last = &$elems[count($elems)-1];
    $last = "class.".$last.".inc";
    
    $filename = atkconfig("atkroot").implode("/", $elems);
    
    if ($failsafe)
    {
      require_once($filename);
    }
    else
    {
      include_once($filename);
    }
  }
  
  function &atknew($fullclassname)
  {
    atkimport($fullclassname, true);
    
    $elems = explode(".", strtolower($fullclassname));
    $classname = $elems[count($elems)-1];
    
    if (class_exists($classname))
    {      
      return new $classname;
    }
    else
    {
      atkerror("Class $fullclassname not found.");
      return NULL;
    }
  }

  /**
  * TODO: DELETE THIS FUNCTION!!!
  *
  * @return void
  */
  function kdb($m)
  {	
    atkdebug("<font color=green><b>$m</b></font>");
  }


?>
