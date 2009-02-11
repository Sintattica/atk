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
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /** flag(s) specific for atkDurationAttribute */
  define("AF_DURATION_STRING",              AF_SPECIFIC_1); // date must be entered as an english date string (strtotime)
  define("DURATIONFORMAT_TIME",0);
  define("DURATIONFORMAT_DECIMAL",1);

  /**
   * The atkDurationAttribute is an attribute for entering a length of time.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   */
  class atkDurationAttribute extends atkAttribute
  {
    /** member vars **/
    var $m_resolution_min;
    var $m_maxtime_min;


    /**
     * Default Constructor, sets up atkAttribute
     *
     * @param string $name The name of this attribute
     * @param string $resolution
     * @param string $maxtime
     * @param int $flags The falgs of this attribute
     * @param int $size This attributes size
     * @see atkAttribute
     */
    function atkDurationAttribute($name, $resolution="1m", $maxtime="10h", $flags=0, $size=0)
    {
      $this->atkAttribute($name,$flags, $size); // base class constructor
      $hms = substr($resolution,-1);
      $resolution = substr($resolution,0,-1);

      if (strtoupper($hms)=="H") $factor = 60;
      else $factor=1;

      $this->m_resolution_min = $resolution*$factor;

      $hms = substr($maxtime,-1);
      $maxtime = substr($maxtime,0,-1);
      if (strtoupper($hms)=="H") $factor = 60;
      else $factor=1;

      $this->m_maxtime_min = $maxtime*$factor;
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">)
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode with hidden form elements that post
     *                this attribute's value without showing it.
     */
    function hide($record, $fieldprefix="")
    {
      // hide as a parseable string
      $record[$this->fieldName()] = $this->_minutes2string($record[$this->fieldName()]);
      return parent::hide($record, $fieldprefix);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value. (hours, minutes and seconds will be a dropdownbox)
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return String Piece a of HTML Code
     */
    function edit($record="", $fieldprefix="", $mode="")
    {
      $id = $fieldprefix.$this->formName();
      $fieldvalue = atkArrayNvl($record, $this->fieldName(), "");
      if(!$this->hasFlag(AF_DURATION_STRING))
      {
        $result = '';
        if ($this->m_maxtime_min>=60)
        {
          $curhours = $this->_getHourPart($fieldvalue);
          $curminutes = $this->_getMinutePart($fieldvalue);
          $result .= '<select id="'.$id.'_hours" name="'.$fieldprefix.$this->formName().'[hours]">';
          $this->registerKeyListener($id.'_hours', KB_CTRLCURSOR|KB_LEFTRIGHT);
          for ($h=0;$h<=$this->m_maxtime_min/60;)
          {
            $result .= '<option value="'.$h.'" ';
            if ($curhours==$h) $result.= "selected";
            $result .='>'.$h.' '.atktext('hours','atk');
            if ($this->m_resolution_min<=60)
            {
              $h++;
            }
            else
            {
              $h = floor($h+$this->m_resolution_min/60);
            }
          }
          $result .= '</select>';
        }

        if ($this->m_maxtime_min>=1&&$this->m_resolution_min<60)
        {
          $this->registerKeyListener($id.'_minutes', KB_CTRLCURSOR|KB_LEFTRIGHT);
          $result .= '&nbsp;<select id="'.$id.'_minutes" name="'.$fieldprefix.$this->formName().'[minutes]">';
          for ($m=0;$m<60||($this->m_maxtime_min<60 && $m<$this->m_maxtime_min);)
          {
            $result .= '<option value="'.$m.'" ';
            if ($curminutes==$m) $result.= "selected";
            $result .='>'.$m.' '.atktext('minutes','atk');
            if ($this->m_resolution_min<=1)
            {
              $m++;
            }
            else
            {
              $m = $m+$this->m_resolution_min;
            }
          }
          $result .= '</select>';
        }
      }
      else
      {
        $curval = ($fieldvalue > 0) ? $this->_minutes2string($fieldvalue) : '';
        $result = '<input type="text" name="'.$fieldprefix.$this->formName().
                '" value="'.$curval.'"'.
                ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'>';
      }
      return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to search values.
     * @param array $record Array of records, with for each record 2 fields (hours, minutes)
     * @return String Piece a of HTML Code
     */
    function search($record="")
    {
      return "&nbsp;"; // currently not searchable.
    }

    /**
     * This function displays the time
     *
     * The regular atkAttribute uses PHP's nl2br() and htmlspecialchars()
     * methods to prepare a value for display, unless $mode is "cvs".
     *
     * @param array $rec   The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list"
     *                     for displaying in recordlists, "edit" for
     *                     displaying in editscreens, "add" for displaying in
     *                     add screens. "csv" for csv files. Applications can
     *                     use additional modes.
     * @return String with YYYY-MM-DD
     */
    function display($rec, $mode="")
    {
      return $this->_minutes2string($rec[$this->fieldName()]);
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * @return array List of supported searchmodes
     */
    function getSearchModes()
    {
      return array("exact");
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return String The 'generic' type of the database field for this
     *                attribute.
     */
    function dbFieldType()
    {
      return "number";
    }

    /**
     * Convert the value into minutes
     *
     * @param string $value
     * @return integer with number of minutes
     */
    function _string2minutes($value)
    {
      if(strpos($value,':')===false)
      {
        // decimal format
        $tmp = explode('.',$value);
        if(strlen($tmp[1])==1) $tmp[1]=$tmp[1]*10;
        return $tmp[0]*60+$tmp[1]*(60/100);
      }
      else
      {
        // hh:mm format
        $tmp = explode(':',$value);
        return $tmp[0]*60+$tmp[1];
      }
    }

    /**
     * Convert minutes to string
     *
     * @param mixed $minutes
     * @return string with minutes
     */
    function _minutes2string($minutes)
    {
      $prefix = "";
      if($minutes<0)
      {
        $prefix="- ";
        $minutes = abs($minutes);
      }

      if(atkconfig("durationformat",0)==DURATIONFORMAT_DECIMAL)
      {
        $decimalvalue = $this->_getHourPart($minutes) + (self::_getMinutePart($minutes)/60);
        return $prefix.sprintf("%02.02f", $decimalvalue);
      }
      elseif(atkconfig("durationformat",0)==DURATIONFORMAT_TIME)
      {
        return $prefix.sprintf("%02d:%02d",self::_getHourPart($minutes), self::_getMinutePart($minutes));
      }
    }

    /**
     * Get the hour part from the number of minutes
     *
     * @param mixed $minutes
     * @return string with hours
     */
    function _getHourPart($minutes)
    {
      if(!is_array($minutes))
      {
        return floor($minutes/60);
      }
      else
      {
        return $minutes["hours"];
      }
    }

    /**
     * Get the minute part from the number of minutes
     *
     * @param mixed $minutes
     * @return string with minutes
     */
    function _getMinutePart($minutes)
    {
      if(!is_array($minutes))
      {
        return ($minutes-(floor($minutes/60)*60));
      }
      else
      {
        return $minutes["minutes"];
      }
    }

    /**
     * Fetch values
     * @param $rec Array with values
     * @return String without slashes
     */
    function fetchValue($rec)
    {
      if ($this->hasFlag(AF_DURATION_STRING))
      {
        return $this->_string2minutes($rec[$this->fieldName()]);
      }
      else
      {
        return $rec[$this->fieldName()]["hours"]*60 + $rec[$this->fieldName()]["minutes"];
      }
    }
    
    /**
     * Check if a record has an empty value for this attribute.
     * @param array $record The record that holds this attribute's value.
     * @return boolean
     */
    function isEmpty($record)
    {    
      return parent::isEmpty($record)||$record[$this->fieldName()]==0;
    }
  }
?>
