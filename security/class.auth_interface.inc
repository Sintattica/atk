<?php

  /**
   * This class is the interface for all auth_ classes
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
   * Revision 1.3  2001/04/02 08:22:58  ivo
   * Improved groupbased attribute security.
   *
   * Revision 1.2  2001/03/16 07:52:00  ivo
   * Improved group-based security.
   *
   * Revision 1.1  2001/02/15 16:20:24  ivo
   * Major new feature: security.
   *
   *
   */

  class auth_interface 
  {         
    function validateUser($user, $passwd)
    {
      return false; // dummy implementation. should not be used.
    }
    
    function getUser($user)
    {
      return Array("name"=>"","level"=>0); // dummy implementation, should not be used.
    }
    
    function getEntity($node, $action)
    {
      return Array();
    }
    
    function getAttribEntity($node, $attrib, $mode)
    {
      return Array();
    }
  }

?>