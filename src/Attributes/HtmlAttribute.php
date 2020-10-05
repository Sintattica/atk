<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use \HTMLPurifier;
use \HTMLPurifier_Config;

/**
 * The HtmlAttribute class is the same as a normal Attribute. It only
 * (has a different display function. For this attribute, the value is
 * rendered as-is, which means you can use html codes in the text.
 *
 * There might me times where you want the user to be able to use html tags,
 * but you don't want to have the inconvenience of using br's for each line.
 * The AF_NL2BR flag tells attribute to do a newline-to-br conversion.
 *
 * By default, it filters out XSS attempts using HTMLPurifier before saving
 * data to database. If you want sanitization before displaying value, you
 * should add AF_SANITIZE_OUTPUT flag, and may remove AF_SANITIZE_INPUT flag.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class HtmlAttribute extends TextAttribute
{
    /**
     * Replace newlines by '<br/>' tags before displaying
     */
    const AF_NL2BR = 16777216;

    /**
     * Clean against XSS on input (before saving to BDD) [default behaviour]
     */
    const AF_SANITIZE_INPUT = 33554432;

    /**
     * Clean against XSS on output (before displaying in page)
     */
    const AF_SANITIZE_OUTPUT = 67108864;

    /**
     * New line to BR boolean
     *
     * @DEPRECATED in favor of AF_NL2BR flag
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
        if ($nl2br) {
            $flags |= self::AF_N2BR;
        }
        parent::__construct($name, $flags | self::AF_SANITIZE_INPUT);
    }

    /**
     * Sanitize a string using HTMLPurifier.
     *
     * @param string $text to check against dangerous elements
     *
     * @return string without XSS
     */
    protected function sanitize($text)
    {
        $purifierConfig = HTMLPurifier_Config::createDefault();
        $purifierConfig->set('Core.Encoding', Tools::atkGetCharset());
        $purifier = new HTMLPurifier($purifierConfig);
        return $purifier->purify($text);
    }

    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * If AF_SANITIZE_INPUT is set, sanitization is performed.
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     *
     * @return string|null The internal value
     */
    public function fetchValue($postvars)
    {
        $value = parent::fetchValue($postvars);
        if (is_null($value) || !$this->hasFlag(self::AF_SANITIZE_INPUT)) {
            return $value;
        }
        return $this->sanitize($value);
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
        if ($this->hasFlag(self::AF_NL2BR) || $this->nl2br) {
            $value = nl2br($record[$this->fieldName()]);
        } else {
            $value = $record[$this->fieldName()];
        }

        if ($this->hasFlag(self::AF_SANITIZE_OUTPUT)) {
            return $this->sanitize($value);
        } else {
            return $value;
        }
    }
}
