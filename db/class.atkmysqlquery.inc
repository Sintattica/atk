<?php

/**
* $Id$
* $Log$
* Revision 4.2  2001/07/15 16:37:19  ivo
* New atk.inc includefile in skel.
* New feature: extended search.
* Fixed a bug in session management; style.php and other files that get
* loaded between two dispatches could corrupt a session.
*
* Revision 4.1  2001/05/01 09:49:49  ivo
* Replaced all require() and include() calls by require_once() and
* include_once() calls. The if(!DEFINED)... inclusion protection in files
* is now obsolete.
*
* Revision 4.0  2001/04/23 10:20:39  ivo
* revision tags.
*
* Revision 1.1.1.1  2001/04/23 10:17:10  ivo
* Added atk4 to repository.
*
* Revision 1.1.1.1  2001/01/10 13:57:58  sandy
* Achievo Tool Kit
*
*/

 /**
 *  Class to handle specific MySQL command's
 *  @author Sandy Pleyte (sandy@ibuildings.net)
 *  @version 0.0.1
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