<?php
  /**
   * This class does authentication through a set of tables in a db
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
   * Revision 1.5  2001/04/02 08:22:58  ivo
   * Improved groupbased attribute security.
   *
   * Revision 1.4  2001/03/16 07:52:00  ivo
   * Improved group-based security.
   *
   * Revision 1.3  2001/03/15 14:31:50  ivo
   * changed userdb and accessdb options to usertable and accesstable.
   *
   * Revision 1.2  2001/03/14 16:16:30  sandy
   * database name and userid field is configurable
   *
   * Revision 1.1  2001/02/15 16:20:24  ivo
   * Major new feature: security.
   *
   *
   */
   
   class auth_db extends auth_interface
   {
     function validateUser($user, $passwd)
     {       
       global $g_db;
       
       $query = "SELECT ".$GLOBALS["config_auth_passwordfield"]." FROM ".$GLOBALS["config_auth_usertable"]." WHERE ".$GLOBALS["config_auth_userfield"]." ='$user'";
       atkdebug($query);
       
       $recs = $g_db->getrows($query);
              
       if ($GLOBALS["config_authentication_md5"]) $passwd = md5($passwd);
          
       $valid = (count($recs)>0 && $user!="" && $recs[0][$GLOBALS["config_auth_passwordfield"]]==$passwd);      

       return $valid;          
     }
     
     function getUser($user)
     {
       global $g_db;
       
       $query = "SELECT * FROM ".$GLOBALS["config_auth_usertable"]." WHERE ".$GLOBALS["config_auth_userfield"]." ='$user'";
       
       $recs = $g_db->getrows($query);
       
       return Array("name"=>$user, "level"=>$recs[0]["entity"]);
     }
     
     function getEntity($node, $action)
     {
       global $g_db;
       
       $query = "SELECT * FROM ".$GLOBALS["config_auth_accesstable"]." WHERE node='$node' AND action IN ('$action','*') ORDER BY action DESC";
       atkdebug($query);           
                          
       $rights = $g_db->getrows($query);
       
//       var_dump($rights);
       $result = Array();
       
       for ($i=0;$i<count($rights);$i++)
       {
         $result[] = $rights[$i]["entity"];
       }
             
       return $result;
     }
     
     function getAttribEntity($node, $attrib, $mode)
     {
       global $g_db;
       
       $query = "SELECT * FROM attribaccess WHERE node='$node' AND attribute='$attrib' AND mode='$mode'";
                          
       $rights = $g_db->getrows($query);
       
//       var_dump($rights);
       $result = Array();
                                 
       for ($i=0;$i<count($rights);$i++)
       {         
         if ($rights[$i]["entity"]!="") 
         {
           $result[] = $rights[$i]["entity"];
         }
       }
       
       return $result;
     }
   }

?>
