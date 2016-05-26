<?php

namespace Sintattica\Atk\Attributes;

/**
 * The RadioAttribute class represents an attribute of a node
 * that has a field with radio button  to select from predefined values.
 *
 * This attribute is almost identical to atkListAttribute,
 * with some slight modification to show radiobuttons instead of a listbox
 *
 * @author Rene Bakx <rene@ibuildings.nl>
 */
class RadioAttribute extends ListAttribute
{
    /**
     * Flag(s) specific for atkRadioAttribute.
     */
    /**
     * Displays the set of radio buttons vertically.
     */
    const AF_DISPLAY_VERT = 67108864;

    // Default number of cols / rows
    public $m_amount = 1;
    public $m_cols = false;
    public $m_rows = false;
    public $m_clickableLabel = true;

    /*
     * Array with comments per option
     *
     * @var array
     */
    public $m_comments = [];
    public $m_onchangehandler_init = "var newvalue = el.value;\n";

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param array $optionArray Array with options
     * @param array $valueArray Array with values. If you don't use this parameter, values are assumed to be the same as the options.
     */
    public function __construct($name, $flags = 0, $optionArray, $valueArray = null)
    {
        // Default options
        if ($this->hasFlag(self::AF_DISPLAY_VERT)) {
            $this->m_rows = true;
        } else {
            $this->m_cols = true;
        }
        $this->m_amount = count($optionArray);

        parent::__construct($name, $flags, $optionArray, $valueArray);
    }

    /**
     * Set comment for a specific option.
     *
     * @param string $option The option the comment is for
     * @param string $comment The comment itself
     */
    public function setComment($option, $comment)
    {
        $key = array_search($option, $this->m_options);
        $this->m_comments[$key] = $comment;
    }

    /**
     * Set clickablelabel for the radioattribute.
     *
     * @param bool $label
     */
    public function setClickableLabel($label = true)
    {
        $this->m_clickableLabel = $label;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record Array with fields
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string piece of html code with radioboxes
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $values = $this->getValues();

        $total_items = count($values);
        $items = 0;
        if ($this->m_cols && !$this->hasFlag(self::AF_DISPLAY_VERT)) {
            $items = $this->m_amount;
        } elseif ($this->m_rows && !$this->hasFlag(self::AF_DISPLAY_VERT)) {
            $items = ceil($total_items / $this->m_amount);
        } elseif ($this->m_cols && $this->hasFlag(self::AF_DISPLAY_VERT)) {
            $items = ceil($total_items / $this->m_amount);
            $total_items = $items;
        } elseif ($this->m_rows && $this->hasFlag(self::AF_DISPLAY_VERT)) {
            $items = $this->m_amount;
            $total_items = $this->m_amount;
        }

        $result = '<table class="table">';
        if (!$this->hasFlag(self::AF_DISPLAY_VERT)) {
            $result .= '<tr>';
        }
        $item_count = 0;

        for ($i = 0; $i < $total_items; ++$i) {
            if ($values[$i] == $record[$this->fieldName()] && $record[$this->fieldName()] !== '') {
                $sel = 'checked';
            } else {
                $sel = '';
            }

            $labelID = $fieldprefix.$this->fieldName().'_'.$values[$i];
            if ($this->hasFlag(self::AF_DISPLAY_VERT)) {
                $result .= '<tr>';
            }
            $id = $this->getHtmlId($fieldprefix);

            $onchange = '';
            if (count($this->m_onchangecode)) {
                $onchange = 'onClick="'.$id.'_onChange(this);" ';
                $this->_renderChangeHandler($fieldprefix);
            }

            $comment = isset($this->m_comments[$i])?$this->m_comments[$i]:'';

            $commenthtml = '<br/><div class="atkradio_comment">'.$comment.'</div>';

            $result .= '<td><input id="'.$labelID.'" type="radio" name="'.$fieldprefix.$this->fieldName().'" '.$this->getCSSClassAttribute('atkradio').' value="'.$values[$i].'" '.$onchange.$sel.'>
        '.$this->renderValue($labelID, $this->_translateValue($values[$i],
                    $record)).($this->hasFlag(self::AF_DISPLAY_VERT) && $comment != '' ? $commenthtml : '').'</td>';

            if ($this->hasFlag(self::AF_DISPLAY_VERT)) {
                if ($this->hasFlag(self::AF_DISPLAY_VERT) && $this->m_rows) {
                    $tmp_items = count($values);
                } else {
                    $tmp_items = $items * $this->m_amount;
                }

                for ($j = ($items + $i); $j < $tmp_items; $j = $j + $items) {
                    if ($this->m_values[$j] == $record[$this->fieldName()] && $record[$this->fieldName()] != '') {
                        $sel = 'checked';
                    } else {
                        $sel = '';
                    }
                    if ($values[$j] != '') {
                        $result .= '<td><input id="'.$labelID.'" type="radio" name="'.$fieldprefix.$this->fieldName().'" '.$this->getCSSClassAttribute('atkradio').' value="'.$values[$j].'" '.$onchange.$sel.'>
              '.$this->renderValue($labelID, $this->_translateValue($values[$j], $record)).($comment != '' ? $commenthtml : '').'</td>';
                    } else {
                        $result .= '<td>&nbsp;</td>';
                    }
                }
                $result .= '</tr>';
            }

            ++$item_count;
            if ($item_count == $items && !$this->hasFlag(self::AF_DISPLAY_VERT)) {
                $result .= '</tr><tr>';
                $item_count = 0;
            }
        }
        // Fill with empty boxes when we have a horizontal display
        if (!$this->hasFlag(self::AF_DISPLAY_VERT)) {
            for ($i = 0; $i < ($items - $item_count); ++$i) {
                $result .= '<td>&nbsp;</td>';
            }
            $result .= '</tr>';
        }
        $result .= '</table>';

        return $result;
    }

    /**
     * Render value with or without a clickable label.
     *
     * @param string $labelID Label ID
     * @param string $value Label value
     *
     * @return string Label
     */
    public function renderValue($labelID, $value)
    {
        if ($this->m_clickableLabel) {
            return '<label for="'.$labelID.'">'.$value.'</label>';
        }

        return $value;
    }
}
