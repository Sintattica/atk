<?php
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
   *
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
        $this->createDestination();
        $pkfield = $this->m_destInstance->m_primaryKey[0];
        return $rec[$this->fieldName()][$pkfield];
      }
      else
      {
        // This never happens, does it?      
        return "";
      }
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
        $this->createDestination();        
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
      $this->createDestination();

      $title = $this->m_destInstance->descriptor($record[$this->fieldName()]);
      
      $result = $title; // default result is just the title.            
      
      if ($this->hasFlag(AF_RELATION_AUTOLINK)) // create link to edit/view screen
      {
        if ($this->m_destInstance->allowed("edit")&&!$this->m_destInstance->hasFlag(NF_NO_EDIT))
        {
          $result = href("dispatch.php?atknodetype=".$this->m_destination."&atkaction=edit&atkselector=".rawurlencode($this->m_destInstance->primaryKey($record[$this->fieldName()])), $title, SESSION_NESTED);
        }
        else if ($this->m_destInstance->allowed("view")&&!$this->m_destInstance->hasFlag(NF_NO_VIEW))
        {        
          $result = href("dispatch.php?atknodetype=".$this->m_destination."&atkaction=view&atkselector=".rawurlencode($this->m_destInstance->primaryKey($record[$this->fieldName()])), $title, SESSION_NESTED);
        }
      }
      
      return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Record
     * @return Piece of html code that can  be used in a form to edit this
     */
    function edit($record="", $fieldprefix="")
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
      $this->createDestination();            
      $currentPk = $this->m_destInstance->primaryKey($record[$this->fieldName()]);
      $result = '<input type="hidden" name="'.$fieldprefix.$this->formName().
                '" value="'.$currentPk.'">';
      return $result;             
    }


    /**
     * Returns a piece of html code that can be used in a form to search
     * @param $record Record
     * @param $size Size of the editbox
     * @param $maxsize Maxsize of the editbox
     * @return Piece of html code that can  be used in a form to edit this
     */
    function search($record="")
    {
      $this->createDestination();
      if ($this->m_destinationFilter!="")
      {
        $this->m_destInstance->addFilter(stringparse($this->m_destinationFilter,$record));
      }
      $recordset = $this->m_destInstance->selectDb("","","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));

      $result = '<select name="atksearch['.$this->fieldName().']">';

      $pkfield = $this->m_destInstance->primaryKeyField();

      $result.= '<option value="">'.text('search_all');

      for ($i=0;$i<count($recordset);$i++)
      {
        $pk = $recordset[$i][$pkfield];
        if ($record[$this->fieldName()]==$pk) $sel = "selected"; else $sel = "";
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
      $query->addSearchCondition($query->exactCondition($table.".".$this->fieldName(),escapeSQL($value)));
      
      //return $table.".".$this->fieldName()." = '".$value."'";
      
      // *** THIS DOES NOT WORK AT THE MOMENT BECAUSE OF THE WAY ATK CREATES JOINS!!! *** 
  //    $descriptor = $this->m_destInstance->descriptorFields();
//      $desc_size = count($descriptor);
      
      /* no descriptor fields */
      //if ($desc_size == 0) 
      //{
//        return $table.".".$this->fieldName()." = '".$value."'";

      /* build condition based on descriptor fields */
      //$condition = "(";
      //for ($i = 0; $i < $desc_size; $i++)
      //{
      //  $p_attrib = &$this->m_destInstance->m_attribList[$descriptor[$i]];
      //  $condition .= $p_attrib->searchCondition($this->m_destInstance->m_table, $value).($i < $desc_size-1 ? " OR " : "");
      //}
      //$condition .= ")";
     
      /* return search condition */
      //return $condition;
//willrhee to change above?          $query->addJoin($this->m_destInstance->m_table, $this->fieldName(),$realtablename.$this->fieldName()."=".$this->fieldName().".".$this->m_destInstance->m_primaryKey[0], !$this->hasFlag(AF_OBLIGATORY));
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
        if ($level<2)        
        {
          $this->createDestination();
          $this->m_destInstance->addToQuery($query, $this->fieldName(), $level+1);

          $query->addJoin($this->m_destInstance->m_table, $this->fieldName(),$realtablename.$this->fieldName()."=".$this->fieldName().".".$this->m_destInstance->m_primaryKey[0], $this->m_leftjoin);
        }
        else
        {
          // only primary key..         
          $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
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
      $this->createDestination();
      return ($recA[$this->fieldName()][$this->m_destInstance->primaryKeyField()]
              ==
              $recB[$this->fieldName()][$this->m_destInstance->primaryKeyField()]);
    }
  }
  
  include $config_atkroot."atk/relations/class.atkmanytoonetreerelation.inc";
?>
