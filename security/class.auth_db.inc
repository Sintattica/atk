<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage security
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Driver for authentication and authorization using tables in the database.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage security
   *
   */
   class auth_db extends auth_interface
   {
     var $m_rightscache = array();  
   
     function validateUser($user, $passwd)
     {
       $db = &atkGetDb();
       $userexpr = $db->escapeSql($user);
       if (atkconfig("auth_accountdisablefield")!="")
       {
         $query = "SELECT ".atkconfig("auth_passwordfield").",".atkconfig("auth_accountdisablefield")." FROM ".atkconfig("auth_usertable")." WHERE ".atkconfig("auth_userfield")." ='$userexpr'";
         if (atkconfig("auth_accountenableexpression")!="") $query .= " AND ".atkconfig("auth_accountenableexpression");
       }
       else
       {
         $query = "SELECT ".atkconfig("auth_passwordfield")." FROM ".atkconfig("auth_usertable")." WHERE ".atkconfig("auth_userfield")." ='$userexpr'";
         if (atkconfig("auth_accountenableexpression")!="") $query .= " AND ".atkconfig("auth_accountenableexpression");
       }
       $recs = $db->getrows($query);
       if (isset($recs[0][atkconfig("auth_accountdisablefield")])&&$recs[0][atkconfig("auth_accountdisablefield")]==1)
       {
         return AUTH_LOCKED;
       }

       return ((count($recs)>0 && $user!="" && $this->matchPasswords($recs[0][atkconfig("auth_passwordfield")], $passwd))?AUTH_SUCCESS:AUTH_MISMATCH);
     }
     
     /**
      * Match 2 passwords.
      * In normal situations, $dbpassword and $userpasswd are considered equal
      * if they are a case-insensitive match. When $config_auth_cryptedpassword
      * is true, they are only considered a match if $dbpassword is equal to the
      * crypt() of $userpasswd, where $dbpassword itself is used as the 'salt'.
      * (This method is used by Bugzilla, among other apps)
      */
     function matchPasswords($dbpasswd, $userpasswd)
     {     
       // crypt password method, like in bugzilla
       if (atkconfig("auth_usecryptedpassword", false))
       {
       	 // password is stored using the crypt method, using the cryptedpassword itself as the salt.      
       	 return (crypt($userpasswd, $dbpasswd)==$dbpasswd);
       }
       else 
       {
       	 // regular match, perhaps with md5.
     	   return (strtoupper($dbpasswd)==strtoupper($userpasswd));
       }
     }

     function canMd5()
     {
       return atkconfig("authentication_md5");
     }

     function getUser($user)
     {
       $db = &atkGetDb();

       $usertable = atkconfig("auth_usertable");
       $leveltable = atkconfig("auth_leveltable");
       $levelfield = atkconfig("auth_levelfield");
       $userfield = atkconfig("auth_userfield");
       $userpk = atkconfig("auth_userpk");
       $userfk = atkconfig("auth_userfk", $userpk);
       $grouptable = atkconfig("auth_grouptable");         
       $groupfield = atkconfig("auth_groupfield");         
       $groupparentfield = atkconfig("auth_groupparentfield");         
       
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
	       $qryobj =&$db->createQuery();
	       $qryobj->addTable($usertable);
	       $qryobj->addField("$usertable.*");
	       $qryobj->addField("usergroup.*");
	       $qryobj->addJoin($leveltable, "usergroup", "$usertable.$userpk = usergroup.$userfk", true);
	       $qryobj->addCondition("$usertable.$userfield = '$user'");
	       
	       if (!empty($groupparentfield))
	       {
	         $qryobj->addField("grp.$groupparentfield");
	         $qryobj->addJoin($grouptable, "grp", "usergroup.$levelfield = grp.$groupfield", true);
	       }
	       $query = $qryobj->buildSelect();
       }

       $recs = $db->getrows($query);

       $groups = array();

       // We might have more then one level, so we loop the result.
       if (count($recs)>0)
       {
         $level = array();
         
         $parents = array();

         for ($i=0;$i<count($recs);$i++)
         {
           $level[] = $recs[$i][atkconfig("auth_levelfield")];
           $groups[] = $recs[$i][atkconfig("auth_levelfield")];
           
           if (!empty($groupparentfield) && $recs[$i][$groupparentfield] != "")
             $parents[] = $recs[$i][$groupparentfield];
         }

         $groups = array_merge($groups, $parents);
         while (count($parents) > 0)
         {
  	       $query = &$db->createQuery();
  	       $query->addField($groupparentfield);  	    
    	     $query->addTable($grouptable);
  	       $query->addCondition("$grouptable.$groupfield IN (".implode(',', $parents).")");
  	       $precs = $db->getrows($query->buildSelect(TRUE));  
  	       
           $parents = array();  	       
  	       foreach ($precs as $prec)
  	         if ($prec[$groupparentfield] != "")
  	           $parents[] = $prec[$groupparentfield];         
  	         
           $groups = array_merge($groups, $parents);  	         
         }
         
         $groups = array_unique($groups);
       }
       
       if (count($level) == 1)
         $level = $level[0];

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
         $access = "";
         if (isset($recs[0][atkconfig("auth_accesslevelfield")]))
         {
           $access = $recs[0][atkconfig("auth_accesslevelfield")];
         }
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
       $db = &atkGetDb();

       if (!isset($this->m_rightscache[$node])||count($this->m_rightscache[$node])==0)
       {
         $query = "SELECT * FROM ".atkconfig("auth_accesstable")." WHERE node='$node'";

         $this->m_rightscache[$node] = $db->getrows($query);
       }

       $result = Array();
       
       $rights = $this->m_rightscache[$node];

       for ($i=0, $_i=count($rights); $i<$_i; $i++)
       {
         if ($rights[$i]['action'] == $action)
         {
           $result[] = $rights[$i][atkconfig("auth_levelfield")];
         }
       }

       return $result;
     }

     function getAttribEntity($node, $attrib, $mode)
     {
       $db = &atkGetDb();

       $query = "SELECT * FROM attribaccess WHERE node='$node' AND attribute='$attrib' AND mode='$mode'";

       $rights = $db->getrows($query);

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
       $db = &atkGetDb();
       $query = "SELECT * FROM ".atkconfig("auth_usertable");

       $accountdisablefield = atkconfig("auth_accountdisablefield");
       $accountenableexpression = atkconfig("auth_accountenableexpression");
       if ($accountenableexpression!="")
       {
         $query.= " WHERE $accountenableexpression";
         if ($accountdisablefield!="") $query.= " AND $accountdisablefield = 0";
       }
       else
       {
         if ($accountdisablefield!="") $query.= " WHERE $accountdisablefield = 0";
       }

       $recs = $db->getrows($query);

       $userlist = array();
       for ($i=0, $_i=count($recs); $i<$_i; $i++)
       {
         $userlist[] = array("userid"=>$recs[$i][atkconfig("auth_userfield")],
                             "username"=>stringparse(atkconfig("auth_userdescriptor"), $recs[$i]));
       }
       return $userlist;

     }
     
     function setPasswordAllowed()
     {
        return true;
     }
     
   }

?>
