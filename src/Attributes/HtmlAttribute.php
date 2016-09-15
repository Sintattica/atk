<?php

namespace Sintattica\Atk\Attributes;

/**
 * The HtmlAttribute class is the same as a normal Attribute. It only
 * (has a different display function. For this attribute, the value is
 * rendered as-is, which means you can use html codes in the text.
 *
 * There might me times where you want the user to be able to use html tags,
 * but you don't want to have the inconvenience of using br's for each line.
 * For this reason the constructor accepts a parameter which tells it to do
 * a newline-to-br conversion.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class HtmlAttribute extends TextAttribute
{
    /*
     * New line to BR boolean
     */
    public $nl2br = false;

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags of the attribute
     * @param bool $nl2br nl2br boolean
     */
    public function __construct($name, $flags = 0, $nl2br = false)
    {
        parent::__construct($name, $flags);
        $this->nl2br = $nl2br;
    }

    /**
     * Returns a displayable string for this value.
     *
     * @param array $record Array wit fields
     * @param string $mode
     *
     * @return string Formatted string
     */
    public function display($record, $mode)
    {
        if ($this->nl2br) {
            return nl2br($record[$this->fieldName()]);
        } else {
            return $record[$this->fieldName()];
        }
    }
}
