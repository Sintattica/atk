<?php
  /**
   * This class is a non-authenticating authentication thingee. (it 
   * is used for applacations that don't need security)
   *
   * <b>Todo's</b>   : - everything
   *
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * <b>Changes</b>:
   *    
   * $Id$
   * $Log$
   * Revision 1.1  2001/04/23 10:17:14  ivo
   * Initial revision
   *
   * Revision 1.4  2001/04/02 08:22:58  ivo
   * Improved groupbased attribute security.
   *
   * Revision 1.3  2001/03/16 07:52:00  ivo
   * Improved group-based security.
   *
   * Revision 1.2  2001/02/15 19:04:59  martin
   * Fixed parse error line 27! (bleh)
   *
   * Revision 1.1  2001/02/15 16:20:24  ivo
   * Major new feature: security.
   *
   *
   */
   
   class auth_none extends auth_interface
   {
     function validateUser($user, $passwd)
     {
       return true;
     }
     
     function getUser($user)
     {
       return Array("name"=>"","level"=>-1);
     }
     
     function getEntity($node,$action)
     {
       return Array();
     }
     
     function getAttribEntity($node, $attrib, $mode)
     {
       return Array();
     }
   }

?>