<?php

  /* flag(s) specific for atkManyToOneRelation */
  define("AF_RELATION_AUTOLINK", AF_SPECIFIC_1); // create edit/view links for the items in a manytoonerelation dropdown.
  
  /**
   * A N:1 relation between two classes.
   * For example, projects all have one coordinator, but one
   * coordinator can have multiple projects. So in the project
   * class, there's a ManyToOneRelation to a coordinator.
   *
   * This relation essentially creates a dropdown box, from which
   * you can select from a set of records.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @version $Revision$
   *
   * $Id$
   */
  class atkManyToOneRelation extends atkRelation
  {  
    var $m_leftjoin = true; // by default, we do a left join. this means that
                            // records that don't have a record in this
                            // relation, will be displayed anyway. NOTE: set
                            // this to false only if you know what you're
                            // doing. When in doubt, 'true' is usually the
                            // best option.
               
    /**
     * Constructor
     * @param $name ?
     * @param $destination ?
     * @param $flags Flags for the relation
     */
    function atkManyToOneRelation($name, $destination, $flags=0)
    {
      $this->atkRelation($name, $destination, $flags);
    }

    /**
     * Convert value to DataBase value
     * todo: watch out.. pk may not be the first element..
     * @param $rec
     * @return
     */
    function value2db($rec)
    {
      if (is_array($rec[$this->fieldName()]))
      {
        if ($this->createDestination())
        {
          $pkfield = $this->m_destInstance->m_primaryKey[0];
          return $rec[$this->fieldName()][$pkfield];
        }
      }
      // This never happens, does it?
      return "";
    }

    /**
     * Fetch value out of record
     * @param $rec Record
     * @return decoded value
     */
    function fetchValue($rec)
    {
      // We must put the result of decodeKey into a variable first,
      // because PHP3 can't handle each() with a non-constant expression.
      $tmp = decodeKeyValuePair($rec[$this->fieldName()]);
      list($key,$value) = each($tmp);

      // Tablename must be stripped out because it is in the way..
      if (strpos($key,'.')>0)
      {
        $field = substr($key,strpos($key,'.')+1);
      }
      else
      {
        $field = $key;
      }

      return array($field=>$value);
    }


    /**
     * Converts DataBase value to normal value
     * @param $rec Record
     * @return decoded value
     */
    function db2value($rec)
    {
      $myrec = $rec[$this->fieldName()];
            
      if (is_array($myrec)) return $myrec;
      else
      {
        // if the record is not an array, probably only the value of the primary key was loaded..
        if ($this->createDestination())
          return array($this->m_destInstance->primaryKeyField() => $myrec);
      }
    }

    /**
     * Returns a displayable string for this value.
     * @param $record Record
     * @return a displayable string
     */
    function display($record)
    {      
      if ($this->createDestination())
      {
        if (count($record[$this->fieldName()])==1)
        {
          // The record consist of only one field, which is the primary key.
          // This means we have to load the rest of the data, in order to 
          // load a descriptor. 
          atkdebug("Delayed loading of descriptor fields for ".$this->m_name);
          $recs = $this->m_destInstance->selectDb($this->m_destInstance->primaryKey($record[$this->fieldName()]), "", "", "", $this->m_destInstance->descriptorFields());
          $title = $this->m_destInstance->descriptor($recs[0]);
        }
        else
        {
          $title = $this->m_destInstance->descriptor($record[$this->fieldName()]);
        }

        $result = $title; // default result is just the title.
 
        if ($this->hasFlag(AF_RELATION_AUTOLINK)) // create link to edit/view screen
        {
          /* FOLLOWING LINES ARE INTENTIONALLY DISABLED, in display mode 
            (when we're viewing something that is readonly) we should not 
            allow editing the destination record:            
          if ($this->m_destInstance->allowed("edit")&&!$this->m_destInstance->hasFlag(NF_NO_EDIT))
          {
            $result = href("dispatch.php?atknodetype=".$this->m_destination."&atkaction=edit&atkselector=".rawurlencode($this->m_destInstance->primaryKey($record[$this->fieldName()])), $title, SESSION_NESTED);
          }
          else */
          
          // if we are allowed to edit, we're cerrtainly allowed to view.
          if (($this->m_destInstance->allowed("view")||$this->m_destInstance->allowed("view"))&&!$this->m_destInstance->hasFlag(NF_NO_VIEW))
          {
            $result = href("dispatch.php?atknodetype=".$this->m_destination."&atkaction=view&atkselector=".rawurlencode($this->m_destInstance->primaryKey($record[$this->fieldName()])), $title, SESSION_NESTED);
          }
        }
        return $result;
      }
      return "";
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Record
     * @return Piece of html code that can  be used in a form to edit this
     */
    function edit(&$record, $fieldprefix="")
    {
      global $ATK_VARS;

      $this->createDestination();
      // Two variances.. one with a dropdownbox containing all records,
      // and one with a dropdownbox that contains the last 10 used records,
      // with a link to a selector with all records. This second one is for
      // relations with large tables, so is only used when AF_LARGE is set.

      if (!$this->hasFlag(AF_LARGE))
      {
        // normal dropdown..
        $currentPk = $this->m_destInstance->primaryKey($record[$this->fieldName()]);

        if ($this->m_destinationFilter!="")
        {
          $filter = stringparse($this->m_destinationFilter,$record);
          $this->m_destInstance->addFilter($filter);
        }
        $recordset = $this->m_destInstance->selectDb("","","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));

        // autoselect if there is only one record (if obligatory is not set,
        // we don't autoselect, since user may wist to select 'none' instead
        // of the 1 record.
        if (count($recordset)==1&&$this->hasFlag(AF_OBLIGATORY))
        {
          $result = $this->m_destInstance->descriptor($recordset[0])."&nbsp;&nbsp;";
          $result.= $this->hide(array($this->fieldName()=>$recordset[0]), $fieldprefix);
        }
        else
        {
          $autolink = "";
          if ($this->hasFlag(AF_RELATION_AUTOLINK)) // auto edit/view link
          {
            global $g_layout;
            $g_layout->register_script("atk/javascript/class.atkmanytoonerelation.js");
            $editlink = session_url("dispatch.php?atknodetype=".$this->m_destination."&atkaction=edit&atkselector=REPLACEME",SESSION_NESTED);

            $autolink = "<a href='javascript:atkSubmit(mto_parse(\"".atkurlencode($editlink)."\", document.entryform.".$fieldprefix.$this->fieldName().".value))'>".text('edit')."</a>";
            $autolink.= "&nbsp;".href("dispatch.php?atknodetype=".$this->m_destination."&atkaction=add".($filter!=""?"&atkfilter=".rawurlencode($filter):""),text("new"),SESSION_NESTED,true);
          }
          $result.= '<select name="'.$fieldprefix.$this->formName().'">';
          if ($this->hasFlag(AF_OBLIGATORY)==false)
          {
            // Relation may be empty, so we must provide an empty selectable..
            $result.= '<option value="'.$this->m_destInstance->m_table.".".$this->m_destInstance->primaryKeyField().'=0">'.text('select_none');
          }

          for ($i=0;$i<count($recordset);$i++)
          {
            $pk = $this->m_destInstance->primaryKey($recordset[$i]);
            if ( $pk== $currentPk) $sel = "selected"; else $sel = "";
            $result.= '<option value="'.$pk.'" '.$sel.'>'.$this->m_destInstance->descriptor($recordset[$i]);
          }
          $result.='</select>';

          $result.=$autolink;
        }
      }
      else
      {
        // Large mode
        // TODO: Fill a dropdown box with recent records..

        $selname = $fieldprefix.$this->fieldName()."_newsel";
        $remotekey = $this->m_destInstance->primaryKeyField();

        if ($ATK_VARS[$selname]!="")
        {
          // new record selected..
          $record[$this->fieldName()][$remotekey] = $ATK_VARS[$selname];
        }

        $currentPk = $this->m_destInstance->primaryKey($record[$this->fieldName()]);

        $result.=$this->hide($record, $fieldprefix);

        if ($record[$this->fieldName()][$remotekey]!="")
        {
          // TODO FIXME: this selectDb isn't always necessary. In regular edit mode (no new
          // record selected), the record is already present in $record, so this selectDb
          // is not necessary.
          $selrec = $this->m_destInstance->selectDb($currentPk,"","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));

          $result.= $this->m_destInstance->descriptor($selrec[0])."&nbsp;&nbsp;";
        }

        // we use the current level to automatically return to this page
        // when we come from the select..
        $atktarget = rawurlencode("dispatch.php?atklevel=".atkLevel()."&".$selname."=[".$remotekey."]");

        $result.= href("dispatch.php?atknodetype=".$this->m_destination.
                                   "&atkaction=select&atktarget=".$atktarget,
                       text("link_select_".getNodeType($this->m_destination)),
                       SESSION_NESTED,
                       true);

        //var_dump($record[$this->fieldName()]);
      }
      return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="", $fieldprefix="")
    {
      if ($this->createDestination())
      {
        $currentPk = $this->m_destInstance->primaryKey($record[$this->fieldName()]);
        $result = '<input type="hidden" name="'.$fieldprefix.$this->formName().
                  '" value="'.$currentPk.'">';
        return $result;
      }
      return "";
    }


    /**
     * Returns a piece of html code that can be used in a form to search
     * @param $record Record
     * @param $size Size of the editbox
     * @param $maxsize Maxsize of the editbox
     * @return Piece of html code that can  be used in a form to edit this
     */
    function search($record="", $extended=false)
    {
      if ($this->createDestination())
      {
        if ($this->m_destinationFilter!="")
        {
          $this->m_destInstance->addFilter(stringparse($this->m_destinationFilter,$record));
        }
        $recordset = $this->m_destInstance->selectDb("","","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));

        $result = '<select ';
        if ($extended)
        {
          $result.='multiple size="'.min(5,count($recordset)+1).'"';
        }
        $result.='name="atksearch['.$this->fieldName().'][]">';

        $pkfield = $this->m_destInstance->primaryKeyField();

        $result.= '<option value="">'.text('search_all');

        for ($i=0;$i<count($recordset);$i++)
        {
          $pk = $recordset[$i][$pkfield];
          if (atk_in_array($pk, $record[$this->fieldName()])) $sel = "selected"; else $sel = "";
          $result.= '<option value="'.$pk.'" '.$sel.'>'.$this->m_destInstance->descriptor($recordset[$i]);
        }
        $result.='</select>';
        return $result;
      }
      return "";
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
        if (count($value)==1) // exactly one value
        {
          $query->addSearchCondition($query->exactCondition($table.".".$this->fieldName(),escapeSQL($value[0])));
        }
        else // search for more values using IN()
        {
          $query->addSearchCondition($table.".".$this->fieldName()." IN ('".implode("','",$value)."')");
        }
      }
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
      if ($mode != "update" && $mode != "add")
      {
        if ($tablename!="") $realtablename=$tablename.".";
        if ($this->createDestination())
	{
          $query->addJoin($this->m_destInstance->m_table, $fieldaliasprefix.$this->fieldName(),$realtablename.$this->fieldName()."=".$fieldaliasprefix.$this->fieldName().".".$this->m_destInstance->m_primaryKey[0], $this->m_leftjoin);
          $this->m_destInstance->addToQuery($query, $fieldaliasprefix.$this->fieldName(), $level+1);
	}
      }
      else
      {
        $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
      }
    }


    /**
     * Dummy function
     */
    function validate(&$record, $mode)
    {
    }

    function equal($recA, $recB)
    {
      if ($this->createDestination())
      {
        return (($recA[$this->fieldName()][$this->m_destInstance->primaryKeyField()]
                 ==
                 $recB[$this->fieldName()][$this->m_destInstance->primaryKeyField()])
               ||
                ($this->isEmpty($recA)&&$this->isEmpty($recB)));
             // we must also check empty values, because empty values need not necessarily
             // be equal (can be "", NULL or 0.
      }
      return false;
    }
    
    function dbFieldType()
    {
      // The type of field that we need to store the foreign key, is equal to 
      // the type of field of the primary key of the node we have a 
      // relationship with.
      if ($this->createDestination())
      {
        return $this->m_destInstance->m_attribList[$this->m_destInstance->primaryKeyField()]->dbFieldType();
      }
      return "";
    }
    
    function dbFieldSize()
    {
      // The size of the field we need to store the foreign key, is equal to 
      // the size of the field of the primary key of the node we have a 
      // relationship with.
      if ($this->createDestination())
      {
        return $this->m_destInstance->m_attribList[$this->m_destInstance->primaryKeyField()]->dbFieldSize();
      }
      return 0;
    }
  }

?>
