<?php
/**
 * The atkPasswordAttribute class represents an attribute of a node
 * that is a password field. It automatically encrypts passwords
 * with the MD5 method of PHP. To update a password a user has to
 * supply the old password first, unless you use the special created
 * AF_PASSWORD_NOVALIDATE flag, in which case the password just gets
 * overwritten without any check.
 *
 * @author Peter Verhage (peter@achievo.org)
 * @version $Revision$
 *
 * $Id$
 *
 */
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
  function atkPasswordAttribute($name, $generate, $flags=0)
  {
    /* compatiblity with old versions */
    if (func_num_args() == 3) $this->m_generate = $generate;
    else
    {
      $flags = $generate;
      $this->m_generate = FALSE;
    }
    $this->atkAttribute($name, $flags|AF_HIDE_SEARCH); // you can't search by password.
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
    /* insert */
    if ($mode != 'edit' && $mode != 'update')
    {
      if (!$this->m_generate)
      {
        $result = text("password_new").':<br>'.
                  '<input type="password" name="'.$fieldprefix.$this->fieldName().'[new]"'.
                  ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                  ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '')."><br><br>".

                  text("password_again").':<br>'.
                  '<input type="password" name="'.$fieldprefix.$this->fieldName().'[again]"'.
                  ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                  ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').">";
      }
      else
      {
        $password = $this->makePassword();
        $result = '<input type="hidden" name="'.$fieldprefix.$this->fieldName().'[again]"'.
                  ' value ="'.$password.'">'.
                  '<input type="text" name="'.$fieldprefix.$this->fieldName().'[new]"'.
                  ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                  ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').' value ="'.$password.'" onchange="this.form.elements[\''.$fieldprefix.$this->fieldName().'[again]\'].value=this.value">';
      }
    }

    /* edit */
    else
    {
      $result = '<input type="hidden" name="'.$fieldprefix.$this->fieldName().'[hash]"'.
                ' value="'.$record[$this->fieldName()]["hash"].'">';


      if (!$this->hasFlag(AF_PASSWORD_NOVALIDATE))
        $result .= text("password_current").':<br>'.
                   '<input type="password" name="'.$fieldprefix.$this->fieldName().'[current]"'.
                   ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                   ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>';

      $result .= text("password_new").':<br>'.
                 '<input type="password" name="'.$fieldprefix.$this->fieldName().'[new]"'.
                 ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                 ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>'.

                 text("password_again").':<br>'.
                 '<input type="password" name="'.$fieldprefix.$this->fieldName().'[again]"'.
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
    if(strlen($rec[$this->fieldname()]["new"])>0)
    {
      // Use new password
      return addslashes(md5($rec[$this->fieldName()]["new"]));
    }
    else
    {
      // Use old hash
      return addslashes($rec[$this->fieldName()]["hash"]);
    }
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
   * Fetch values
   * @param $rec array with values
   * @return String without slashes
   */
  function fetchValue($rec)
  {
    if (strlen($rec[$this->fieldName()]["new"]) == 0 && strlen($rec[$this->fieldName()]["again"]) == 0)
      return $rec[$this->fieldName()];
    else return array("hash" =>$rec[$this->fieldName()]["hash"],
                      "new"=>$rec[$this->fieldName()]["new"],
                      "again"=>$rec[$this->fieldName()]["again"],
                      "current"=>$rec[$this->fieldname()]["current"]
                      );
  }

  /**
   * Validates the supplied passwords
   * @param &$record Record that contains value to be validated.
   *                 Errors are saved in this record
   * @param $mode can be either "add" or "update"
   * @return $record
   */
  function validate($record, $mode)
  {
    $value = $record[$this->fieldName()];

    if ($mode == 'update'
        && (strlen($value["new"]) > 0 || strlen($value["again"]) > 0)
        && !$this->hasFlag(AF_PASSWORD_NOVALIDATE)
        && md5($value["current"]) != $value["hash"])
          triggerError($record, $this->fieldName(), 'error_password_incorrect');

    if ($value["new"] != $value["again"])
      triggerError($record, $this->fieldName(), 'error_password_nomatch');
  }


  /**
   * Returns a piece of html code that can be used in a form to display
   * hidden values for this attribute.
   * @param $record Array with values
   * @return Piece of htmlcode
   */
  function hide($record="")
  {
    return "";
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

  function getSearchModes()
  {
    // exact match and substring search should be supported by any database.
    // (the LIKE function is ANSI standard SQL, and both substring and wildcard
    // searches can be implemented using LIKE)
    // Possible values
    //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
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
}
?>