<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage utils
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Generic string parser.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   *
   */
  class atkStringParser
  {
    var $m_fields = array();
    var $m_string = "";

    /**
     * Create a new stringparser
     */
    function atkStringParser($string)
    {
      $this->m_string = $string;
    }

    /**
     * Parse data into the string.
     * @param array The data to parse in the string
     * @param bool Wether or not to do a rawurlencode
     * @return String The parsed string
     */
    function parse($data, $encode=false)
    {
      $fields = $this->getFields();

      $string = $this->m_string;

      for ($i=0;$i<count($fields);$i++)
      {
        $elements = split("\.",$fields[$i]);
        $databin = $data;
        for($j=0;$j<count($elements);$j++)
        {
          if (is_array($databin) && !array_key_exists($elements[$j],$databin)) $databin[$elements[$j]]='';
          $value = $databin[$elements[$j]];
          $databin = $databin[$elements[$j]];
        }
        if ($encode)
        {
          $string = str_replace("[".$fields[$i]."]",rawurlencode($value),$string);
        }
        else
        {
          $string = str_replace("[".$fields[$i]."]",$value,$string);
        }
      }
      return $string;
    }

    /**
     * Does the data contains everything needed to be parsed into the string?
     */
    function isComplete($data)
    {
      $fields = $this->getFields();
      for ($i=0;$i<count($fields);$i++)
      {
        $elements = split("\.",$fields[$i]);
        $databin = $data;
        for($j=0;$j<count($elements);$j++)
        {
          $value = $databin[$elements[$j]];
          if (!isset($value)) return false; // Missing value.
          $databin = $databin[$elements[$j]];
        }
        if (!isset($value)) return false; // Missing value.
      }
      return true;
    }

    /**
     * Get the [ ] Fields out of a String
     */
    function getFields()
    {
      if (!count($this->m_fields))
      {
        $tmp = "";
        $adding = false;
        for ($i=0;$i<strlen($this->m_string);$i++)
        {
          if ($this->m_string[$i]=="]")
          {
            $adding = false;
            $this->m_fields[] = $tmp;
            $tmp="";
          }
          else if ($this->m_string[$i]=="[")
          {
            $adding = true;
          }
          else
          {
            if ($adding) $tmp.=$this->m_string[$i];
          }
        }
      }
      return $this->m_fields;
    }


    /**
     * Same as getFields but if a relation is referenced using
     * a dot only returns the attribute name before the dot.
     *
     * @return array attributes used in template
     */
    function getAttributes()
    {
      $attrs = array();

      $fields = $this->getFields();
      foreach ($fields as $field)
      {
        list($attr) = explode('.', $field);
        $attrs[] = $attr;
      }

      return $attrs;
    }
  }
?>
