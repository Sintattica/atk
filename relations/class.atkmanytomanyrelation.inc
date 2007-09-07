<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage relations
   *
   * @copyright (c)2000-2005 Ibuildings.nl BV
   * @copyright (c)2000-2005 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

   /** @internal base include */
   atkimport("atk.relations.atkrelation");

  /**
   * Many to many relation. Should not be used directly.
   *
   * This class is used as base class for special kinds of manytomany
   * relations, like the manyboolrelation. Note that most many-to-many
   * relationships can be normalized to a combination of one-to-many and
   * many-to-one relations.
   *
   * @abstract
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage relations
   *
   */
  class atkManyToManyRelation extends atkRelation
  {
    var $m_localKey = "";
    var $m_remoteKey = "";
    var $m_link = "";
    var $m_linkInstance = NULL;
    var $m_store_deletion_filter = "";
    var $m_localFilter = NULL;

    /**
     * Constructor
     * @param String $name The name of the relation
     * @param String $link The full name of the node that is used as
     *                     intermediairy node. The intermediairy node is
     *                     assumed to have 2 attributes that are named
     *                     after the nodes at both ends of the relation.
     *                     For example, if node 'project' has a M2M relation
     *                     with 'activity', then the intermediairy node
     *                     'project_activity' is assumed to have an attribute
     *                     named 'project' and one that is named 'activity'.
     *                     You can set your own keys by calling setLocalKey()
     *                     and setRemoteKey()
     * @param String $destination The full name of the node that is the other
     *                            end of the relation.
     * @param int $flags Flags for the relation.
     */
    function atkManyToManyRelation($name, $link, $destination, $flags=0)
    {
      $this->m_link = $link;
      $this->atkRelation($name, $destination, $flags|AF_CASCADE_DELETE|AF_NO_SORT);
    }


    function _getSelectableRecords($record=array(), $mode="")
    {
      $method = $this->fieldName()."_selection";
      if (method_exists($this->m_ownerInstance, $method))
        return $this->m_ownerInstance->$method($record, $mode);
      else return $this->getSelectableRecords($record, $mode);
    }

    function getSelectableRecords($record=array(), $mode="",$force=false)
    {
      static $resultList = array();

      if ($this->createDestination())
      {
        $filter = "";

        if ($this->m_destinationFilter!="")
        {
          $filter = $this->parseFilter($this->m_destinationFilter, $record);
          $this->m_destInstance->addFilter($filter);
        }

        $filterkey = md5($this->m_destInstance->m_type.".".$filter);
        if (!array_key_exists($filterkey, $resultList))
        {
          $resultList[$filterkey] = $this->m_destInstance->selectDb("","","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));
        }

        return $resultList[$filterkey];
      }

      return array();
    }

    /**
     * Returns the primary keys of the currently selected records retrieved
     * from the given record.
     *
     * @param array $record current record
     * 
     * @return array list of selected record keys
     */
    function getSelectedRecords($record)
    {
      $keys = array();
      
      if (isset($record[$this->fieldName()]))
      {
        for ($i = 0; $i < count($record[$this->fieldName()]); $i++)
        {
          if (is_array($record[$this->fieldName()][$i][$this->getRemoteKey()]))
          {
            $key = $this->m_destInstance->primaryKey($record[$this->fieldName()][$i][$this->getRemoteKey()]);
          }
          else
          {
            $key = $this->m_destInstance->primaryKey(array($this->m_destInstance->primaryKeyField()=>$record[$this->fieldName()][$i][$this->getRemoteKey()]));
          }
  
          $keys[] = $key;
        }
      }
      
      return $keys;
    }

    /**
     * Create instance of the intermediairy link node.
     *
     * If succesful, the instance is stored in the m_linkInstance member
     * variable.
     * @return boolean True if successful, false if not.
     */
    function createLink()
    {
      if ($this->m_linkInstance == NULL)
      {
        $this->m_linkInstance = &newNode($this->m_link);

        // Validate if destination was created succesfully
        if (!is_object($this->m_linkInstance))
      	{
	        atkerror("Relation with unknown nodetype '".$this->m_link."' (in node '".$this->m_owner."')");
          $this->m_linkInstance = NULL;
          return false;
        }
      }

      return true;
    }

    /**
     * Get the name of the attribute of the intermediairy node that points
     * to the master node.
     * @return String The name of the attribute.
     */
    function getLocalKey()
    {
      if ($this->m_localKey=="")
      {
        $this->m_localKey = $this->determineKeyName($this->m_owner);
      }
      return $this->m_localKey;
    }

    /**
     * Change the name of the attribute of the intermediairy node that points
     * to the master node.
     * @param String $attributename The name of the attribute.
     */
    function setLocalKey($attributename)
    {
      $this->m_localKey = $attributename;
    }

    /**
     * Get the name of the attribute of the intermediairy node that points
     * to the node on the other side of the relation.
     * @return String The name of the attribute.
     */
    function getRemoteKey()
    {
      $this->createDestination();

      if ($this->m_remoteKey=="")
      {
        list($module, $nodename) = explode(".", $this->m_destination);
        $this->m_remoteKey = $this->determineKeyName($nodename);
      }
      return $this->m_remoteKey;
    }

    /**
     * Determine the name of the foreign key based on the name of the
     *  relation.
     *
     * @param String $name the name of the relation
     * @return the probable name of the foreign key
     */
    function determineKeyName($name)
    {
      if ($this->createLink())
      {
        if (isset($this->m_linkInstance->m_attribList[$name]))
        {
          // there's an attribute with the same name as the role.
          return $name;
        }
        else
        {
          // find out if there's a field with the same name with _id appended to it
          if (isset($this->m_linkInstance->m_attribList[$name."_id"]))
          {
            return $name."_id";
          }
        }
      }
      return $name;
    }

    /**
     * Change the name of the attribute of the intermediairy node that points
     * to the node on the other side of the relation.
     * @param String $attributename The name of the attribute.
     */
    function setRemoteKey($attributename)
    {
      $this->m_remoteKey = $attributename;
    }

    /**
     * Returns a displayable string for this value.
     * @param $record
     * @return a displayable string for this value
     */
    function display($record, $mode="")
    {
      $result = "&nbsp;";
      if ($this->createDestination())
      {
        $recordset = array();
        if (is_array($record[$this->fieldName()]))
        {
          $remotekey = $this->getRemoteKey();
          for ($i=0;$i<count($record[$this->fieldName()]);$i++)
          {
            if(!is_array($record[$this->fieldName()][$i][$remotekey]))
            {
              $selector = $this->m_destInstance->m_table.".".$this->m_destInstance->primaryKeyField()."='".$record[$this->fieldName()][$i][$remotekey]."'";
              list($rec) = $this->m_destInstance->selectDb($selector,"","","",$this->m_destInstance->descriptorFields());
              $recordset[] = $this->m_destInstance->descriptor($rec);
            }
            else
            {
              $recordset[] = $this->m_destInstance->descriptor($record[$this->fieldName()][$i][$remotekey]);
            }
          }
        }
        if (!in_array($mode, array("csv", "plain")))
        {
          $result = "<ul><li>".implode("<li>",$recordset)."</ul>";
        }
        else
        {
          $result = implode(", ",$recordset);
        }
      }
      return $result;
    }

    /**
     * Dummy function
     */
    function edit($record="", $fieldprefix="")
    {
    }

    /**
     *Dummy function (we don't add ourselves to the query)
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec, $level, $mode)
    {
      // we don't add ourselves to the query;
    }

    /**
     * load function
     * @param $notused
     * @param $record
     */
    function load($notused, $record)
    {
      if ($this->createLink())
      {
        $where = $this->_getLoadWhereClause($record);
        $rel = &$this->m_linkInstance;
        return $rel->selectDb($where);
      }
      return array();
    }

    function _getLoadWhereClause($record)
    {
      $whereelems = array();
      $localkey = $this->getLocalKey();
      if(!is_array($localkey)) $localkey = array($localkey);

      $ownerfields = $this->m_ownerInstance->m_primaryKey;

      for ($i=0, $_i = count($localkey); $i<$_i; $i++)
      {
        $primkeyattr = &$this->m_ownerInstance->m_attribList[$ownerfields[$i]];

        if (!$primkeyattr->isEmpty($record))
        {
          $whereelems[] = $this->m_linkInstance->m_table.".".$localkey[$i]."='".$primkeyattr->value2db($record)."'";
        }
      }

      if ($this->m_localFilter != NULL)
        $whereelems[] = $this->m_localFilter;

      return "(".implode(") AND (", $whereelems).")";
    }

    /**
     * delete relational records..
     */
    function delete($record)
    {
      if ($this->createLink())
      {
        $rel = &$this->m_linkInstance;
        $where = $this->_getLoadWhereClause($record);
        if($where!='')
          return $rel->deleteDb($where);
      }
      return false;
    }

    /**
     * Stores the values in the database
     * @param $notused Not used
     * @param $record Current record
     */
    function store($notused, $record, $mode)
    {
      $this->createLink();
      $this->createDestination();
      $rel = &$this->m_linkInstance;

      // Find items that are checked.
      $checked = array();
      for ($i=0;$i<count($record[$this->fieldName()]);$i++)
      {
        if(is_array($record[$this->fieldName()][$i][$this->getRemoteKey()]))
          $checked[] = $record[$this->fieldName()][$i][$this->getRemoteKey()][$this->m_destInstance->primaryKeyField()];
        else
          $checked[] = $record[$this->fieldName()][$i][$this->getRemoteKey()];
      }

      $filter="";
      if(count($checked) > 0)
        $filter = "NOT IN ('".implode("','",$checked)."')";

      $selector = "(".$this->_getLoadWhereClause($record).")".($filter!=""?" AND ".$this->m_linkInstance->m_table.'.'.$this->getRemoteKey()." $filter":"");

      //append the store deletion filter (if set)
      if($this->m_store_deletion_filter!="")
      {
        if($selector!="") $selector.= " AND ";

        $selector.= " ".$this->m_store_deletion_filter;
      }

      if (is_object($rel) && $rel->deleteDb($selector))
      {
        for ($i=0;$i<count($record[$this->fieldName()]);$i++)
        {
          // magical stuff..
          $remKey    = $this->checkKeyDimension($record[$this->fieldName()][$i][$this->getRemoteKey()], $this->m_destInstance->primaryKeyField());

          $newrecord = array_merge($rel->initial_values(), $record[$this->fieldName()][$i]);

          if (is_array($record[$this->fieldName()][$i][$this->getRemoteKey()]))
          {
            $newrecord[$this->getRemoteKey()][$this->m_destInstance->primaryKeyField()] = $remKey;
          }
          else
          {
            $newrecord[$this->getRemoteKey()] = $remKey;
          }
          $localKey = $this->getLocalKey();
          if(is_array($localKey))
          {
            for($j=0;$j<count($localKey);$j++)
            {
              $locKey    = $this->checkKeyDimension($record[$this->m_ownerInstance->m_primaryKey[$j]]);
              $newrecord[$localKey[0]][$this->m_ownerInstance->m_primaryKey[$j]] = $locKey;
            }
          }
          else
          {
            $locKey    = $this->checkKeyDimension($record[$this->m_ownerInstance->primaryKeyField()]);
            $newrecord[$localKey][$this->m_ownerInstance->primaryKeyField()] = $locKey;
          }

          // First check if the record does not exist yet.
          /* @var $rel atkNode */
          $where = $this->_getLoadWhereClause($record).
                     " AND ".$this->m_linkInstance->m_table.'.'.$this->getRemoteKey()."='".
                     $remKey."'";

                     atk_var_dump($newrecord, "<h1>NEWREC</h1>");
          $existing = $rel->selectDb($where, "", "", "", $rel->m_primaryKey);
          if (!count($existing))
          {
            if (!$rel->addDb($newrecord, true, $mode))
            {
              return false;
            }
          }
        }
        return true;
      }
      return false;
    }

    /**
     * Check if the attribute is empty
     * @return true if it's empty
     */
    function isEmpty($postvars)
    {
      return (!is_array($postvars[$this->fieldName()]) || count($postvars[$this->fieldName()])==0);
    }


    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">)
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode with hidden form elements that post
     *                This attribute's value without showing it.
     */
    function hide($record="", $fieldprefix="")
    {
      $result = "";
      if(is_array(atkArrayNvl($record,$this->fieldName())) && $this->createDestination())
      {
        for($i=0,$_i=count($record[$this->fieldName()]);$i<$_i;$i++)
        {
          if(atkArrayNvl($record[$this->fieldName()][$i],$this->getLocalKey()))
            $result .= '<input type="hidden" name="'.$fieldprefix.$this->formName().
                       '['.$i.']['.$this->getLocalKey().']" value="'.
                       $this->checkKeyDimension($record[$this->fieldName()][$i][$this->getLocalKey()],
                         $this->m_ownerInstance->primaryKeyField()).'">';

          if(atkArrayNvl($record[$this->fieldName()][$i],$this->getRemoteKey()))
            $result .= '<input type="hidden" name="'.$fieldprefix.$this->formName().
                       '['.$i.']['.$this->getRemoteKey().']" value="'.
                       $this->checkKeyDimension($record[$this->fieldName()][$i][$this->getRemoteKey()],
                         $this->m_destInstance->primaryKeyField()).'">';
        }
      }
      return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to search
     * @param $record Record
     * @param $size Size of the editbox
     * @param $maxsize Maxsize of the editbox
     * @return Piece of html code that can  be used in a form to edit this
     */
    function search($record="", $extended=false, $fieldprefix="")
    {
      $this->createDestination();

      // now select all records
      $recordset = $this->m_destInstance->selectDb("","","","*",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));
        $result = '<select ';
        if ($extended)
        {
          $result.='multiple size="'.min(5,count($recordset)+1).'"';
        }

        if (strstr($fieldprefix,"]["))
          $result.='name="atksearch['. $fieldprefix.$this->fieldName().'][]">';
        else
          $result.='name="atksearch'.$fieldprefix.'['. $this->fieldName().'][]">';

        $pkfield = $this->m_destInstance->primaryKeyField();

        $result.= '<option value="">'.atktext("search_all", "atk");

        for ($i=0;$i<count($recordset);$i++)
        {
          $pk = $recordset[$i][$pkfield];
          if (atk_in_array($pk, $record[$this->fieldName()])) $sel = "selected"; else $sel = "";
          $result.= '<option value="'.$pk.'" '.$sel.'>'.$this->m_destInstance->descriptor($recordset[$i]);
        }
        $result.='</select>';
        return $result;
    }

    /**
     * Creates an search condition for a given search value
     * @param $table the table name
     * @param $value the search value
     * @return a piece of the SQL where clause
     */
    function searchCondition(&$query, $table, $value, $searchmode)
    {
      // We only support 'exact' matches.
      // But you can select more than one value, which we search using the IN() statement,
      // which should work in any ansi compatible database.
      if (is_array($value) && count($value)>0 && $value[0]!="") // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.
      {
    		$this->createLink();
    		$query->addJoin($this->m_linkInstance->m_table, $this->fieldName(), $table.".".$this->m_ownerInstance->primaryKeyField()."=".$this->fieldName().".".$this->getLocalKey(),FALSE );
    		$query->setDistinct(TRUE);

        if (count($value)==1) // exactly one value
        {
          $query->addSearchCondition($query->exactCondition($this->fieldName().".".$this->getRemoteKey(),$this->escapeSQL($value[0])));
        }
        else // search for more values using IN()
        {
          $query->addSearchCondition($this->fieldName().".".$this->getRemoteKey()." IN ('".implode("','",$value)."')");
        }
      }
    }

    /**
     * Checks if a key is not an array
     * @param $key   field containing the key values
     * @param $field field to return if an array
     * @return value of $field
     */
    function checkKeyDimension($key, $field="id")
    {
      if (is_array($key))
      {
        return $key[$field];
      }
      return $key;
    }

    /**
     * Fetch value. If nothing selected, return empty array instead
     * of nothing.
     *
     * @param array $postvars
     */
    function fetchValue($postvars)
    {
      $value = parent::fetchValue($postvars);
      return $value == NULL ? array() : $value;
    }

    /**
     * Function adds a custom filter that is used when deleting items during the store() function.
     *
     * Example:
     * Normally the delete function would do something like this:
     *
     * DELETE FROM phase WHERE phase.template NOT IN (1,2,3)
     *
     * If the template field is NULL, although it is not specified in the NOT IN (1,2,3), it will not be deleted.
     * An extra check can be added just in case the template value is not NULL but 0 or '' (which would delete the phase).
     *
     * @param String $filter The filter that is used when deleting records in the store function.
     * @return none
     */
    function setStoreDeletionFilter($filter)
    {
      $this->m_store_deletion_filter = $filter;
    }

    /**
     * Local filter is used to only show values that are once selected
     * that comply with the local filter. A local filter is also automatically
     * set as store deletion filter.
     *
     * @param string $filter filter
     */
    function setLocalFilter($filter)
    {
      $this->setStoreDeletionFilter($filter);
      $this->m_localFilter = $filter;
    }
  }
?>
