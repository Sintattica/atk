<?php
  /**
   * This class does authentication through a set of tables in a db
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
   *
   */
   
   class auth_db extends auth_interface
   {
     function validateUser($user, $passwd)
     {       
       global $g_db;
       if (atkconfig("auth_accountdisablefield")!="") 
       {
         $query = "SELECT ".atkconfig("auth_passwordfield").",".atkconfig("auth_accountdisablefield")." FROM ".atkconfig("auth_usertable")." WHERE ".atkconfig("auth_userfield")." ='$user'";
       }
       else 
       {
         $query = "SELECT ".atkconfig("auth_passwordfield")." FROM ".atkconfig("auth_usertable")." WHERE ".atkconfig("auth_userfield")." ='$user'";         
       }
       $recs = $g_db->getrows($query);              
       if ($recs[0][atkconfig("auth_accountdisablefield")]==1)
       {         
         return AUTH_LOCKED;
       }
       
       return ((count($recs)>0 && $user!="" && $recs[0][atkconfig("auth_passwordfield")]==$passwd)?AUTH_SUCCESS:AUTH_MISMATCH);
     }
     
     function canMd5()
     { 
       return atkconfig("authentication_md5");
     }
     
     function getUser($user)
     {
       global $g_db;
       
       $usertable = atkconfig("auth_usertable");
       $leveltable = atkconfig("auth_leveltable");
       $userfield = atkconfig("auth_userfield");
                     
       if ($usertable==$leveltable||$leveltable=="")
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
         $query = "SELECT 
                     *
                   FROM 
                     $usertable
                   LEFT JOIN 
                     $leveltable
                   ON 
                     $usertable.$userfield = $leveltable.$userfield
                   WHERE
                     $usertable.$userfield = '$user'";                     
                     
       }
              
       $recs = $g_db->getrows($query);
                     
       $groups = array();       
       
       // We might have more then one level, so we loop the result. 
       if (count($recs)>1)
       {
         $level = array();
         for ($i=0;$i<count($recs);$i++)
         {                      
           $level[] = $recs[$i][atkconfig("auth_levelfield")];
           $groups[] = $recs[$i][atkconfig("auth_levelfield")];
         }         
       }
       else
       {
         $level = $recs[0][atkconfig("auth_levelfield")];
         $groups = array($recs[0][atkconfig("auth_levelfield")]);         
       }
       
       // We might have more then one access level, so we loop the result.
       if (count($recs)>1)
       {
         $access = array();
         for ($i=0;$i<count($recs);$i++)
         {
           if($i==0) $access = $recs[$i][atkconfig("auth_accesslevelfield")];
           if($recs[$i][atkconfig("auth_accesslevelfield")]>$access) $access = $recs[$i][atkconfig("auth_accesslevelfield")];
//           $access[] = $recs[$i][atkconfig("auth_accesslevelfield")];
         }
       }
       else
       {
         $access = $recs[0][atkconfig("auth_accesslevelfield")];
       }
			 $userinfo = $recs[0];
			 $userinfo["name"]=$user;
			 $userinfo["level"]=$level; // deprecated. But present for backwardcompatibility.
       $userinfo["groups"]=$groups;       
       $userinfo["access_level"]=$access;
       return $userinfo;
     }
     
     function getEntity($node, $action)
     {
       global $g_db;
       
       $query = "SELECT * FROM ".atkconfig("auth_accesstable")." WHERE node='$node' AND action IN ('$action','*') ORDER BY action DESC";
                          
       $rights = $g_db->getrows($query);
       

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
     
     function getUserList()
     {
       global $g_db;
       $query = "SELECT * FROM ".atkconfig("auth_usertable");
       
       $accountdisablefield = atkconfig("auth_accountdisablefield");
       if ($accountdisablefield!="") $query.= " WHERE $accountdisablefield = 0";
       
       $recs = $g_db->getrows($query);
       
       $userlist = array();     
       for ($i=0, $_i=count($recs); $i<$_i; $i++)
       {                
         $userlist[] = array("userid"=>$recs[$i][atkconfig("auth_userfield")], 
                             "username"=>stringparse(atkconfig("auth_userdescriptor"), $recs[$i]));
       }       
       return $userlist;
       
     }
     
   }

?>
