<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;

/**
 * The PasswordAttribute class represents an attribute of a node
 * that is a password field. It automatically encrypts passwords
 * with the password_hash() method of PHP. To update a password a user has to
 * supply the old password first, unless you use the special created
 * self::AF_PASSWORD_NO_VALIDATE flag, in which case the password just gets
 * overwritten without any check.
 *
 * @author Peter Verhage <peter@ibuildings.nl>
 */
class PasswordAttribute extends Attribute
{
    /**
     * Flag(s) specific for atkPasswordAttribute.
     */
    const AF_PASSWORD_NO_VALIDATE = 33554432; // disables password check when editing password field
    const AF_PASSWORD_NO_ENCODE = 67108864;

    /**
     * Categories of password character categories.
     */
    const UPPERCHARS = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
    const LOWERCHARS = 'abcdefghijklmnopqrstuvwxyz';
    const ALPHABETICCHARS = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz';
    const NUMBERS = '0123456789';
    const SPECIALCHARS = "!@#$%^&*()-+_=[]{}\|;:'\",.<>/?"; // <- only used when generating a password
    const EASYVOWELS = 'bcdfghjkmnpqrstvwxz';
    const EASYCONSONANTS = 'aeuy';

    /* generate? */
    public $m_generate;

    /*
     * Restrictions to apply when changing/setting the password
     *
     * @var array
     */
    public $m_restrictions;
    
    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param bool $generate Generate password (boolean)
     * @param array $restrictions
     */
    public function __construct($name, $flags = 0, $generate = false, $restrictions = [])
    {
        $flags = $flags | self::AF_HIDE_SEARCH;
        $this->m_generate = $generate;

        parent::__construct($name, $flags);
        $this->setRestrictions($restrictions);
    }

    /**
     * Encodes the given value only if the
     * self::AF_PASSWORD_NO_ENCODE flag is not set.
     *
     * @param string $value
     *
     * @return string
     */
    public function encode($value)
    {
        return $this->hasFlag(self::AF_PASSWORD_NO_ENCODE) ? $value : password_hash($value, PASSWORD_DEFAULT);
    }

    /**
     * @param $password string
     * @param $hash string
     * @return bool
     */
    public function verify($password, $hash)
    {
        if ($this->hasFlag(self::AF_PASSWORD_NO_ENCODE)) {
            return $password == $hash;
        }

        return password_verify($password, $hash);
    }

    /**
     * Sets the restrictions on passwords.
     *
     * @param array $restrictions Restrictions that should apply to this attribute
     */
    public function setRestrictions($restrictions)
    {
        $this->m_restrictions = array(
            'minsize' => 0,
            'minupperchars' => 0,
            'minlowerchars' => 0,
            'minalphabeticchars' => 0,
            'minnumbers' => 0,
            'minspecialchars' => 0,
        );
        if (is_array($restrictions)) {
            foreach ($restrictions as $name => $value) {
                if (in_array(strtolower($name), array(
                    'minsize',
                    'minupperchars',
                    'minlowerchars',
                    'minalphabeticchars',
                    'minnumbers',
                    'minspecialchars',
                ))) {
                    $this->m_restrictions[strtolower($name)] = $value;
                } else {
                    Tools::atkdebug("atkPasswordAttribute->setRestrictions(): Unknown restriction: \"$name\"=\"$value\"", Tools::DEBUG_WARNING);
                }
            }
        }
    }

    /**
     * Returns the password restrictions that apply to this password.
     *
     * @return array Restrictions that should apply to this attribute
     */
    public function getRestrictions()
    {
        return $this->m_restrictions;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record array with fields
     * @param string $fieldprefix the field's prefix
     * @param string $mode the mode (add, edit etc.)
     *
     * @return string piece of html code with a textarea
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);
        $cssClass = $this->getCSSClassAttribute(array('form-control'));

        if ($mode != 'edit' && $mode != 'update') {
            /* insert */
            if (!$this->m_generate) {
                $result = Tools::atktext('password_new',
                        'atk').':<br>'.'<input autocomplete="off" '.$cssClass.' type="password" id="'.$id.'[new]" name="'.$name.'[new]"'.($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>'.Tools::atktext('password_again',
                        'atk').':<br>'.'<input autocomplete="off" '.$cssClass.' type="password" id="'.$id.'[again]" name="'.$name.'[again]"'.($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'>';
            } else {
                $password = $this->generatePassword(8, true);
                $result = '<input type="hidden" id="'.$id.'[again]" name="'.$name.'[again]"'.' value ="'.$password.'" '.$cssClass.'>';
                $result .= '<input type="text" '.$cssClass.' id="'.$id.'[new]" name="'.$name.'[new]"'.($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').' value ="'.$password.'" onchange="this.form.elements[\''.$fieldprefix.$this->fieldName().'[again]\'].value=this.value">';
            }
        } else {
            /* edit */

            $result = '<input type="hidden" name="'.$name.'[hash]"'.' value="'.$record[$this->fieldName()]['hash'].'">';

            if (!$this->hasFlag(self::AF_PASSWORD_NO_VALIDATE)) {
                $result .= Tools::atktext('password_current',
                        'atk').':<br>'.'<input autocomplete="off" type="password" '.$cssClass.' id="'.$id.'[current]" name="'.$name.'[current]"'.($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>';
            }
            $result .= Tools::atktext('password_new',
                    'atk').':<br>'.'<input autocomplete="off" type="password" '.$cssClass.' id="'.$id.'[new]" name="'.$name.'[new]"'.($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>'.Tools::atktext('password_again',
                    'atk').':<br>'.'<input autocomplete="off" type="password" '.$cssClass.' id="'.$id.'[again]" name="'.$name.'[again]"'.($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'>';
        }

        return $result;
    }


    public function search($atksearch, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        // We don't support searching for passwords!
        return '';
    }

    /**
     * @param array $rec Array with values
     *
     * @return string
     */
    public function value2db($rec)
    {
        return $rec[$this->fieldName()]['hash'];
    }

    /**
     * @param array $rec array with values
     *
     * @return array with hash field
     */
    public function db2value($rec)
    {
        $value = isset($rec[$this->fieldName()]) ? $rec[$this->fieldName()] : null;

        return ['hash' => $value];
    }

    /**
     * Counts the number characters in the password that are contained within the chars array.
     *
     * @param string $password Password in which we should look for chars
     * @param string $chars Characters that should be looked for in password
     *
     * @return int Number of characters in password that match
     */
    public function _countCharMatches($password, $chars)
    {
        $count = 0;
        for ($i = 0, $_i = strlen($password); $i < $_i; ++$i) {
            if (strpos($chars, substr($password, $i, 1)) !== false) {
                ++$count;
            }
        }

        return $count;
    }

    /**
     * Validates the password to the restrictions.
     *
     * @param string $password
     *
     * @return bool True if password succesfully validates to the restrictions
     */
    public function validateRestrictions($password)
    {
        // Mainain the failed status as boolean (false by default)
        $validationfailed = false;

        // Loop through all restrictions
        foreach ($this->m_restrictions as $name => $value) {
            // Get the number of actual characters that should be checked against this restriction
            $actual = 0;
            switch ($name) {
                case 'minsize':
                    $actual = strlen($password);
                    break;
                case 'minupperchars':
                    $actual = $this->_countCharMatches($password, self::UPPERCHARS);
                    break;
                case 'minlowerchars':
                    $actual = $this->_countCharMatches($password, self::LOWERCHARS);
                    break;
                case 'minalphabeticchars':
                    $actual = $this->_countCharMatches($password, self::ALPHABETICCHARS);
                    break;
                case 'minnumbers':
                    $actual = $this->_countCharMatches($password, self::NUMBERS);
                    break;
                case 'minspecialchars':
                    $actual = strlen($password) - $this->_countCharMatches($password, self::ALPHABETICCHARS.self::NUMBERS);
                    break;
            }

            // If the number of actual characters is lower than the minimum set by the restriction, set
            // validationfailed to true (if that wasn't done already)
            $validationfailed |= $actual < $value;
        }

        // Return True if validation succeeded, Fals if validation failed
        return !$validationfailed;
    }

    /**
     * Composes a string describing the restrictions.
     *
     * @return string Description of restrictions
     */
    public function getRestrictionsText()
    {
        // If no restrictions are set, return "No restrictions apply to this password"
        if (Tools::count($this->m_restrictions) == 0) {
            return Tools::atktext('no_restrictions_apply_to_this_password', 'atk');
        }

        // Start with an empty string
        $text = '';

        // Loop through all restrictions
        foreach ($this->m_restrictions as $name => $value) {
            // Add a human readable form of the current restriction to the text string and append a linebreak
            if ($value > 0) {
                if ($name == 'minsize') {
                    $text .= sprintf(Tools::atktext('the_password_should_be_at_least_%d_characters_long', 'atk'), $value);
                } else {
                    $text .= sprintf(Tools::atktext('the_password_should_at_least_contain_%d_%s', 'atk'), $value, Tools::atktext(substr($name, 3), 'atk'));
                }
                $text .= "<br />\n";
            }
        }

        // Return the generated text
        return $text;
    }

    /**
     * Validates the supplied passwords.
     *
     * @param array $record Record that contains value to be validated.
     *                       Errors are saved in this record
     * @param string $mode can be either "add" or "update"
     */
    public function validate(&$record, $mode)
    {
        $error = false;
        $value = $record[$this->fieldName()];

        if ($mode == 'update' && (Tools::atk_strlen($value['new']) > 0 || Tools::atk_strlen($value['again']) > 0) && !$this->hasFlag(self::AF_PASSWORD_NO_VALIDATE) && !$this->verify($value['current'],
                $value['hash'])
        ) {
            Tools::triggerError($record, $this->fieldName(), 'error_password_incorrect');
        }

        if (Tools::atk_strlen($value['new']) > 0 && Tools::atk_strlen($value['again']) > 0 && $value['new'] != $value['again']) {
            $error = true;
            Tools::triggerError($record, $this->fieldName(), 'error_password_nomatch');
        }

        if ($mode == 'add' && $this->hasFlag(self::AF_OBLIGATORY) && Tools::atk_strlen($value['new']) == 0) {
            $error = true;
            Tools::triggerError($record, $this->fieldName(), 'error_obligatoryfield');
        }

        // Check if the password meets the restrictions. If not, set error to true and
        // triger an error with the human readable form of the restrictions as message.
        if (isset($value['new']) && Tools::atk_strlen($value['new']) > 0 && !$this->validateRestrictions($value['new'])) {
            $error = true;
            Tools::triggerError($record, $this->fieldName(), $this->getRestrictionsText());
        }

        // new password?
        if (!$error && isset($value['new']) && Tools::atk_strlen($value['new']) > 0) {
            $record[$this->fieldName()]['hash'] = $this->encode($record[$this->fieldName()]['new']);
        }
    }

    /**
     * Check if the attribute is empty.
     *
     * @param array $record The record that holds this attribute's value.
     *
     * @return true if it's empty
     */
    public function isEmpty($record)
    {
        /* unfortunately we cannot check this here */
        return false;
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     *
     * @return string html
     */
    public function hide($record, $fieldprefix, $mode)
    {
        $result = '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'[hash]"'.' value="'.$record[$this->fieldName()]['hash'].'">';

        return $result;
    }

    /**
     * We don't display the password.
     *
     * @param array $record the record with display data
     * @param string $mode
     *
     * @return string with value to display
     */
    public function display($record, $mode)
    {
        return Tools::atktext('password_hidden', 'atk');
    }

    /**
     * There can not be searched for passwords!
     */
    public function getSearchModes()
    {
        return [];
    }

    /**
     * Generates a random string using the given character set.
     *
     * @param string|array $chars String or array of strings containing the available characters to use
     * @param int $count Length of the resulting string
     *
     * @return string Generated random string
     */
    public function getRandomChars($chars, $count)
    {
        // Always use an array
        $charset = is_array($chars) ? $chars : array($chars);

        // Seed the random generator using microseconds
        mt_srand((double)microtime() * 1000000);

        // Start with an empty result
        $randomchars = '';

        // Add a character one by one
        for ($i = 0; $i < $count; ++$i) {
            // Pick the set of characters to be used from the array
            $chars = $charset[$i % Tools::count($charset)];

            // Choose a character randomly and add it to the result
            $randomchars .= substr($chars, mt_rand(0, strlen($chars) - 1), 1);
        }

        // Return the resulting random characters
        return $randomchars;
    }

    /**
     * Generates a random password which meets the restrictions.
     *
     * @param int $length Length of the password (could be overridden by higher restrictions)
     * @param bool $easytoremember If true, generated passwords are more easy to remember, but also easier to crack. Defaults to false.
     *
     * @return string Generated password
     */
    public function generatePassword($length = 8, $easytoremember = false)
    {
        // Use short notation
        $r = $this->m_restrictions;

        // Compose a string that meets the character-specific minimum restrictions
        $tmp = $this->getRandomChars(self::LOWERCHARS, $r['minlowerchars']);
        $tmp .= $this->getRandomChars(self::UPPERCHARS, $r['minupperchars']);
        $alphabeticchars = ($r['minalphabeticchars'] > strlen($tmp)) ? ($r['minalphabeticchars'] - strlen($tmp)) : 0;
        $tmp .= $this->getRandomChars(self::LOWERCHARS.self::UPPERCHARS, $alphabeticchars);
        $tmp .= $this->getRandomChars(self::NUMBERS, $r['minnumbers']);
        $tmp .= $this->getRandomChars(self::SPECIALCHARS, $r['minspecialchars']);

        // Determine how many characters we still need to add to meet the overall minimum length
        $remainingchars = max($r['minsize'], $length) > strlen($tmp) ? (max($r['minsize'], $length) - strlen($tmp)) : 0;

        // At this point we have gathered the characters we need to meet the
        // charactertype-specific restrictions. From now we can split ways to
        // make the password either easy to remember or as random as possible.
        if ($easytoremember) {
            // Add random characters to the string to fill up until the minimum size or passed length
            $out = $this->getRandomChars(array(self::EASYVOWELS, self::EASYCONSONANTS, self::EASYVOWELS), $remainingchars);

            // Add the characters that make this password meet the restrictions
            // at the end of the password, so we keep at least the most of it
            // easy to remember.
            $out .= $tmp;
        } else {
            // Add random characters to the string to fill up until the minimum size or passed length
            $tmp .= $this->getRandomChars(self::LOWERCHARS.self::UPPERCHARS.self::NUMBERS.self::SPECIALCHARS, $remainingchars);

            // The output should be a shuffled to make it really random
            $out = str_shuffle($tmp);
        }

        // Return the output
        return $out;
    }

    /** Due to the new storeType functions
     * password field is not allways saved from within the password attrib.
     *
     * Added a "dynamic" needsUpdate to cancel updates if no password fields where used
     * to alter the password. This overcomes the overwriting with an empty password.
     *
     * @param array $record The record that contains this attribute's value
     *
     * @return bool
     */
    public function needsUpdate($record)
    {
        $value = $record[$this->fieldName()];

        // new is set from an update using the password attrib edit() function

        if (Tools::atkArrayNvl($value, 'new', '') != '' || Tools::atkArrayNvl($value, 'hash', '') != '') {
            return true;
        }

        return false;
    }
}
