<?php

namespace Sintattica\Atk\Attributes;

/**
 * Attribute for keeping track of record creation times.
 *
 * The atkCreateStampAttribute class can be used to automatically store the
 * date and time of the creation of a record.
 * To use this attribute, add a DATETIME field to your table and add this
 * attribute to your node. No params are necessary, no initial_values need
 * to be set. The timestamps are generated automatically.
 * This attribute is automatically set to readonly, and to af_hide_add
 * (because we only have the timestamp AFTER a record is added).
 *
 * (the attribute was posted at www.achievo.org/forum/viewtopic.php?p=8608)
 *
 * @author Rich Kucera <kucerar@hhmi.org>
 */
class CreateStampAttribute extends UpdateStampAttribute
{
    
    /**
     * This function is called by the framework to determine if the attribute
     * needs to be saved to the database in an updateDb call.
     * This attribute should never be updated.
     *
     * @param array $record The record that is going to be saved.
     *
     * @return bool True if this attribute should participate in the update
     *              query; false if not.
     */
    public function needsUpdate($record)
    {
        // no matter what, we NEVER save a new value.
        return false;
    }
}
