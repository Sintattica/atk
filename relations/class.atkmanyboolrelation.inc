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
   * Revision 4.5  2001/07/15 16:37:19  ivo
   * New atk.inc includefile in skel.
   * New feature: extended search.
   * Fixed a bug in session management; style.php and other files that get
   * loaded between two dispatches could corrupt a session.
   *
   * Revision 4.4  2001/05/10 08:31:01  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 4.3  2001/05/07 15:13:49  ivo
   * Put config_atkroot in all files.
   *
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
      $rel = getNode($classname);
            
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
