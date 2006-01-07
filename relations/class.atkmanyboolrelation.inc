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
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
  
  /** @internal include baseclass. */
  userelation("atkmanytomanyrelation");
   
  /**
   * Many-to-many relation.
   *
   * The relation shows a list of available records, and a set of checkboxes
   * to link the records with the current record on the source side.
   *   
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage relations
   *
   */  
  class atkManyBoolRelation extends atkManyToManyRelation
  {
    var $m_cols=3;

    /**
     * Constructor.
     * @param String $name The name of the relation.
     * @param String $link The full name of the node that is used as
     *                     intermediairy node.
     * @param String $destination The full name of the node that is the
     *                            destination end of the relationship.
     * @param int $flags Attribute flags.
     */
    function atkManyBoolRelation($name, $link, $destination, $flags=0)
    {
      $this->atkManyToManyRelation($name, $link, $destination, $flags);
    }

    /**
     * Return a piece of html code to edit the attribute
     * @param $record Current record
     * @return String piece of html code
     */
    function edit($record="", $fieldprefix="")
    {
      $cols = $this->m_cols;
      $modcols = $cols-1;
      $this->createDestination();
      $this->createLink();

      $selectedPk = array();

      // first the selected records..
      for ($i=0;$i<count($record[$this->m_name]);$i++)
      {

        if(is_array($record[$this->fieldName()][$i][$this->getRemoteKey()]))
          $newselected = $this->m_destInstance->primaryKey($record[$this->m_name][$i][$this->getRemoteKey()]);
        else{
          $newselected = $this->m_destInstance->primaryKey(array($this->m_destInstance->primaryKeyField()=>$record[$this->m_name][$i][$this->getRemoteKey()]));
        }
        $selectedPk[] = $newselected;
       }

      if ($this->m_destinationFilter!="")
      {
	      $parser = new atkStringParser($this->m_destinationFilter);
	      $filter = $parser->parse($record);
	      $recordset = $this->m_destInstance->selectDb($filter);
      }
      else
      // now select all records
      $recordset = $this->m_destInstance->selectDb();

      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkprofileattribute.js.php");
	  
      $result ='<div align="left"><font size="-2">
                [<a href="javascript:profile_checkAll(\''.$this->fieldName().'\')">'.
                     atktext("check_all", "atk").
                '</a> <a href="javascript:profile_checkNone(\''.$this->fieldName().'\')">'.
                     atktext("check_none", "atk").
                '</a> <a href="javascript:profile_checkInvert(\''.$this->fieldName().'\')">'.
                     atktext("invert_selection", "atk").'</a>]</font></div>';
       	
      $result.= '<table border="0"><tr>';
      for ($i=0;$i<count($recordset);$i++)
      {
        $detaillink = "&nbsp;";
        $selector="";
        if (in_array($this->m_destInstance->primaryKey($recordset[$i]),$selectedPk))
        {
          $sel = "checked";
          if (!$this->m_linkInstance->hasFlag(NF_NO_EDIT)&&$this->m_linkInstance->allowed("edit"))
          {
            $selector = $this->m_linkInstance->m_table.'.'.$this->getLocalKey()."='".$record[$this->m_ownerInstance->primaryKeyField()]."'".
              ' AND '.$this->m_linkInstance->m_table.'.'.$this->getRemoteKey().
              "='".$recordset[$i][$this->m_destInstance->primaryKeyField()]."'";

            // Create link to details.
            $detaillink = href(dispatch_url($this->m_link, "edit", array("atkselector"=>$selector)),
              "[".atktext("details", "atk")."]",
              SESSION_NESTED,
              true);
          }
        }
        else
        {
          $sel = "";
        }
        $result.= '<td class="table"><input type="checkbox" name="'.
                       $fieldprefix.$this->fieldName().
                       '_AMDAE_'.$this->getRemoteKey().
                       '[]" value="'.
                       $recordset[$i][$this->m_destInstance->primaryKeyField()].
                       '" '.$sel.'></td><td class="table">'.
                       $this->m_destInstance->descriptor($recordset[$i]).
                       '</td><td class="table">'.$detaillink.'</td>';
		if ($i%$cols == $modcols) $result .="</tr><tr>\n";
      }
      $result.="</tr></table>\n";
      return $result;
    }

    /**
     * Stores the values in the database
     * @param $notused Not used
     * @param $record Current record
     */
    function store($notused, $record, $mode)
    {
      //atkerror(arrayToString($record));
    
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
      
      if(count($checked) > 0)
        $filter = "NOT IN ('".implode("','",$checked)."')";

      if (is_object($rel) && $rel->deleteDb($this->m_linkInstance->m_table.'.'.$this->getLocalKey()."='".$record[$this->m_ownerInstance->primaryKeyField()]."' AND ".$this->m_linkInstance->m_table.'.'.$this->getRemoteKey()." $filter"))
      {
        for ($i=0;$i<count($record[$this->fieldName()]);$i++)
        {
          // magical stuff..
          $newrecord = $this->m_destInstance->initial_values(); 
          $newrecord[$this->getRemoteKey()][$this->m_destInstance->primaryKeyField()] = $record[$this->fieldName()][$i][$this->getRemoteKey()];
  
          $newrecord[$this->getLocalKey()][$this->m_ownerInstance->primaryKeyField()] = $record[$this->m_ownerInstance->primaryKeyField()];
					
          // First check if the record does not exist yet.
          /* @var $rel atkNode */
          $existing = $rel->selectDb($rel->primaryKey($newrecord), "", "", "", $rel->m_primaryKey);
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
      if(is_array($record[$this->fieldName()]))
      {
        for ($i=0;$i<count($this->m_values);$i++)
        {
          if(in_array($this->m_values[$i],$record[$this->fieldName()]))
          $result .= '<input type="hidden" name="'.$fieldprefix.$this->fieldName().'[]"
                      value="'.$this->m_values[$i].'">';
        }
      } else return parent::hide($record,$fieldprefix);
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
      $recordset = $this->m_destInstance->selectDb();

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
    		$query->addJoin($this->m_linkInstance->m_table, $this->fieldName(), $table.".".$this->m_ownerInstance->primaryKeyField()."=".$this->fieldName().".".$this->getLocalKey(),FALSE );//!$this->hasFlag(AF_OBLIGATORY)
    		$query->setDistinct(TRUE);
        if (count($value)==1) // exactly one value
        {
          $query->addSearchCondition($query->exactCondition($this->fieldName().".".$this->getRemoteKey(),escapeSQL($value[0])));
        }
        else // search for more values using IN()
        {
          $query->addSearchCondition($this->fieldName().".".$this->remoteKey()." IN ('".implode("','",$value)."')");
        }
      }
    }
  }
?>