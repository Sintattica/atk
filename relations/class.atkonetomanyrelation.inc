<?php
  /**
   * Can be used to create '1 library has N books' relations.
   *
   * @author Ivo Jansch (ivo@achievo.com)
   * @version $Revision$
   *
   * $Id$
   *
   */
  class atkOneToManyRelation extends atkRelation
  {
    /*** Member variables ***/
    
    /**
     * The referential key in the target node
     */
    var $m_refKey="";
    
    /** 
     * The maximum number of records that you may have a relation with..
     */
    var $m_maxRecords = 0;
  
    /**
     * Constructor
     * @param $name ?
     * @param $destination ?
     * @param $flags Flags for the relation
     */
    function atkOneToManyRelation($name, $destination, $refKey="", $flags=0)
    {
      $this->atkRelation($name, $destination, $flags|AF_HIDE_ADD|AF_HIDE_SEARCH); // 1toM Relations are NEVER edited when adding a rec.
                                                                                 // and for now, we cannot search them.
      $this->m_refKey = $refKey;
    }

    /**
     * Returns a displayable string for this value.
     * @param $record Record
     * @return a displayable string
     */
    function display($record)
    {
      $myrecords = $record[$this->fieldName()];
      
      $this->createDestination();
      
      if (count($myrecords)!=0)
      {
        $result = "<ul>";
        for ($i = 0; $i < count($myrecords);$i++)
        {
          $result.="<li>".$this->m_destInstance->descriptor($myrecords[$i]);
        }
        $result.="</ul>";
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
      atkdebug("edit called for ".$this->fieldName());

      $this->createDestination();      

      $output =$GLOBALS['g_layout']->ret_table_simple(0,true);
      
      $myrecords = $record[$this->fieldName()];
                  

      if (count($myrecords)==0)
      {
        // If someone pressed 'save', no records are loaded from the db.
        // since we don't post all records in this relation like we do with
        // other attributes, we have to load them at 'edit' time.
//        $myrecords = $this->load($notused, array($keyfield=>$record[$keyfield]));
        $myrecords = $this->load($notused, $record);
      }
      
      if (count($myrecords)!=0)
      {
        $output.="<tr>";
        $cell="";                       
        
        $actions = array();
        if ($this->m_destinationFilter!="")
        {
          $strfilter = '&atkfilter='.rawurlencode($this->m_destinationFilter);
        }
        if (!$this->m_destInstance->hasFlag(NF_NO_EDIT))
        {
          $actions[] = href('dispatch.php?atknodetype='.$this->m_destination.'&atkaction=edit&atkselector=[pk]'.$strfilter,text("edit"),SESSION_NESTED,true);
        }        
        if (!$this->m_destInstance->hasFlag(NF_NO_DELETE))
        {
          $actions[] = href('dispatch.php?atknodetype='.$this->m_destination.'&atkaction=delete&atkselector=[pk]'.$strfilter,text("delete"),SESSION_NESTED,true);
        }                 
        
        // User may not search in embedded forms..        
        $this->m_destInstance->m_flags |= NF_NO_SEARCH;    
        $cell.=$this->m_destInstance->recordList($myrecords, $actions,false,array($this->m_refKey));
      
        $output.=$GLOBALS['g_layout']->ret_td($cell);
        $output.="</tr>";
      }

      if (($this->m_maxRecords==0||$this->m_maxRecords>count($myrecords))&&!$this->m_destInstance->hasFlag(NF_NO_ADD))
      {
        //$strfilter = $this->m_refKey.".".$this->m_ownerInstance->primaryKeyField()."=".$record[$this->m_ownerInstance->primaryKeyField()];
      //echo $record["atkprimkey"];
        // we need to set the filter of the record we are going to add. The referential key must
        // be set to the value of the current primary key. However, the current primary key 
        // contains the table name and a dot. So we strip that.
        list($tmp,$refkeyvalue) = explode(".",$record["atkprimkey"]);
        $strfilter = $this->m_refKey.".".$refkeyvalue;

        if ($this->m_destinationFilter!="")
        {
          $strfilter.=' AND '.$this->m_destinationFilter;
        }
        $add_url = 'dispatch.php?atknodetype='.$this->m_destination.'&atkaction=add&atkfilter='.rawurlencode($strfilter);
        $output.="<tr><td>".href($add_url,text("link_".getNodeType($this->m_destination)."_add"),SESSION_NESTED,true)."</td></tr>";      
      }
      $output.="</table><br>";
      return $output;
    } 

    /**
     * load values 
     * @param $notused ?
     * @param $record Record
     * @return ??
     */
    function load($notused, $record)
    {
      atkdebug("load called for ".$this->m_name);
      $this->createDestination();
      if ($this->m_refKey=="") $this->m_refKey=$this->m_owner;            
      
      
      $key = decodeKeyValueSet($record["atkprimkey"]);
      // we assume singular referential keys, which means, there's only one value in $keyvalue;
      list($tmp,$keyvalue) = each($key);
      
      $where = $this->m_destInstance->m_table.".".$this->m_refKey."='".$keyvalue."'";

      if ($this->m_destinationFilter!="")
      {
        $this->m_destInstance->addFilter($this->m_destinationFilter);
      }
      $recordset = $this->m_destInstance->selectDb($where,"","",$this->m_destInstance->m_listExcludes);
      
      return $recordset;
    }


  /** 
     * delete relational records..
     */
    function delete($record)
    {
      $classname = $this->m_destination;      
      $rel = getNode($classname);      
      atkdebug("O2M DELETE voor $classname: ".$this->m_refKey."='".$record[$this->m_ownerInstance->primaryKeyField()]."'");
 
      $rel->deleteDb($rel->m_table.'.'.$this->m_refKey."='".$record[$this->m_ownerInstance->primaryKeyField()]."'");
    }
    

    /**
     * Store values only when it's an array and not empty
     * For onetomanyrelation, this function does not have much use, since it 
     * stores records using it's 'add link'. But the copyDb function however
     * might use this.
     * To prevent double storage when clicking the "save" button on a form
     * we only store something when in add mode. In 'normal' add forms, we 
     * don't have a oneToManyRelation, so this add is only called when 
     * doing 'abnormal' adds, like a copy function.
     *
     * @param $notused ??
     * @param $record Record
     */
    function store($notused, $record, $mode)
    { 
      if ($mode=="add") // when copying, mode is "add".
      {
        $onetomanyrecs = $record[$this->fieldName()];
        if (is_array($onetomanyrecs)&&count($onetomanyrecs)>0)
        {
          $this->createDestination();
          for ($i=0;$i<count($onetomanyrecs);$i++)
          {
            // the referential key of the onetomanyrecs could be wrong, if we 
            // are called for example from a copy function. So just in case, 
            // we reset the correct key.
            $onetomanyrecs[$i][$this->m_refKey][$this->m_ownerInstance->primaryKeyField()] = $record[$this->m_ownerInstance->primaryKeyField()];
          
            $this->m_destInstance->addDb($onetomanyrecs[$i]);
          }
        }
      }
    }
  }
?>