<?php
  /**
   * The atkRelation class defines a relation to an attribute
   * (or a relation between two attributes?)
   * (maybe a source and destination node?)
   * (and should we distinguish between 1:1, 1:N, N:M relations?)
   * Note        : A relation is a (very) special kind of attribute.
   * @author      : Ivo Jansch <ivo@achievo.com>
   * @version     : 0.5
   *
   * $Id$
   * $Log$
   * Revision 4.2  2001/05/01 09:49:49  ivo
   * Replaced all require() and include() calls by require_once() and
   * include_once() calls. The if(!DEFINED)... inclusion protection in files
   * is now obsolete.
   *
   * Revision 4.1  2001/04/25 08:46:08  ivo
   * Big optimisations: using references for attribute manipulation.
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:13  ivo
   * Added atk4 to repository.
   *
   * Revision 1.4  2001/04/10 14:43:24  ivo
   * Removed debug output.
   *
   * Revision 1.3  2001/04/10 13:11:57  ivo
   * Fixes and optimisations to all relations.
   *
   * Revision 1.2  2001/02/22 22:47:16  peter
   * changed search/edit API, some clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:57:59  sandy
   * Achievo Tool Kit
   *
   */
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
    function edit($record="")
    {
    //var_dump($record);
      $this->createDestination();    

      $selectedPk = array();
      
      // first the selected records..
      for ($i=0;$i<count($record[$this->m_name]);$i++)
      {
        $this->m_destInstance->m_records[0] = $record[$this->m_name][$i][$this->remoteKey()];
        $selectedPk[] = $this->m_destInstance->primaryKey();
      }     

      // now select all records 
      $this->m_destInstance->selectDb();

      for ($this->m_destInstance->m_currentRec=0;$this->m_destInstance->m_currentRec<count($this->m_destInstance->m_records);$this->m_destInstance->m_currentRec++)
      {

        if (in_array($this->m_destInstance->primaryKey(),$selectedPk))
        {
          $sel = "checked";
        }
        else
        {
          $sel = "";
        }
        $result.= '<input type="checkbox" name="'.$this->fieldName().'_AMDAE_'.$this->remoteKey().'[]" value="'.$this->m_destInstance->m_records[$this->m_destInstance->m_currentRec][$this->m_destInstance->primaryKeyField()].'" '.$sel.'>'.$this->m_destInstance->descriptor().'<br>'."\n";
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
      include_once("class.".$classname.".inc");
      $rel = new $classname();
            
      $this->createDestination();
      
      $rel->deleteDb($this->localKey()."=".$record[$this->m_ownerInstance->primaryKeyField()]);     
      
      for ($i=0;$i<count($record[$this->fieldName()]);$i++)
      { 
        // magical stuff..
        
        $rel->m_records[0][$this->remoteKey()][$this->m_destInstance->primaryKeyField()] = $record[$this->fieldName()][$i][$this->remoteKey()];        
    
        $rel->m_records[0][$this->localKey()][$this->m_ownerInstance->primaryKeyField()] = $record[$this->m_ownerInstance->primaryKeyField()];
        
        $rel->addDb();
      }
    }   

  }
