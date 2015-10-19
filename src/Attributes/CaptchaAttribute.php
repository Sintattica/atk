<?php namespace Sintattica\Atk\Attributes;

/**
 * With the atkCaptchaAttribute class you can easily add a captcha to a forum
 * or guestbook.
 *
 * Use the flag self::AF_NOLABEL if you want to start at the beginning of the
 * line.
 *
 * @author Nico de Boer <nico@ibuildings.nl>
 * @package atk
 * @subpackage attributes
 *
 */
class CaptchaAttribute extends Attribute
{

    /**
     * Constructor
     * @param string $name The name of the attribute
     * @param int $flags The attribute flags
     * @return -
     */
    function __construct($name, $flags = 0)
    {
        // A Captcha attribute should not be searchable and sortable
        $flags |= self::AF_HIDE_SEARCH | self::AF_NO_SORT;

        parent::__construct($name, $flags); // base class constructor
    }

    /**
     * Edit  record
     * Here it will only return the text, no edit box.
     * @param array $record Array with fields
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return Parsed string
     */
    function edit($record = "", $fieldprefix = "", $mode = "")
    {
        $target = "atk/ext/captcha/img/captcha.jpg.php";
        $img = Tools::session_url("include.php?file=" . $target);

        $html = '<img src="' . $img . '"><br>';
        $html .= '<br>';
        $html .= '<small>' . Tools::atktext("captcha_explain", "atk") . '</small><br>';
        $html .= '<input type="text" name="' . $fieldprefix . $this->fieldName() . '">';
        return $html;
    }

    /**
     * Make sure the value is not stored. (always calculated on the fly)
     * @access private
     * @return int
     */
    function storageType()
    {
        return self::NOSTORE;
    }

    /**
     * Make sure the value is not loaded.
     * @access private
     * @return int
     */
    function loadType()
    {
        return self::NOLOAD;
    }

    /**
     * Validate the value fo this attribute
     *
     * @param array $record The record that contains the value for this attribute
     * @param string $mode The mode for which should be validated ("add" or
     *                     "update")
     */
    function validate(&$record, $mode)
    {
        $sCaptchaCode = $record[$this->fieldName()];
        if (md5(strtoupper($sCaptchaCode)) != $_SESSION['php_captcha']) {
            Tools::triggerError($record, $this->fieldName(), 'error_captchafield');
        }

        // clear to prevent re-use
        $_SESSION['php_captcha'] = '';
    }

}


