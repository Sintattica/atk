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
    function store($notused, $record, $mode)
    {
      $this->createLink();
      $this->createDestination();
      $rel = &$this->m_linkInstance;

      if (is_object($rel) && $rel->deleteDb($this->localKey()."=".$record[$this->m_ownerInstance->primaryKeyField()]))
      {
        for ($i=0;$i<count($record[$this->fieldName()]);$i++)
        {
          // magical stuff..
          $newrecord = array();
          $newrecord[$this->remoteKey()][$this->m_destInstance->primaryKeyField()] = $record[$this->fieldName()][$i][$this->remoteKey()];
  
          $newrecord[$this->localKey()][$this->m_ownerInstance->primaryKeyField()] = $record[$this->m_ownerInstance->primaryKeyField()];
  
          if (!$rel->addDb($newrecord, true, $mode))
          {            
            return false;
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

  }

?>
