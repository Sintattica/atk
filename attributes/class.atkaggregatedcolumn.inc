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

   * @version $Revision: 6309 $

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
     * @param int $flags Flags for this attribute
     * @param Array  $searchfields Array with fields, in which search will be perform
     *                             If ommited, fields from $template will be used
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

    /**
     * The display function for this attribute
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list"
     *                     for displaying in recordlists, "edit" for
     *                     displaying in editscreens, "add" for displaying in
     *                     add screens. "csv" for csv files. Applications can
     *                     use additional modes.
     * @return html code to display the value of this attribute
     */
    function display($record, $mode="")
    {
      $rec = array();
      foreach ($this->m_displayfields as $field)
      {

        $p_attrib = $this->m_ownerInstance->getAttribute($field);

        $rec[$field] = $p_attrib->display($record[$this->fieldName()], $mode);

      }
      atkimport("atk.utils.atkstringparser");
      $parser = new atkStringParser($this->m_template);
      return $parser->parse($rec);
    }

    /**
     * Adds the attribute / field to the list header. This includes the column name and search field.
     *
     * @param String $action the action that is being performed on the node
     * @param array  $arr reference to the the recordlist array
     * @param String $fieldprefix the fieldprefix
     * @param int    $flags the recordlist flags
     * @param array  $atksearch the current ATK search list (if not empty)
     * @param string $atkorderby Order by string
     * @see atkNode::listArray
     */
    function addToListArrayHeader($action, &$arr, $fieldprefix, $flags, $atksearch, $atkorderby)
    {
      if (!$this->hasFlag(AF_HIDE_LIST) && !($this->hasFlag(AF_HIDE_SELECT) && $action == "select"))
      {
        $arr["heading"][$fieldprefix.$this->fieldName()]["title"] = $this->label();

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
            session_url(atkSelf().'?atknodetype='.$this->m_ownerInstance->atkNodeType().'&atkaction='.$action.'&atkorderby='.rawurlencode($order));
        }

        if (!hasFlag($flags, RL_NO_SEARCH) && $this->hasFlag(AF_SEARCHABLE))
        {
          $arr["search"][$fieldprefix.$this->fieldName()] = $this->search($atksearch,false,$fieldprefix);
          $arr["search"][$fieldprefix.$this->fieldName()].='<input type="hidden" name="atksearchmode['.$this->formName().']" value="'.$this->getSearchMode(false).'">';
        }
      }
    }

    /**
     * We do not want this attribute to store anything in the database, so we implement an empty store function
     *
     * @param atkDb $db
     * @param array $record 
     * @param string $mode
     * @return boolean to indicate if store went succesfull
     */
    function store($db, $record, $mode)
    {
      return true;
    }

    /**
     * Adds this attribute to database queries.
     *
     * @param atkQuery $query The SQL query object
     * @param String $tablename The name of the table of this attribute
     * @param String $fieldaliasprefix Prefix to use in front of the alias
     *                                 in the query.
     * @param Array $rec The record that contains the value of this attribute.
     * @param int $level Recursion level if relations point to eachother, an
     *                   endless loop could occur if they keep loading
     *                   eachothers data. The $level is used to detect this
     *                   loop. If overriden in a derived class, any subcall to
     *                   an addToQuery method should pass the $level+1.
     * @param String $mode Indicates what kind of query is being processing:
     *                     This can be any action performed on a node (edit,
     *                     add, etc) Mind you that "add" and "update" are the
     *                     actions that store something in the database,
     *                     whereas the rest are probably select queries.
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      $allfields = atk_array_merge($this->m_displayfields,$this->m_searchfields);
      $alias = $fieldaliasprefix.$this->fieldName()."_AE_";
      foreach ($allfields as $field)
      {
        $p_attrib = $this->m_ownerInstance->m_attribList[$field];
        $p_attrib->addToQuery($query, $tablename, $alias,$rec, $level,$mode);
      }
    }

    /**
     * Creates a search condition for a given search value, and adds it to the
     * query that will be used for performing the actual search.
     *
     * @param atkQuery $query The query to which the condition will be added.
     * @param String $table The name of the table in which this attribute
     *                      is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param String $searchmode The searchmode to use. This can be any one
     *                           of the supported modes, as returned by this
     *                           attribute's getSearchModes() method.
     * @param string $fieldaliasprefix optional prefix for the fiedalias in the table
     */
    function searchCondition(&$query, $table, $value, $searchmode, $fieldaliasprefix='')
    {
      $searchcondition = $this->getSearchCondition($query, $table, $value, $searchmode, $fieldaliasprefix='');
      if (!empty($searchcondition))
        $query->addSearchCondition($searchcondition);
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
      // Get search condition for all searchFields
      foreach ($this->m_searchfields as $field)
      {
        $p_attrib = &$this->m_ownerInstance->getAttribute($field);

        if(is_object($p_attrib))
        {
          $condition = $p_attrib->getSearchCondition($query, $table, $value, $searchmode);
          if (!empty($condition))
          {
            $searchconditions[] = $condition;
          }
        }
      }
      // When searchmode is substring also search the value in a concat of all searchfields
      if($searchmode=='substring')
      {

        $value = $this->escapeSQL(trim($value));

        $data = array();
        foreach($this->m_searchfields as $field)
        {
          if(strpos($field,'.')==false) $data[$field]=$table.".".$field;
          else $data[$field]=$field;
        }

        atkimport("atk.utils.atkstringparser");
        $parser = new atkStringParser($this->m_template);
        $concatFields = $parser->getAllParsedFieldsAsArray($data);

        $db = $this->getDb();
        $condition = "UPPER(".$db->func_concat($concatFields).") LIKE UPPER('%".$value."%')";
        $searchconditions[] = $condition;
      }
      return "(".implode(" OR ",$searchconditions).")";
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * @return array List of supported searchmodes
     */
    function getSearchModes()
    {
      return array("exact","substring","wildcard","regexp");
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return String The 'generic' type of the database field for this
     *                attribute.
     */
    function dbFieldType()
    {
      return "";
    }
  }

?>