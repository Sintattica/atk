<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   *
   * This file contains a set of general-purpose utility functions.
   * @todo Move all these functions to relevant classes.
   * @todo Document all of the functions
   *
   * @copyright (c)2000-2007 Ibuildings.nl BV
   * @copyright (c)2000-2007 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Converts applicable characters to html entities so they aren't
   * interpreted by the browser.
   */
  define("DEBUG_HTML", 1);

  /**
   * Wraps the text into html bold tags in order to make warnings more
   * clearly visible.
   */
  define("DEBUG_WARNING", 2);

  /**
   * Hides the debug unless in level 2.
   * This should be used for debug you only really want to see if you
   * are developing (like deprecation warnings).
   */
  define("DEBUG_NOTICE", 4);

  /**
   * Error message.
   */
  define("DEBUG_ERROR", 8);

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
    $errortype = array (
      E_ERROR              => "Error",
      E_WARNING            => "Warning",
      E_PARSE              => "Parsing Error",
      E_NOTICE             => "Notice",
      E_CORE_ERROR         => "Core Error",
      E_CORE_WARNING       => "Core Warning",
      E_COMPILE_ERROR      => "Compile Error",
      E_COMPILE_WARNING    => "Compile Warning",
      E_USER_ERROR         => "User Error",
      E_USER_WARNING       => "User Warning",
      E_USER_NOTICE        => "User Notice",
    );

    //E_STRICT is available since 5.0.0
    if (version_compare(phpversion(), "5.0.0")>=0)
    {
      $errortype[E_STRICT] = "Strict Notice";
    }

    //E_RECOVERABLE is available since 5.2.0
    if (version_compare(phpversion(), "5.2.0")>=0)
    {
      $errortype[E_RECOVERABLE_ERROR] = "Catchable Fatal Error";
    }


    // Translate the given errortype into a string
    $errortypestring = $errortype[$errtype];

    if ((version_compare(phpversion(), "5.0.0")>=0) && $errtype==E_STRICT)
    {
      //Ignore strict notices for now.
      return;
    }

    if ($errtype==E_NOTICE || error_reporting()==0)
    {
      // Just show notices in the debug log, but don't influence the program flow
      atkdebug("[$errortypestring] $errstr in $errfile (line $errline)",DEBUG_NOTICE);
      return;
    }
    else if (($errtype & (E_WARNING|E_USER_WARNING)) > 0)
    {
      // This is something we should pay attention to, but we don't need to die.
      atkerror("[$errortypestring] $errstr in $errfile (line $errline)");
      return;
    }
    else
    {
      atkerror("[$errortypestring] $errstr in $errfile (line $errline)");

      // we must die. we can't even output anything anymore..
      // we can do something with the info though.
      handleError();
      atkOutput::getInstance()->outputFlush();
      die;
    }
  }

  /**
   * @deprecated Use atkhalt instead.
   */
  function halt($msg,$level="warning")
  {
    return atkhalt($msg, $level);
  }

  /**
   * Function atkhalt
   * Halts on critical errors and also on warnings if specified in the config file.
   * @param string $msg   The message to be displayed
   * @param string $level The level of the error,
   *                      ("critical"|"warning" (default))
   * @return bool false if something goes horribly wrong
   */
  function atkhalt($msg,$level="warning")
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
      $output=atkOutput::getInstance();
      $res ="<html>";
      $res.='<body bgcolor="#ffffff" color="#000000">';
      $res.="<font color=\"$level_color\"><b>".atktext($level,"atk")."</b></font>: $msg.<br />\n";
      $res.="<b>Halted</b>";
      $output->output($res);
      $output->outputFlush();
      die("Halted...");
    }
    else
    {
      atkerror("$msg");
    }
    return false;
  }

  /**
   * @deprecated Use atkDebugger::getMicroTime()
   * @return int the microtime
   */
  function getmicrotime()
  {
    atkimport("atk.utils.atkdebugger");
    return atkDebugger::getMicroTime();
  }

  /**
   * @deprecated Use atkDebugger::elapsed();
   *
   * @return string elapsed time in microseconds
   */
  function elapsed()
  {
    atkimport("atk.utils.atkdebugger");
    return atkDebugger::elapsed();
  }

  /**
   * Function atkdebug
   *
   * Adds debug text to the debug log
   * @param String $txt The text that will be added to the log
   * @param Integer $flags An optional combination of DEBUG_ flags
   */
  function atkdebug($txt, $flags = 0)
  {
    global $g_debug_msg;
    $level = atkconfig("debug");
    if ($level>=0)
    {
      if (hasFlag($flags, DEBUG_HTML))
        $txt = atk_htmlentities($txt);
      if (hasFlag($flags, DEBUG_WARNING))
        $txt = "<b>" . $txt . "</b>";

      $line = atkGetTimingInfo().$txt;
      atkWriteLog($line);

      if (hasFlag($flags, DEBUG_ERROR))
      {
        $line = '<span class="atkDebugError">'.$line.'</span>';
      }

      if ($level>2)
      {
        atkimport("atk.utils.atkdebugger");
        if (!atkDebugger::addStatement($line))
        {
          $g_debug_msg[]=$line;
        }
      }
      else if (!hasFlag($flags,DEBUG_NOTICE))
      {
        $g_debug_msg[]=$line;
      }
    }
    else if ($level>-1) // at 0 we still collect the info so we
                        // have it in error reports. At -1, we don't collect
    {
      $g_debug_msg[]=$txt;
    }
  }

  /**
   * Send a notice to the debug log.
   * A notice doesn't get show unless your debug level is 3 or higher.
   *
   * @param String $txt The text that will be added to the log
   */
  function atknotice($txt)
  {
    atkdebug($txt,DEBUG_NOTICE);
  }

  /**
   * Send a warning to the debug log.
   * A warning gets shown more prominently than a normal debug line.
   * However it does not trigger a mailreport
   * or anything else that an atkerror triggers.
   *
   * @param unknown_type $txt
   * @return unknown
   */
  function atkwarning($txt)
  {
    atkdebug($txt,DEBUG_WARNING);
  }

  function atkGetTimingInfo()
  {
    return "[".elapsed().(atkconfig('debug')>0 && function_exists("memory_get_usage")?" / ".sprintf("%02.02f", (memory_get_usage()/1024/1024))."MB":"")."] ";
  }

  /**
   * Like atkdebug, this displays a message at the bottom of the screen.
   * The difference is, that this is also displayed when debugging is turned
   * off.
   *
   * If errorreporting by email is turned on, the errormessages are also handled (from atkOutput)
   *
   * @param string $txt the text to display
   */
  function atkerror($txt)
  {
    global $g_error_msg;
    $g_error_msg[]="[".elapsed()."] ".$txt;

    atkdebug($txt, DEBUG_ERROR);

    if (function_exists('debug_backtrace'))
      atkdebug("Trace:".atk_get_trace(), DEBUG_ERROR);
  }

  /**
   * Returns a trace-route from all functions where-through the code has been executed
   *
   * @param string $format (html|plaintext)
   * @return string Backtrace in html or plaintext format
   */
  function atkGetTrace($format="html")
  {
    // Return if the debug_backtrace function doesn't exist
    if(!function_exists("debug_backtrace"))
      return "Incorrect php-version for atk_get_trace()";

    // Get the debug backtrace
    $traceArr = debug_backtrace();

    // Remove the call of atk_get_trace
    array_shift($traceArr);

    // Start with an empty result;
    $ret = "";

    $theSpacer = "";

    // Loop through all items found in the backtrace
    for($i=0, $_i = count($traceArr); $i < $_i; $i++)
    //for($i=count($traceArr)-1; $i >= 0; $i--)
    {
      // Skip this item in the backtrace if empty
      if (empty($traceArr[$i]))
        continue;

      // Don't display an atkerror statement itself.
      if ($traceArr[$i]["function"]=="atkerror")
        continue;

      // Read the source location
      if (isset($traceArr[$i]["file"]))
        $location = $traceArr[$i]["file"] . (isset($traceArr[$i]["line"]) ? sprintf(", line %d", $traceArr[$i]["line"]) : "[Unknown line]");
      else
        $location = "[PHP KERNEL]";

      // Read the statement
      if (isset($traceArr[$i]["class"]))
      {
        $statement = $traceArr[$i]["class"];
        if (isset($traceArr[$i]["type"]))
          $statement .= $traceArr[$i]["type"];
      }
      else
      {
        $statement = "";
      }
      $statement .= $traceArr[$i]["function"];

      // Initialize the functionParamArr array
      $functionParamArr = array();

      // Parse any arguments into the array
      if(isset($traceArr[$i]["args"]))
      {
        foreach($traceArr[$i]["args"] as $val)
        {
          if(is_array($val))
          {
            $valArr = array();
            foreach($val as $name=>$value)
            {
              if(is_numeric($name))
                $valArr[] = $name;
              else
              {
                if (is_object($value))
                  $valArr[] = sprintf("%s=Object(%s)", $name, get_class($val));
                else
                  $valArr[] = $name."=".$value;
              }
            }
            $stringval = "array(".implode(", ", $valArr).")";
          }
          else if (is_null($val)) $stringval = 'null';
          else if (is_object($val)) $stringval = sprintf("Object(%s)", get_class($val));
          else if (is_bool($val)) $stringval = $val ? 'true' : 'false';
          else
          {
            if(strlen($val.$theSpacer) > 103)
              $stringval = '"'.substr($val, 0, 100-strlen($theSpacer)).'"...';
            else
              $stringval = '"'.$val.'"';
          }
          $functionParamArr[] = $theSpacer."  ".$stringval;
        }
      }
      $functionParams = implode(",\n", $functionParamArr);

      $ret .= $theSpacer."@".$location."\n";
      $ret .= $theSpacer.$statement;
      $ret .= (strlen($functionParams)?"\n".$theSpacer."(\n".$functionParams."\n".$theSpacer.")":"()")."\n";

      // Add indentation
      $theSpacer .= "  ";
    }

    // If html format should be used, replace the html special chars with html entities and put the backtrace within preformat tags.
    if ($format=="html")
      $ret = "<pre>" . htmlspecialchars($ret) . "</pre>";

    // Return the generated trace
    return $ret;
  }

  /**
   * @deprecated Use atkGetTrace instead
   */
  function atk_get_trace($format="html")
  {
    return atkGetTrace($format);
  }

  /**
   * Writes info to a given file.
   * Useful for writing to any log files.
   * @param String $text text to write to the logfile
   * @param String $file the file name
   */
  function atkWriteToFile($text, $file="")
  {
    $fp = @fopen($file, "a");
    if ($fp)
    {
      fwrite($fp, $text."\n");
      fclose($fp);
    }
  }

  /**
   * Writes info to the optional debug logfile.
   * Please notice this feature will heavily decrease the performance
   * and should therefore only be used for debugging and development
   * purposes.
   * @param String $text text to write to the logfile
   */
  function atkWriteLog($text)
  {
    if (atkconfig("debug") > 0 && atkconfig("debuglog"))
    {
      atkWriteToFile($text, atkconfig("debuglog"));
    }
  }


  /**
   * Replaces the [vars] with the values from the language files
   * Please note that it is important, for performance reasons,
   * that you pass along the module where the language files can be found
   * @param mixed $string           string or array of strings containing the name(s) of the string to return
   *                                when an array of strings is passed, the second will be the fallback if
   *                                the first one isn't found, and so forth
   * @param String $module          module in which the language file should be looked for,
   *                                defaults to core module with fallback to ATK
   * @param String $node            the node to which the string belongs
   * @param String $lng             ISO 639-1 language code, defaults to config variable
   * @param String $firstfallback   the first module to check as part of the fallback
   * @param boolean $nodefaulttext  if true, then it doesn't return a default text
   *                                when it can't find a translation
   * @return String the string from the languagefile
   * @deprecated Use atktext instead
   */
  function text($string, $node="", $module="", $lng="", $firstfallback="", $nodefaulttext=false)
  {
    atkdebug("Call to deprecated text() function",DEBUG_WARNING);
    atkimport("atk.atklanguage");
    return atkLanguage::text($string, $module, $node, $lng, $firstfallback, $nodefaulttext);
  }

  /**
   * Replaces the [vars] with the values from the language files
   * Please note that it is important, for performance reasons,
   * that you pass along the module where the language files can be found
   * @param mixed $string           string or array of strings containing the name(s) of the string to return
   *                                when an array of strings is passed, the second will be the fallback if
   *                                the first one isn't found, and so forth
   * @param String $module          module in which the language file should be looked for,
   *                                defaults to core module with fallback to ATK
   * @param String $node            the node to which the string belongs
   * @param String $lng             ISO 639-1 language code, defaults to config variable
   * @param String $firstfallback   the first module to check as part of the fallback
   * @param boolean $nodefaulttext  if true, then it doesn't return a default text
   *                                when it can't find a translation
   * @param boolean $modulefallback Wether or not to use all the modules of the application in the fallback,
   *                                when looking for strings
   * @return String the string from the languagefile
   */
  function atktext($string, $module="",$node="", $lng="", $firstfallback="", $nodefaulttext=false, $modulefallback=false)
  {
    atkimport("atk.atklanguage");
    return atkLanguage::text($string, $module, $node, $lng, $firstfallback, $nodefaulttext,$modulefallback);
  }

  /**
   * @deprecated Use atkSession::newLevel() instead
   */
  function session_level($sessionstatus=SESSION_DEFAULT, $levelskip=1)
  {
    return atkSessionManager::newLevel($sessionstatus, $levelskip);
  }

  /**
   * @deprecated Use atkSessionManager::formState instead.
   */
  function session_form($sessionstatus=SESSION_DEFAULT, $returnbehaviour=NULL, $fieldprefix='')
  {
    return atkSessionManager::formState($sessionstatus, $returnbehaviour, $fieldprefix);
  }

  /**
   * @deprecated Use atkSessionManager::sessionVars() instead.
   */
  function session_vars($sessionstatus=SESSION_DEFAULT, $levelskip=1, $url="")
  {
    return atkSessionManager::sessionVars($sessionstatus, $levelskip, $url);
  }

  /**
   * @deprecated use atkSessionManager::sessionUrl() instead.
   */
  function session_url($url,$sessionstatus=SESSION_DEFAULT, $levelskip=1)
  {
    return atkSessionManager::sessionUrl($url, $sessionstatus, $levelskip);

  }

  /**
   * @deprecated use atkHref or atkSessionManager::href instead.
   */
  function href($url,$name="",$sessionstatus=SESSION_DEFAULT, $saveform=false, $extraprops="")
  {
    return atkSessionManager::href($url, $name, $sessionstatus, $saveform, $extraprops);
  }

  /**
   * Convenience wrapper for atkSessionManager::href().
   * @see atkSessionManager::href
   */
  function atkHref($url, $name="", $sessionstatus=SESSION_DEFAULT, $saveform=false, $extraprops="")
  {
    return atkSessionManager::href($url, $name, $sessionstatus, $saveform, $extraprops);
  }


  /**
   * Same as array_merge from php, but without duplicates..
   * Supports unlimited number of arrays as arguments.
   *
   * @param Array $array1 the first array
   * @param Array $array2 the second array
   * @return Array The result of the merge between $array1 and $array2
   */
  function atk_array_merge($array1, $array2)
  {
    $res = Array();

    $arrays = func_get_args();
    for ($i = 0, $_i = count($arrays); $i < $_i; $i++)
    {
      for ($j = 0, $_j = count($arrays[$i]); $j < $_j; $j++)
      {
        if (!in_array($arrays[$i][$j], $res))
        {
          $res[] = $arrays[$i][$j];
        }
      }
    }

    return $res;
  }

  /**
   * Same as array_merge_recursive from PHP but without duplicates.
   * Supports unlimited number of arrays as arguments.
   *
   * @param array $array1 first array
   * @param array $array2 second array
   *
   * @return array merged arrays
   */
  function atk_array_merge_recursive($array1, $array2)
  {
    $arrays = func_get_args();

    $result = array();

    foreach ($arrays as $array)
    {
      foreach ($array as $key => $value)
      {
        if (isset($result[$key]) && is_array($result[$key]) && is_array($value))
        {
          $result[$key] = atk_array_merge_recursive($result[$key], $value);
        }
        else
        {
          $result[$key] = $value;
        }
      }
    }

    return $result;
  }

  /**
   * Same as array_merge from php, but this function preserves key=>index
   * association in case of numerical indexes. Supports unlimited number
   * of arrays as arguments.
   *
   * @param Array $array unlimited number of arrays
   * @return Array The result of the merge between the given arrays
   */
  function atk_array_merge_keys()
  {
    $args = func_get_args();
    $result = array();
    foreach($args as $array)
    {
      foreach($array as $key=>$value)
      {
        $result[$key] = $value;
      }
    }
    return $result;
  }

  /**
   * Since php triggers an error if you perform an in_array on an
   * uninitialised array, we provide a small wrapper that performs
   * an is_array on the haystack first, just to make sure the user
   * doesn't get an error message.
   *
   * @param mixed   $needle   The value to search for.
   * @param Array   $haystack The array to search.
   * @param boolean $strict   If true, type must match.
   * @return bool wether or not the value is in the array
   */
  function atk_in_array($needle, $haystack, $strict=false)
  {
    return (is_array($haystack)&&in_array($needle, $haystack, $strict));
  }

  /**
   * Function dataSetContains
   *
   * Checks if a value is in a Array
   * @param array $set  the array
   * @param var $key    the key in the array as in $array[$key]
   * @param var $value  the value we are looking for
   * @return bool wether or not the value is in the array
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
   * Strips ' or  " from the begin and end of a string (only if they are
   * on both sides, e.g. foo' remains foo' but 'bar' becomes bar.
   * @param string $string the string we need to strip
   * @return string the stripped string
   */
  function stripQuotes($string)
  {
    $temp = trim($string);
    if (substr($temp,0,1)=="'" && substr($temp,-1)=="'") return substr($temp,1,-1);
    if (substr($temp,0,1)=='"' && substr($temp,-1)=='"') return substr($temp,1,-1);
    return $string;
  }

  /**
   * Translates a string like id='3' into Array("id"=>3)
   * @param string $pair the string which is to be decoded
   * @return array the decoded array
   */
  function decodeKeyValuePair($pair)
  {
    list($key,$value)=split("==|!=|<>|>=|<=|=|<|>",$pair);
    return array($key=>stripQuotes($value));
  }

  /**
   * Translates a string like id='3 AND name='joe'' into Array("id"=>3,"name"=>"joe")
   * @todo we should also support <=>, >=, >, <=, <, <>
   * @param string $set the string to decode
   * @return array the decoded array
   */
  function decodeKeyValueSet($set)
  {
    $result = array();
    $items=split(" AND ",$set);
    for ($i=0;$i<count($items);$i++)
    {
      if (strstr($items[$i], '!=') !== false)
      {
        list($key,$value) = explode("!=",$items[$i]);
        $result[trim($key)] = stripQuotes($value);
      }
      elseif (strstr($items[$i], '=') !== false)
      {
        list($key,$value) = explode("=",$items[$i]);
        $result[trim($key)] = stripQuotes($value);
      }
      elseif (stristr($items[$i], 'IS NULL') !== false)
      {
        list($key) = spliti('IS NULL', $items[$i]);
        $result[trim($key)] = NULL;
      }
    }
    return $result;
  }


  /**
   * Translates Array("id"=>3,"name"=>"joe") into a string like id='3 AND name='joe''
   * @param array $set the array to be encoded
   * @return string the encoded string
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
   * Same as strip_slashes from php, but if the passed value is an array,
   * all elements of the array are stripped. Recursive function.
   * @param var &$var the value/array to strip the slashes of
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
      $var = stripslashes($var);
    }
  }


  /**
   * Performs stripslashes on all vars and translates:
   *                 something_AMDAE_other[] into something[][other]
   *                 something_AE_other into something[other]
   *                 (and a_AE_b_AE_c into a[b][c] and so on...
   * @param array &$vars the array to be stripped and translated
   */
  function atkDataDecode(&$vars)
  {
    $magicQuotes = get_magic_quotes_gpc();

    foreach (array_keys($vars) as $varname)
    {
      $value = &$vars[$varname];
      // We must strip all slashes from the input, since php puts slashes
      // in front of quotes that are passed by the url. (magic_quotes_gpc)
      if ($value !== NULL && $magicQuotes)
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
    }
  }

  /**
   * Weird function. $dest is an associative array, that may contain
   * stuff like $dest["a_AE_c_AE_b"] = 3.
   * Now if you run this function like this:
   *  AE_decode($dest, "a_AE_c_AE_b");
   * then $dest will contain a decoded array:
   *  echo $dest["a"]["b"]["c"]; <- this will display 3
   * @param array &$dest  the array to put the decoded var in
   * @param string $var   the var to decode
   */
  function AE_decode(&$dest, $var)
  {
    $items = explode("_AE_", $var);
    if (count($items) <= 1) return;

    $current = &$dest;
    foreach ($items as $key)
    {
      $current = &$current[$key];
    }

    if (is_array($dest[$var]))
    {
      $current = atk_array_merge_recursive((array)$current, $dest[$var]);
    }
    else
    {
      $current = $dest[$var];
    }

    unset($dest[$var]);
  }

  /**
   * Get the [ ] Fields out of a String
   * @deprecated please use the atkStringParser class
   */
  function stringfields($string)
  {
    atkdebug("Warning: deprecated use of stringfields(). Use atkStringParser class instead");
    $tmp = "";
    $adding = false;
    $fields = array();
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
   * Parse strings
   * @deprecated please use the atkStringParser class
   */
  function stringparse($string, $data,$encode=false)
  {
    atkdebug("Warning: deprecated use of stringparse(). Use atkStringParser class instead");
    $fields = stringfields($string);
    for ($i=0;$i<count($fields);$i++)
    {
      $elements = split("\.",$fields[$i]);
      $databin = $data;
      for($j=0;$j<count($elements);$j++)
      {
        if (array_key_exists($elements[$j],$databin))
        {
          $value = $databin[$elements[$j]];
          $databin = $databin[$elements[$j]];
        }
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
   *
   * @param string $string  the url to encode
   * @param bool $pref      wether or not to use a prefix, default true
   * @return string the encoded url
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
   * Wrap lines, add a newline after 100 characters
   * @param string $line the line to add new lines in
   * @return string the line with newlines
   */
  function _wordwrap($line)
  {
    return wordwrap($line,100,"\n",1);
  }

  /**
   * Send a detailed error report to the maintainer.
   *
   */
  function mailreport()
  {
    global $g_error_msg, $g_debug_msg;
    include_once(atkconfig('atkroot'). 'atk/errors/class.atkerrorhandlerbase.inc');
    $errorHandlerObject = atkErrorHandlerBase::get('mail', array('mailto'=>atkconfig('mailreport')));
    $errorHandlerObject->handle($g_error_msg, $g_debug_msg);
  }

  /**
   * Handle errors that occurred in ATK, available handlers from /atk/errors/ can be added to
   * the error_handlers config.
   *
   */
  function handleError()
  {
    global $g_error_msg, $g_debug_msg;
    include_once(atkconfig('atkroot'). 'atk/errors/class.atkerrorhandlerbase.inc');
    $errorHandlers = atkconfig('error_handlers', array('mail'=>array('mailto' => atkconfig('mailreport'))));
    foreach ($errorHandlers as $key => $value)
    {
      if (is_numeric($key))
        $key = $value;
      $errorHandlerObject = atkErrorHandlerBase::get($key, $value);
      $errorHandlerObject->handle($g_error_msg, $g_debug_msg);
    }
  }


  /**
   * Wrapper for escapeSQL function
   * @param String $string The string to escape.
   * @param boolean $wildcard Set to true to convert wildcard chars ('%').
   *                          False (default) will leave them unescaped.
   * @return String A SQL compatible version of the input string.
   */
  function escapeSQL($string, $wildcard=false)
  {
    $db = &atkGetDb();
    return $db->escapeSQL($string, $wildcard);
  }

  /**
   * Return the atk version number.
   * @return string the version number of ATK
   */
  function atkversion()
  {
    include(atkconfig("atkroot")."atk/version.inc");
    return $atk_version;
  }

  /**
   * Convenience wrapper for atkDb::getInstance()
   * @param String $conn The name of the connection to retrieve
   * @return atkDb Database connection instance
   */
  function &atkGetDb($conn='default', $reset=false, $mode="r")
  {
    atkimport("atk.db.atkdb");
    $db = &atkDb::getInstance($conn, $reset, $mode);
    return $db;
  }

  /**
   * Returns a url to open a popup window
   * @param string $target  the target of the popup
   * @param string $params  extra params to pass along
   * @param string $winName the name of the window
   * @param int $width      the width of the popup
   * @param int $height     the height of the popup
   * @param string $scroll  allow scrolling? (no (default)|yes)
   * @param string $resize  allow resizing? (no (default)|yes)
   * return string the url for the popup window
   */
  function atkPopup($target,$params,$winName,$width,$height,$scroll='no',$resize='no')
  {
    $url = session_url("include.php?file=".$target."&".$params, SESSION_NESTED);
    $popupurl ="javascript:NewWindow('".$url."','".$winName."',".$height.",".$width.",'".$scroll."','".$resize."')";
    return $popupurl;
  }

  /**
   * Adds new element to error array en $record. When
   * $msg is empty the multilange error string is used.
   * @param array &$rec var in which to add element to error array
   * @param var$attrib attributename or an array with attribute names
   * @param string $err multilanguage error string
   * @param string $msg optinal error string
   */
  function triggerError(&$rec, $attrib, $err, $msg="", $tab="", $label='', $module='atk')
  {
    if($msg=="")  $msg = atktext($err, $module);
    $rec['atkerror'][] = array( "attrib_name"=> $attrib, "err" => $err, "msg" => $msg, "tab" => $tab, "label" => $label);
  }

  /**
   * Adds new element to the record error array. When no message
   * is given the multi-language error string is used.
   *
   * @param array              $record  record
   * @param atkAttribute|array $attrib attribute or array of attributes
   * @param string             $error multi-language error string
   * @param string             $message error message (optional)
   */
  function atkTriggerError(&$record, &$attrib, $error, $message='')
  {
    if (is_array($attrib))
    {
      $attribName = array();
      $label = array();

      for ($i = 0; $i < count($attrib); $i++)
      {
        $attribName[$i] = $attrib[$i]->fieldName();
        $label[$i] = $attrib[$i]->label($record);
      }

      $tab = $attrib[0]->m_tabs[0];
      $message = $attrib[0]->text($error);
    }
    else
    {
      $attribName = $attrib->fieldName();
      $label = $attrib->label($record);
      $tab = $attrib->m_tabs[0];
      $message = $attrib->text($error);
    }

    triggerError($record, $attribName, $error, $message, $tab, $label);
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
   * Does a var dump of an array. Makes use of atkdebug for displaying the values.
   *
   * @param $a data to be displayed
   * @param $d name of the data that's being displayed.
   */
  function atk_var_dump($a, $d="")
  {
    ob_start();
    var_dump($a);
    $data = ob_get_contents();
    atkdebug("vardump: ".($d!=""?$d." = ":"")."<pre>".$data."</pre>");
    ob_end_clean();
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
      $mime='application/x-gzip';
      $filename.= ".gz";
    }
    else if($compression=="zip")
    {
      $mime='application/x-zip';
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
   * @param string $file     the local filename (the file you want to open
   *                         on the serverside)
   * @param string $filename the name the file will get when the user downloads it.
   * @param string $mimetype the mimetype of the file
   * @return bool wether or not the export worked
   */
  function exportFile($file, $filename,$mimetype="", $detectmime=true)
  {
    include_once(atkconfig("atkroot")."atk/atkbrowsertools.inc");
    $browser = getBrowserInfo();
    if (eregi("ie", $browser["browser"]))
    {
      $mime = "application/octetstream";
      $disp = 'attachment';
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
    else if ($mimetype=="" && $detectmime && function_exists('mime_content_type'))
      $mime = mime_content_type($file);

    $fp = @fopen($file,"rb");
    if ($fp!=NULL)
    {
      header('Content-Type: '. $mime);
      header("Content-Length: ".filesize($file));
      header('Content-Disposition:  '.$disp.'; filename="'.$filename.'"');
      if(eregi("ie", $browser["browser"])) header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
      if (($_SERVER["SERVER_PORT"] == "443" || $_SERVER['HTTP_X_FORWARDED_PROTO' ]==='https') && eregi("msie", $_SERVER["HTTP_USER_AGENT"]))
      {
        header('Pragma: public');
      }
      else
      {
        header('Pragma: no-cache');
      }
      header('Expires: 0');

      header("Content-Description: File Transfer");
      header("Content-Transfer-Encoding: binary");

      fpassthru($fp);
      return true;
    }
    return false;
  }

  /**
   * Includes the file containing the specified attribute
   *
   * @param string $attribute The attribute to include in the format "module.attribute". ATK will
   *                          search in [moduledir]/attributes/ for the attribute file.
   *                          When no modulename is specified ATK will search for the attribute
   *                          in [atkdir]/attributes/
   */
  function useattrib($attribute)  { atkuse("attribute", $attribute); }

  /**
   * Includes the file containing the specified relation
   *
   * @param string $relation  The relation to include in the format "module.relation". ATK will
   *                          search in [moduledir]/relations/ for the relation file.
   *                          When no modulename is specified ATK will search for the relation
   *                          in [atkdir]/relations/
   */
  function userelation($relation) { atkuse("relation" , $relation);  }
  function usefilter($filter)     { atkuse("filter"   , $filter);    }

  /**
   * Returns the include file for an atk class (attribute, relation or filter)
   * @param string $type the type of the class (attribute|relation|filter)
   * @param string $name the name of the class
   */
  function atkgetinclude($type, $name)
  {
    global $config_atkroot;
    $a = explode(".", $name);
    if (count($a) == 2)
      $include = moduleDir(strtolower($a[0])).$type."s/class.".strtolower($a[1]).".inc";
    else $include = $config_atkroot."atk/".$type."s/class.".strtolower($name).".inc";
    return $include;
  }

  /**
   * Check if an atk class exists (attribute, relation or filter)
   * @param string $type the type of the class (attribute|relation|filter)
   * @param string $name the name of the class
   */
  function atkexists($type, $name)
  {
    return file_exists(atkgetinclude($type, $name));
  }

  /**
   * Use an atk class (attribute, relation or filter)
   * @param string $type the type of the class (attribute|relation|filter)
   * @param string $name the name of the class
   */
  function atkuse($type, $name)
  {
    include_once(atkgetinclude($type, $name));
  }

  /**
   * Returns the (virtual) hostname of the server.
   * @return string the hostname of the server
   */
  function atkHost()
  {
    $atkHost = $_SERVER["HTTP_HOST"]!=""?$_SERVER["HTTP_HOST"]:$_SERVER["SERVER_NAME"];

    // if we're running on our cluster environment
    // we seem to have a specific portid within the HTTP_HOST
    // If so, remove it from the hostname

    $dummy = explode(":", $atkHost);
    return $dummy[0];
  }

  /**
   * Returns the next unique ID for the given sequence.
   * NOTE: ID's are only unique for the script execution!
   * @param string $sequence the sequence name
   * @return int next unique ID for the given sequence
   */
  function getUniqueID($sequence)
  {
    static $unique = array();
    if (!isset($unique[$sequence]))
    {
      $unique[$sequence] = 0;
    }
    return ++$unique[$sequence];
  }

  /**
   * Checks if the variable $var contains the given flag ($flag).
   * @param string $var the variable which might contain flags
   * @param var $flag the flag you want to check for
   * @return bool result of check
   */
  function hasFlag($var, $flag)
  {
    return ($var & $flag) == $flag;
  }

  /**
   * Makes an url from the target var and all postvars
   * @param string $target the path of the file to open
   * @param string the url with the postvars
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
   * Makes an string with hidden input fields containing all posted vars
   * @param array $excludes array with the vars to exclude, default empty
   */
  function makeHiddenPostvars($excludes=array())
  {
    global $ATK_VARS;
    $str = "";

    if(count($ATK_VARS ))
    {
      foreach ($ATK_VARS as $key => $val)
      {
        if (!in_array($key, $excludes))
          $str .= "<input type='hidden' name=\"$key\" value=\"".atk_htmlentities(strval($val))."\">\n";
      }
      return $str;
    }
    return "";
  }

  /**
   * Returns a string representation of an action status.
   * @param var $status status of the action
   *                    (ACTION_FAILED|ACTION_SUCCESS|ACTION_CANCELLED)
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
   * Build query string based on an array of parameters.
   *
   * @param array $params array of parameters
   */
  function buildQueryString($params, $parent="")
  {
    $query = "";

    foreach ($params as $key => $value)
    {
      if (!empty($query))
        $query .= '&';

      if (!empty($parent))
        $key = "{$parent}[{$key}]";

      if (!is_array($value))
      {
        $query .= "$key=".rawurlencode($value);
      }
      else
      {
        $query .= buildQueryString($value, $key);
      }
    }

    return $query;
  }

  /**
   * Generate a dispatch menu URL for use with nodes and their specific
   * actions.
   *
   * Note that this does not necessarily create a link to the current php
   * file (dispatch.php, index.php). It asks the controller which one to use.
   *
   * @param string $node the (module.)node name
   * @param string $action the atk action the link will perform
   * @param string $params: A key/value array with extra options for the url
   * @param string $phpfile The php file to use for dispatching, if not set we look at the theme for the dispatchfile
   * @return string url for the node with the action
   */
  function dispatch_url($node, $action, $params=array(),$phpfile='')
  {
    $c = &atkinstance("atk.atkcontroller");
    if (!$phpfile) $phpfile =  $c->getPhpFile();
    $url = $phpfile;
    $atkparams = array();
    if($node!="")
      $atkparams["atknodetype"] = $node;
    if($action!="")
      $atkparams["atkaction"] = $action;
    $params = array_merge($atkparams, $params);

    if ($params != "" && is_array($params) && count($params) > 0)
      $url .= '?'.buildQueryString($params);

    return $url;
  }

  /**
   * @deprecated Use atkcontroller::getPhpFile() instead.
   */
  function getDispatchFile()
  {
    $c = &atkinstance("atk.atkcontroller");
    return $c->getPhpFile();
  }

  /**
   * Generate a partial url.
   *
   * @param string $node the (module.)node name
   * @param string $action the atkaction
   * @param string $partial the partial name
   * @param array $params a key/value array with extra params
   * @param int $sessionStatus session status (default SESSION_PARTIAL)
   * @return string url for the partial action
   */
  function partial_url($node, $action, $partial, $params=array(), $sessionStatus=SESSION_PARTIAL)
  {
    if (!is_array($params))
      $params = array();
    $params['atkpartial'] = $partial;

    return session_url(dispatch_url($node, $action, $params), $sessionStatus);
  }

  /**
   * Writes trace file to system tmp directory
   * @param string $msg message to display in the trace
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

  /**
   * Creates a session aware button
   * @param string $text       the text to display on the button
   * @param string $url        the url to use for the button
   * @param var $sessionstatus the session flags
   *              (SESSION_DEFAULT (default)|SESSION_NEW|SESSION_REPLACE|
   *               SESSION_NESTED|SESSION_BACK)
   * @param string $cssclass   the css class the button should get
   * @param bool $embeded      wether or not it's an embedded button
   */
  function atkButton($text, $url="", $sessionstatus=SESSION_DEFAULT, $embedded=true, $cssclass="")
  {
    $page = &atkPage::getInstance();
    $page->register_script(atkconfig("atkroot")."atk/javascript/formsubmit.js");
    static $cnt=0;

    if ($cssclass == "")
      $cssclass = "btn";

    $cssclass = ' class="'.$cssclass.'"';
    $script = 'atkSubmit("'.atkurlencode(session_url($url,$sessionstatus)).'")';
    $button = '<input type="button" name="atkbtn'.(++$cnt).'" value="'.$text.'" onClick=\''.$script.'\''.$cssclass.'>';

    if (!$embedded)
    {
      $res = '<form name="entryform">';
      $res.= session_form();
      $res.= $button.'</form>';
      return $res;
    }
    else
    {
      return $button;
    }
  }

  /**
   * Imports a file
   * @param string $fullclassname Name of class in atkformat (map1.map2.classfile)
   * @param bool   $failsafe      If $failsafe is true (default), the class is required.  Otherwise, the
   *                                class is included.
   * @param bool   $path          Whether or not it is NOT an ATK classname
   *                                 ("map.class"), if true it will interpret classname
   *                                 as: "map/class.classname.inc", default false.
   * @return bool whether the file we want to import was actually imported or not
   */
  function atkimport($fullclassname, $failsafe=true, $path = false)
  {
    return atkClassLoader::import($fullclassname, $failsafe, $path);
  }

  /**
   * Imports a zend-file
   * @param string $classname Name of class in zend-format (starting with a Capital)
   */
  function zendimport($classname)
  {
    $current_path = getcwd();
    chdir(atkConfig('atkroot')."atk/ext/zend/");

    $filename = 'Zend/'.$classname.'.php';

    if (file_exists($filename))
    {
      require_once $filename;
    }

    chdir($current_path);
  }

  /**
   * Clean-up the given path.
   *
   * @param string $path
   * @return cleaned-up path
   *
   * @see http://nl2.php.net/manual/en/function.realpath.php (comment of 21st of September 2005)
   */
  function atkCleanPath($path)
  {
    return atkClassLoader::cleanPath($path);
  }

  /**
   * Converts an ATK classname ("map1.map2.classname")
   * to a pathname ("/map1/map2/class.classname.inc")
   * @param string $fullclassname  ATK classname to be converted
   * @param bool $class            is the file a class? defaults to true
   * @return string converted filename
   */
  function getClassPath($fullclassname, $class = true)
  {
    return atkClassLoader::getClassPath($fullclassname, $class);
  }

  /**
   * Converts a pathname ("/map1/map2/class.classname.inc")
   * to an ATK classname ("map1.map2.classname")
   * @param string $classpath pathname to be converted
   * @param bool $class       is the file a class? defaults to true
   * @return string converted filename
   */
  function getClassName($classpath, $class=true)
  {
    return atkClassLoader::getClassName($classpath, $class);
  }

  /**
   * Returns a new instance of a class
   * @param string $fullclassname the ATK classname of the class ("map1.map2.classname")
   * @return obj instance of the class
   */
  function atknew($fullclassname)
  {
    $args = func_get_args();
    array_shift($args);
    $args = array_values($args);
    return atkClassLoader::newInstanceArgs($fullclassname, $args);
  }

  /**
   * Return a singleton instance of the specified class.
   *
   * This works for all singletons that implement the getInstance() method.
   *
   * @param string $fullclassname the ATK classname of the class ("map1.map2.classname")
   * @return obj instance of the class
   * */
  function atkinstance($fullclassname, $reset=false)
  {
    return atkClassLoader::getSingletonInstance($fullclassname, $reset);
  }

  /**
   * Compares two assosiative multi dimensonal array's
   * if arrays differ, return true, otherwise it returns false
   * @param array $array1 original array
   * @param array $array2 new array
   * @return boolean wether or not the arrays differ
   */
  function atkArrayCompare($array1, $array2)
  {
    $difference = atkArrayDiff($array1, $array2);

    return !is_array($difference) ? false : true;
  }

  /**
   * Compares two assosiative multi dimensonal array's
   * if arrays differ, return differences, otherwise it returns false
   * @param array $array1 original array
   * @param array $array2 new array
   * @return mixed differences or false if they do not differ
   */
  function atkArrayDiff($array1, $array2)
  {
    foreach($array1 as $key => $value)
    {
      if(is_array($value))
      {
        if(!is_array($array2[$key]))
        {
          $difference[$key] = $value;
        }
        else
        {
          $new_diff = atkArrayDiff($value, $array2[$key]);
          if($new_diff != FALSE)
          {
            $difference[$key] = $new_diff;
          }
        }
      }
      elseif(!isset($array2[$key]) || $array2[$key] != $value)
      {
        $difference[$key] = $value;
      }
    }

    return !isset($difference) ? false : $difference;
  }

  /**
   * Recursive function that checks an array for values
   * because sometimes arrays will be filled with other empty
   * arrays and therefore still show up filled.
   *
   * WARNING: take care with using this function as it is recursive
   * and if you have a value linking back to it's self in one way or another,
   * you may spend a loooong time waiting on your application
   *
   * @param Array $array The array that
   * @return bool Wether or not we found anything
   */
  function atk_value_in_array($array)
  {
    if (is_array($array) && !empty($array))
    {
      foreach ($array as $key => $value)
      {
        if (is_array($value))
        {
          if (atk_value_in_array($value)) return true;
        }
        else if($value) return true;
      }
    }
    return false;
  }

  /**
   * Recursive function to look if the needle exists in the haystack
   *
   * WARNING: take care with using this function as it is recursive
   * and if you have a value linking back to it's self in one way or another,
   * you may spend a loooong time waiting on your application
   *
   * @param String $needle The value which will be searched in the haystack
   * @param Array $haystack Array with values
   * @return Boolean True if needle exists in haystack
   */
  function atk_in_array_recursive($needle, $haystack)
  {
    foreach ($haystack as $key=>$value)
    {
      if ($value==$needle) return true;
      else if (is_array($value))
      {
        if (atk_in_array_recursive($needle, $value)) return true;
      }
    }
    return false;
  }

  /**
   * Escapes the predefined characters
   *
   * When there are predefined characters used this function will escape them
   * and returns right pattern.
   *
   * @param String $pattern Raw string to be escaped
   * @return String Returns a pattern with the predefined pattern escaped
   */
  function escapeForRegex($pattern)
  {
    $escaped='';
    $escapechars = array("/","?",'"', "(", ")", "'","*",".","[","]");
    for ($counter = 0; $counter<strlen($pattern);$counter++)
    {
      $curchar = substr($pattern, $counter, 1);
      if (in_array($curchar,$escapechars))
      $escaped .= "\\";
      $escaped.=$curchar;
    }
    return $escaped;
  }

  /*
   * Returns the postvars
   * Returns a value or an array with all values
   */
  function atkGetPostVar($key="")
  {
    if(empty($key) || $key=="")
    {
      return $_REQUEST;
    }
    else
    {
      if (array_key_exists($key,$_REQUEST) && $_REQUEST[$key]!="") return $_REQUEST[$key];
      return "";
    }
  }

  /**
   * ATK version of the PHP htmlentities function. Works just like PHP's
   * htmlentities function, but falls back to atkGetCharset() instead of
   * PHP's default charset, if no charset is given.
   *
   * @param String $string    string to convert
   * @param int $quote_style  quote style (defaults to ENT_COMPAT)
   * @param String $charset   character set to use (default to atkGetCharset())
   *
   * @return String encoded string
   */
  function atk_htmlentities($string, $quote_style=ENT_COMPAT, $charset=null)
  {
    return atkString::htmlentities($string, $quote_style, $charset);
  }

  /**
   * ATK version of the PHP html_entity_decode function. Works just like PHP's
   * html_entity_decode function, but falls back to atkGetCharset() instead of
   * PHP's default charset, if no charset is given.
   *
   * @param String $string    string to convert
   * @param int $quote_style  quote style (defaults to ENT_COMPAT)
   * @param String $charset   character set to use (default to atkGetCharset())
   *
   * @return String encoded string
   */
  function atk_html_entity_decode($string, $quote_style=ENT_COMPAT, $charset=null)
  {
    return atkString::html_entity_decode($string, $quote_style, $charset);
  }

  /**
   * Get string length
   * @param string $str The string being checked for length
   * @return int
   */
  function atk_strlen($str)
  {
  	return atkString::strlen($str);
  }

  /**
   * Get part of string
   * @param string $str The string being checked.
   * @param int $start The first position used in $str
   * @param int $length[optional] The maximum length of the returned string
   * @return string
   */
   function atk_substr($str,$start,$length='')
   {
	 return atkString::substr($str,$start,$length);
   }

   /**
	*  Find position of first occurrence of string in a string
	* @param object $haystack The string being checked.
	* @param object $needle The position counted from the beginning of haystack .
	* @param object $offset[optional] The search offset. If it is not specified, 0 is used.
	* @return int|boolean
	*/
   function atk_strpos($haystack,$needle,$offset=0)
   {
 	 return atkString::strpos($haystack,$needle,$offset);
   }

   /**
	* Make a string lowercase
	* @param string $str The string being lowercased.
	* @return string
	*/
   function atk_strtolower($str)
   {
     return atkString::strtolower($str);
   }

  /**
   *
   * Make a string uppercase
   * @param string $str The string being uppercased.
   * @return string
   */
   function atk_strtoupper($str)
   {
	 return atkString::strtoupper($str);
   }

  /**
   * Return the default charset, first we look if the
   * config_default_charset is set, else we use the
   * charset in the languge file;
   * @return string
   */
  function atkGetCharset()
  {
	return atkconfig('default_charset',atktext('charset','atk'));
  }

  /**
   * Looks up a value using the given key in the given array and returns
   * the value if found or a default value if not found.
   *
   * @param array $array Array to be searched for key
   * @param string $key Key for which we are looking in array
   * @param mixed $defaultvalue Value we will return if key was not found in array
   * @return mixed Value retrieved from array or default value if not found in array
   */
  function atkArrayNvl($array, $key, $defaultvalue=null)
  {
    return (isset($array[$key]) ? $array[$key] : $defaultvalue);
  }

  /**
   * Resolve a classname to its final classname.
   *
   * An application can overload a class with a custom version. This
   * method resolves the initial classname to its overloaded version
   * (if any).
   *
   * @param String $class The name of the class to resolve
   * @return String The resolved classname
   */
  function atkResolveClass($class)
  {
    atkimport("atk.utils.atkclassloader");
    return atkClassLoader::resolveClass($class);
  }

  /**
   * Returns the IP of the remote client.
   *
   * @return string ip address
   */
  function atkGetClientIp()
  {
    static $s_ip = NULL;

    if ($s_ip === NULL)
    {
      if (getenv("HTTP_CLIENT_IP"))
        $s_ip = getenv("HTTP_CLIENT_IP");
      elseif (getenv("HTTP_X_FORWARDED_FOR"))
        $s_ip = getenv("HTTP_X_FORWARDED_FOR");
      elseif (getenv("REMOTE_ADDR"))
        $s_ip = getenv("REMOTE_ADDR");
      else $s_ip = 'x.x.x.x';
    }

    return $s_ip;
  }

  /**
   * Function checks php version and clones the
   * given attribute in the right way
   *
   * @param object $attribute The attribute to clone
   * @return object $attr
   */
  function atkClone($attribute)
  {
    if (intval(substr(phpversion(),0,1))<5)
      $attr = $attribute;
    else
      $attr = clone($attribute);

    return $attr;
  }

  /**
   * Return the current script file. Like $_SERVER['PHP_SELF'], but
   * sanitized for security reasons
   *
   * @return String
   */
  function atkSelf()
  {
    $self = $_SERVER['PHP_SELF'];
    if (strpos($self, '"')!==false) $self = substr($self, 0, strpos($self, '"')); //XSS attempt
    return atk_htmlentities(strip_tags($self)); // just in case..
  }

  /**
   * ATK wrapper of the PHP iconv function. Check if iconv function is present in
   * the system. If yes - use it for converting string, if no - save string untouch
   * and make warning about it.
   *
   * @param string $in_charset  from charset
   * @param string $out_charset to charset
   * @param string $str string to convert
   *
   * @return string encoded string
   */
  function atk_iconv($in_charset, $out_charset, $str)
  {
  	return atkString::iconv($in_charset,$out_charset,$str);
  }

  /**
   * Returns the first argument that is not null.
   *
   * @param mixed ... arguments
   * @return mixed first argument that is not null
   */
  function atkNvl()
  {
    for ($i = 0; $i < func_num_args(); $i++)
    {
      $arg = func_get_arg($i);
      if (!is_null($arg))
      {
        return $arg;
      }
    }

    return null;
  }

  function atkEcho($message)
  {
    if(strpos(atkSelf(),"runcron"))
    {
      echo $message;
    }
  }

  /**
   * Format date according to a format string, uses ATK's language files to translate
   * months, weekdays etc.
   *
   * @param $date    timestamp or date array (gotten with getdate())
   * @param $format  format string, compatible with PHP's date format functions
   * @param $weekday always include day-of-week or not
   *
   * @return string formatted date
   */
  function atkFormatDate($date, $format, $weekday=false)
  {
    static $langcache = array();

    if (!is_array($date))
    {
      $date = getdate($date);
    }

    /* format month */
    $format = str_replace("M", "%-%",   $format);
    $format = str_replace("F", "%=%",   $format);

    /* format day */
    $format = str_replace("D", "%&%", $format);
    $format = str_replace("l", "%*%", $format);

    if ($weekday && strpos($format, '%&%') === FALSE && strpos($format, '%*%') === FALSE)
    {
      $format = str_replace("d", "%*% d", $format);
      $format = str_replace("j", "%*% j", $format);
    }

    /* get date string */
    require_once(atkconfig('atkroot')."atk/utils/adodb-time.inc.php");
    $str_date = adodb_date($format, $date[0]);

    $month = $date['month'];
    $shortmonth = substr(strtolower($date["month"]), 0, 3);

    /* store the text calls */
    if(!isset($langcache[$month]))
    {
      $langcache[$month]= atktext(strtolower($month),"atk");
    }

    if(!isset($langcache[$shortmonth]))
    {
      $langcache[$shortmonth] = atktext($shortmonth);
    }

    /* replace month/week name */
    $str_date = str_replace("%-%", $langcache[$shortmonth], $str_date);
    $str_date = str_replace("%=%", $langcache[$month], $str_date);
    $str_date = str_replace("%*%", atktext(strtolower($date["weekday"]),"atk"), $str_date);
    $str_date = str_replace("%&%", atktext(substr(strtolower($date["weekday"]), 0, 3),"atk"), $str_date);

    /* return string */
    return $str_date;
  }
?>
