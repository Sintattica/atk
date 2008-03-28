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

  /**
   * @internal Includes
   */
  atkimport("atk.attributes.atkdateattribute");
  atkimport("atk.attributes.atktimeattribute");

  /**
   * The atkDateTimeAttribute class can be used for date and time entry.
   * It corresponds to a DATETIME field in the database.
   *
   * @author Sandy Pleyte <sandy@achievo.org>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkDateTimeAttribute extends atkAttribute
  {
    var $m_time = "";
    var $m_date = "";

    var $m_utcOffset = null;
    var $m_timezoneAttribute = null;


    /**
     * Converts a date array to a timestamp
     * year, month, day are obligatory !!
     *
     * @param array $dateArray Date Array
     * @return int Timestamp
     */
    function arrayToDateTime($dateArray)
    {
      $hour = 0;
      $min = 0;
      $sec = 0;
      $dateValid = true;

      if(!empty($dateArray["hours"])) $hour = $dateArray["hours"];
      if(!empty($dateArray["minutes"])) $min = $dateArray["minutes"];
      if(!empty($dateArray["seconds"])) $sec = $dateArray["seconds"];
      if(!empty($dateArray["day"])) $day = $dateArray["day"];
      else $dateValid = false;
      if(!empty($dateArray["month"])) $month = $dateArray["month"];
      else $dateValid = false;
      if(!empty($dateArray["year"])) $year = $dateArray["year"];
      else $dateValid = false;

      if($dateValid) return adodb_mktime($hour,$min,$sec,$month,$day,$year);
      else return adodb_mktime(0,0,0);
    }

    /**
     * Constructor
     *
     * @todo I don't know who added the default_date and default_time parameters,
     *       but at least the atkDateAttribute doesn't support a default date. if
     *       you want to set a default date you should use initial_values anyways
     *       so I think the default_date / default_time parameters should be removed.
     *       But can we keep this backwards compatible somehow? At least now we
     *       are certain it doesn't work. ;) (PCV)
     *
     * @param $name        Name of the attribute
     * @param $default_date   start date
     * @param $default_time     start time
     * @param $flags Flags for this attribute
     */
    function atkDateTimeAttribute($name, $default_date="",$default_time="",$flags=0)
    {
      $default_steps = array();
      for($i=0;$i<60;$i++)
      {
        $default_steps[$i] = $i;
      }

      if (is_numeric($default_date))
      {
        $flags = $default_date;
        $default_date = "";
      }

      if($default_date=="") { $default_date = ""; }
      if($default_time=="") { $default_time = ""; }

      $this->m_time = new atkTimeattribute($name,0,23,$default_steps,$default_time, $flags);
      $this->m_date = new atkDateattribute($name,'','',0,0,$flags);

      $this->atkAttribute($name,$flags); // base class constructor

      if ($this->hasFlag(AF_OBLIGATORY))
      {
       $this->setInitialValue(atkDateTimeAttribute::datetimeArray());
      }
    }

    function setDateMin($min=0)
    {
      $this->m_date->setDateMin($min);
    }

    function setDateMax($max=0)
    {
      $this->m_date->setDateMax($max);
    }

    function validate(&$record, $mode)
    {
      //if the datetime string is not an array, make it one to make sure the
      //validation functions of atkDateAttribute and atkTimeAttribute do not
      //cripple the data.
      if(!is_array($record[$this->fieldName()]))
      {
        $stamp = strtotime($record[$this->fieldName()]);
        $record[$this->fieldName()] = $this->datetimeArray(date("YmdHi",$stamp));
      }

      $this->m_date->validate($record,$mode);
      $this->m_time->validate($record,$mode);
    }

    /**
     * Converts a date/time string (YYYYMMDDHHMISS) to an
     * array with 5 fields (day, month, year, hours, minutes, seconds).
     * Defaults to current date/time.
     *
     * @param $datetime the time string
     * @return array with 6 fields (day, month, year, hours, minutes, seconds)
     */
    function datetimeArray($datetime=NULL)
    {
      if ($datetime == NULL)
        $datetime = date("YmdHis");
      $date = substr($datetime, 0, 8);
      $time = substr($datetime, 8, 6);
      return array_merge(atkDateAttribute::dateArray($date), atkTimeAttribute::timeArray($time));
    }

    function init()
    {

      $this->m_time->m_owner=$this->m_owner;
      $this->m_date->m_owner=$this->m_owner;
      $this->m_time->m_ownerInstance=&$this->m_ownerInstance;
      $this->m_date->m_ownerInstance=&$this->m_ownerInstance;
    }


    function fetchMeta($metadata)
    {
      $this->m_date->fetchMeta($metadata);
      $this->m_time->fetchMeta($metadata);
    }

    /**
     * Display's html version of Record
     * @param $record
     * @return text string of $record
     */
    function display($record, $mode="")
    {
      $date = $this->m_date->display($record, $mode);
      $time = $this->m_time->display($record, $mode);
      if($date!='' && $time!='')
        return $date.(($mode=="csv"||$mode=="plain")?" ":"&nbsp;").$time;
      else
        return "";
    }

    function fetchValue($postvars)
    {
      $date = $this->m_date->fetchValue($postvars);
      if ($date == NULL) return NULL;

      $time = $this->m_time->fetchValue($postvars);
      if ($time == NULL)
        $time = array('hours' => '00', 'minutes' => '00', 'seconds' => '00');

      return array_merge($date, $time);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="", $fieldprefix="")
    {
      $dateEdit = $this->m_date->edit($record,$fieldprefix);
      $timeEdit = $this->m_time->edit($record,$fieldprefix);
      return $dateEdit."&nbsp;&nbsp;-&nbsp;&nbsp;".$timeEdit;
    }

    function value2db($rec)
    {
      if (is_array($rec[$this->fieldName()]))
      {
        $value = $rec[$this->fieldName()];
        $value = $this->toUTC($value, $rec);
        $rec[$this->fieldName()] = $value;

        $date = $this->m_date->value2db($rec);
        $time = $this->m_time->value2db($rec);

        if ($date != NULL && $time != NULL)
          return $date." ".$time;
      }
      else if (!empty($rec[$this->fieldName()]))
      {
        $stamp = strtotime($rec[$this->fieldName()]);
        $stamp = $this->toUTC($stamp, $rec);
        return date('Y-m-d H:i:s',$stamp);
      }
     return NULL;
    }

    /**
     * Convert database value to datetime array
     * @param $rec database record with date field
     * @return array with 3 fields (hours:minutes:seconds)
     */
    function db2value($rec)
    {
      if(isset($rec[$this->fieldName()]) && $rec[$this->fieldName()] != NULL)
      {
        /**
         * @todo Fix handling of 0 and NULL db values in the date, time and datetime attributes
         * Currently the date attribute gives an empty string when parsing 0000-00-00,
         * the time attribute gives an array with all three values set to 00,
         * and the datetimeattribute gives an empty string now (previously it gave a php warning
         * because it was trying to array_merge the empty string from the date attribute with the
         * array of the time attribute).
         */
        if ($rec[$this->fieldName()] == "0000-00-00 00:00:00")
          return "";

        $datetime = split(" ",$rec[$this->fieldname()]);

        $tmp_rec = $rec;
        $tmp_rec[$this->fieldname()]=$datetime[0];
        $result_date=$this->m_date->db2value($tmp_rec);
        if ($result_date == NULL) return NULL;

        $tmp_rec = $rec;
        $tmp_rec[$this->fieldname()]=$datetime[1];
        $result_time = $this->m_time->db2value($tmp_rec);
        if ($result_time == NULL)
          $result_time = array('hours' => '00', 'minutes' => '00', 'seconds' => '00');

        $value = array_merge((array)$result_date,(array)$result_time);
        $value = $this->fromUTC($value, $tmp_rec);
        return $value;
      }
      else return NULL;
    }

    /**
     * Add's fields to query
     * @param $query The SQL query
     * @param $tablename Tablename
     * @param $fieldaliasprefix Field alias prefix
     * @param $level ?
     * @param $mode select/admin/copy/edit
     * @return New SQL Query
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode == "add" || $mode == "update")
      {
        if($this->value2db($rec) == NULL)
        {
          $query->addField($this->fieldName(),'NULL','','',false);
        }
        else
        {
          $db = &$this->m_ownerInstance->getDb();
          if ($db->getType() != 'oci9')
            $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
          else
          {
            $value = $this->value2db($rec);
            $query->addField($this->fieldName(),$value,"","",!$this->hasFlag(AF_NO_QUOTES), $mode, "DATETIME");
          }
        }
      }
      else
      {
        if (atkconfig('database') != 'oci9')
          $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
        else
        {
          $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES), $mode, "DATETIME");
        }
      }
    }

    /**
     * Returns a piece of html code that can be used in a form to search values
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function search($record="", $extended=false, $fieldprefix="")
    {
     return $this->m_date->search($record, $extended, $fieldprefix);
    }

    function searchCondition(&$query, $table, $value, $searchmode)
    {
      $this->m_date->searchCondition($query, $table, $value, $searchmode);
    }


    function hide($record="", $fieldprefix="")
    {
      // we only need to return the date part, because the dateattribute also
      // hides the other (time) elements that are present in the record (is that
      // a bug of the dateattribute?)
      return $this->m_date->hide($record, $fieldprefix);
    }

    function getSearchModes()
    {
      return $this->m_date->getSearchModes();
    }

    function dbFieldType()
    {
      // TODO FIXME: Is this correct? Or does the datetimeattribute currently only support varchar fields?
      return "datetime";
    }

    function parseStringValue($stringvalue)
    {
      $datetime = split(" ",$stringvalue);
      $formatsdate = array("dd-mm-yyyy","dd-mm-yy","d-mm-yyyy","dd-m-yyyy","d-m-yyyy","yyyy-mm-dd","yyyy-mm-d","yyyy-m-dd","yyyy-m-d");
      $retval=array_merge(atkDateAttribute::parseDate($datetime[0],$formatsdate),
                          atkTimeAttribute::parseTime($datetime[1]));
      return $retval;
    }


    /**
     * Sets the timezone attribute. This can also be a timezone
     * attribute retrieved from a atkManyToOneRelation. If so then please
     * use the dot notation.
     *
     * @param string $attrName attribute name
     */
    function setTimezoneAttribute($attrName)
    {
      $this->m_timezoneAttribute = $attrName;
    }

    /**
     * Returns the timezone attribute name.
     *
     * @return string timezone attribute name
     */
    function getTimezoneAttribute()
    {
      return $this->m_timezoneAttribute;
    }

    /**
     * Sets the UTF offset in seconds.
     *
     * @param int UTC offset in seconds
     */
    function setUTCOffset($offset)
    {
      $this->m_utcOffset = $offset;
    }

    /**
     * Resets the UTC offset.
     */
    function resetUTCOffset()
    {
      $this->m_utcOffset = null;
    }

    /**
     * Returns the UTC offset if set.
     *
     * @return int UTC offset in seconds if set.
     */
    function getUTCOffset()
    {
      return $this->m_utcOffset;
    }

    /**
     * Returns the UTC offset in seconds. If the UTC offset is set explicitly
     * using the setUTCOffset method this offset is returned. Else if a timezone
     * attribute is set the offset is determined by looking at the timezone
     * using the given timezone attribute. If no offset and no attribute are set
     * an offset of 0 is returned.
     *
     * @param array $record record
     * @return int UTC offset in seconds
     */
    function _getUTCOffset(&$record, $stamp=null)
    {
      if ($this->m_utcOffset !== null)
      {
        return $this->m_utcOffset;
      }
      else if ($this->m_timezoneAttribute !== null)
      {
        $parts = explode('.', $this->m_timezoneAttribute);
        $node = $this->getOwnerInstance();

        while (count($parts) > 0)
        {
          $part = array_shift($parts);
          $attr = $node->getAttribute($part);

          // relation, prepare for next iteration
          if (is_a($attr, 'atkManyToOneRelation'))
          {
            if (count($parts) > 0 && !isset($record[$part][$parts[0]]))
            {
              $attr->populate($record, array($parts[0]));
            }

            $record = $record[$attr->fieldName()];
            $node = $attr->m_destInstance;
          }

          // timezone attribute, calculate and return offset
          else if (is_a($attr, 'atkTimezoneAttribute'))
          {
            return $attr->getUTCOffset($record[$attr->fieldName()], $stamp);
          }

          // assume the attribute in question already has the offset saved in seconds
          else
          {
            return (int)$record[$attr->fieldName()];
          }
        }

        atkdebug('WARNING: could not determine UTC offset for atkDateTimeAttribute "'.$this->fieldName().'"!');
        return 0;
      }
      else
      {
        return 0;
      }
    }

    /**
     * Convert the given ATK date/time array to a UTC date/time array.
     *
     * @param int|array $value UNIX timestamp or ATK date/time array
     * @param array $record record
     *
     * @return int|array UNIX timestamp or ATK date/time array (depending on input)
     */
    function toUTC($value, &$record)
    {
      $stamp = is_int($value) ? $value : $this->arrayToDateTime($value);
      $offset = $this->_getUTCOffset($record, $stamp);
      $stamp = $stamp - $offset;
      $value = is_int($value) ? $stamp : $this->datetimeArray(date("YmdHis", $stamp));
      return $value;
    }

    /**
     * Convert the given UTC ATK date/time array to a date/time array in a certain timezone.
     *
     * @param int|array $value UNIX timestamp or ATK date/time array
     * @param array $record record
     *
     * @return int|array UNIX timestamp or ATK date/time array (depending on input)
     */
    function fromUTC($value, &$record)
    {
      $stamp = is_int($value) ? $value : $this->arrayToDateTime($value);
      $offset = $this->_getUTCOffset($record, $stamp);
      $stamp = $stamp + $offset;
      $value = is_int($value) ? $stamp : $this->datetimeArray(date("YmdHis", $stamp));
      return $value;
    }

    /**
     * If a timezone attribute is set, make sure
     * it's always loaded.
     */
    function postInit()
    {
      if ($this->m_timezoneAttribute !== null)
      {
        $node = &$this->getOwnerInstance();
        $parts = explode('.', $this->m_timezoneAttribute);
        $attr = &$node->getAttribute($parts[0]);
        $attr->addFlag(AF_FORCE_LOAD);
      }
    }
  }
?>