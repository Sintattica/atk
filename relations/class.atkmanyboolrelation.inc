<?php
  /**
   *
   *
   * @author      : Ivo Jansch <ivo@achievo.org>
   * @version     : $Revision$
   *
   * $Id$
   *
   */
  include_once($config_atkroot."atk/relations/class.atkmanytomanyrelation.inc");  
   
  class atkManyBoolRelation extends atkManyToManyRelation
  {
 
    /** 
    * Constructor
    * @param $name
    * @param $destination
    * @param $flags
    */
    function atkManyBoolRelation($name, $destination, $flags=0)
    {
      $this->atkManyToManyRelation($name, $destination, $flags);
    }
    
    /** 
     * Return a piece of html code to edit the attribute
     * @param $record Current record
     * @result piece of html code
     */
    function edit($record="", $fieldprefix="")
    {
    //var_dump($record);
      $this->createDestination();    

      $selectedPk = array();
      
      // first the selected records..
      for ($i=0;$i<count($record[$this->m_name]);$i++)
      {        
        $selectedPk[] = $this->m_destInstance->primaryKey($record[$this->m_name][$i][$this->remoteKey()]);
      }     

      // now select all records 
      $recordset = $this->m_destInstance->selectDb();

      for ($i=0;$i<count($recordset);$i++)
      {

        if (in_array($this->m_destInstance->primaryKey($recordset[$i]),$selectedPk))
        {
          $sel = "checked";
        }
        else
        {
          $sel = "";
        }
        $result.= '<input type="checkbox" name="'.
                       $fieldprefix.$this->fieldName().
                       '_AMDAE_'.$this->remoteKey().
                       '[]" value="'.
                       $recordset[$i][$this->m_destInstance->primaryKeyField()].
                       '" '.$sel.'>'.
                       $this->m_destInstance->descriptor($recordset[$i]).'<br>'."\n";
      }
      return $result;
    }

    /**
     * Stores the values in the database
     * @param $notused Not used
     * @param $record Current record
     */    
    function store($notused, $record)
    {
      $classname = $this->m_name;      
      $rel = &getNode($classname);
            
      $this->createDestination();
      
      $rel->deleteDb($this->localKey()."=".$record[$this->m_ownerInstance->primaryKeyField()]);     
      
      for ($i=0;$i<count($record[$this->fieldName()]);$i++)
      { 
        // magical stuff..
        $newrecord = array();
        $newrecord[$this->remoteKey()][$this->m_destInstance->primaryKeyField()] = $record[$this->fieldName()][$i][$this->remoteKey()];        
    
        $newrecord[$this->localKey()][$this->m_ownerInstance->primaryKeyField()] = $record[$this->m_ownerInstance->primaryKeyField()];
        
        $rel->addDb($newrecord);
      }
    }   

  }
