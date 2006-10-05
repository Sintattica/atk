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
  
  atkimport("atk.attributes.atknumberattribute");

  define('AF_WEEKDAY_SMALL_EDIT', AF_SPECIFIC_1);
  
  define('WD_MONDAY',     1);
  define('WD_TUESDAY',    2);
  define('WD_WEDNESDAY',  4);
  define('WD_THURSDAY',   8);  
  define('WD_FRIDAY',    16);  
  define('WD_SATURDAY',  32);  
  define('WD_SUNDAY',    64);       
  
  /**
   * Attribute for selection the days of the week.
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkWeekdayAttribute extends atkNumberAttribute
  {
    var $m_mapping = array(1 => 'monday', 2 => 'tuesday', 4 => 'wednesday', 8 => 'thursday', 16 => 'friday', 32 => 'saturday', 64 => 'sunday');
    
    var $m_extra = array();
    
    /**
     * Constructor.
     *
     * @param String $name Name of the attribute (unique within a node, and
     *                     corresponds to the name of the datetime field 
     *                     in the database where the stamp is stored.
     * @param int $extraOrFlags Flags for the attribute or array of extra options
     *                           these options will be numbered from 2^7 (128) to 2^x.
     * @param int $flags        Flags for the attribute. Only used if no set in previous param.
     */
    function atkWeekdayAttribute($name, $extraOrFlags=0, $flags=0)
    { 
      
      if (is_numeric($extraOrFlags))
        $flags = $extraOrFlags;   
      elseif (is_array($extraOrFlags))
        $this->m_extra = $extraOrFlags;
        
      $this->atkNumberAttribute($name, ($flags|AF_HIDE_SEARCH)^AF_SEARCHABLE);
    }
    
    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * For the regular atkAttribute, this means getting the field with the
     * same name as the attribute from the html posting.
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     * @return String The internal value
     */
    function fetchValue($postvars)
    {
      if (!is_array($postvars) || !is_array($postvars[$this->fieldName()])) return 0;
      else return array_sum($postvars[$this->fieldName()]);
    }    
    
    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="", $fieldprefix="", $mode='add')
    {
      $result = ''; 

      $name = $fieldprefix.$this->fieldName();
      $value = (int)$record[$this->fieldName()];

      $separator = $this->hasFlag(AF_WEEKDAY_SMALL_EDIT) || $mode == 'list' ? '&nbsp;' : '<br>';      
      
      $max = 7 + count($this->m_extra);
      for ($i = 1; $i <= $max; $i++)
      {
        $day = pow(2, $i-1);
        
        if ($i <= 7)
          $weekday = atktext($this->m_mapping[$day]);
        else $weekday = $this->m_extra[$i - 8];

        $weekday = ucfirst($weekday);
        $fullWeekday = $weekday;
        if ($this->hasFlag(AF_WEEKDAY_SMALL_EDIT) || $mode == 'list')
          $weekday = substr($weekday, 0, 2);
        
        $checked = hasFlag($value, $day) ? ' checked' : '';
        
        $result .= '<span title="'.$fullWeekday.'"><input type="checkbox" id="'.$name.'" name="'.$name.'['.$i.']" value="'.$day.'" '.$checked.'> '.$weekday.'</span>'.($i < $max ? $separator : '');
        $this->registerKeyListener($name.'['.$i.']', KB_CTRLCURSOR|KB_CURSOR);        
      }

      return $result;
    }    
    

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * In this case, the timestamp is returned in human readable format.
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list" 
     *                     for displaying in recordlists). The regular
     *                     atkAttribute does not use this parameter, but
     *                     derived attributes may use it to distinguish
     *                     between the two display modes.
     * @return String HTML String 
     */
    function display($record, $mode="list")
    {
      $result = '';      
      $value = (int)$record[$this->fieldName()];
      
      $max = 7 + count($this->m_extra);      
      for ($i = 1; $i <= $max; $i++)
      {
        $day = pow(2, $i-1);
        
        if (hasFlag($value, $day))
        {
          if ($i <= 7)
          {
            $weekday = $this->m_mapping[$day];
            if ($mode == 'list') 
              $weekday = substr($weekday, 0, 3);
            $weekday = atktext($weekday);
          }
          else 
          {
            $weekday = $this->m_extra[$i - 8];
          }
          
          $result .= (empty($result) ? '' : ($mode == 'list' ? ', ' : '<br>')).$weekday;
        }
      }
      
      if (empty($result))
        return atktext('none');
      else return $result;
    }
  }
?>