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
   * Revision 4.2  2001/06/12 12:24:23  ivo
   * New feature: users can now have more then one group in db authentication.
   *
   * Revision 4.1  2001/05/21 13:38:18  ivo
   * Added a cookie that can remember your login if you close your browser.
   * (disabled by default, enable in your configfile if you need it)
   *
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
       
       $query = "SELECT ".atkconfig("auth_passwordfield")." FROM ".atkconfig("auth_usertable")." WHERE ".atkconfig("auth_userfield")." ='$user'";
       atkdebug($query);
       
       $recs = $g_db->getrows($query);              
          
       $valid = (count($recs)>0 && $user!="" && $recs[0][atkconfig("auth_passwordfield")]==$passwd);      

       return $valid;          
     }
     
     function getUser($user)
     {
       global $g_db;
       
       $usertable = atkconfig("auth_usertable");
       $leveltable = atkconfig("auth_leveltable");
       $userfield = atkconfig("auth_userfield");
       
       if ($usertable==$leveltable)
       {
         // Level and userid are stored in the same table.
         // This means one user can only have one level.
         $query = "SELECT * FROM $usertable WHERE $userfield ='$user'";
       }
       else
       {
         // Level and userid are stored in two separate tables. This could 
         // mean (but doesn't have to) that a user can have more than one
         // level.
         $query = "SELECT * 
                   FROM 
                     $usertable
                   LEFT JOIN 
                     $leveltable
                   ON 
                     $usertable.$userfield = $leveltable.$userfield
                   WHERE
                     $usertable.$userfield = '$user'";                     
                     
       }
       
       atkdebug("getUser query: $query");
       $recs = $g_db->getrows($query);
       
       // We might have more then one level, so we loop the result. 
       if (count($recs)>1)
       {
         $level = array();
         for ($i=0;$i<count($recs);$i++)
         {
           $level[] = $recs[$i][atkconfig("auth_levelfield")];
         }
       }
       else
       {
         $level = $recs[0][atkconfig("auth_levelfield")];
       }
       
       return Array("name"=>$user, "level"=>$level);
     }
     
     function getEntity($node, $action)
     {
       global $g_db;
       
       $query = "SELECT * FROM ".atkconfig("auth_accesstable")." WHERE node='$node' AND action IN ('$action','*') ORDER BY action DESC";
       atkdebug($query);           
                          
       $rights = $g_db->getrows($query);
       
//       var_dump($rights);
       $result = Array();
       
       for ($i=0;$i<count($rights);$i++)
       {
         $result[] = $rights[$i][atkconfig("auth_levelfield")];
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
         if ($rights[$i][atkconfig("auth_levelfield")]!="") 
         {
           $result[] = $rights[$i][atkconfig("auth_levelfield")];
         }
       }
       
       return $result;
     }
   }

?>
