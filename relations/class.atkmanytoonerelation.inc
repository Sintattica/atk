<?php
  /**
   * A N:1 relation between two classes.
   * For example, projects all have one coordinator, but one
   * coordinator can have multiple projects. So in the project
   * class, there's a ManyToOneRelation to a coordinator.
   *
   * @author Ivo Jansch <ivo@achievo.com>
   * @version 0.8
   *
   * <b>Changed</b>
   *
   * 0.8 :   - Fixed the 0.7 fix, it didn't work for PHP3. (Ivo)
   * 0.7 :   - Fix a bug when atkManyToOneRelation was used as a primary key. (Ivo)
   *
   * $Id$
   * $Log$
   * Revision 4.18  2001/09/12 15:07:07  ivo
   * Added an option to manytoonerelation to perform non-leftjoins.
   *
   * Revision 4.17  2001/08/31 15:28:44  ivo
   * manytoonerelation now stores '0' instead of '' if no record is selected.
   *
   * Revision 4.16  2001/08/27 07:38:58  ivo
   * changed validate() api: atk now passes record to the function instead of
   * just one value. Also added a mode parameter that indicates whether we
   * are in add or update mode.
   *
   * Revision 4.15  2001/08/16 08:37:11  ivo
   * Added an equal() function to atkAttribute, to compare two values.
   *
   * Revision 4.14  2001/07/15 16:37:19  ivo
   * New atk.inc includefile in skel.
   * New feature: extended search.
   * Fixed a bug in session management; style.php and other files that get
   * loaded between two dispatches could corrupt a session.
   *
   * Revision 4.13  2001/06/27 15:25:43  peter
   * A lot of fixes to the oci8 support, including better outer joins, and
   * limitting queries! Fields now have an asterix behind them if they are
   * required. Some other small enhancements.
   *
   * Revision 4.12  2001/06/20 14:33:51  ivo
   * ManyToOneRelation now supports templated destinationfilters.
   *
   * Revision 4.11  2001/05/22 08:38:40  peter
   * Added searchCondition method for this relation, note this method
   * should be extended to create a search condition for the descriptor fields!
   *
   * Revision 4.10  2001/05/18 07:49:42  ivo
   * Fixes to sessionmanagement.
   *
   * Revision 4.9  2001/05/17 10:27:12  ivo
   * Fixes to session management, new AF_LARGE feature for manytoonerelation.
   * Improved handling of GET_VARS and POST_VARS (requires a new dispatcher)
   *
   * Revision 4.8  2001/05/16 06:52:25  ivo
   * removed php short-tags.
   *
   * Revision 4.7  2001/05/11 12:25:52  ivo
   * Fixed bugs in atkOneToOneRelation and atkManyToManyRelation (bug only
   * appeared when the two were used in combination)
   *
   * Revision 4.6  2001/05/10 08:31:01  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 4.5  2001/05/07 15:13:49  ivo
   * Put config_atkroot in all files.
   *
   * Revision 4.4  2001/05/04 11:37:42  sandy
   * small bug fixes (split function, refresh, treeview,etc)
   *
   * Revision 4.3  2001/05/02 12:44:21  ivo
   * multilevel session_back feature.
   * new relational features: maxRecords for oneToManyRelation, filter for
   * onetomanyrelation.
   *
   * Revision 4.2  2001/05/01 12:32:54  sandy
   * removed the debug functions and created a new Tree relations
   *
   * Revision 4.1  2001/04/23 13:21:22  peter
   * Introduction of module support. An ATK application can now have zero
   * or more modules which can, but don't have to, contain ATK nodes.
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:13  ivo
   * Added atk4 to repository.
   *
   * Revision 1.3  2001/04/04 09:01:49  ivo
   * AF_READONLY now also affects add forms.
   * Implemented hide() method for manytoonerelation.
   *
   * Revision 1.2  2001/02/22 22:47:16  peter
   * changed search/edit API, some clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:58:00  sandy
   * Achievo Tool Kit
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

      return $this->m_destInstance->descriptor($record[$this->fieldName()]);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Record
     * @return Piece of html code that can  be used in a form to edit this
     */
    function edit($record="", $fieldprefix="")
    {
      global $atklevel, $ATK_VARS;  
    
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
          $this->m_destInstance->addFilter(stringparse($this->m_destinationFilter,$record));
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
          $result = '<select name="'.$fieldprefix.$this->formName().'">';
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
        $atktarget = rawurlencode("dispatch.php?atklevel=".$atklevel."&".$selname."=[".$remotekey."]");
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
      $query->addCondition($query->exactCondition($table.".".$this->fieldName(),escapeSQL($value)));
      
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
      //  $condition .= $p_attrib->searchCondition(&$this->m_destInstance->m_table, $value).($i < $desc_size-1 ? " OR " : "");
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
      if ($mode == "select")
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

      else if ($mode == "add" || $mode == "edit")
        $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
    }


    /**
     * Dummy function
     */
    function validate($record, $mode)
    {
      return "";
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
