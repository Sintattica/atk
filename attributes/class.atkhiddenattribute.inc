<?php

/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage attributes
 *
 * @copyright (c)2010 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 */

/** 
 * @internal base class include
 */
useattrib("atkdummyattribute");

/**
 * The atkHiddenAttribute behaves very similar to an atkDummyAttribute, but with the main difference
 * being that the attribute is always hidden, and the text passed to the attribute is not displayed
 * visibly but posted as a hidden form value. 
 *
 * @author Ivo Jansch <ivo@ibuildings.com>
 * @package atk
 * @subpackage attributes
 */
class atkHiddenAttribute extends atkDummyAttribute
{
    /** 
     * The atkHiddenAttribute has a custom constructor. It's purpose is to force the AF_HIDE
     * flag, regardless of flags passed. Its behaviour is identical to atkDummyAttribute's 
     * constructor in every other way.
     */
    public function __construct($name, $text="", $flags=0) 
    {
        // A hidden  attrikbute should be... HIDDEN! (srlsy?)
        $flags |= AF_HIDE;

        parent::__construct($name, $text, $flags);
    }
 

    /**
     * This method is called by the framework whenever an attribute needs to be rendered within a hidden form. 
     * In this case, the attribute renders a hidden input field using its text as its hidden value.
     */
    public function hide($record="", $fieldprefix="") 
    {
        $id = $id = $this->getHtmlId($fieldprefix);
        $result = '<input type="hidden" id="'.$id.'" name="'.$fieldprefix.$this->formName().'" value="'.
                       htmlspecialchars($this->m_text).'">';
         return $result;
    }
}
