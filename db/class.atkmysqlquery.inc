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
    function regexpCondition($field, $value)
    {
      return $field." REGEXP '$value'";
    }
    
    /**
     * Generate a searchcondition that does a soundex match of two strings.
     */ 
    function soundexCondition($field, $value)
    {
      return "soundex($field) like concat('%',substring(soundex('$value') from 2),'%')";
    }
  }

?>