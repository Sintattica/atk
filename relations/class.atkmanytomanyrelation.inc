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
      
      $recordset = array();
      
      for ($i=0;$i<count($record[$this->m_name]);$i++)
      {
        $recordset[] = $record[$this->m_name][$i][$this->remoteKey()];
      }

      if (count($record[$this->m_name])!=0)
      {
        $result = "<ul>";
        for ($i=0;$i<count($recordset);$i++)
        {
          $result.="<li>".$this->m_destInstance->descriptor($recordset[$i]);
        }
        $result.="</ul>";
      }

      return $result;
    }

    /**
    * Dummy function
    */
    function edit($record="", $fieldprefix="")
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
      $rel = getNode($classname); 
      
      return $rel->selectDb($this->m_name.".".$this->localKey()."=".$record[$this->m_ownerInstance->primaryKeyField()]);
    }
    
    /** 
     * delete relational records..
     */
    function delete($record)
    {
      $classname = $this->m_name;      
      $rel = getNode($classname);

      $rel->deleteDb($this->localKey()."=".$record[$this->m_ownerInstance->primaryKeyField()]);
    }

  }

?>
