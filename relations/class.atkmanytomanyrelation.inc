<?php
  /**
   * The atkRelation class defines a relation to an attribute
   * (or a relation between two attributes?)
   * (maybe a source and destination node?)
   * (and should we distinguish between 1:1, 1:N, N:M relations?)
   * Note        : A relation is a (very) special kind of attribute.
   * @author Ivo Jansch (ivo@achievo.com)
   * @version 0.5
   *
   * $Id$
   * $Log$
   * Revision 1.1  2001/04/23 10:17:13  ivo
   * Initial revision
   *
   * Revision 1.3  2001/04/10 13:11:57  ivo
   * Fixes and optimisations to all relations.
   *
   * Revision 1.2  2001/02/22 22:47:16  peter
   * changed search/edit API, some clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:58:00  sandy
   * Achievo Tool Kit
   *
   */

  class atkManyToManyRelation extends atkRelation
  {
    var $m_localKey = "";
    var $m_remoteKey = "";

    /**
    * Constructor
    * @param $name ?
    * @param $destination ?
    * @param $flags Flags for the relation
    */
    function atkManyToManyRelation($name, $destination, $flags=0)
    {
      $this->atkRelation($name, $destination, $flags|AF_CASCADE_DELETE); 
    }
    
    function localKey()
    {
      if ($this->m_localKey=="") return $this->m_owner;
      return $this->m_localKey;
    }
    
    function remoteKey()
    {
      if ($this->m_remoteKey=="") return $this->m_destination;
      return $this->m_remoteKey;
    }


    /**
    * Returns a displayable string for this value.
    * @param $record
    * @return a displayable string for this value
    */
    function display($record)
    {
      $this->createDestination();
      
      for ($i=0;$i<count($record[$this->m_name]);$i++)
      {
        $this->m_destInstance->m_records[] = $record[$this->m_name][$i][$this->remoteKey()];
      }

      if (count($record[$this->m_name])!=0)
      {
        $result = "<ul>";
        $this->m_destInstance->resetRec();
        while ($this->m_destInstance->nextRec())
        {
          $result.="<li>".$this->m_destInstance->descriptor();
        }
        $result.="</ul>";
      }

      return $result;
    }

    /**
    * Dummy function
    */
    function edit($record="")
    {
    }

   /**
    *Dummy function (we don't add ourselves to the query)
    */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec, $level, $mode)
    {
      // we don't add ourselves to the query;
    }

    /**
    * load function
    * @param $notused
    * @param $record
    */
    function load($notused, $record)
    {
      $classname = $this->m_name;
      include "class.".$classname.".inc";
      $rel = new $classname();

      $this->createOwner();

      $rel->selectDb($this->m_name.".".$this->localKey()."=".$record[$this->m_ownerInstance->primaryKeyField()]);
      return $rel->m_records;
    }
    
    /** 
     * delete relational records..
     */
    function delete($record)
    {
      $classname = $this->m_name;
      include "class.".$classname.".inc";
      $rel = new $classname();
      $this->createOwner();

      $rel->deleteDb($this->localKey()."=".$record[$this->m_ownerInstance->primaryKeyField()]);
    }

  }

?>
