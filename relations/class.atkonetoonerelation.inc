<?php
  /**
   * An atkOneToOneRelation defines a relation between two tables where there
   * is one record in the first table that belongs to one record in the
   * second table.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
   *
   */

  class atkOneToOneRelation extends atkRelation
  {
    /*** Member variables ***/

    /**
     * The referential key in the target node
     */
    var $m_refKey="";

    /**
     * Constructor
     * @param $name ?
     * @param $destination ?
     * @param $flags Flags for the relation
     */
    function atkOneToOneRelation($name, $destination, $refKey="", $flags=0)
    {
      $this->atkRelation($name, $destination, $flags|AF_HIDE_ADD); // 1toM Relations are NEVER edited when adding a rec.
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

      $this->createDestination();

      if (is_array($myrecord))
      {
        /* display in list mode */
        if ($mode == "list")
          $result = $this->m_destInstance->descriptor($myrecord);

        /* display in view mode */
        else
        {
          if (!empty($this->m_destinationFilter)) $filter = rawurlencode($this->m_destinationFilter);
          if (!$this->m_destInstance->hasFlag(NF_NO_VIEW))
            $actions = array("view" => dispatch_url($this->m_destination, "view", array("atkselector" => "[pk]", "atkfilter" => $filter)));
          else $actions = array();
          $this->m_destInstance->m_flags |= NF_NO_SEARCH;
          $result = $this->m_destInstance->normalRecordList(array($myrecord), $actions, RL_EMBED, array($this->m_refKey))."<br>";
        }
      }

      else $result = text("none");

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
      if ($mode!= "update" && $mode!="add")
      {
        if ($tablename!="") $tablename.=".";

        $this->createDestination();

        $condition = $tablename.$this->m_ownerInstance->m_primaryKey[0]."=".$fieldaliasprefix.$this->fieldName().".".$this->m_refKey;
        if ($this->m_destinationFilter!="")
        {
          $condition.=" AND ".$this->m_name.".".$this->m_destinationFilter;
        }
        $query->addJoin($this->m_destInstance->m_table, $fieldaliasprefix.$this->fieldName(),$condition, true);

        // we pass true as the last param to addToQuery, because we need all fields..
        $this->m_destInstance->addToQuery($query, $fieldaliasprefix.$this->fieldName(), $level+1, true);
      }

      // When storing, we don't add to the query.. we have our own store() method..
    }



  /**
     * delete relational records..
     */
    function delete($record)
    {
      $classname = $this->m_destination;
      $rel = &getNode($classname);
      atkdebug("O2O DELETE for $classname: ".$this->m_refKey."=".$record[$this->m_ownerInstance->primaryKeyField()]);

      $rel->deleteDb($rel->m_table.'.'.$this->m_refKey."=".$record[$this->m_ownerInstance->primaryKeyField()]);
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
      $this->createDestination();

      if ($rec[$this->fieldName()]!=NULL)
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

    function fetchValue($rec)
    {
      // we need to pass all values to the destination node, so it can
      // run it's fetchValue stuff over it..
      $this->createDestination();

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

    /**
     * Store values
     * @param $notused ??
     * @param $record Record
     */
    function store($notused, &$record)
    {
      $vars = &$record[$this->fieldName()];

      $this->createDestination();

      if ($vars["mode"]=="edit")
      {
        atkdebug("Updating existing one2one record");
        // we put the vars in the postvars, because there is information
        // like atkorgkey in it that is vital.
        $this->m_destInstance->m_postvars = $vars;
        $this->m_destInstance->updateDb($vars);
      }
      else if ($vars["mode"]=="add")
      {
        atkdebug("Adding new one2one record");
        $this->m_destInstance->addDb($vars);
      }
      else
      {
        atkdebug("Nothing to store in one2one record");
      }
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

      $this->createDestination();

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
      $forceList = decodeKeyValueSet($this->m_destinationFilter);
      $forceList[$this->m_refKey.".".$this->m_ownerInstance->primaryKeyField()] = $record[$this->m_ownerInstance->primaryKeyField()];

      $output.= $this->m_destInstance->hideform($mode,$myrecord,$forceList,$this->fieldName()."_AE_");
      return $output;
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
        parent::addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix);
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
          $this->createDestination();

          /* we first check if the record doesn't already exist */
          if ($defaults[$this->fieldName()] != NULL)
          {
            /* record has no primarykey yet, so we must add instead of update */
            $myrecord = $defaults[$this->fieldName()];
            if ($myrecord[$this->m_destInstance->primaryKeyField()] == NULL) $mode = "add";

            /* record exists! */
            else
            {
              $mode = "edit";
              $myrecord["atkprimkey"] = $this->m_destInstance->primaryKey($myrecord);
            }
          }

          /* record does not exist */
          else $mode = "add";

          /* mode */
          $arr["hide"][] = '<input type="hidden" name="'.$fieldprefix.$this->fieldName().'[mode]" value="'.$mode.'">';

          /* add fields */
          $forceList = decodeKeyValueSet($this->m_destinationFilter);
          $forceList[$this->m_refKey.".".$this->m_ownerInstance->primaryKeyField()] = $defaults[$this->m_ownerInstance->primaryKeyField()];
          $a = $this->m_destInstance->editArray($mode, $myrecord, $forceList, array(), $this->fieldName()."_AE_");

          /* errors and hidden fields */
          $error = array_merge($error, $a["error"]);
          $arr["hide"] = array_merge($arr["hide"], $a["hide"]);

          /* editable fields, if AF_NOLABEL is specified or if there is just 1 field with the
           * same name as the relation we don't display a label
           */
          if (!$this->hasFlag(AF_NOLABEL) && !(count($a["fields"]) == 1 && $a["fields"][0]["name"] == $this->m_name))
          {
            /* seperator and name */
            $arr["fields"][] = "-";
            $arr["fields"][] = "<b>".text($this->m_name, $this->m_ownerInstance->m_type)."</b>";
          }
          $arr["fields"] = array_merge($arr["fields"], $a["fields"]);
          if (!$this->hasFlag(AF_NOLABEL) && !(count($a["fields"]) == 1 && $a["fields"][0]["name"] == $this->m_name))
          {
            /* seperator */
            $arr["fields"][] = "-";
          }
        }
      }
    }
  }
?>
