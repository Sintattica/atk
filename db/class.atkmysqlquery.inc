<?php

 /**
  * Class to handle MySQL specific queries.
  *
  * @author Sandy Pleyte (sandy@ibuildings.net)
  * @version $Revision$
  *
  * $Id$
  *
  */

  class atkmysqlquery extends atkquery
  {
    /**
     * Generate a searchcondition that does a regular expression match.
     */ 
    function regexpCondition($field, $value,$inverse=false)
    {
      if($value[0]=='!') 
      {
        return $field." NOT REGEXP '".substr($value,1,strlen($value))."'";
      }
      else
      {
        return $field." REGEXP '$value'";
      }
    }
    
    /**
     * Generate a searchcondition that does a soundex match of two strings.
     */ 
    function soundexCondition($field, $value,$inverse=false)
    {
      if($value[0]=='!') 
      {
        return "soundex($field) NOT like concat('%',substring(soundex('".substr($value,1,strlen($value))."') from 2),'%')";
      }
      else
      {
        return "soundex($field) like concat('%',substring(soundex('$value') from 2),'%')";
      }
    }
  }

?>