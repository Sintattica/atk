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
        //list($key,$value) = each($rec[$this->fieldName()]); // we only support relations with a single pk.. (the first element)
        //return $value;
        // The above doesn't work.. we have to do the clean thing..
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

      return Array($field=>$value);
    }


    /**
     * Converts DataBase value to normal value
     * @param $rec Record
     * @return decoded value
     */
    function db2value($rec)
    {    
      return $rec[$this->fieldName()];
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
      $this->createDestination();
      $recordset = $this->m_destInstance->selectDb("","","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));
      
      $currentPk = $this->m_destInstance->primaryKey($record[$this->fieldName()]);

      $result = '<select name="'.$fieldprefix.$this->formName().'">';

      if ($this->hasFlag(AF_OBLIGATORY)==false)
      {
        // Relation may be empty, so we must provide an empty selectable..
        $result.= '<option value="0">'.text('select_none');
      }

      for ($i=0;$i<count($recordset);$i++)
      {
        $pk = $this->m_destInstance->primaryKey($recordset[$i]);
        if ( $pk== $currentPk) $sel = "selected"; else $sel = "";
        $result.= '<option value="'.$pk.'" '.$sel.'>'.$this->m_destInstance->descriptor($recordset[$i]);
      }
      $result.='</select>';
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
        if ($tablename!="") $tablename.=".";
        if ($level<2)
        {
          $this->createDestination();
          $this->m_destInstance->addToQuery($query, $this->fieldName(), $level+1);

          $query->addJoin($this->m_destInstance->m_table, $this->fieldName(),$tablename.$this->fieldName()."=".$this->fieldName().".".$this->m_destInstance->m_primaryKey[0], true);
        }
      }

      else if ($mode == "add" || $mode == "edit")
        $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
    }


    /**
     * Dummy function
     */
    function validate($value)
    {
      return "";
    }
  }
  include $config_atkroot."atk/relations/class.atkmanytoonetreerelation.inc";
?>
