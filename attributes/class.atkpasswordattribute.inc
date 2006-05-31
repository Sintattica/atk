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
   * @version $Revision$
   * $Id$
   */

   atkimport("atk.attributes.atkattribute");

  /**
   * Flag(s) specific for atkPasswordAttribute
   */
  define("AF_PASSWORD_NOVALIDATE",      AF_SPECIFIC_1); // disables password check when editing password field
  define("AF_PASSWORD_NO_VALIDATE",     AF_SPECIFIC_1); // disables password check when editing password field

  /**
   * Categories of password restrictions
   */
  define("UPPERCHARS", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  define("LOWERCHARS", "abcdefghijklmnopqrstuvwxyz");
  define("ALPHABETICCHARS", UPPERCHARS.LOWERCHARS);
  define("NUMBERS", "0123456789");
  define("SPECIALCHARS", "!@#$%^&*()-+_=[]{}\|;:'\",.<>/?"); // <- only used when generating a password

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
     * @param $name Name of the attribute
     * @param $generate Generate password (boolean)
     * @param $flags Flags for this attribute
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
            atkdebug("atkPasswordAttribute->setRestrictions(): Applying restriction: \"$name\"=\"$value\"");
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
                    '<input type="password" id="'.$id.'[new]" name="'.$id.'[new]"'.
                    ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                    ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '')."><br><br>".

                    atktext("password_again", "atk").':<br>'.
                    '<input type="password" id="'.$id.'[again]" name="'.$id.'[again]"'.
                    ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                    ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').">";
        }
        else
        {
          $password = $this->makePassword();
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
                     '<input type="password" id="'.$id.'[current]" name="'.$id.'[current]"'.
                     ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                     ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>';
        }
        $this->registerKeyListener($id.'[new]', KB_CTRLCURSOR|KB_UPDOWN);
        $this->registerKeyListener($id.'[again]', KB_CTRLCURSOR|KB_UPDOWN);
        $result .= atktext("password_new", "atk").':<br>'.
                   '<input type="password" id="'.$id.'[new]" name="'.$id.'[new]"'.
                   ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                   ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>'.

                   atktext("password_again", "atk").':<br>'.
                   '<input type="password" id="'.$id.'[again]" name="'.$id.'[again]"'.
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
     * @param &$record Record that contains value to be validated.
     *                 Errors are saved in this record
     * @param $mode can be either "add" or "update"
     */
    function validate(&$record, $mode)
    {
      $error = FALSE;
      $value = $record[$this->fieldName()];

      if ($mode == 'update'
          && (strlen($value["new"]) > 0 || strlen($value["again"]) > 0)
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

      if ($mode =="add" && $this->hasFlag(AF_OBLIGATORY) && strlen($value["new"]) == 0)
      {
        $error = TRUE;
        triggerError($record, $this->fieldName(), 'error_obligatoryfield');
      }

      // Check if the password meets the restrictions. If not, set error to true and
      // triger an error with the human readable form of the restrictions as message.
      if (!$error && !$this->validateRestrictions($value["newplain"]))
      {
        $error = TRUE;
        triggerError($record, $this->fieldName(), $this->getRestrictionsText());
      }

      // new password?
      if (!$error && strlen($value["new"]) > 0)
      $record[$this->fieldName()]["hash"] = $record[$this->fieldName()]["new"];
    }

    /**
     * Check if the attribute is empty
     * @return true if it's empty
     */
    function isEmpty($postvars)
    {
      /* unfortunately we cannot check this here */
      return FALSE;
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="")
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

//    /**
//     * Generates a random password which isn't to bad to remember.
//     * @param $times number of syllables
//     * @return generated password string
//     * @todo makePassword does not respect the restrictions.
//     */
//    function makePassword($times = 2)
//    {
//      $vowels = array("b","c","d","f","g","h","j","k","m","n","p","q","r","s","t","v","w","x","z");
//      $consonants = array("a","e","y","u");
//      $out = "";
//      for ($i = 0; $i < $times; $i++)
//      {
//        srand((double)microtime()*1000000);
//        mt_srand((double)microtime()*1000000);
//        $out .= $vowels[rand()%count($vowels)];
//        $out .= $consonants[mt_rand()%count($consonants)];
//        $out .= $vowels[mt_rand()%count($vowels)];
//      }
//      return $out;
//    }
    function getRandomChars($chars, $count)
    {
      mt_srand((double)microtime()*1000000);
      $randomchars = "";
      for($i=0; $i<$count; $i++)
      {
        $randomchars .= substr($chars, mt_rand(0, strlen($chars)-1), 1);
      }
      return $randomchars;
    }

    /**
     * Generates a random password which isn't to bad to remember.
     * @param $times length of password divided by 3 (to maintain backwards compatibility)
     * @return generated password string
     */
    function makePassword($times = 2)
    {
      // Use short notation
      $r = $this->m_restrictions;

      // Compose a string that meets the minimum requirements
      $tmp = $this->getRandomChars(LOWERCHARS, $r["minlowerchars"]);
      $tmp.= $this->getRandomChars(UPPERCHARS, $r["minupperchars"]);
      $alphabeticchars = ($r["minalphabeticchars"] > strlen($tmp)) ? ($r["minalphabeticchars"] - strlen($tmp)) : 0;
      $tmp.= $this->getRandomChars(LOWERCHARS.UPPERCHARS, $alphabeticchars);
      $tmp.= $this->getRandomChars(NUMBERS, $r["minnumbers"]);
      $tmp.= $this->getRandomChars(SPECIALCHARS, $r["minspecialchars"]);
      $remainingchars = max($r["minsize"], $times * 3) > strlen($tmp) ? max($r["minsize"], $times * 3) : 0;
      $tmp.= $this->getRandomChars(LOWERCHARS.UPPERCHARS.NUMBERS.SPECIALCHARS, $remainingchars);

      // Shuffle the password's characters and return it
      return str_shuffle($tmp);
    }

    /**
    * Overwriting the fetchValue to ensure all passwords are hashed
    */
    function fetchValue($rec)
    {
      if ($rec[$this->fieldName()]["current"])
      {
        $rec[$this->fieldName()]["current"] = md5($rec[$this->fieldName()]["current"]);
      }
      if ($rec[$this->fieldName()]["new"])
      {
        $rec[$this->fieldName()]["newplain"] = $rec[$this->fieldName()]["new"];
        $rec[$this->fieldName()]["new"] = md5($rec[$this->fieldName()]["new"]);
      }
      if ($rec[$this->fieldName()]["again"])
      {
        $rec[$this->fieldName()]["again"] = md5($rec[$this->fieldName()]["again"]);
      }
      return $rec[$this->fieldName()];
    }


    // Due to the new storeType functions
    // password field is not allways saved from within the password attrib
    //
    // Added a "dynamic" needsUpdate to cancel updates if no password fields where used
    // to alter the password. This overcomes the overwriting with an empty password.
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
