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
  * Revision 1.1  2001/04/23 10:17:02  ivo
  * Initial revision
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
  function halt($msg,$level) 
  {
    global $g_debug_msg;
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
      if($level=="warning") { $prefix="warning:"; } else { $prefix="dbg:"; }
      $g_debug_msg.="$prefix $msg <br>\n";
      //$g_debug_msg.=$GLOBALS['config_halt_on_error'];
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
     $g_debug_msg.="dbg: $txt <br>\n";
  }

  /**
    * function ArrayToUrlVars
    *
    * Creates an Url String
    */
  function arrayToUrlVars($array, $leadchar="?", $prefix="")
  {
    $str = "";
    if (count($array>0))
    {
      $str = $leadchar;
      for  ($i=0;list($key,$value) = each($array);$i++)
      {        
        if (is_array($value))
        {
          $str.=arrayToUrlVars($value,"",$prefix.$key."_AMDAE_")."&";
        }
        else
        {
          $str.=$prefix.$key."=".rawurlencode($value);
          if ($i<count($array)-1) $str.="&";
        }
      }
    }
    return $str; 
  }


   /**
   * Function Text
   *
   * Replaces the txt_[vars] with the values from the language files
   */
  function text($var)
  {
    $result =  $GLOBALS["txt_".$var];
    if ($result=="") return str_replace('_',' ',$var);
    else return $result;
  }
  
  /**
    * Function url
    *
    *  Makes an url   ( <a href="url" extraprops>name</a> )
    */
  function url($url,$name="",$extraprops="")
  {
    if ($name=="") $name=$url;
    return '<a href="'.$url.'" '.$extraprops.'>'.$name.'</a>';
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
    list($key,$value)=split("=",$pair);
    return Array($key=>stripQuotes($value));    
  }
  
  /**
    * Function decodeKeyValueSet
    *
    * translates a string like id='3 AND name='joe'' into Array("id"=>3,"name"=>"joe")
    */
  function decodeKeyValueSet($set)
  {
    $result = Array();
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
    * function atkDataDecode
    *
    * translate: something_AMDAE_other[] into something[][other]
    * something_AMDAE_other into something[other]
    * something_BMDAE_other['test'] into something[other]['test']  
    */
  function atkDataDecode(&$vars)
  {
    reset($vars);
    while(list($varname,$value)=each($vars))
    {
      // if (!is_array($value)) $vars[$varname]=stripslashes(rawurldecode($value)); // it's a mistery to me why this stripslashes is necessary...
      if (!is_array($value)) $vars[$varname]=stripslashes($value); // it's a mistery to me why this stripslashes is necessary...
          
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
          }/*
          // Multidimensional thing
          while (list($key,$val) = each($value))
          {
            $vars[strtolower($dimension1)][$key][strtolower($dimension2)] = $val;
          }*/
        }
        else
        {
          $vars[strtolower($dimension1)][strtolower($dimension2)] = $value;
        }
      }
      elseif (strpos(strtoupper($varname),'_BMDAE_')>0) // Now I *know* that strpos could return 0 if _AMDAE_ *is* found
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

?>
