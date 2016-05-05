<?php

namespace Sintattica\Atk\Attributes;

/**
 * This attribute can be used to automatically store the user that inserted
 * a record.
 *
 * @author Yury Golovnya <ygolovnya@ccenter.utel.com.ua>
 */
class CreatedByAttribute extends UpdatedByAttribute
{
    /**
     * needsUpdate always returns false for this attribute.
     * @param array $record The record that is going to be saved.
     * @return false
     */
    public function needsUpdate($record)
    {
        return false;
    }
}
