<?php
  /**
   * This class does authentication via a user/password in the configfile
   *
   * <b>Todo's</b>   : - everything
   *
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * <b>Changes</b>:
   *    
   * $Id$
   * $Log$
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:14  ivo
   * Added atk4 to repository.
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
   
   class auth_config extends auth_interface
   {
     function validateUser($user, $passwd)
     {       
       global $config_user;
                   
       $valid = ($user!="" && $config_user[$user]["password"]==$passwd);
       
       return $valid;   
     }
      
     function getUser($user)
     {
       global $config_user;
       return Array("name"=>$user, "level"=>$config_user[$user]["level"]);
     }
     
     function getEntity($node, $action)
     {
       global $config_access;
                     
       $rights = $config_access[$node];
       
       $result = Array();
                                 
       for ($i=0;$i<count($rights);$i++)
       {         
         if ($rights[$i][$action]!="") 
         {
           $result[] = $rights[$i][$action];
         }
         if ($rights[$i]["*"]!="") 
         {
           $result[] = $rights[$i]["*"];
         }
       }
       
       return $result;
     }
     
     function getAttribEntity($node, $attrib, $mode)
     {
       global $config_attribrestrict;
       
       // $entity is an array of entities that may do $mode on $node.$attrib.
       $entity = $config_attribrestrict[$node][$attrib][$mode];
              
       return $entity;      
     }
   }

?>