<?php

 /**
  * Atk Tools
  *
  * A collection of functions which are used in ATK
  * 
  * Author: Ivo Jansch <ivo@ibuildings.nl>
  *
  * $Id$
  * $Log$
  * Revision 4.18  2001/09/07 09:50:45  ivo
  * Added session debug information to the error reporter.
  *
  * Revision 4.17  2001/07/27 13:16:14  sandy
  * advanced colorpicker attrib + new popup system
  *
  * Revision 4.16  2001/07/11 21:14:30  ivo
  * Added a global atk version number.
  *
  * Revision 4.15  2001/07/09 15:41:09  ivo
  * Created an atk_in_array that does an is_array before it calls in_array.
  *
  * Revision 4.14  2001/07/01 10:09:52  ivo
  * Replaced the clickheretoadd_prefix/postfix string by link_<nodetype>_add.
  * If you upgrade, please verify your language files!
  *
  * Revision 4.13  2001/06/29 11:26:59  ivo
  * Patches to oracle support.
  *
  * Revision 4.12  2001/06/22 15:34:44  ivo
  * Dirty hack to atkurlencode [pk]'s once they have been replaced.
  * TODO: clean up!
  *
  * Revision 4.11  2001/06/22 14:42:25  ivo
  * Better errorreport template.
  *
  * Revision 4.10  2001/06/22 11:44:42  ivo
  * Workaround for IE4 'bug' that causes javascript to decode urlencoded
  * values that it shouldn't encode: atkurlencode and atkurldecode.
  *
  * Revision 4.9  2001/06/12 13:55:59  ivo
  * If no string can be found in the language file, the default one generated
  * by atk is now more usable. (strips title_ etc.) (contributed by Tom)
  *
  * Revision 4.8  2001/05/21 15:18:26  ivo
  * Made securitymanager cookie and session based. Authentication now only
  * executes once per session.
  *
  * Revision 4.7  2001/05/17 10:27:12  ivo
  * Fixes to session management, new AF_LARGE feature for manytoonerelation.
  * Improved handling of GET_VARS and POST_VARS (requires a new dispatcher)
  *
  * Revision 4.6  2001/05/14 10:20:19  ivo
  * Improved error reporting when database queries fail.
  * New feature: atk can now send a detailed error report by email.
  *
  * Revision 4.5  2001/05/11 09:03:12  ivo
  * fixed an internetexplorer single/doublequote javascript issue.
  *
  * Revision 4.4  2001/05/10 08:31:00  ivo
  * Major upgrade. Changes:
  * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
  *   singletons by default, and nodefunctions pass around recordsets.
  * + Session management for forms. If you now leave a page through a click on
  *   a link, the session remembers everything from your form and restores it
  *   when you return.
  * + New relation: oneToOneRelation
  * + Reimplemented the embedded editForm feature (forms inside forms)
  *
  * Revision 4.3  2001/05/04 11:37:42  sandy
  * small bug fixes (split function, refresh, treeview,etc)
  *
  * Revision 4.2  2001/05/02 12:44:21  ivo
  * multilevel session_back feature.
  * new relational features: maxRecords for oneToManyRelation, filter for
  * onetomanyrelation.
  *
  * Revision 4.1  2001/05/01 09:15:51  ivo
  * Initial session based atk version.
  *
  * Revision 4.0  2001/04/23 10:19:38  ivo
  * Revision number update
  *
  * Revision 1.1.1.1  2001/04/23 10:17:02  ivo
  * Added atk4 to repository.
  *
  * Revision 1.5  2001/03/30 15:08:13  ivo
  * Improved the stringparser: It can now parse associative arrays.
  *
  * Revision 1.4  2001/03/14 10:22:27  ivo
  * Attributes now have a hide() function to do their own input=hidden implementation.
  *
  * Revision 1.3  2001/02/23 11:17:30  sandy
  * - Updated the layout class with new template engine
  * - New Dummy attribute
  * - New skel directory
  * - and some small new config vars
  *
  * Revision 1.2  2001/02/15 16:20:24  ivo
  * Major new feature: security.
  *
  * Revision 1.1.1.1  2001/01/10 13:57:53  sandy
  * Achievo Tool Kit
  *
  */

 

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
   

  /**
    * Function atkdebug
    *
    * Adds the debug txt to the debug log
    */
  function atkdebug($txt)
  {
     global $g_debug_msg;
     $g_debug_msg[]=$txt;
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
  function text($var)
  {
    $result =  $GLOBALS["txt_".$var];
    if ($result=="") return ucfirst(str_replace("_"," ",str_replace('title_','',$var)));
    else return $result;
  }
  
  /** 
   * Function session_form
   *
   * Adds session information to a form
   */
  function session_form($sessionstatus=SESSION_DEFAULT)
  {
    global $atklevel;
    
    if (!isset($atklevel)||$atklevel=="") $atklevel = 0;
    
    switch ($sessionstatus)
    {
      case SESSION_NEW:
      {
        $newlevel = -1; 
        break;
      }
      case SESSION_NESTED:
      {
        $newlevel = $atklevel+1; 
        break;
      }
      case SESSION_BACK:
      {
        $newlevel = max(0,$atklevel-1);
        break;
      }
      default:
      {
        $newlevel = $atklevel;
      }
    }
    
    if ($newlevel!=0) 
    {
      return '<input type="hidden" name="atklevel" value="'.$newlevel.'">';
    }
    return "";
  
  }
  
  /**
   * Function session_url
   *
   *  Makes a session-aware url thingee...
   */
  function session_url($url,$sessionstatus=SESSION_DEFAULT, $levelskip=1)
  {
    global $atklevel;
    
    if (!isset($atklevel)||$atklevel=="") $atklevel = 0;
    
    switch ($sessionstatus)
    {
      case SESSION_NEW:
      {
        $newlevel = -1; 
        break;
      }
      case SESSION_NESTED:
      {
        $newlevel = $atklevel+1; 
        break;
      }
      case SESSION_BACK:
      {
        $newlevel = max(0,$atklevel-$levelskip); 
        break;
      }
      default:
      {
        $newlevel = $atklevel;
      }
    }
    
    if ($name=="") $name=$url;
    if (strpos($url,"?")>0) 
    {
      $start = "&";
    }
    else 
    {
      $start = "?";
    }
    // atklevel is already set manually, we don't append it.. 
    if ($newlevel!=0 && !strpos($url,"atklevel=")>0)
    {
      $url.= $start."atklevel=".$newlevel."&".SID;
    }
    else
    {
      $url.= $start.SID;
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
        atk_stripslashes(&$var[$key]);
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
      atk_stripslashes(&$value);    

      AE_decode(&$vars, $varname);                
          
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
   *
   * Note, this may be implemented more efficiently.
   */      
  function AE_decode(&$dest, $var)
  {                  
    $items = split("_AE_",$var);
            
    if (count($items)>1)
    {          
      $result = $dest[$var];
      
      for ($i=count($items)-1;$i>0;$i--)
      {     
        $tmp = $result;
        $result = array();
        $result[$items[$i]] = $tmp;
      }            
      if (is_array($dest[$items[0]]))
      {
        // the decoded var already exists! so we add the value to the already
        // existing variable.
        $dest[$items[0]][$items[1]] = $result[$items[1]];
      }
      else
      {
        $dest[$items[0]] = $result;
      }
    }          
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
  
  function atkurlencode($string)
  {
    return str_replace("%","_#",rawurlencode($string));
  }
  
  function atkurldecode($string)
  {
    return rawurldecode(str_replace("_#","%",$string));
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
          foreach ($item as $key=>$value)          
          {
            $body.=$key.str_repeat(" ",max(1,30-strlen($key)))." = ".$value."\n";
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
?>
