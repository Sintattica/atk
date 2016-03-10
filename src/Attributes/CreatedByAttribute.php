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
     *
     * @return false
     */
    public function needsUpdate()
    {
        return false;
    }
}
