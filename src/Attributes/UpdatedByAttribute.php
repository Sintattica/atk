<?php namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Relations\ManyToOneRelation;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Security\SecurityManager;

/**
 * This attribute can be used to automatically store the user that inserted
 * or last modified a record.
 *
 * Note that this attribute relies on the config value $config_auth_usernode.
 * If you use this attribute, be sure to set it in your config.inc.php file.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage attributes
 *
 */
class UpdatedByAttribute extends ManyToOneRelation
{

    /**
     * Constructor.
     *
     * @param string $name Name of the field
     * @param int $flags Flags for this attribute.
     * @return UpdatedByAttribute
     */
    function __construct($name, $flags = 0)
    {
        parent::__construct($name, Config::getGlobal("auth_usernode"), $flags | self::AF_READONLY | self::AF_HIDE_ADD);
        $this->setForceInsert(true);
        $this->setForceUpdate(true);
    }

    function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        if ($mode == 'add' || $mode == 'update') {
            Attribute::addToQuery($query, $tablename, $fieldaliasprefix, $record, $level, $mode);
        } else {
            parent::addToQuery($query, $tablename, $fieldaliasprefix, $record, $level, $mode);
        }
    }

    /**
     * This method is overriden to make sure that when a form is posted ('save' button), the
     * current record is refreshed so the output on screen is accurate.
     *
     * @return array Array with userinfo, or "" if no user is logged in.
     */
    function initialValue()
    {
        $fakeRecord = array($this->fieldName() => SecurityManager::atkGetUser());
        $this->populate($fakeRecord);
        return $fakeRecord[$this->fieldName()];
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * @param array $record The record that holds this attribute's value.
     * @return String The database compatible value
     */
    function value2db($record)
    {
        $record[$this->fieldName()] = $this->initialValue();
        return parent::value2db($record);
    }

}

