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
    const FORCE_ADMIN = 'forceadministrator'; // use it to force creation user null

    public function __construct($name, $flags = 0)
    {
        $flags = $flags | self::AF_READONLY | self::AF_HIDE_ADD | self::AF_LARGE;
        parent::__construct($name, $flags, Config::getGlobal('auth_usernode'));
        $this->setForceInsert(true);
        $this->setForceUpdate(true);

        $this->setNoneLabel($this->text('system'));
    }

    /**
     * This method is overridden to make sure that when a form is posted ('save' button), the
     * current record is refreshed so the output on screen is accurate.
     *
     * @return array Array with userinfo, or "" if no user is logged in.
     */
    public function initialValue()
    {
        $fakeRecord = [$this->fieldName() => SecurityManager::atkGetUser()];
        $this->populate($fakeRecord);

        return $fakeRecord[$this->fieldName()];
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record = [], $level = 0, $mode = '')
    {
        if ($mode == 'add' || $mode == 'update') {
            $query->addField($this->fieldName(), $this->value2db($record), '', '', !$this->hasFlag(self::AF_NO_QUOTES), true);
        } else {
            parent::addToQuery($query, $tablename, $fieldaliasprefix, $record, $level, $mode);
        }
    }

    public function display(array $record, string $mode): string
    {
        $value = parent::display($record, $mode);
        if (!$value && $mode == 'list') {
            // in list, if value is null, will be shown "system"
            $value = $this->getNoneLabel($mode);
        }
        return $value;
    }

    public function value2db(array $record)
    {
        if (!$record[$this->fieldName()]) { // only if it has no value
            $record[$this->fieldName()] = $this->initialValue();
        } elseif ($record[$this->fieldName()] == self::FORCE_ADMIN) { // force "system" user
            $record[$this->fieldName()] = null;
        }

        return parent::value2db($record);
    }
}
