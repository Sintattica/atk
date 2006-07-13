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
   * @copyright (c)2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *

   * @version $Revision$

   * $Id$

   */     

  /**
   * The atkAggregatedColumn aggregates multiple attributes to one colunm in
   * list view. The attribute displays and sorts according to the $template 
   * parameter and searches in fields, indicated in $searchfields array
   * parameter.
   * This attribute shows in recordlist only.
   *
   * @author Yury Golovnya <ygolovnya@kyiv.utel.com.ua>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkAggregatedColumn extends atkAttribute
  {
    /**
     * The display/sort template
     * @var String 
     * @access private
     */
    var $m_template;
    
    /**
     * The array with searchs fileds
     * @var Aray 
     * @access private
     */
    var $m_searchfields = array();
    
    /**
     * The array with displays fileds
     * @var Aray 
     * @access private
     */
    var $m_displayfields = array();
  	
    /**
     * Constructor
     * @param String $name         Name of the attribute
     * @param String $template     Display/sort template.
     * @param Array  $searchfields Array with fileds, in which search will be perform
     *                             If ommited, fields from $template will be used
     * @param $flags Flags for this attribute
     */
    function atkAggregatedColumn($name, $template, $flags=0, $searchfields = "")
    {
      $this->atkAttribute($name,$flags|AF_HIDE_EDIT|AF_HIDE_ADD|AF_HIDE_VIEW); // base class constructor
      $this->m_template = $template;
      
      atkimport("atk.utils.atkstringparser");
      $parser = new atkStringParser($template);
      $this->m_displayfields = $parser->getFields();
      
      if(!is_array($searchfields)) $this->m_searchfields = $this->m_displayfields;
      else $this->m_searchfields = $searchfields;
    }
  
  

    function display($record, $mode="")

    {
      $rec = array();
      foreach ($this->m_displayfields as $field)
      {

        $p_attrib = $this->m_ownerInstance->getAttribute($field);

        $rec[$field] = $p_attrib->display($record, $mode);

      }				
      atkimport("atk.utils.atkstringparser");
      $parser = new atkStringParser($this->m_template);
      return $parser->parse($rec);
    }
    
    function addToListArrayHeader($action, &$arr, $fieldprefix, $flags, $atksearch, $atkorderby)
    {
      if (!$this->hasFlag(AF_HIDE_LIST) && !($this->hasFlag(AF_HIDE_SELECT) && $action == "select"))
      {
        $arr["heading"][$fieldprefix.$this->fieldName()]["title"] = text($this->fieldName(), $this->m_ownerInstance->m_type);
        
        if (!hasFlag($flags, RL_NO_SORT) && !$this->hasFlag(AF_NO_SORT))
        {
          $rec = array();
          foreach ($this->m_displayfields as $field)
          {
            $rec[] = $this->m_ownerInstance->m_table.".".$field;
          }				
          $order = implode(", ",$rec);
          if ($atkorderby == $order)
          {
            $order = implode(" DESC,",$rec);
            $order .= " DESC";
          }
          $arr["heading"][$fieldprefix.$this->fieldName()]["url"] =
            session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->m_ownerInstance->atkNodeType().'&atkaction='.$action.'&atkorderby='.rawurlencode($order));
        }
        
        if (!hasFlag($flags, RL_NO_SEARCH) && $this->hasFlag(AF_SEARCHABLE))
          $arr["search"][$fieldprefix.$this->fieldName()] = $this->search($atksearch);
      }
    }
  
    function store($a, $b)
    {
      return true;
    }
  
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      $allfields = atk_array_merge($this->m_displayfields,$this->m_searchfields);
      foreach ($allfields as $field)
      {
        $p_attrib = $this->m_ownerInstance->m_attribList[$field];
        $p_attrib->addToQuery($query, $tablename, $fieldaliasprefix,$rec, $level,$mode);
      }				
    }

    /**
     * Creates a searchcondition for the field,
     * was once part of searchCondition, however, 
     * searchcondition() also immediately adds the search condition.
     *
     * @param atkQuery $query     The query object where the search condition should be placed on
     * @param String $table       The name of the table in which this attribute
     *                              is stored
     * @param mixed $value        The value the user has entered in the searchbox
     * @param String $searchmode  The searchmode to use. This can be any one
     *                              of the supported modes, as returned by this
     *                              attribute's getSearchModes() method.
     */
    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      if ($this->m_searchmode) $searchmode = $this->m_searchmode;
           
      $func = $searchmode."Condition";
      if (method_exists($query,$func))
      {
        $searchconditions = array();
        foreach ($this->m_searchfields as $field)
        {
          $p_attrib = $this->m_ownerInstance->m_attribList[$field];
          $searchconditions[] = $query->$func($table.".".$p_attrib->fieldName(),$this->escapeSQL($value));
        }
        return "(".implode(" OR ",$searchconditions).")";
      }
      else
      {

        atkdebug("Database doesn't support searchmode '$searchmode' for ".$this->fieldName().", ignoring condition.");

      }
    }

    function getSearchModes()
    {
      return array("exact","substring","wildcard","regexp");
    }
    
    function dbFieldType()
    {
      return "";
    }
  }
  
?>