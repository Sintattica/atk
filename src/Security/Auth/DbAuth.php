<?php

namespace Sintattica\Atk\Security\Auth;

use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Utils\StringParser;

/**
 * Driver for authentication and authorization using tables in the database.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class DbAuth extends AuthInterface
{
    public $m_rightscache = [];

    /**
     * Build the query for selecting the user for authentication.
     *
     * @param string $user
     * @param string $usertable
     * @param string $userfield
     * @param string $passwordfield
     * @param string $accountdisablefield
     * @param string $accountenbleexpression
     *
     * @return string which contains the query
     */
    public function buildSelectUserQuery(
        $usertable,
        $userfield,
        $passwordfield,
        $accountdisablefield = null,
        $accountenbleexpression = null
    ) {
        $disableexpr = '';
        if ($accountdisablefield) {
            $disableexpr = ', '.Db::quoteIdentifier($accountdisablefield);
        }
        $query = 'SELECT '.Db::quoteIdentifier($passwordfield)." $disableexpr FROM ".Db::QuoteIdentifier($usertable).
            ' WHERE '.Db::quoteIdentifier($userfield)." =:user";
        if ($accountenbleexpression) {
            $query .= " AND $accountenbleexpression";
        }

        return $query;
    }

    /**
     * Authenticate a user.
     *
     * @param string $user The login of the user to authenticate.
     * @param string $passwd The password of the user. Note: if the canMd5
     *                       function of an implementation returns true,
     *                       $passwd will be passed as an md5 string.
     *
     * @return int SecurityManager::AUTH_SUCCESS - Authentication succesful
     *             SecurityManager::AUTH_MISMATCH - Authentication failed, wrong
     *             user/password combination
     *             SecurityManager::AUTH_LOCKED - Account is locked, can not login
     *             with current username.
     *             SecurityManager::AUTH_ERROR - Authentication failed due to some
     *             error which cannot be solved by
     *             just trying again. If you return
     *             this value, you *must* also
     *             fill the m_fatalError variable.
     */
    public function validateUser($user, $passwd)
    {
        if ($user == '') {
            return SecurityManager::AUTH_UNVERIFIED;
        } // can't verify if we have no userid

        $db = Db::getInstance(Config::getGlobal('auth_database'));
        $query = $this->buildSelectUserQuery(Config::getGlobal('auth_usertable'), Config::getGlobal('auth_userfield'),
            Config::getGlobal('auth_passwordfield'), Config::getGlobal('auth_accountdisablefield'), Config::getGlobal('auth_accountenableexpression'));
        $recs = $db->getRows($query, [':user' => $user]);
        if (Tools::count($recs) > 0 && $this->isLocked($recs[0])) {
            return SecurityManager::AUTH_LOCKED;
        }

        $matchPassword = $this->matchPasswords($this->getPassword(isset($recs[0]) ? $recs[0] : null), $passwd);

        return (Tools::count($recs) > 0 && $user != '' && $matchPassword) ? SecurityManager::AUTH_SUCCESS : SecurityManager::AUTH_MISMATCH;
    }

    public function isValidUser($user)
    {
        $db = Db::getInstance(Config::getGlobal('auth_database'));
        $usertable = Db::quoteIdentifier(Config::getGlobal('auth_usertable'));
        $userfield = Db::quoteIdentifier(Config::getGlobal('auth_userfield'));
        $disablefield = Db::quoteIdentifier(Config::getGlobal('auth_accountdisablefield'));
        $enableexpression = Config::getGlobal('auth_accountenableexpression');
        $sql = "SELECT COUNT(*) AS cnt FROM {$usertable} WHERE {$userfield} = :user";


        if ($disablefield) {
            $sql .= " AND NOT($disablefield)";
        }
        if ($enableexpression) {
            $sql .= " AND $enableexpression";
        }

        if ($db->getValue($sql, [':user' => $user])) {
            return true;
        };

        return false;
    }

    /**
     * returns the users password from record.
     *
     * @param array $rec record from database
     *
     * @return mixed userspw
     */
    public function getPassword($rec)
    {
        return (isset($rec[Config::getGlobal('auth_passwordfield')])) ? $rec[Config::getGlobal('auth_passwordfield')] : false;
    }

    /**
     * checks wether the useraccount is locked.
     *
     * @param array $rec record from db
     *
     * @return bool true in case of a locked account
     */
    public function isLocked($rec)
    {
        return isset($rec[Config::getGlobal('auth_accountdisablefield')]) && $rec[Config::getGlobal('auth_accountdisablefield')] == 1;
    }

    /**
     * Match 2 passwords
     *
     * @param string $dbpasswd The password from the database
     * @param string $userpasswd The password the user provided
     *
     * @return bool which indicates if the passwords are equal
     */
    public function matchPasswords($dbpasswd, $userpasswd)
    {
        if (Config::getGlobal('auth_ignorepasswordmatch')) {
            return true;
        }

        if (Config::getGlobal('auth_usecryptedpassword', false)) {
            return password_verify($userpasswd, $dbpasswd);
        } else {
            // regular match
            return $dbpasswd === $userpasswd;
        }
    }

    /**
     * Select the user record from the database.
     *
     * @param string $user
     *
     * @return array with user information
     */
    public function selectUser($user)
    {
        $usertable = Config::getGlobal('auth_usertable');
        $userfield = Config::getGlobal('auth_userfield');
        $leveltable = Config::getGlobal('auth_leveltable');
        $levelfield = Config::getGlobal('auth_levelfield');
        $userpk = Config::getGlobal('auth_userpk');
        $userfk = Config::getGlobal('auth_userfk', $userpk);
        $grouptable = Config::getGlobal('auth_grouptable');
        $groupfield = Config::getGlobal('auth_groupfield');
        $groupparentfield = Config::getGlobal('auth_groupparentfield');
        $accountenableexpression = Config::getGlobal('auth_accountenableexpression');

        $db = Db::getInstance(Config::getGlobal('auth_database'));
        $query = $db->createQuery($usertable);
        $query->addAllFields()
            ->addCondition(Db::quoteIdentifier($usertable, $userfield).'=:user', [':user' => $user]);
        if ($usertable != $leveltable && $leveltable != '') {
            // Level and userid are stored in two separate tables. This could
            // mean (but doesn't have to) that a user can have more than one
            // level.
            $query->addJoin($leveltable, 'usergroup', [$userfk => [$usertable, $userpk]], true);
            $query->addField($levelfield, '', 'usergroup');

            if (!empty($groupparentfield)) {
                $query->addJoin($grouptable, 'grp', [$groupfield => ['usergroup', $levelfield]], true);
                $query->addField("grp.$groupparentfield");
            }
        }

        if ($accountenableexpression) {
            $query->addCondition(" AND $accountenableexpression");
        }
        return $query->executeSelect();
    }

    /**
     * Get the parent groups.
     *
     * @param array $parents
     *
     * @return array with records of the parent groups
     */
    public function getParentGroups($parents)
    {
        $db = Db::getInstance(Config::getGlobal('auth_database'));

        $grouptable = Config::getGlobal('auth_grouptable');
        $groupfield = Config::getGlobal('auth_groupfield');
        $groupparentfield = Config::getGlobal('auth_groupparentfield');

        $query = $db->createQuery($grouptable);
        $query->addField($groupparentfield);
        $query->addCondition("$grouptable.$groupfield IN (".implode(',', $parents).')');

        return $query->executeSelect(true);
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
     * the user table.
     *
     * @param string $user The login of the user to retrieve.
     *
     * @return array|null Information about a user.
     */
    public function getUser($user)
    {
        $groupparentfield = Config::getGlobal('auth_groupparentfield');

        $recs = $this->selectUser($user);
        $groups = [];
        $level = [];
        $parents = [];

        // user not found
        if (Tools::count($recs) == 0) {
            return null;
        }

        // We might have more then one level, so we loop the result.
        if (Tools::count($recs) > 0) {
            for ($i = 0; $i < Tools::count($recs); ++$i) {
                $level[] = $recs[$i][Config::getGlobal('auth_levelfield')];
                $groups[] = $recs[$i][Config::getGlobal('auth_levelfield')];

                if (!empty($groupparentfield) && $recs[$i][$groupparentfield] != '') {
                    $parents[] = $recs[$i][$groupparentfield];
                }
            }

            $groups = array_merge($groups, $parents);
            while (Tools::count($parents) > 0) {
                $precs = $this->getParentGroups($parents);
                $parents = [];
                foreach ($precs as $prec) {
                    if ($prec[$groupparentfield] != '') {
                        $parents[] = $prec[$groupparentfield];
                    }
                }

                $groups = array_merge($groups, $parents);
            }

            $groups = array_unique($groups);
        }
        if (Tools::count($level) == 1) {
            $level = $level[0];
        }

        $userinfo = $recs[0];
        $userinfo['name'] = $user;
        $userinfo['level'] = $level; // deprecated. But present for backwardcompatibility.
        $userinfo['groups'] = $groups;
        $userinfo['access_level'] = $this->getAccessLevel($recs);

        return $userinfo;
    }

    /**
     * Get the access level from the user.
     *
     * @param array $recs The records that are returned by the selectUser function
     *
     * @return int the access level
     */
    public function getAccessLevel($recs)
    {
        $access = 0;
        $levelField = Config::getGlobal('auth_accesslevelfield');
        if($levelField === null) {
            return $access;
        }

        // We might have more then one access level, so we loop the result.
        for ($i = 0; $i < Tools::count($recs); $i++) {
            if (isset($recs[$i][$levelField]) && $recs[$i][$levelField] > $access) {
                $access = $recs[$i][$levelField];
            }
        }

        return $access;
    }

    /**
     * This function returns the level/group(s) that are allowed to perform
     * the given action on a node.
     *
     * @param string $node The full nodename of the node for which to check
     *                       the privilege. (modulename.nodename)
     * @param string $action The privilege to check.
     *
     * @return array entities that are allowed to perform the action.
     */
    public function getEntity($node, $action)
    {

        if (!isset($this->m_rightscache[$node]) || Tools::count($this->m_rightscache[$node]) == 0) {
            $db = Db::getInstance(Config::getGlobal('auth_database'));
            $accessTable = Config::getGlobal('auth_accesstable');
            $query = $db->createQuery($accessTable);
            $field = Config::getGlobal('auth_accessfield');
            if (empty($field)) {
                $field = Config::getGlobal('auth_levelfield');
            }
            $query->addExpression('entity', Db::quoteIdentifier($field));
            $query->addField('action');

            $query->addCondition($query->simpleValueCondition($accessTable, 'node', $node));

            $this->m_rightscache[$node] = $query->executeSelect();
        }

        $result = [];

        $rights = $this->m_rightscache[$node];

        foreach($rights as $right) {
            if ($right['action'] == $action) {
                $result[] = $right['entity'];
            }
        }

        return $result;
    }

    /**
     * This function returns the level/group(s) that are allowed to
     * view/edit a certain attribute of a given node.
     *
     * @param string $node The full nodename of the node for which to check
     *                       attribute access.
     * @param string $attrib The name of the attribute to check
     * @param string $mode "view" or "edit"
     *
     * @return array entities that are allowed to perform the action.
     */
    public function getAttribEntity($node, $attrib, $mode)
    {
        $db = Db::getInstance(Config::getGlobal('auth_database'));

        $query = $db->createQuery('attribaccess');
        $field = Config::getGlobal('auth_accessfield') ?? Config::getGlobal('auth_levelfield');
        $query->addExpression('entity', $field);
        $query->addCondition($query->simpleValueCondition('attribacess', 'node', $node));
        $query->addCondition($query->simpleValueCondition('attribacess', 'attribute', $attrib));
        $query->addCondition($query->simpleValueCondition('attribacess', 'mode', $mode));
        $query->addCondition(Db::quoteIdentifier($field)." != ''");

        $rights = $db->getRows($query);

        $result = [];
        foreach ($query->executeSelect() as $row) {
            $result[] = $row[$field];
        }

        return $result;
    }

    /**
     * Compare function for sorting users on their username.
     *
     * @param array $a
     * @param array $b
     *
     * @return bool
     */
    public function userListCompare($a, $b)
    {
        return strcmp($a['username'], $b['username']);
    }

    /**
     * This function returns the list of users that may login. This can be
     * used to display a dropdown of users from which to choose.
     *
     * @return array List of users as an associative array with the following
     *               format: array of records, each record is an associative
     *               array with a userid and a username field.
     */
    public function getUserList()
    {
        $stringparser = new StringParser(Config::getGlobal('auth_userdescriptor'));
        $db = Db::getInstance(Config::getGlobal('auth_database'));
        $query = $db->createQuery(Config::getGlobal('auth_usertable'));
        $query->addField(Config::getGlobal('auth_userfield'));
        foreach($stringparser->getFields() as $field) {
            $query->addField($field);
        }

        $accountdisablefield = Config::getGlobal('auth_accountdisablefield');
        $accountenableexpression = Config::getGlobal('auth_accountenableexpression');
        if ($accountenableexpression != '') {
            $query->addCondition($accountenableexpression);
        }
        if ($accountdisablefield != '') {
            $query->addCondition('!'.Db::quoteIdentifier($accountdisablefield));
        }

        $userlist = [];
        foreach ($query->executeSelect() as $row) {
            $userList = [
                'userid' => $row[Config::getGlobal('auth_userfield')],
                'username' => $stringparser->parse($row),
            ];
        }
        usort($userlist, array('auth_db', 'userListCompare'));

        return $userlist;
    }

    /**
     * This function returns "get password" policy for current auth method.
     *
     * @return int const
     */
    public function getPasswordPolicy()
    {
        return self::PASSWORD_RETRIEVABLE;
    }

    /**
     * This function returns password or false, if password can't be retrieve/recreate.
     *
     * @param string $username User for which the password should be regenerated
     *
     * @return mixed string with password or false
     */
    public function gettingPassword($username)
    {
        // Query the database for user records having the given username and return if not found
        $atk = Atk::getInstance();
        $usernode = $atk->atkGetNode(Config::getGlobal('auth_usernode'));
        $selector = Query::simpleValueCondition(Config::getGlobal('auth_usertable'), Config::getGlobal('auth_userfield'), $username);
        $userrecords = $usernode->select($selector)->mode('edit')->includes(array(
            Config::getGlobal('auth_userpk'),
            Config::getGlobal('auth_emailfield'),
            Config::getGlobal('auth_passwordfield'),
        ))->fetchAll();
        if (Tools::count($userrecords) != 1) {
            Tools::atkdebug("User '$username' not found.");

            return false;
        }

        // Retrieve the email address
        $email = $userrecords[0][Config::getGlobal('auth_emailfield')];
        if (empty($email)) {
            Tools::atkdebug("Email address for '$username' not available.");

            return false;
        }

        // Regenerate the password
        /** @var \Sintattica\Atk\Attributes\PasswordAttribute $passwordattr */
        $passwordattr = $usernode->getAttribute(Config::getGlobal('auth_passwordfield'));
        $newpassword = $passwordattr->generatePassword();

        // Update the record in the database
        $userrecords[0][Config::getGlobal('auth_passwordfield')]['hash'] = password_hash($newpassword, PASSWORD_DEFAULT);
        $usernode->updateDb($userrecords[0], true, '', array(Config::getGlobal('auth_passwordfield')));

        $usernode->getDb()->commit();

        // Return true
        return $newpassword;
    }
}
