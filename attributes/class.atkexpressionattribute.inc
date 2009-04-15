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
   * @version $Revision: 6309 $
   * $Id$
   */

   atkimport("atk.attributes.atkattribute");

  /**
   * With the atkExpressionAttribute class you can select arbitrary SQL expressions
   * like subqueries etc. It's not possible to save values using this attribute.
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   */
  class atkExpressionAttribute extends atkAttribute
  {
    var $m_searchType = "string";    
    var $m_expression;    

    /**
     * Constructor.
     * 
     * @param string $name               The name of the attribute.
     * @param string $expression         The SQL expression.
     * @param mixed  $searchTypeOrFlags  The search type (string) or flags (numeric) for this attribute. At the moment
     *                                   only search types "string" and "number" are supported.
     * @param int    $flags              The flags for this attribute.
     */
    function atkExpressionAttribute($name, $expression, $searchTypeOrFlags=0, $flags=0)
    {
      if (is_numeric($searchTypeOrFlags))
        $flags = $searchTypeOrFlags;

      $this->atkAttribute($name, $flags|AF_HIDE_ADD|AF_READONLY_EDIT);
      
      $this->m_expression = $expression;
      
      if (!is_numeric($searchTypeOrFlags))
        $this->setSearchType($searchTypeOrFlags);
    }
    
    /**
     * No storage.
     * 
     * @param string $mode The type of storage ("add" or "update")
     */
    function storageType($mode='')
    {
      return NOSTORE;
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
      $expression = str_replace("[table]", $tablename, $this->m_expression);
      $query->addExpression($this->fieldName(), $expression, $fieldaliasprefix);
    }
    
    /**
     * Returns the order by statement for this attribute.
     *
     * @return order by statement
     */
    function getOrderByStatement()
    {
      $expression = str_replace("[table]", $this->m_ownerInstance->m_table, $this->m_expression);
      return "($expression)";
    }    

    /**
     * Sets the search type.
     *
     * @param array $type the search type (string or number)
     */
    function setSearchType($type)
    {
      $this->m_searchType = $type;
    }
    
    /**
     * Returns the search type.
     *
     * @return string the search type (string or number)
     */
    function getSearchType()
    {
      return $this->m_searchType;
    }    

    /**
     * We don't know our field type plus we can't be stored anyways.
     * So return an empty field type.
     *
     * @return string field type (empty string)
     */
    function dbFieldType()
    {
      return "";
    }
    
    /**
     * Returns the search modes.
     *
     * @return array list of search modes
     */
    function getSearchModes()
    {
      if ($this->getSearchType() == "number")
        return atkNumberAttribute::getSearchModes();
      else return parent::getSearchModes();
    }    
    
   /**
     * Returns a piece of html code that can be used to search for an attribute's value.
     * 
     * @param array $record Array with values
     * @param boolean $extended if set to false, a simple search input is
     *                          returned for use in the searchbar of the
     *                          recordlist. If set to true, a more extended
     *                          search may be returned for the 'extended'
     *                          search page. The atkAttribute does not
     *                          make a difference for $extended is true, but
     *                          derived attributes may reimplement this.
     * @param string $fieldprefix The fieldprefix of this attribute's HTML element.
     *
     * @return String A piece of html-code
     */
    function search($record="", $extended=false, $fieldprefix="")
    {
      if ($this->getSearchType() == "number")
        return atkNumberAttribute::search($record, $extended, $fieldprefix);
      else return parent::search($record, $extended, $fieldprefix);
    }
    
    /**
     * Creates a search condition for this attribute.
     * 
     * @param atkQuery $query     The query object where the search condition should be placed on
     * @param String $table       The name of the table in which this attribute
     *                              is stored
     * @param mixed $value        The value the user has entered in the searchbox
     * @param String $searchmode  The searchmode to use. This can be any one
     *                              of the supported modes, as returned by this
     *                              attribute's getSearchModes() method.
     * @return String The searchcondition to use.
     */
    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      // If we are accidentally mistaken for a relation and passed an array
      // we only take our own attribute value from the array
      if ($this->m_searchmode) $searchmode = $this->m_searchmode;
      
      $expression = "(".str_replace("[table]", $table, $this->m_expression).")";
      
      $searchcondition = "";
      
      if ($this->getSearchType() == "number")
        $value = atkNumberAttribute::processSearchValue($value);

      if ($searchmode != "between")
      {
        // Use default handler, and use only the first field.
        if ($this->getSearchType() == "number")
          $value = $value["from"];
          
        $func = $searchmode."Condition";
        if (method_exists($query,$func) && $value !== "" && $value !== NULL)
        {
          return $query->$func($expression, $this->escapeSQL($value));
        }
        else
        {
          return false;
        }        
      }
      else
      {
        return atkNumberAttribute::getBetweenCondition($query, $expression, $value);
      }
    }    
  }
?>