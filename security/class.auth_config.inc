<?php
  /**
   * This class does authentication via a user/password in the configfile
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
   *
   */
   
   class auth_config extends auth_interface
   {
     function validateUser($user, $passwd)
     {       
       global $config_user;       
       if ($user!="" && $passwd!="" && $config_user[$user]["password"]==$passwd) 
       {        
         return AUTH_SUCCESS;       
       }
       else
       {
         return AUTH_MISMATCH;
       }
     }
     
     function canMd5()
     {
       return atkconfig("authentication_md5");
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
