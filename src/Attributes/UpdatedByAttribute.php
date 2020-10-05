<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Relations\ManyToOneRelation;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Security\SecurityManager;

/**
 * This attribute can be used to automatically store the user that inserted
 * or last modified a record.
 *
 * Note that this attribute relies on the config value auth_usernode.
 * If you use this attribute, be sure to set it in your config file.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class UpdatedByAttribute extends ManyToOneRelation
{
    /**
     * Constructor.
     *
     * @param string $name Name of the field
     * @param int $flags Flags for this attribute.
     *
     * @return UpdatedByAttribute
     */
    public function __construct($name, $flags = 0)
    {
        $flags = $flags | self::AF_READONLY | self::AF_HIDE_ADD;
        parent::__construct($name, $flags, Config::getGlobal('auth_usernode'));
        $this->setForceInsert(true);
        $this->setForceUpdate(true);
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        if ($mode == 'add' || $mode == 'update') {
            $query->addField($this->fieldName(), $this->value2db($record));
        } else {
            parent::addToQuery($query, $tablename, $fieldaliasprefix, $record, $level, $mode);
        }
    }

    /**
     * This method is overridden to make sure that when a form is posted ('save' button), the
     * current record is refreshed so the output on screen is accurate.
     *
     * @return array Array with userinfo, or "" if no user is logged in.
     */
    public function initialValue()
    {
        $fakeRecord = array($this->fieldName() => SecurityManager::atkGetUser());
        $this->populate($fakeRecord);

        return $fakeRecord[$this->fieldName()];
    }

    public function value2db($record)
    {
        $record[$this->fieldName()] = $this->initialValue();

        return parent::value2db($record);
    }
}
