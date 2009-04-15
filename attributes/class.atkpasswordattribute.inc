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
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6309 $
   * $Id$
   */

   atkimport("atk.attributes.atkattribute");

  /**
   * Flag(s) specific for atkPasswordAttribute
   */
  define("AF_PASSWORD_NOVALIDATE",      AF_SPECIFIC_1); // disables password check when editing password field
  define("AF_PASSWORD_NO_VALIDATE",     AF_SPECIFIC_1); // disables password check when editing password field
  define("AF_PASSWORD_NO_ENCODE",       AF_SPECIFIC_2);

  /**
   * Categories of password character categories
   */
  define("UPPERCHARS", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  define("LOWERCHARS", "abcdefghijklmnopqrstuvwxyz");
  define("ALPHABETICCHARS", UPPERCHARS.LOWERCHARS);
  define("NUMBERS", "0123456789");
  define("SPECIALCHARS", "!@#$%^&*()-+_=[]{}\|;:'\",.<>/?"); // <- only used when generating a password
  define("EASYVOWELS", "bcdfghjkmnpqrstvwxz");
  define("EASYCONSONANTS", "aeuy");

  /**
   * The atkPasswordAttribute class represents an attribute of a node
   * that is a password field. It automatically encrypts passwords
   * with the MD5 method of PHP. To update a password a user has to
   * supply the old password first, unless you use the special created
   * AF_PASSWORD_NOVALIDATE flag, in which case the password just gets
   * overwritten without any check.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkPasswordAttribute extends atkAttribute
  {
    /* generate? */
    var $m_generate;

    /**
     * Restrictions to apply when changing/setting the password
     *
     * @var Array
     */
    var $m_restrictions;

    /**
     * Constructor
     * @param string $name Name of the attribute
     * @param bool $generate Generate password (boolean)
     * @param integer $flags Flags for this attribute
     * @param mixed $size  The size(s) of the attribute. See the $size
     *                     parameter of the setAttribSize() method for more
     *                     information on the possible values of this
     *                     parameter.
     * @param array $restrictions  
     */
    function atkPasswordAttribute($name, $generate, $flags=0, $size=0, $restrictions="")
    {
      // compatiblity with old versions
      if (func_num_args() >= 3)
      {
      $this->m_generate = $generate;
      }
      else
      {
        $flags = $generate;
        $this->m_generate = FALSE;
      }

      // Call the parent constructor
      $this->atkAttribute($name, $flags|AF_HIDE_SEARCH, $size); // you can't search by password.

      // Set the restrictions
      $this->setRestrictions($restrictions);
    }

    /**
     * Encodes the given value only if the
     * AF_PASSWORD_NO_ENCODE flag is not set.
     *
     * @param string $value
     * @return string
     */
    function encode($value)
    {
      return $this->hasFlag(AF_PASSWORD_NO_ENCODE) ? $value : md5($value);
    }

    /**
     * Sets the restrictions on passwords
     *
     * @param Array $restrictions Restrictions that should apply to this attribute
     */
    function setRestrictions($restrictions)
    {
      $this->m_restrictions = array("minsize"=>0, "minupperchars"=>0, "minlowerchars"=>0, "minalphabeticchars"=>0, "minnumbers"=>0, "minspecialchars"=>0);
      if (is_array($restrictions))
      {
        foreach($restrictions as $name=>$value)
        {
          if (in_array(strtolower($name), array("minsize", "minupperchars", "minlowerchars", "minalphabeticchars", "minnumbers", "minspecialchars")))
          {
            $this->m_restrictions[strtolower($name)] = $value;
          }
          else
          {
            atkdebug("atkPasswordAttribute->setRestrictions(): Unknown restriction: \"$name\"=\"$value\"", DEBUG_WARNING);
          }
        }
      }
    }

    /**
     * Returns the password restrictions that apply to this password
     *
     * @return Array Restrictions that should apply to this attribute
     */
    function getRestrictions()
    {
      return $this->m_restrictions;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record array with fields
     * @param $fieldprefix the field's prefix
     * @param $mode the mode (add, edit etc.)
     * @return piece of html code with a textarea
     */
    function edit($record="", $fieldprefix="", $mode="")
    {
      $id = $fieldprefix.$this->fieldName();
      /* insert */
      if ($mode != 'edit' && $mode != 'update')
      {
        if (!$this->m_generate)
        {
          $this->registerKeyListener($id.'[new]', KB_CTRLCURSOR|KB_UPDOWN);
          $this->registerKeyListener($id.'[again]', KB_CTRLCURSOR|KB_UPDOWN);
          $result = atktext("password_new", "atk").':<br>'.
                    '<input autocomplete="off" type="password" id="'.$id.'[new]" name="'.$id.'[new]"'.
                    ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                    ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '')."><br><br>".

                    atktext("password_again", "atk").':<br>'.
                    '<input autocomplete="off" type="password" id="'.$id.'[again]" name="'.$id.'[again]"'.
                    ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                    ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').">";
        }
        else
        {
          $password = $this->generatePassword(8,TRUE);
          $this->registerKeyListener($id.'[new]', KB_CTRLCURSOR|KB_UPDOWN);
          $this->registerKeyListener($id.'[again]', KB_CTRLCURSOR|KB_UPDOWN);
          $result = '<input type="hidden" id="'.$id.'[again]" name="'.$id.'[again]"'.
                    ' value ="'.$password.'">'.
                    '<input type="text" id="'.$id.'[new]" name="'.$id.'[new]"'.
                    ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                    ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').' value ="'.$password.'" onchange="this.form.elements[\''.$fieldprefix.$this->fieldName().'[again]\'].value=this.value">';
        }
      }

      /* edit */
      else
      {
        $result = '<input type="hidden" name="'.$id.'[hash]"'.
                  ' value="'.$record[$this->fieldName()]["hash"].'">';


        if (!$this->hasFlag(AF_PASSWORD_NOVALIDATE))
        {
          $this->registerKeyListener($id.'[current]', KB_CTRLCURSOR|KB_UPDOWN);
          $result .= atktext("password_current", "atk").':<br>'.
                     '<input autocomplete="off" type="password" id="'.$id.'[current]" name="'.$id.'[current]"'.
                     ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                     ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>';
        }
        $this->registerKeyListener($id.'[new]', KB_CTRLCURSOR|KB_UPDOWN);
        $this->registerKeyListener($id.'[again]', KB_CTRLCURSOR|KB_UPDOWN);
        $result .= atktext("password_new", "atk").':<br>'.
                   '<input autocomplete="off" type="password" id="'.$id.'[new]" name="'.$id.'[new]"'.
                   ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                   ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>'.

                   atktext("password_again", "atk").':<br>'.
                   '<input autocomplete="off" type="password" id="'.$id.'[again]" name="'.$id.'[again]"'.
                   ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                   ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'>';
      }

      return $result;
    }

    /**
     * We don't support searching for passwords!
     * @param $record array with fields
     * @return search field
     */
    function search($record="")
    {
      return "&nbsp;";
    }

    /**
     * Add's slashes to the string for the database
     * @param $rec Array with values
     * @return String with slashes
     */
    function value2db($rec)
    {
      return addslashes($rec[$this->fieldName()]["hash"]);
    }

    /**
     * Removes slashes from the string and save to array
     * @param $rec array with values
     * @return array with hash field without slashes
     */
    function db2value($rec)
    {
      $value = isset($rec[$this->fieldName()]) ? stripslashes($rec[$this->fieldName()]) : null;
      return array("hash" => $value);
    }

    /**
     * Counts the number characters in the password that are contained within the chars array
     *
     * @param string $password Password in which we should look for chars
     * @param string $chars Characters that should be looked for in password
     * @return int Number of characters in password that match
     */
    function _countCharMatches($password, $chars)
    {
      $count = 0;
      for($i=0,$_i=strlen($password); $i<$_i; $i++)
      {
        if (strpos($chars, substr($password, $i, 1)) !== false)
          $count++;
      }
      return $count;
    }

    /**
     * Validates the password to the restrictions
     *
     * @param string $password
     * @return boolean True if password succesfully validates to the restrictions
     */
    function validateRestrictions($password)
    {
      // Mainain the failed status as boolean (false by default)
      $validationfailed = false;

      // Loop through all restrictions
      foreach ($this->m_restrictions as $name => $value)
      {
        // Get the number of actual characters that should be checked against this restriction
        $actual = 0;
        switch ($name)
        {
          case "minsize":           $actual = strlen($password); break;
          case "minupperchars":     $actual = $this->_countCharMatches($password, UPPERCHARS); break;
          case "minlowerchars":     $actual = $this->_countCharMatches($password, LOWERCHARS); break;
          case "minalphabeticchars":$actual = $this->_countCharMatches($password, ALPHABETICCHARS); break;
          case "minnumbers":        $actual = $this->_countCharMatches($password, NUMBERS); break;
          case "minspecialchars":   $actual = strlen($password) - $this->_countCharMatches($password, ALPHABETICCHARS.NUMBERS); break;
        }

        // If the number of actual characters is lower than the minimum set by the restriction, set
        // validationfailed to true (if that wasn't done already)
        $validationfailed |= $actual < $value;
      }

      // Return True if validation succeeded, Fals if validation failed
      return !$validationfailed;
    }

    /**
     * Composes a string describing the restrictions
     *
     * @return string Description of restrictions
     */
    function getRestrictionsText()
    {
      // If no restrictions are set, return "No restrictions apply to this password"
      if (count($this->m_restrictions) == 0)
      {
        return atktext("no_restrictions_apply_to_this_password", "atk");
      }

      // Start with an empty string
      $text = "";

      // Loop through all restrictions
      foreach ($this->m_restrictions as $name => $value)
      {
        // Add a human readable form of the current restriction to the text string and append a linebreak
        if ($value > 0)
        {
          if ($name == "minsize")
            $text .= sprintf(atktext("the_password_should_be_at_least_%d_characters_long", "atk"), $value);
          else
            $text .= sprintf(atktext("the_password_should_at_least_contain_%d_%s", "atk"), $value, atktext(substr($name, 3), "atk"));
          $text .= "<br />\n";
        }
      }

      // Return the generated text
      return $text;
    }

    /**
     * Validates the supplied passwords
     * @param $record Record that contains value to be validated.
     *                 Errors are saved in this record
     * @param $mode can be either "add" or "update"
     */
    function validate(&$record, $mode)
    {
      $error = FALSE;
      $value = $record[$this->fieldName()];

      if ($mode == 'update'
          && (atk_strlen($value["new"]) > 0 || atk_strlen($value["again"]) > 0)
          && !$this->hasFlag(AF_PASSWORD_NOVALIDATE)
          && $value["current"] != $value["hash"])
      {
        $error = TRUE;
        triggerError($record, $this->fieldName(), 'error_password_incorrect');
      }

      if ($value["new"] != $value["again"])
      {
        $error = TRUE;
        triggerError($record, $this->fieldName(), 'error_password_nomatch');
      }

      if ($mode =="add" && $this->hasFlag(AF_OBLIGATORY) && atk_strlen($value["new"]) == 0)
      {
        $error = TRUE;
        triggerError($record, $this->fieldName(), 'error_obligatoryfield');
      }

      // Check if the password meets the restrictions. If not, set error to true and
      // triger an error with the human readable form of the restrictions as message.
      if ((atk_strlen($value["new"]) > 0) && (!$this->validateRestrictions($value["newplain"])))
      {
        $error = TRUE;
        triggerError($record, $this->fieldName(), $this->getRestrictionsText());
      }

      // new password?
      if (!$error && atk_strlen($value["new"]) > 0)
      $record[$this->fieldName()]["hash"] = $record[$this->fieldName()]["new"];
    }

    /**
     * Check if the attribute is empty
     * 
     * @param array $record The record that holds this attribute's value.
     * @return true if it's empty
     */
    function isEmpty($record)
    {
      /* unfortunately we cannot check this here */
      return FALSE;
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return Piece of htmlcode
     */
    function hide($record="", $fieldprefix="")
    {
      $result = '<input type="hidden" name="'.$fieldprefix.$this->fieldName().'[hash]"'.
                ' value="'.$record[$this->fieldName()]["hash"].'">';
      return $result;
    }

    /**
     * We don't display the password
     * @param $rec the record with display data
     * @return string with value to display
     */
    function display($rec)
    {
      return atktext("password_hidden", "atk");
    }

    /**
     * There can not be searched for passwords!
     */
    function getSearchModes()
    {
      return array();
    }

    /**
     * Generates a random string using the given character set
     *
     * @param string/array $chars String or array of strings containing the available characters to use
     * @param int $count Length of the resulting string
     * @return Generated random string
     */
    function getRandomChars($chars, $count)
    {
      // Always use an array
      $charset = is_array($chars) ? $chars : array($chars);

      // Seed the random generator using microseconds
      mt_srand((double)microtime()*1000000);

      // Start with an empty result
      $randomchars = "";

      // Add a character one by one
      for($i=0; $i<$count; $i++)
      {
        // Pick the set of characters to be used from the array
        $chars = $charset[$i % count($charset)];

        // Choose a character randomly and add it to the result
        $randomchars .= substr($chars, mt_rand(0, strlen($chars)-1), 1);
      }

      // Return the resulting random characters
      return $randomchars;
    }

    /**
     * Generates a random password which meets the restrictions
     *
     * @param int $length Length of the password (could be overridden by higher restrictions)
     * @param boolean $easytoremember If true, generated passwords are more easy to remember, but also easier to crack. Defaults to false.
     * @return string Generated password
     */
    function generatePassword($length = 8, $easytoremember = false)
    {
      // Use short notation
      $r = $this->m_restrictions;

      // Compose a string that meets the character-specific minimum restrictions
      $tmp = $this->getRandomChars(LOWERCHARS, $r["minlowerchars"]);
      $tmp.= $this->getRandomChars(UPPERCHARS, $r["minupperchars"]);
      $alphabeticchars = ($r["minalphabeticchars"] > strlen($tmp)) ? ($r["minalphabeticchars"] - strlen($tmp)) : 0;
      $tmp.= $this->getRandomChars(LOWERCHARS . UPPERCHARS, $alphabeticchars);
      $tmp.= $this->getRandomChars(NUMBERS, $r["minnumbers"]);
      $tmp.= $this->getRandomChars(SPECIALCHARS, $r["minspecialchars"]);

      // Determine how many characters we still need to add to meet the overall minimum length
      $remainingchars = max($r["minsize"], $length) > strlen($tmp) ? (max($r["minsize"], $length) - strlen($tmp)) : 0;

      // At this point we have gathered the characters we need to meet the
      // charactertype-specific restrictions. From now we can split ways to
      // make the password either easy to remember or as random as possible.
      if ($easytoremember)
      {
        // Add random characters to the string to fill up until the minimum size or passed length
        $out = $this->getRandomChars(array(EASYVOWELS, EASYCONSONANTS, EASYVOWELS), $remainingchars);

        // Add the characters that make this password meet the restrictions
        // at the end of the password, so we keep at least the most of it
        // easy to remember.
        $out.= $tmp;
      }
      else
      {
        // Add random characters to the string to fill up until the minimum size or passed length
        $tmp.= $this->getRandomChars(LOWERCHARS . UPPERCHARS . NUMBERS . SPECIALCHARS, $remainingchars);

        // The output should be a shuffled to make it really random
        $out = str_shuffle($tmp);
      }

      // Return the output
      return $out;
    }

    /**
     * Generates a random password which isn't to bad to remember.
     * @deprecated The object-function generatePassword should be used instead of this static function.
     * @static Some applications still call this method statically, so to keep this function
     *         backwards compatible it has to remain static.
     * @param int $times Number of syllables (password length will be $times * 3)
     * @return string Generated password
     */
    function makePassword($times = 2)
    {
      // Show a debugmessage about this function being deprecated
      atkdebug("atkPasswordAttribute::makePassword() is deprecated, use generatePassword() on an atkPasswordAttribute instead.");

      // Construct a new passwordattribute, generate the password and return it
      $passwordattribute = new atkPasswordAttribute("dummy",true);
      return $passwordattribute->getRandomChars(array(EASYVOWELS, EASYCONSONANTS, EASYVOWELS), $times * 3);
    }

    /**
     * Overwriting the fetchValue to ensure all passwords are hashed
     * 
     * @param array $rec The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     */
    function fetchValue($rec)
    {
      if (isset($rec[$this->fieldName()]["current"]) && !empty($rec[$this->fieldName()]["current"]))
      {
        $rec[$this->fieldName()]["current"] = $this->encode($rec[$this->fieldName()]["current"]);
      }
      if (isset($rec[$this->fieldName()]["new"]) && !empty($rec[$this->fieldName()]["new"]))
      {
        $rec[$this->fieldName()]["newplain"] = $rec[$this->fieldName()]["new"];
        $rec[$this->fieldName()]["new"] = $this->encode($rec[$this->fieldName()]["new"]);
      }
      if (isset($rec[$this->fieldName()]["again"]) && !empty($rec[$this->fieldName()]["again"]))
      {
        $rec[$this->fieldName()]["again"] = $this->encode($rec[$this->fieldName()]["again"]);
      }
      return $rec[$this->fieldName()];
    }


    /** Due to the new storeType functions
     * password field is not allways saved from within the password attrib
     *
     * Added a "dynamic" needsUpdate to cancel updates if no password fields where used
     * to alter the password. This overcomes the overwriting with an empty password.
     * 
     * @param array $record The record that contains this attribute's value
     */
    function needsUpdate($record)
    {
      $value = $record[$this->fieldName()];

      // new is set from an update using the password attrib edit() function

      if (atkArrayNvl($value, "new", "") != "" || atkArrayNvl($value, "hash", "") != "")
      {
        return true;
      }
      return false;
    }

  }
?>
