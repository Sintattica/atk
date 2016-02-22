<?php namespace Sintattica\Atk\Attributes;


/**
 * This attribute can be used to automatically store the user that inserted
 * a record.
 *
 * @author Yury Golovnya <ygolovnya@ccenter.utel.com.ua>
 * @package atk
 * @subpackage attributes
 *
 */
class CreatedByAttribute extends UpdatedByAttribute
{

    /**
     * Constructor.
     *
     * @param string $name Name of the field
     * @param int $flags Flags for this attribute.
     * @return CreatedByAttribute
     */
    function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags);
    }

    /**
     * needsUpdate always returns false for this attribute.
     * @return false
     */
    function needsUpdate()
    {
        return false;
    }

}

