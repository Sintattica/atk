<?php
  /**
   * This class is a non-authenticating authentication thingee. (it 
   * is used for applacations that don't need security)
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$   
   *
   */
   
   class auth_none extends auth_interface
   {
     function validateUser($user, $passwd)
     {
       return 1;
     }          
     
     function canMd5()
     {
       return false;
     }
   }

?>