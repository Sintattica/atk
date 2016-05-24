<?php

namespace Sintattica\Atk\Security\Auth;

use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Db\Db;

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
 */
class SspiAuth extends DbAuth
{
    public function auth_sspi()
    {
        global $ATK_VARS;

        if (isset($ATK_VARS['atklogout'])) {
            if ($this->validateUser() == SecurityManager::AUTH_SUCCESS) {
                // On se reconnecte par defaut
                $session = &SessionManager::getSession();

                $session['relogin'] = 1;
            }
        }
    }

    public function buildSelectUserQuery(
        $sspiaccount,
        $usertable,
        $userfield,
        $sspiaccountfield,
        $accountdisablefield = null,
        $accountenbleexpression = null
    ) {
        // On recherche le compte sspi
        $disableexpr = '';
        if ($accountdisablefield) {
            $disableexpr = ", $accountdisablefield";
        }
        $query = "SELECT $userfield $disableexpr FROM $usertable WHERE $sspiaccountfield ='".$sspiaccount."'";
        if ($accountenbleexpression) {
            $query .= " AND $accountenbleexpression";
        }

        return $query;
    }

    public function validateUser($user = '', $passwd = '')
    {
        global $ATK_VARS;
        $sspipath = $_SERVER ['REMOTE_USER'];
        $position = strpos($sspipath, '\\');
        $domain = substr($sspipath, 0, $position);
        $user = substr($sspipath, $position + 1, strlen($sspipath) - $position);
        if (!isset($sspipath) || ($sspipath == '') || !in_array($domain, Config::getGlobal('auth_sspi_trusted_domains'))) {
            return SecurityManager::AUTH_UNVERIFIED;
        }

        // Si on ne recharge pas chaque fois l'utilisateur et si l'utilisateur n'a pas change
        // @todo, what is auth_reloadusers? does not seem relevant to this piece of code, doesn't exist 
        // elsewhere in atk.
        if (!Config::getGlobal('auth_reloadusers') && ($user == $_SERVER['PHP_AUTH_USER'])) {
            // On autorise
            return SecurityManager::AUTH_SUCCESS;
        }

        $_SERVER['PHP_AUTH_USER'] = '';
        $ATK_VARS['auth_user'] = '';
        $db = Db::getInstance(Config::getGlobal('auth_database'));
        $query = $this->buildSelectUserQuery($user, Config::getGlobal('auth_usertable'), Config::getGlobal('auth_userfield'),
            Config::getGlobal('auth_sspi_accountfield'), Config::getGlobal('auth_accountdisablefield'), Config::getGlobal('auth_accountenableexpression'));

        $recs = $db->getRows($query);
        if (count($recs) > 0 && $this->isLocked($recs[0])) {
            return SecurityManager::AUTH_LOCKED;
        }
        // Erreur : on affiche le domaine et l'utilisateur dans la fenetre de login
        if (count($recs) == 0) {
            $_SERVER['PHP_AUTH_USER'] = $domain.'.'.$user;
            $ATK_VARS['auth_user'] = $domain.'.'.$user;

            return SecurityManager::AUTH_MISMATCH;
        }

        if ((count($recs) == 1)) {
            // Mise jour des variables directement : l'utilisateur n'a pas ete renseigne donc on le renseigne
            $_SERVER['PHP_AUTH_USER'] = $user;
            $ATK_VARS['auth_user'] = $user;
            $_SERVER['PHP_AUTH_PW'] = $domain;
            $ATK_VARS['auth_pw'] = $domain;

            return SecurityManager::AUTH_SUCCESS;
        } else {
            return SecurityManager::AUTH_MISMATCH;
        }
    }

    public function selectUser($user)
    {
        $usertable = Config::getGlobal('auth_usertable');
        $sspifield = Config::getGlobal('auth_sspi_accountfield');
        $leveltable = Config::getGlobal('auth_leveltable');
        $levelfield = Config::getGlobal('auth_levelfield');
        $userpk = Config::getGlobal('auth_userpk');
        $userfk = Config::getGlobal('auth_userfk', $userpk);
        $grouptable = Config::getGlobal('auth_grouptable');
        $groupfield = Config::getGlobal('auth_groupfield');
        $groupparentfield = Config::getGlobal('auth_groupparentfield');

        $db = Db::getInstance(Config::getGlobal('auth_database'));
        if ($usertable == $leveltable || $leveltable == '') {
            // Level and userid are stored in the same table.
            // This means one user can only have one level.
            $query = "SELECT * FROM $usertable WHERE $sspifield ='$user'";
        } else {
            // Level and userid are stored in two separate tables. This could
            // mean (but doesn't have to) that a user can have more than one
            // level.
            $qryobj = $db->createQuery();
            $qryobj->addTable($usertable);
            $qryobj->addField("$usertable.*");
            $qryobj->addField('usergroup.*');
            $qryobj->addJoin($leveltable, 'usergroup', "$usertable.$userpk = usergroup.$userfk", true);
            $qryobj->addCondition("$usertable.$sspifield = '$user'");

            if (!empty($groupparentfield)) {
                $qryobj->addField("grp.$groupparentfield");
                $qryobj->addJoin($grouptable, 'grp', "usergroup.$levelfield = grp.$groupfield", true);
            }
            $query = $qryobj->buildSelect();
        }
        $recs = $db->getRows($query);

        return $recs;
    }

    public function getUser(&$user)
    {
        $groupfield = Config::getGlobal('auth_groupfield');
        $groupparentfield = Config::getGlobal('auth_groupparentfield');
        $user = $_SERVER['PHP_AUTH_USER'];

        $recs = $this->selectUser($user);
        $groups = array();
        $level = array();
        $parents = array();

        // We might have more then one level, so we loop the result.
        if (count($recs) > 0) {
            for ($i = 0; $i < count($recs); ++$i) {
                $level[] = $recs[$i][Config::getGlobal('auth_levelfield')];
                $groups[] = $recs[$i][$groupfield];

                if (!empty($groupparentfield) && $recs[$i][$groupparentfield] != '') {
                    $parents[] = $recs[$i][$groupparentfield];
                }
            }

            $groups = array_merge($groups, $parents);
            while (count($parents) > 0) {
                $precs = $this->getParentGroups($parents);
                $parents = array();
                foreach ($precs as $prec) {
                    if ($prec[$groupparentfield] != '') {
                        $parents[] = $prec[$groupparentfield];
                    }
                }

                $groups = array_merge($groups, $parents);
            }

            $groups = array_unique($groups);
        }
        if (count($level) == 1) {
            $level = $level[0];
        }

        $userinfo = $recs[0];
        $userinfo['name'] = $user;
        $userinfo['level'] = $level; // deprecated. But present for backwardcompatibility.
        $userinfo['groups'] = $groups;
        $userinfo['access_level'] = $this->getAccessLevel($recs);

        return $userinfo;
    }
}
