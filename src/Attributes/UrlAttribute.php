<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;

/**
 * The UrlAttribute class represents a field containing URLs.
 *
 * @author Przemek Piotrowski <przemek.piotrowski@nic.com.pl>
 * @author Jeroen van Sluijs <jeroenvs@ibuildings.nl>
 */
class UrlAttribute extends Attribute
{
    /**
     * Add one space after each "/", "?" and "&" to fit a (long) url into <td></td>.
     */
    const AF_URL_ALLOWWRAP = 33554432;

    /**
     * Don't display "http://". Link remains intact.
     */
    const AF_URL_STRIPHTTP = 67108864;

    /**
     * Check if URL is a valid absolute URL.
     */
    const ABSOLUTE = 1;

    /**
     * Check if URL is a valid relative URL.
     */
    const RELATIVE = 2;

    /**
     * Check if URL is a valid anchor.
     */
    const ANCHOR = 4;

    public $m_accepts_url_flag = 0;
    public $m_newWindow = false;
    public $m_allowWrap = false;
    public $m_stripHttp = false;

    /**
     * base url. Set it by
     * calling setBaseUrl()
     * on the attribute.
     *
     * @var string
     */
    public $m_baseUrl = null;

    public function __construct($name, $flags = 0)
    {
        if (self::AF_POPUP === ($flags & self::AF_POPUP)) {
            $this->m_newWindow = true;
            $flags &= (~self::AF_POPUP);
        }

        if (self::AF_URL_ALLOWWRAP === ($flags & self::AF_URL_ALLOWWRAP)) {
            $this->m_allowWrap = true;
            $flags &= (~self::AF_URL_ALLOWWRAP);
        }

        if (self::AF_URL_STRIPHTTP === ($flags & self::AF_URL_STRIPHTTP)) {
            $this->m_stripHttp = true;
            $flags &= (~self::AF_URL_STRIPHTTP);
        }

        parent::__construct($name, $flags);
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * @param array $record The record that holds the value for this attribute
     * @param string $mode The display mode ("view" for viewpages, or "list"
     *                       for displaying in recordlists, "edit" for
     *                       displaying in editscreens, "add" for displaying in
     *                       add screens. "csv" for csv files. Applications can
     *                       use additional modes.
     *
     * @return string HTML String
     */
    public function display($record, $mode)
    {
        if (empty($record[$this->fieldName()])) {
            return '';
        }
        $url = $record[$this->fieldName()];

        /*
         * prepend a custom hostname to make the link
         * go to a custom domain. But only when you are using relative
         * urls.
         */
        if (($this->getBaseUrl()) && (($this->m_accepts_url_flag & self::RELATIVE) == self::RELATIVE)) {
            $base = $this->getBaseUrl();
            $url = $base.$url;
        }

        if (in_array($mode, array('csv', 'plain'))) {
            return $url;
        }

        $target = $this->m_newWindow ? ' target="_new"' : '';
        $text = $record[$this->fieldName()];
        if (true === $this->m_stripHttp) {
            $text = preg_replace('/^http:\/\//', '', $text);
        }
        if (true === $this->m_allowWrap) {
            $text = preg_replace('/([^\/?])\/([^\/?])/', '\1/ \2', $text);
            $text = preg_replace('/([?&].)/', ' \1', $text);
        }
        return '<a href="'.htmlspecialchars($url).'"'.$target.'">'.htmlspecialchars($text).'</a>';
    }

    /**
     * Return the base url (if set).
     */
    public function getBaseUrl()
    {
        return $this->m_baseUrl;
    }

    /**
     * Set the base url to
     * help the display function
     * set the correct domain.
     *
     * @param string $baseUrl http://my.domain.com
     */
    public function setBaseUrl($baseUrl)
    {
        $this->m_baseUrl = $baseUrl;
    }

    /**
     * Specify which URL-types are allowed.
     *
     * Example: acceptUrls(ABSOLUTE) - only absolute are accepted
     *          acceptUrls(ABSOLUTE|ANCHOR) - accept absolute URL's, anchors and an absolute URL followed by an anchor
     *
     * @param int $accepts_flag
     */
    public function acceptUrls($accepts_flag)
    {
        $this->m_accepts_url_flag = $accepts_flag;
    }

    /**
     * Checks if a value is valid.
     *
     * @param array $record The record that holds the value for this
     *                       attribute. If an error occurs, the error will
     *                       be stored in the 'atkerror' field of the record.
     * @param string $mode The mode for which should be validated ("add" or
     *                       "update")
     */
    public function validate(&$record, $mode)
    {
        $this->validateUrl($record, $mode, true);
    }

    /**
     * Validates absolute, relative and anchor URL through regular expression.
     *
     * @param array $record Record that contains value to be validated.
     *                           Errors are saved in this record, in the 'atkerror'
     *                           field.
     * @param string $mode Validation mode. Can be either "add" or "update"
     * @param bool $show_error fire a triggerError when validation fails
     */
    public function validateUrl(&$record, $mode, $show_error = false)
    {
        $result = false;

        $absolute_result = true;
        $anchor_result = true;
        $absolute_anchor_result = true;
        $relative_result = true;

        $base_url_regex = "(ft|htt)ps?:\/\/[a-zA-Z0-9\.\-\_]+\.[a-zA-Z]{2,4}";
        $relative_url_regex = "[a-zA-Z0-9\.\-\_\/?&=%]";
        $relative_url_regex_with_anchor = "[a-zA-Z0-9\.\-\_\/?&=%#]";

        /*
         * Validate URL, check if format is absolute (external URL's) and has no anchor
         *
         * Example: http://www2-dev.test_url.com
         * or:      ftp://www2-dev.test_url.com/index.php?/feeds/index.rss2
         */
        if (($this->m_accepts_url_flag & self::ABSOLUTE) == self::ABSOLUTE) {
            $absolute_result = preg_match('/^'.$base_url_regex.$relative_url_regex.'*$/Ui', $record[$this->fieldName()]) ? true : false;

            $result = $result || $absolute_result;
        }

        /*
         * Validate URL, check if format is a valid anchor
         *
         * Example: #internal_bookmark
         */
        if (($this->m_accepts_url_flag & self::ANCHOR) == self::ANCHOR) {
            $anchor_result = preg_match('/^#'.$relative_url_regex.'*$/Ui', $record[$this->fieldName()]) ? true : false;

            $result = $result || $anchor_result;
        }

        /*
         * Validate URL, check if format is absolute (external URL's) and has (optional) anchor
         *
         * Example: http://www2-dev.test_url.com
         * or:      ftp://www2-dev.test_url.com/index.php?/feeds/index.rss2
         * or:      https://www2-dev.test_url.com/index.php?/history.html#bookmark
         */
        if ((($this->m_accepts_url_flag & self::ABSOLUTE) == self::ABSOLUTE) && (($this->m_accepts_url_flag & self::ANCHOR) == self::ANCHOR)) {
            $absolute_anchor_result = preg_match('/^'.$base_url_regex.$relative_url_regex_with_anchor.'*$/Ui', $record[$this->fieldName()]) ? true : false;

            $result = $result || $absolute_anchor_result;
        }

        /*
         * Validate URL, check if format is relative
         *
         * Example: /mysite/guestbook/index.html
         */
        if (($this->m_accepts_url_flag & self::RELATIVE) == self::RELATIVE) {
            $relative_result = preg_match('/^'.$relative_url_regex_with_anchor.'+$/Ui', $record[$this->fieldName()]) ? true : false;

            $result = $result || $relative_result;
        }

        /*
         * If an error occured, show applicable message(s)
         */
        if (!$result && $show_error) {
            // if result of all validations is false, display error-messages
            if ($absolute_result === false) {
                Tools::triggerError($record, $this->fieldName(), 'invalid_absolute_no_anchor_url', Tools::atktext('invalid_absolute_no_anchor_url'));
            }
            if ($anchor_result === false) {
                Tools::triggerError($record, $this->fieldName(), 'invalid_url_anchor', Tools::atktext('invalid_url_anchor'));
            }
            if ($absolute_anchor_result === false) {
                Tools::triggerError($record, $this->fieldName(), 'invalid_absolute_url', Tools::atktext('invalid_absolute_url'));
            }
            if ($relative_result === false) {
                Tools::triggerError($record, $this->fieldName(), 'invalid_relative_url', Tools::atktext('invalid_relative_url'));
            }
        }

        if(!$result){
            parent::validate($record, $mode);
        }
    }
}
