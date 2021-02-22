<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;

/**
 * Class VersionAttribute
 *
 * Counter of the updates done on the record. Use MySQL trigger to increment this value every updates.
 *
 */
class VersionAttribute extends NumberAttribute
{
    function __construct()
    {
        parent::__construct('__version', self::AF_HIDE ^ self::AF_HIDE_EDIT ^ self::AF_HIDE_VIEW | self::AF_READONLY_EDIT);
        $this->setLabel($this->text('record_version'));
        $this->setForceReload(true);
    }

    function initialValue()
    {
        return 0;
    }

    function validate(&$record, $mode)
    {
        if ($mode == 'update') {
            $sql = sprintf("SELECT %s FROM %s WHERE %s", $this->fieldName(), $this->getOwnerInstance()->getTable(), $this->getOwnerInstance()->getPrimaryKey($record));
            $version = $this->getDb()->getValue($sql);

            if ($version != $record[$this->fieldName()]) {
                Tools::atkTriggerError($record, $this, 'error_record_modified');
            }
        }
    }
}
