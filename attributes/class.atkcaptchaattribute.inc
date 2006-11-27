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
   * @version
   * 
   */

  atkimport("atk.attributes.atkattribute");
  
  /**
   * With the atkCaptchaAttribute class you can easily add a captcha to a forum 
   * or guestbook.
   *
   * Use the flag AF_NOLABEL if you want to start at the beginning of the
   * line.
   *
   * @author Nico de Boer <nico@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkCaptchaAttribute extends atkAttribute 
  {

    /**
     * Constructor
     * @param $name   The name of the attribute
     * @param $flags  The attribute flags
     * @return -
     */
    function atkCaptchaAttribute($name, $flags=0) 
    {
    	// A Captcha attribute should not be searchable and sortable
      $flags |= AF_HIDE_SEARCH|AF_NO_SORT;

      $this->atkAttribute($name,$flags); // base class constructor
    }

    /**
     * Edit  record
     * Here it will only return the text, no edit box.
     * @param $record Array with fields
     * @param $fieldprefix 
     * @return Parsed string
     */
    function edit($record="", $fieldprefix="")
    {
      $html =  '<img src="'.atkConfig::get("captcha", "captcha_dir") . 'img/captcha.jpg.php'.'"><br>';
      $html .= '<br>';
      $html .= '<small>'.atktext("captcha_explain", "atk").'</small><br>';
      $html .= '<input type="text" name="'.$fieldprefix.$this->fieldName().'">';
      return $html;
    }
    
   /**
    * VOID implementation.. parserAttribute has no data associated with it, so you can't search it.
    * @param $record Array with fields
    */
    function search($record="", $size=0, $maxsize=0)
    {
      return "&nbsp;";
    }

   /**
    * Display a record
    * Here it will only return the captcha.
    * @param $record  Array with fields
    * @return Parsed string
    */
    function display($record)
    {
      // create new image 
      return "";
    }
    
    /**
    * No function, but is neccesary
    */
    function store($a, $b)
    {
      return true;
    }

    function db2value($record)
    {
      return NULL;
    }

    /**
    * No function, but is neccesary
    */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="",$level)
    {
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

    function dbFieldType()
    {
      return "";
    }
    
    function validate(&$record, $mode)
    {
      $sCaptchaCode = $record["captcha"];
      if (md5(strtoupper($sCaptchaCode)) != $_SESSION['php_captcha'])
      {
        triggerError($record, $this->fieldName(), 'error_obligatoryfield');
      }

      // clear to prevent re-use
      $_SESSION['php_captcha'] = '';
    }
  }

?>
