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
   * @version $Revision: 6320 $
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
     * 
     * @param string $string The string to parse
     */
    function atkStringParser($string)
    {
      $this->m_string = $string;
    }

    /**
     * Parse data into the string.
     * 
     * @param array $data                The data to parse in the string
     * @param bool  $encode              Wether or not to do a rawurlencode
     * @param bool  $ignoreUnknownFields Replace unknown fields with an empty string,
     *                                   if set to false unknown fields will be left
     *                                   untouched.
     * 
     * @return String The parsed string
     */
    function parse($data, $encode=false, $replaceUnknownFields=true)
    {
      $string = $this->m_string;
      
      $fields = $this->getFields();
      foreach ($fields as $field)
      {
      	$value = $data;
      	
        $elements = explode(".", $field);
        foreach ($elements as $i => $el)
        {
          if (is_array($value) || $value instanceof ArrayAccess)
          {
          	if (isset($value[$el]))
          	{
          		$value = $value[$el];
          	}
          	else if ($replaceUnknownFields)
          	{
              atknotice("atkStringparser({$this->m_string})->parse(): Attempting to get element '{$el}', but {$elements[$i - 1]} is not an array!");
              $value = '';
              break;
          	}
           	else
           	{
           		// field not found, continue with next field without 
           		// replacing the field in the template
           		continue 2;
           	}
          }
          else if (!$replaceUnknownFields)
          {
            // field not found, continue with next field without 
            // replacing the field in the template          	
          	continue 2;
          }
        }
        
        if ($encode)
        {
        	$value = rawurlencode($value);
        }
        
        $string = str_replace("[".$field."]", $value, $string);
      }
      
      return $string;
    }

    /**
     * Does the data contains everything needed to be parsed into the string?
     * 
     * @param array $data
     */
    function isComplete($data)
    {
      $fields = $this->getFields();
      for ($i=0;$i<count($fields);$i++)
      {
        $elements = explode(".",$fields[$i]);
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
     * Get all fields from a string
     *
     * <b>Example:</b>
     *        string: [firstname], [lastname] [city]
     *        would return array('[firstname]',', ','[lastname]',' ','[city]')
     *        
     * @return array
     */
    function getAllFieldsAsArray()
    {
      $matches = array();
      preg_match_all("/\[[^\]]*\]|[^[]+/",$this->m_string,$matches);
      return $matches;
    }

    /**
     * Parse data into the string and return all fields as an array
     *
     * @param array $data
     * @param boolean $split_tags_and_fields return fields and separators separated in resultarray (separators are not used in query, so quotes aren't used)
     * @return array
     */
    function getAllParsedFieldsAsArray($data, $split_tags_and_fields = false)
    {
      $matches = $this->getAllFieldsAsArray();
      atk_var_dump($matches,"MATCHES".($split_tags_and_fields?" (split tags and separators)":""));
            
      $fields=array();
      if(is_array($matches))
      {
        foreach ($matches[0] as $match)
        {
          // Check if need to parse the match
          if(strpos($match,'[')!==false && strpos($match,']')!==false)
          {
            $parser = new atkStringParser($match);
            
            if ($split_tags_and_fields)
            {
              $fields['tags'][] = $parser->parse($data);
            }
            else
            {
              $fields[] = $parser->parse($data);
            }
          }
          else
          {
            if ($split_tags_and_fields)
            {
              $fields['separators'][] = $match;
            }
            else
            {
              $fields[] = "'".$match."'";
            }
          }
        }
      }
      return $fields;
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
