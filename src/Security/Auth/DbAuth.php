<?php namespace Sintattica\Atk\Security\Auth;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Utils\StringParser;
use Sintattica\Atk\Core\Module;
use Sintattica\Atk\Security\SecurityManager;

/**
 * Driver for authentication and authorization using tables in the database.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage security
 *
 */
class DbAuth extends AuthInterface
{
    var $m_rightscache = array();

    /**
     * Build the query for selecting the user for authentication
     *
     * @param string $user
     * @param string $usertable
     * @param string $userfield
     * @param string $passwordfield
     * @param string $accountdisablefield
     * @param string $accountenbleexpression
     * @return string which contains the query
     */
    function buildSelectUserQuery(
        $user,
        $usertable,
        $userfield,
        $passwordfield,
        $accountdisablefield = null,
        $accountenbleexpression = null
    ) {
        $disableexpr = "";
        if ($accountdisablefield) {
            $disableexpr = ", $accountdisablefield";
        }
        $query = "SELECT $passwordfield $disableexpr FROM $usertable WHERE $userfield ='$user'";
        if ($accountenbleexpression) {
            $query .= " AND $accountenbleexpression";
        }
        return $query;
    }

    /**
     * Authenticate a user.
     *
     * @param String $user The login of the user to authenticate.
     * @param String $passwd The password of the user. Note: if the canMd5
     *                       function of an implementation returns true,
     *                       $passwd will be passed as an md5 string.
     *
     * @return int SecurityManager::AUTH_SUCCESS - Authentication succesful
     *             SecurityManager::AUTH_MISMATCH - Authentication failed, wrong
     *                             user/password combination
     *             SecurityManager::AUTH_LOCKED - Account is locked, can not login
     *                           with current username.
     *             SecurityManager::AUTH_ERROR - Authentication failed due to some
     *                          error which cannot be solved by
     *                          just trying again. If you return
     *                          this value, you *must* also
     *                          fill the m_fatalError variable.
     */
    function validateUser($user, $passwd)
    {
        if ($user == "") {
            return SecurityManager::AUTH_UNVERIFIED;
        } // can't verify if we have no userid

        $db = Tools::atkGetDb(Config::getGlobal("auth_database"));
        $query = $this->buildSelectUserQuery($db->escapeSql($user), Config::getGlobal("auth_usertable"),
            Config::getGlobal("auth_userfield"), Config::getGlobal("auth_passwordfield"),
            Config::getGlobal("auth_accountdisablefield"), Config::getGlobal("auth_accountenableexpression"));
        $recs = $db->getrows($query);
        if (count($recs) > 0 && $this->isLocked($recs[0])) {
            return SecurityManager::AUTH_LOCKED;
        }

        return ((count($recs) > 0 && $user != "" && $this->matchPasswords($this->getPassword($recs[0]), $passwd))
            ? SecurityManager::AUTH_SUCCESS : SecurityManager::AUTH_MISMATCH);
    }

    /**
     * returns the users password from record
     *
     * @param array $rec record from database
     * @return mixed userspw
     */
    function getPassword($rec)
    {
        return (isset($rec[Config::getGlobal("auth_passwordfield")])) ? $rec[Config::getGlobal("auth_passwordfield")]
            : false;
    }

    /**
     * checks wether the useraccount is locked
     *
     * @param array $rec record from db
     * @return bool true in case of a locked account
     */
    function isLocked($rec)
    {
        return (isset($rec[Config::getGlobal("auth_accountdisablefield")]) && $rec[Config::getGlobal("auth_accountdisablefield")] == 1);
    }

    /**
     * Match 2 passwords.
     * In normal situations, $dbpassword and $userpasswd are considered equal
     * if they are a case-insensitive match. When $config_auth_cryptedpassword
     * is true, they are only considered a match if $dbpassword is equal to the
     * crypt() of $userpasswd, where $dbpassword itself is used as the 'salt'.
     * (This method is used by Bugzilla, among other apps)
     *
     * @param string $dbpasswd The password from the database
     * @param string $userpasswd The password the user provided
     * @return boolean which indicates if the passwords are equal
     */
    function matchPasswords($dbpasswd, $userpasswd)
    {
        // crypt password method, like in bugzilla
        if (Config::getGlobal("auth_usecryptedpassword", false)) {
            // password is stored using the crypt method, using the cryptedpassword itself as the salt.
            return (crypt($userpasswd, $dbpasswd) == $dbpasswd);
        } else {
            // regular match, perhaps with md5.
            return (strtoupper($dbpasswd) == strtoupper($userpasswd));
        }
    }

    /**
     * Does the authentication method support md5 encoding of passwords?
     *
     * @return boolean True if md5 is always used. false if md5 is not
     *                 supported.
     *                 Drivers that support both md5 and cleartext passwords
     *                 can return Config::getGlobal("authentication_md5") to let the
     *                 application decide whether to use md5.
     */
    function canMd5()
    {
        return Config::getGlobal("authentication_md5");
    }

    /**
     * Select the user record from the database
     *
     * @param string $user
     * @return array with user information
     */
    function selectUser($user)
    {
        $usertable = Config::getGlobal("auth_usertable");
        $userfield = Config::getGlobal("auth_userfield");
        $leveltable = Config::getGlobal("auth_leveltable");
        $levelfield = Config::getGlobal("auth_levelfield");
        $userpk = Config::getGlobal("auth_userpk");
        $userfk = Config::getGlobal("auth_userfk", $userpk);
        $grouptable = Config::getGlobal("auth_grouptable");
        $groupfield = Config::getGlobal("auth_groupfield");
        $groupparentfield = Config::getGlobal("auth_groupparentfield");
        $accountenableexpression = Config::getGlobal("auth_accountenableexpression");

        $db = Tools::atkGetDb(Config::getGlobal("auth_database"));
        if ($usertable == $leveltable || $leveltable == "") {
            // Level and userid are stored in the same table.
            // This means one user can only have one level.
            $query = "SELECT * FROM $usertable WHERE $userfield ='$user'";
        } else {
            // Level and userid are stored in two separate tables. This could
            // mean (but doesn't have to) that a user can have more than one
            // level.
            $qryobj = &$db->createQuery();
            $qryobj->addTable($usertable);
            $qryobj->addField("$usertable.*");
            $qryobj->addField("usergroup.*");
            $qryobj->addJoin($leveltable, "usergroup", "$usertable.$userpk = usergroup.$userfk", true);
            $qryobj->addCondition("$usertable.$userfield = '$user'");

            if (!empty($groupparentfield)) {
                $qryobj->addField("grp.$groupparentfield");
                $qryobj->addJoin($grouptable, "grp", "usergroup.$levelfield = grp.$groupfield", true);
            }
            $query = $qryobj->buildSelect();
        }

        if ($accountenableexpression) {
            $query .= " AND $accountenableexpression";
        }
        $recs = $db->getrows($query);
        return $recs;
    }

    /**
     * Get the parent groups
     *
     * @param array $parents
     * @return array with records of the parent groups
     */
    function getParentGroups($parents)
    {
        $db = Tools::atkGetDb(Config::getGlobal("auth_database"));

        $grouptable = Config::getGlobal("auth_grouptable");
        $groupfield = Config::getGlobal("auth_groupfield");
        $groupparentfield = Config::getGlobal("auth_groupparentfield");

        $query = &$db->createQuery();
        $query->addField($groupparentfield);
        $query->addTable($grouptable);
        $query->addCondition("$grouptable.$groupfield IN (" . implode(',', $parents) . ")");
        $recs = $db->getrows($query->buildSelect(true));
        return $recs;
    }

    /**
     * This function returns information about a user in an associative
     * array with the following elements (minimal):
     * "name" -> the userid (should normally be the same as the $user
     *           variable that gets passed to it.
     * "level" -> The level/group(s) to which this user belongs.
     * "groups" -> The groups this user belongs to
     * "access_level" -> The user's access level
     * The other elemens of the returning array depend on the structure of
     * the user table
     *
     * @param String $user The login of the user to retrieve.
     * @return array Information about a user.
     */
    function getUser($user)
    {
        $groupparentfield = Config::getGlobal("auth_groupparentfield");

        $recs = $this->selectUser($user);
        $groups = array();
        $level = array();
        $parents = array();

        // We might have more then one level, so we loop the result.
        if (count($recs) > 0) {


            for ($i = 0; $i < count($recs); $i++) {
                $level[] = $recs[$i][Config::getGlobal("auth_levelfield")];
                $groups[] = $recs[$i][Config::getGlobal("auth_levelfield")];

                if (!empty($groupparentfield) && $recs[$i][$groupparentfield] != "") {
                    $parents[] = $recs[$i][$groupparentfield];
                }
            }

            $groups = array_merge($groups, $parents);
            while (count($parents) > 0) {
                $precs = $this->getParentGroups($parents);
                $parents = array();
                foreach ($precs as $prec) {
                    if ($prec[$groupparentfield] != "") {
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
        $userinfo["name"] = $user;
        $userinfo["level"] = $level; // deprecated. But present for backwardcompatibility.
        $userinfo["groups"] = $groups;
        $userinfo["access_level"] = $this->getAccessLevel($recs);

        return $userinfo;
    }

    /**
     * Get the access level from the user
     *
     * @param array $recs The records that are returned by the selectUser function
     * @return int the access level
     */
    function getAccessLevel($recs)
    {
        // We might have more then one access level, so we loop the result.
        if (count($recs) > 1) {
            $access = array();
            for ($i = 0; $i < count($recs); $i++) {
                if ($i == 0) {
                    $access = $recs[$i][Config::getGlobal("auth_accesslevelfield")];
                }
                if ($recs[$i][Config::getGlobal("auth_accesslevelfield")] > $access) {
                    $access = $recs[$i][Config::getGlobal("auth_accesslevelfield")];
                }
            }
        } else {
            $access = "";
            if (isset($recs[0][Config::getGlobal("auth_accesslevelfield")])) {
                $access = $recs[0][Config::getGlobal("auth_accesslevelfield")];
            }
        }
        return $access;
    }

    /**
     * This function returns the level/group(s) that are allowed to perform
     * the given action on a node.
     * @param String $node The full nodename of the node for which to check
     *                     the privilege. (modulename.nodename)
     * @param String $action The privilege to check.
     * @return mixed One (int) or more (array) entities that are allowed to
     *               perform the action.
     */
    function getEntity($node, $action)
    {
        $db = Tools::atkGetDb(Config::getGlobal("auth_database"));

        if (!isset($this->m_rightscache[$node]) || count($this->m_rightscache[$node]) == 0) {
            $query = "SELECT * FROM " . Config::getGlobal("auth_accesstable") . " WHERE node='$node'";

            $this->m_rightscache[$node] = $db->getrows($query);
        }

        $result = Array();

        $rights = $this->m_rightscache[$node];

        $field = Config::getGlobal("auth_accessfield");
        if (empty($field)) {
            $field = Config::getGlobal("auth_levelfield");
        }

        for ($i = 0, $_i = count($rights); $i < $_i; $i++) {
            if ($rights[$i]['action'] == $action) {
                $result[] = $rights[$i][$field];
            }
        }

        return $result;
    }

    /**
     * This function returns the level/group(s) that are allowed to
     * view/edit a certain attribute of a given node.
     * @param String $node The full nodename of the node for which to check
     *                     attribute access.
     * @param String $attrib The name of the attribute to check
     * @param String $mode "view" or "edit"
     * @return array
     */
    function getAttribEntity($node, $attrib, $mode)
    {
        $db = Tools::atkGetDb(Config::getGlobal("auth_database"));

        $query = "SELECT * FROM attribaccess WHERE node='$node' AND attribute='$attrib' AND mode='$mode'";

        $rights = $db->getrows($query);

        $result = Array();

        for ($i = 0; $i < count($rights); $i++) {
            if ($rights[$i][Config::getGlobal("auth_levelfield")] != "") {
                $result[] = $rights[$i][Config::getGlobal("auth_levelfield")];
            }
        }

        return $result;
    }

    /**
     * Compare function for sorting users on their username
     *
     * @param array $a
     * @param array $b
     * @return boolean
     */
    function userListCompare($a, $b)
    {
        return strcmp($a["username"], $b["username"]);
    }

    /**
     * This function returns the list of users that may login. This can be
     * used to display a dropdown of users from which to choose.
     *
     * @return array List of users as an associative array with the following
     *               format: array of records, each record is an associative
     *               array with a userid and a username field.
     */
    function getUserList()
    {
        $db = Tools::atkGetDb(Config::getGlobal("auth_database"));
        $query = "SELECT * FROM " . Config::getGlobal("auth_usertable");

        $accountdisablefield = Config::getGlobal("auth_accountdisablefield");
        $accountenableexpression = Config::getGlobal("auth_accountenableexpression");
        if ($accountenableexpression != "") {
            $query .= " WHERE $accountenableexpression";
            if ($accountdisablefield != "") {
                $query .= " AND $accountdisablefield = 0";
            }
        } else {
            if ($accountdisablefield != "") {
                $query .= " WHERE $accountdisablefield = 0";
            }
        }

        $recs = $db->getrows($query);

        $userlist = array();
        $stringparser = new StringParser(Config::getGlobal("auth_userdescriptor"));
        for ($i = 0, $_i = count($recs); $i < $_i; $i++) {
            $userlist[] = array(
                "userid" => $recs[$i][Config::getGlobal("auth_userfield")],
                "username" => $stringparser->parse($recs[$i])
            );
        }
        usort($userlist, array("auth_db", "userListCompare"));
        return $userlist;
    }

    /**
     * This function returns "get password" policy for current auth method
     *
     * @return int const
     */
    function getPasswordPolicy()
    {
        return self::PASSWORD_RETRIEVABLE;
    }

    /**
     * This function returns password or false, if password can't be retrieve/recreate
     *
     * @param string $username User for which the password should be regenerated
     *
     * @return mixed string with password or false
     */
    function gettingPassword($username)
    {
        // Query the database for user records having the given username and return if not found
        $usernode = Module::atkGetNode(Config::getGlobal("auth_usernode"));
        $selector = sprintf("%s.%s = '%s'", Config::getGlobal("auth_usertable"),
            Config::getGlobal("auth_userfield"), $username);
        $userrecords = $usernode->select($selector)->mode('edit')
            ->includes(array(
                Config::getGlobal("auth_userpk"),
                Config::getGlobal("auth_emailfield"),
                Config::getGlobal("auth_passwordfield")
            ))->getAllRows();
        if (count($userrecords) != 1) {
            Tools::atkdebug("User '$username' not found.");
            return false;
        }

        // Retrieve the email address
        $email = $userrecords[0][Config::getGlobal("auth_emailfield")];
        if (empty($email)) {
            Tools::atkdebug("Email address for '$username' not available.");
            return false;
        }

        // Regenerate the password
        /** @var \Sintattica\Atk\Attributes\PasswordAttribute $passwordattr */
        $passwordattr = $usernode->getAttribute(Config::getGlobal("auth_passwordfield"));
        $newpassword = $passwordattr->generatePassword();

        // Update the record in the database
        $userrecords[0][Config::getGlobal("auth_passwordfield")]["hash"] = md5($newpassword);
        $usernode->updateDb($userrecords[0], true, "", array(Config::getGlobal("auth_passwordfield")));

        $usernode->getDb()->commit();

        // Return true
        return $newpassword;
    }

}


