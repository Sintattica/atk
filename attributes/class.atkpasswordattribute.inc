<?php
/**
 * The atkPasswordAttribute class represents an attribute of a node
 * that is a password field. It automatically encrypts passwords
 * with the MD5 method of PHP. To update a password a user has to
 * supply the old password first, unless you use the special created
 * AF_PASSWORD_NOVALIDATE flag, in which case the password just gets
 * overwritten without any check.
 *
 * @author Peter Verhage (peter@ibuildings.nl)
 *
 * $Id$
 * $Log$
 * Revision 4.3  2001/07/15 17:42:39  ivo
 * Disabled extended search for passwordattribute.
 *
 * Revision 4.2  2001/06/26 14:42:09  ivo
 * Passwordattribute was not session save, if you didn't change your password
 * and followed a link and came back, your password would be set to a double
 * hashed value.
 *
 * Revision 4.1  2001/05/17 10:27:12  ivo
 * Fixes to session management, new AF_LARGE feature for manytoonerelation.
 * Improved handling of GET_VARS and POST_VARS (requires a new dispatcher)
 *
 * Revision 4.0  2001/04/23 10:20:39  ivo
 * revision tags.
 *
 * Revision 1.1.1.1  2001/04/23 10:17:10  ivo
 * Added atk4 to repository.
 *
 * Revision 1.9  2001/03/16 08:47:56  ivo
 * Had to disable invalid password check because it went wrong when
 * AF_HIDE_ADD was set for a password attribute.
 *
 * Revision 1.8  2001/03/15 19:01:07  peter
 * forgot about the AF_PASSWORD_NOVALIDATE flag, fixed update bug with empty passwords
 *
 * Revision 1.7  2001/03/15 18:29:58  peter
 * fixed issue with empty password at update
 *
 * Revision 1.6  2001/03/14 16:16:30  sandy
 * database name and userid field is configurable
 *
 * Revision 1.5  2001/03/14 12:23:23  peter
 * added hide method, to reflect Ivo's changes
 *
 * Revision 1.4  2001/02/22 22:46:18  peter
 * changed edit/search API, new member vars, clean-up of code
 *
 * Revision 1.3  2001/02/16 17:53:40  peter
 * fixed some minor bug that prevented a user from updating a password field more then once
 *
 */
class atkPasswordAttribute extends atkAttribute
{
  /**
   * Constructor
   * @param $name Name of the attribute
   * @param $flags Flags for this attribute
   */
  function atkPasswordAttribute($name, $flags=0)
  {
    /* base class constructor */
    $this->atkAttribute($name, $flags|AF_HIDE_SEARCH); // you can't search by password.
  }

  /**
   * Returns a piece of html code that can be used in a form to edit this
   * attribute's value.
   * @param $record array with fields
   * @return piece of html code with a textarea
   */
  function edit($record="")
  {
    global $ATK_VARS;    
    
    $action = $ATK_VARS["atkaction"];
   
    /* insert */
    if ($action != 'edit' && $action != 'update')
    {
      $result = text("password_new").':<br>'.
                '<input type="password" name="'.$this->fieldName().'[new]"'.
                ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '')."><br><br>".                          
                
                text("password_again").':<br>'.
                '<input type="password" name="'.$this->fieldName().'[again]"'.
                ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').">";                                       
    }
    
    /* edit */
    else
    {
    //var_dump($record[$this->fieldName()]["hash"]);
      $result = '<input type="hidden" name="'.$this->fieldName().'[hash]"'.
                ' value="'.$record[$this->fieldName()]["hash"].'">';


      if (!$this->hasFlag(AF_PASSWORD_NOVALIDATE))
        $result .= text("password_current").':<br>'.
                   '<input type="password" name="'.$this->fieldName().'[current]"'.
                   ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                   ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>';
                
      $result .= text("password_new").':<br>'.
                 '<input type="password" name="'.$this->fieldName().'[new]"'.
                 ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').
                 ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'><br><br>'.
                
                 text("password_again").':<br>'.
                 '<input type="password" name="'.$this->fieldName().'[again]"'.
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
   * Fetch values
   * @param $rec array with values
   * @return String without slashes
   */
  function fetchValue($rec)
  {
    global $ATK_VARS;    

    $action = $ATK_VARS["atkaction"];
    //if ($action == 'update' && strlen($rec[$this->fieldName()]["new"]) == 0 && strlen($rec[$this->fieldName()]["again"]) == 0)
    if (strlen($rec[$this->fieldName()]["new"]) == 0 && strlen($rec[$this->fieldName()]["again"]) == 0)
      return $rec[$this->fieldName()];
    else return array("hash" => md5($rec[$this->fieldName()]["new"]), 
                      "new"=>$rec[$this->fieldName()]["new"],
                      "again"=>$rec[$this->fieldName()]["again"]);
  }

  /**
   * Validates the supplied passwords 
   * @param $value array with password values
   * @return empty string or error message in string
   */
  function validate($value)
  {
    global $ATK_VARS;    

    $action = $ATK_VARS["atkaction"];

//    if ($action == 'save' && (strlen($value["new"]) == 0 || strlen($value["again"]) == 0))
//      return text("password_invalid");
      
    if ($action == 'update' && (strlen($value["new"]) > 0 || strlen($value["again"]) > 0) &&
        !$this->hasFlag(AF_PASSWORD_NOVALIDATE) && md5($value["current"]) != $value["hash"])
      return text("password_incorrect");
    
    //var_dump($value);
    if ($value["new"] != $value["again"])
      return text("password_nomatch");
    
    else return "";
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
}
?>
