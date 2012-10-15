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
   * @copyright (c)9 Ibuildings.nl BV
   * @license http://www.atkframework.com/licensing ATK Open Source License
   *
   */

  /**
   * @internal include
   */
  atkimport ( "atk.security.auth_db" );
  
  /**
   * Driver for authentication and authorization using Microsoft's Security 
   * Support Provider Interface (SSPI).
   *
   * To use this authentication module, add a field to your user table that 
   * stores the user's SSPI account. Then add the following lines to your 
   * config.inc.php file: 
   * 
   * // The names of your SSPI trusted domains. 
   * $config_auth_sspi_trusted_domains = Array ( "DOMAINNAME" );
   * 
   * // The field in the user table that stores the sspi account name
   * $config_auth_sspi_accountfield = "sspiaccountfield"; 
   * 
   * Finally, change the following configuration values to enable SSPI.
   * 
   * $config_authentication = "sspi";
   * $config_authorization = "sspi";
   * 
   * @author Giroux
   * @package atk
   * @subpackage security
   *
   */
   class auth_sspi extends auth_db
   {  
     function auth_sspi ()
     {
       global $ATK_VARS;

       if (isset($ATK_VARS["atklogout"]))
       {
         if ( $this->validateUser () == AUTH_SUCCESS )
         {
           // On se reconnecte par defaut
           $session = &atkSessionManager::getSession();
           
           $session["relogin"]=1;
         }
       }
     }
     
     function buildSelectUserQuery($sspiaccount,$usertable,$userfield,$sspiaccountfield,$accountdisablefield=null,$accountenbleexpression=null)
     {
       // On recherche le compte sspi
       $disableexpr="";
       if($accountdisablefield) $disableexpr = ", $accountdisablefield";
       $query = "SELECT $userfield $disableexpr FROM $usertable WHERE $sspiaccountfield ='".$sspiaccount."'";
       if ($accountenbleexpression) $query .= " AND $accountenbleexpression";
       return $query;
     }

     function validateUser($user="", $passwd="")
     {
	   global $ATK_VARS;
	   $sspipath= $_SERVER [ "REMOTE_USER" ];
	   $position = strpos ( $sspipath, "\\" );
	   $domain = substr ( $sspipath, 0, $position ); 
	   $user   = substr ( $sspipath, $position + 1, strlen ( $sspipath ) - $position ); 
	   if (!isset($sspipath) || ($sspipath=="") || ! in_array ( $domain, atkconfig("auth_sspi_trusted_domains")))
	   	  return AUTH_UNVERIFIED;
	   	  
       // Si on ne recharge pas chaque fois l'utilisateur et si l'utilisateur n'a pas change
       // @todo, what is auth_reloadusers? does not seem relevant to this piece of code, doesn't exist 
       // elsewhere in atk.
	   if (!atkconfig("auth_reloadusers") && ( $user == $_SERVER["PHP_AUTH_USER"] ))
	   {
		 // On autorise
         return AUTH_SUCCESS;
       }
       
	   $firstload = !isset($_SERVER["PHP_AUTH_USER"]);
	   $_SERVER["PHP_AUTH_USER"] = "" ;
	   $ATK_VARS["auth_user"] = "" ;
       $db = &atkGetDb(atkconfig("auth_database"));
       $query = $this->buildSelectUserQuery($user,
	   										atkconfig("auth_usertable"),
                                            atkconfig("auth_userfield"),
                                            atkconfig("auth_sspi_accountfield"),
                                            atkconfig("auth_accountdisablefield"),
                                            atkconfig("auth_accountenableexpression"));

       $recs = $db->getrows($query);
       if (count($recs)>0 && $this->isLocked($recs[0]))
       {
         return AUTH_LOCKED;
       }
	   // Erreur : on affiche le domaine et l'utilisateur dans la fenetre de login
	   if ( count($recs)==0 )
	   {
         $_SERVER["PHP_AUTH_USER"] = $domain.".".$user;
         $ATK_VARS["auth_user"] = $domain.".".$user ;
         return AUTH_MISMATCH;
	   }

	   if ((count($recs)==1))
	   {
	   	 // Mise jour des variables directement : l'utilisateur n'a pas ete renseigne donc on le renseigne
		 $_SERVER["PHP_AUTH_USER"] = $user;
		 $ATK_VARS["auth_user"] = $user ;
	     $_SERVER["PHP_AUTH_PW"] = $domain;
         $ATK_VARS["auth_pw"] = $domain;

         return AUTH_SUCCESS;
       }
       else
       {
       	 return AUTH_MISMATCH;
       }
     }

     function selectUser($user)
     {
       $usertable = atkconfig("auth_usertable");
       $sspifield = atkconfig("auth_sspi_accountfield");
       $leveltable = atkconfig("auth_leveltable");
       $levelfield = atkconfig("auth_levelfield");
       $userpk = atkconfig("auth_userpk");
       $userfk = atkconfig("auth_userfk", $userpk);
       $grouptable = atkconfig("auth_grouptable");
       $groupfield = atkconfig("auth_groupfield");
       $groupparentfield = atkconfig("auth_groupparentfield");

       $db = &atkGetDb(atkconfig("auth_database"));
       if ($usertable==$leveltable||$leveltable=="")
       {
         // Level and userid are stored in the same table.
         // This means one user can only have one level.
         $query = "SELECT * FROM $usertable WHERE $sspifield ='$user'";
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
	       $qryobj->addCondition("$usertable.$sspifield = '$user'");

	       if (!empty($groupparentfield))
	       {
	         $qryobj->addField("grp.$groupparentfield");
	         $qryobj->addJoin($grouptable, "grp", "usergroup.$levelfield = grp.$groupfield", true);
	       }
	       $query = $qryobj->buildSelect();
       }
       $recs = $db->getrows($query);
       return $recs;
     }

     function getUser(&$user)
     {
       $grouptable = atkconfig("auth_grouptable");
       $groupfield = atkconfig("auth_groupfield");
       $groupparentfield = atkconfig("auth_groupparentfield");
	   $user = $_SERVER["PHP_AUTH_USER"];

       $recs = $this->selectUser($user);
       $groups = array();

       // We might have more then one level, so we loop the result.
       if (count($recs)>0)
       {
         $level = array();
         $parents = array();

         for ($i=0;$i<count($recs);$i++)
         {
           $level[] = $recs[$i][atkconfig("auth_levelfield")];
           $groups[] = $recs[$i][$groupfield];

           if (!empty($groupparentfield) && $recs[$i][$groupparentfield] != "")
             $parents[] = $recs[$i][$groupparentfield];
         }

         $groups = array_merge($groups, $parents);
         while (count($parents) > 0)
         {
           $precs = $this->getParentGroups($parents);
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

       $userinfo = $recs[0];
       $userinfo["name"]=$user;
       $userinfo["level"]=$level; // deprecated. But present for backwardcompatibility.
       $userinfo["groups"]=$groups;
       $userinfo["access_level"]=$this->getAccessLevel($recs);

       return $userinfo;
     }

   }

?>