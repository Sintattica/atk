<?php
  /* flag(s) specific for atkOneToOneRelation */
  define("AF_ONETOONE_ADD",             AF_SPECIFIC_1); // override the default no add flag
  define("AF_ONETOONE_ERROR",           AF_SPECIFIC_2); // enable error notifications / triggers
  define("AF_ONETOONE_INTEGRATE",       AF_SPECIFIC_3); // invisibly integrate a onetoonerelation as if the fields where part of the current node.  
  define("AF_ONETOONE_LAZY",            AF_SPECIFIC_4); // use lazy loading instead of query addition.
  
  /**
   * An atkOneToOneRelation defines a relation between two tables where there
   * is one record in the first table that belongs to one record in the
   * second table.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
   */
  class atkOneToOneRelation extends atkRelation
  {
    /* The referential key in the target node */
    var $m_refKey="";

    /**
     * Constructor
     * @param $name attribute / relation name
     * @param $destination the destination module/node
     * @param $refKey the referential key in the target node
     * @param $flags Flags for the relation
     */
    function atkOneToOneRelation($name, $destination, $refKey="", $flags=0)
    {
      if ($flags & AF_ONETOONE_ADD != AF_ONETOONE_ADD) $flags |= AF_NO_ADD;
      $this->atkRelation($name, $destination, $flags|AF_ONETOONE_LAZY);
      $this->m_refKey = $refKey;
    }

    /**
     * Returns a displayable string for this value.
     * @param $record Record
     * @return a displayable string
     */
    function display($record, $mode="list")
    {
      $myrecord = $record[$this->fieldName()];

      // default result
      $result = text("none");
      
      if ($this->createDestination())
      {

        if (is_array($myrecord))
        {
          /* display in list mode */
          if ($mode == "list")
            $result = $this->m_destInstance->descriptor($myrecord);
  
          /* display in view mode */
          else
          {
            if(is_array($this->m_destinationFilter))
            {
              $filter="";
              for($i=0,$_i=count($this->m_destinationFilter);$i<$_i;$i++)
              {
                if($filter!="") $filter.=rawurlencode(" AND ");
                $filter.= rawurlencode($this->m_destinationFilter[$i]);
              }
            }
            elseif (!empty($this->m_destinationFilter)) $filter = rawurlencode($this->m_destinationFilter);
            if (!$this->m_destInstance->hasFlag(NF_NO_VIEW))
              $actions = array("view" => dispatch_url($this->m_destination, "view", array("atkselector" => "[pk]", "atkfilter" => $filter)));
            else $actions = array();
            $result = $this->m_destInstance->normalRecordList(array($myrecord), $actions, RL_NO_SEARCH|RL_NO_SORT, array($this->m_refKey))."<br>";
          }
        }
      }
      return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Record
     * @return Piece of html code that can  be used in a form to edit this
     * attribute
     */
    function edit($record="", $fieldprefix="")
    {
      /* dummy */
    }

    /**
     * Function to add values to query
     * @param $query
     * @param $tablename Table name
     * @param $fieldaliasprefix Field alias prefix
     * @param $level Level
     * @result A good query
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
    {   
      if ($this->createDestination())
      {
        if ($mode!= "update" && $mode!="add")
        {
          if ($this->hasFlag(AF_ONETOONE_LAZY))
          {
            if ($this->m_refKey=="")
            {
              return parent::addToQuery($query, $tablename, $fieldaliasprefix, $rec, $level, $mode);
            }
          }
          
          if ($tablename!="") $tablename.=".";  
         
          if ($this->m_refKey!="")
          {
            // Foreign key is in the destination node.
            $condition = $tablename.$this->m_ownerInstance->m_primaryKey[0]."=".$fieldaliasprefix.$this->fieldName().".".$this->m_refKey;
          }
          else
          {
            // Foreign key is in the source node
            $condition = $tablename.$this->fieldName()."=".$fieldaliasprefix.$this->fieldName().".".$this->m_destInstance->m_primaryKey[0];
          }
          $condition.= $this->getDestinationFilterCondition();
          $query->addJoin($this->m_destInstance->m_table, $fieldaliasprefix.$this->fieldName(),$condition, true);  
          // we pass true as the last param to addToQuery, because we need all fields..
          $this->m_destInstance->addToQuery($query, $fieldaliasprefix.$this->fieldName(), $level+1, true);
  	     
        }
  
        // When storing, we don't add to the query.. we have our own store() method..
        // With one exception. If the foreign key is in the source node, we also need to update
        // the refkey value.
        if ($this->m_refKey=="" && $mode=="add")
        {       
          $query->addField($this->fieldName(),$rec[$this->fieldName()][$this->m_destInstance->m_primaryKey[0]],"","",!$this->hasFlag(AF_NO_QUOTES));
        }
      }
    }
    
    function load($g_db, $record, $mode)
    {
      if ($this->createDestination())
      {              
        if ($this->m_refKey=="")
        {
          // Foreign key in owner          
          //$condition = $this->m_destInstance->m_primaryKey[0]."=".$record[$this->fieldName()];        
          $condition = $this->m_destInstance->m_table.'.'.$this->m_destInstance->m_primaryKey[0].
                      "='".$record[$this->fieldName()]."'";
        }
        else
        {          
          // Foreign key in destination
          $condition = $this->m_destInstance->m_table.'.'.$this->m_refKey."=".
                          $this->m_ownerInstance->m_attribList[$this->m_ownerInstance->primaryKeyField()]->value2db($record);
        }
        $fields = atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey);
        $recs = $this->m_destInstance->selectDb($condition, "", "", "", "", $mode);
        return $recs[0];
      }
    }
    
    function getDestinationFilterCondition()
    {
      $condition="";
      if(is_array($this->m_destinationFilter))
      {
        for($i=0,$_i=count($this->m_destinationFilter);$i<$_i;$i++)
        {
          $condition.=" AND ".$this->m_name.".".$this->m_destinationFilter[$i];
        }
      }
      elseif ($this->m_destinationFilter!="")
      {
         $condition.=" AND ".$this->m_name.".".$this->m_destinationFilter;
      }
      return $condition;
    }

    /**
     * Delete relational records..
     */
    function delete($record)
    {
      $classname = $this->m_destination;
      $rel = &getNode($classname);
      atkdebug("O2O DELETE for $classname: ".$this->m_refKey."=".$record[$this->m_ownerInstance->primaryKeyField()]);

      if ($this->m_refKey!="")
      {
        // Foreign key is in the destination node
        $condition = $rel->m_table.'.'.$this->m_refKey."=".
                          $this->m_ownerInstance->m_attribList[$this->m_ownerInstance->primaryKeyField()]->value2db($record);
      }
      else
      {
       // Foreign key is in the source node.        
       $condition = $rel->m_table.'.'.$rel->m_primaryKey[0]."=".$record[$this->fieldName()][$this->m_ownerInstance->primaryKeyField()];
      }
      return $rel->deleteDb($condition);
    }

    /**
     * Converts DataBase value to normal value
     * @param $rec Record
     * @return decoded value
     */
    function db2value($rec)
    {
      // we need to pass all values to the destination node, so it can
      // run it's db2value stuff over it..
      if ($this->hasFlag(AF_ONETOONE_LAZY)&&$this->m_refKey=="")
      {
        return parent::db2value($rec);
      }

      if ($this->createDestination())
      {
        $pkval = $rec[$this->fieldName()][$this->m_destInstance->primaryKeyField()];
        if ($pkval!=NULL && $pkval!="") // If primary key is not filled, there was no record, so we
                                        // should return NULL.
        {
          foreach (array_keys($this->m_destInstance->m_attribList) as $attribname)
          {
            $p_attrib = &$this->m_destInstance->m_attribList[$attribname];
            $rec[$this->fieldName()][$attribname] = $p_attrib->db2value($rec[$this->fieldName()]);
          }
          // also set the primkey..
          $rec[$this->fieldName()]["atkprimkey"] = $this->m_destInstance->primaryKey($rec[$this->fieldName()]);
          return $rec[$this->fieldName()];
        }
      }
      return NULL;
    }

    function fetchValue($rec)
    {
      // we need to pass all values to the destination node, so it can
      // run it's fetchValue stuff over it..
      if ($this->createDestination())
      {
        if ($rec[$this->fieldName()]!=NULL)
        {
          foreach (array_keys($this->m_destInstance->m_attribList) as $attribname)
          {
            $p_attrib = &$this->m_destInstance->m_attribList[$attribname];
            $rec[$this->fieldName()][$attribname] = $p_attrib->fetchValue($rec[$this->fieldName()]);
          }
          return $rec[$this->fieldName()];
        }
      }
    }
    
    function storageType($mode)
    {      
      if ($this->m_refKey!="")
      {
        // foreign key is in destination node, so we must store the
        // destination AFTER we stored the master record.
        return POSTSTORE;
      }
      else
      {
        // foreign key is in source node, so we must store the
        // relation node first, so we can store the foreign key
        // when we store the master record. To store the latter,
        // we must also perform an addToQuery.
        return PRESTORE|ADDTOQUERY;
      }
    }
    
    function loadType($mode)
    {
      if ($this->hasFlag(AF_ONETOONE_LAZY))
      {
        return POSTLOAD|ADDTOQUERY;
      }
      else
      {
        return ADDTOQUERY;
      }
    }

    /**
     * Store values
     * @param $notused ??
     * @param $record Record
     * @param $mode the store mode
     */
    function store($notused, &$record, $mode)
    {
      if ($this->createDestination())
      {
        $vars = &$record[$this->fieldName()];
        if ($this->m_refKey!="")
        {
          // Foreign key is in destination node                  
          if ($this->destinationHasRelation())
          {
            $vars[$this->m_refKey][$this->m_ownerInstance->primaryKeyField()] = $record[$this->m_ownerInstance->primaryKeyField()];          
          }
          else
          {
            //if the a onetoonerelation has no relation on the other side the m_refKey is not an array          
            // experimental, will the next line always work?
            $vars[$this->m_refKey] = $this->m_ownerInstance->m_attribList[$this->m_ownerInstance->primaryKeyField()]->value2db($record);
          }
        }
        else
        {
          // Foreign key is in source node
          // After add, we must store the key value.
        }
        
          
        if ($vars["mode"]=="edit")
        {
          atkdebug("Updating existing one2one record");
          // we put the vars in the postvars, because there is information
          // like atkorgkey in it that is vital.
          $this->m_destInstance->m_postvars = $vars;
          return $this->m_destInstance->updateDb($vars);
        }
        elseif ($vars["mode"]=="add" || $mode == "add" || $mode == "copy")
        {
          atkdebug("Adding new one2one record for mode $mode");
          $result = $this->m_destInstance->addDb($vars, true, $mode);
          
          if ($this->m_refKey=="")
          {
            // Foreign key is in source node, so we must update the record value with 
            $record[$this->fieldName()][$this->m_destInstance->m_primaryKey[0]] = $vars[$this->m_destInstance->m_primaryKey[0]];
          }
          
          return $result;
        }
        else
        {
          atkdebug("Nothing to store in one2one record");
          return true;
        }
      }
    }
        
   /**
    * Check if destination node has a relation to this node, or a 'regular'
    * atkattribute to store it's referential key.
    */
    function destinationHasRelation()
    {
      if ($this->createDestination())
      {
        if ($this->m_refkey!="")
        { 
          // foreign key is in the destination node.            
          $attrib = $this->m_destInstance->m_attribList[$this->m_refKey];                  
        }
        else
        {
          // foreign key is in the source node. In this case, we must check the primary key
          // of the target node.
          $attrib = $this->m_destInstance->m_attribList[$this->m_destInstance->m_primaryKey[0]];
        }
        if (is_object($attrib) && strpos(get_class($attrib), "elation")!==false) return true;
      }
      return false;
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="", $fieldprefix="")
    {
      atkdebug("hide called for ".$this->fieldName());

      if ($this->createDestination())
      {
        if ($record[$this->fieldName()]!=NULL)
      {
          $myrecord = $record[$this->fieldName()];
  
          if ($myrecord[$this->m_destInstance->primaryKeyField()]==NULL)
          {
            // rec has no primkey yet, so we must add instead of update..
            $mode = "add";
          }
          else
          {
            $mode = "edit";
            $myrecord["atkprimkey"] = $this->m_destInstance->primaryKey($myrecord);
        }
        }
        else
        {
          $mode = "add";
        }

        $output.='<input type="hidden" name="'.$fieldprefix.$this->fieldName().'[mode]" value="'.$mode.'">';
        $forceList = decodeKeyValueSet($this->getFilter());
        $output.= $this->m_destInstance->hideform($mode,$myrecord,$forceList,$this->fieldName()."_AE_");
        return $output;
      }
      return "";
    }

    /**
     * Adds the attribute's edit / hide HTML code to the edit array. This method
     * is called by the node if it want the data needed to create an edit form.
     * @param $mode     the edit mode ("add" or "edit")
     * @param $arr      pointer to the edit array
     * @param $defaults pointer to the default values array
     * @param $error    pointer to the error array
     * @param $prefix   the fieldprefix
     */
    function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
      /* hide */
      if (($mode == "edit" && $this->hasFlag(AF_HIDE_EDIT)) || ($mode == "add" && $this->hasFlag(AF_HIDE_ADD)))
      {
        /* when adding, there's nothing to hide... */
        if ($mode=="edit" || ($mode == "add" && !$this->isEmpty($defaults)))
          $arr["hide"][] = $this->hide($defaults, $fieldprefix, $mode);
      }

      /* readonly */
      elseif (($mode=="edit" && $this->hasFlag(AF_READONLY_EDIT)) || ($mode=="add" && $this->hasFlag(AF_READONLY_ADD)))
      {
        parent::addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);
      }

      /* edit */
      else
      {

        /* we first check if there is no edit override method, if there
         * is this method has the same behaviour as the atkAttribute's method
         */
        if (method_exists($this->m_ownerInstance, $this->m_name."_edit") ||
            $this->edit($defaults, $fieldprefix, $mode) !== NULL)
        {
          atkAttribute::addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);
        }

        /* how we handle 1:1 relations normally */
        else
        {

          if ($this->createDestination())
	        {
            /* we first check if the record doesn't already exist */
            if ($defaults[$this->fieldName()] != NULL)
            {
              /* record has no primarykey yet, so we must add instead of update */
              $myrecord = $defaults[$this->fieldName()];
                                    
              if (empty($myrecord[$this->m_destInstance->primaryKeyField()]))
              {
                $mode = "add";
              }
              /* record exists! */
              else
              {
                $mode = "edit";
                $myrecord["atkprimkey"] = $this->m_destInstance->primaryKey($myrecord);
              }
            }

            /* record does not exist */
            else
            {
               $mode = "add";
            }
  
            /* mode */
            $arr["hide"][] = '<input type="hidden" name="'.$fieldprefix.$this->fieldName().'[mode]" value="'.$mode.'">';

            /* add fields */

            $forceList = decodeKeyValueSet($this->m_destinationFilter);
            if ($this->m_refKey!="")
            {            
              if($this->destinationHasRelation())
              {
                $forceList[$this->m_refKey][$this->m_ownerInstance->primaryKeyField()] = $defaults[$this->m_ownerInstance->primaryKeyField()];
              }
              else
              {
                 // its possible that the destination has no relation back. In that case the refKey is just an attribute
                 $forceList[$this->m_refKey] = $defaults[$this->m_ownerInstance->primaryKeyField()];
              }
            }
            $a = $this->m_destInstance->editArray($mode, $myrecord, $forceList, array(), $fieldprefix.$this->fieldName()."_AE_");

            /* errors and hidden fields */
            $error = array_merge($error, $a["error"]);
            $arr["hide"] = array_merge($arr["hide"], $a["hide"]);

            /* editable fields, if AF_NOLABEL is specified or if there is just 1 field with the
             * same name as the relation we don't display a label
             * TODO FIXME
             */
            if (!$this->hasFlag(AF_ONETOONE_INTEGRATE) && !$this->hasFlag(AF_NOLABEL) && !(count($a["fields"]) == 1 && $a["fields"][0]["name"] == $this->m_name))
            {
              /* seperator and name */
              $arr["fields"][] = "-";
              $arr["fields"][] = "<b>".text($this->m_name, $this->m_ownerInstance->m_type)."</b>";
            }              
            $arr["fields"] = array_merge($arr["fields"], $a["fields"]);            
            
            if (!$this->hasFlag(AF_ONETOONE_INTEGRATE) && !$this->hasFlag(AF_NOLABEL) && !(count($a["fields"]) == 1 && $a["fields"][0]["name"] == $this->m_name))
            {
              /* seperator */
              $arr["fields"][] = "-";
            }
          }
        }
      }
    }

    /**
    * description
    *
    * @return void
    */
    function isEmpty($record)
    {	      
      return false;
    }

    /**
     * Validate's values (forward to the destination node)
     * @param &$record record that contains value to be validated
     * @param $mode can be either "add" or "update"
     * @return $record
     */
    function validate(&$record, $mode)
    {      
      // zitten AF_ONETOONE_ERROR en AF_OBLIGATORY elkaar soms in de weg
      if ($this->hasFlag(AF_ONETOONE_ERROR)&&($mode!="add" || !$this->hasFlag(AF_HIDE_ADD)))
      {
        if ($this->createDestination())
	      {
          $this->m_destInstance->validate($record[$this->fieldName()], $mode, array($this->m_refKey));
	      }
      }
    }
    
    function getFilter()
    {
      $filter = $this->m_destinationFilter;
      if(is_array($filter))
      {
        $tmp_filter="";
        for($i=0,$_i=count($filter);$i<$_i;$i++)
        {
          if($tmp_filter!="") $tmp_filter.=" AND ";
          $tmp_filter.=$filter[$i];
        }
        return $tmp_filter;
      }
      else
      {
        return $filter;
      }
    }
    
    /**
     * Integrated onetoone relations must add all tabs that the destination
     * node has, to the main screen.
     */
    function getTabs($action)
    {
      if ($this->hasFlag(AF_ONETOONE_INTEGRATE) && $this->createDestination())
      {
        return $this->m_destInstance->getTabs($action);
      }
      return parent::getTabs($action);
    }
    
    // A onetoone must show itself on a tab, if one of the destination attributes
    // should be shown on that tab. 
    function showOnTab($tab)
    {
      if ($this->hasFlag(AF_ONETOONE_INTEGRATE) && $this->createDestination())
      {
        foreach (array_keys($this->m_destInstance->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_destInstance->m_attribList[$attribname];
          if ($p_attrib->showOnTab($tab)) return true; // If we have one match, we can return true.
        }
        return false;
      }
      
      return parent::showOnTab($tab);      
    }
    
  }
?>