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

  /** flag(s) specific for atkPasswordAttribute */
  define("AF_PASSWORD_NOVALIDATE",      AF_SPECIFIC_1); // disables password check when editing password field
  define("AF_PASSWORD_NO_VALIDATE",     AF_SPECIFIC_1); // disables password check when editing password field  
  
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
  atkimport("atk.attributes.atkattribute");
  class atkPasswordAttribute extends atkAttribute
  {
    /* generate? */
    var $m_generate;

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $generate Generate password (boolean)
     * @param $flags Flags for this attribute
     */
    function atkPasswordAttribute($name, $generate, $flags=0, $size=0)
    {
      /* compatiblity with old versions */
      if (func_num_args() >= 3) $this->m_generate = $generate;
      else
      {
        $flags = $generate;
        $this->m_generate = FALSE;
      }
      $this->atkAttribute($name, $flags|AF_HIDE_SEARCH, $size); // you can't search by password.
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
          $result = text("password_new").':<br>'.
                    '<input type="password" id="'.$id.'[new]" name="'.$id.'[new]"'.
                    ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                    ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '')."><br><br>".
  
                    text("password_again").':<br>'.
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
          $result .= text("password_current").':<br>'.
                     '<input type="password" id="'.$id.'[current]" name="'.$id.'[current]"'.
                     ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                     ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>';
        }
        $this->registerKeyListener($id.'[new]', KB_CTRLCURSOR|KB_UPDOWN);  
        $this->registerKeyListener($id.'[again]', KB_CTRLCURSOR|KB_UPDOWN);  
        $result .= text("password_new").':<br>'.
                   '<input type="password" id="'.$id.'[new]" name="'.$id.'[new]"'.
                   ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                   ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>'.
  
                   text("password_again").':<br>'.
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
      return array("hash" => stripslashes($rec[$this->fieldName()]));
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
  
      /* new password? */
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
      return text("password_hidden");
    }
  
    /**
     * There can not be searched for passwords!
     */
    function getSearchModes()
    {
      return array();
    }
  
    /**
     * Generates a random password which isn't to bad to remember.
     * @param $times number of syllables
     * @return generated password string
     */
    function makePassword($times = 2)
    {
      $vowels = array("b","c","d","f","g","h","j","k","m","n","p","q","r","s","t","v","w","x","z");
      $consonants = array("a","e","y","u");
      for ($i = 0; $i < $times; $i++)
      {
        srand((double)microtime()*1000000);
        mt_srand((double)microtime()*1000000);
        $out .= $vowels[rand()%count($vowels)];
        $out .= $consonants[mt_rand()%count($consonants)];
        $out .= $vowels[mt_rand()%count($vowels)];
      }
      return $out;
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
      
      if ($value["new"] != "" || $value["hash"] != "")
      {
      	return true;
      }
      return false;
    }
    
  }
?>