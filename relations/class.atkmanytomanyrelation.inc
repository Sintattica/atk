<?php
  /**
   * Many to many relation. Should not be used, as manytomany relations
   * should be normalised.
   *
   * This class is used as base class for special kinds of manytomany
   * relations, like the manyboolrelation.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
   *
   */

  class atkManyToManyRelation extends atkRelation
  {
    var $m_localKey = "";
    var $m_remoteKey = "";
    var $m_link = "";
    var $m_linkInstance = NULL;

    /**
    * Constructor
    * @param $name ?
    * @param $destination ?
    * @param $flags Flags for the relation
    */
    function atkManyToManyRelation($name, $link, $destination, $flags=0)
    {
      $this->m_link = $link;
      $this->atkRelation($name, $destination, $flags|AF_CASCADE_DELETE);
    }

    /**
    * Create Link
    */
    function createLink()
    {
      if ($this->m_linkInstance == NULL)
      {
        $this->m_linkInstance = &newNode($this->m_link);

        // Validate if destination was created succesfully
        if (!is_object($this->m_linkInstance))
      	{
	        atkerror("Relation with unknown nodetype '".$this->m_link."' (in node '".$this->m_owner."')");
          $this->m_linkInstance = NULL;
          return false;
        }
      }

      return true;
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
      $this->createLink();
      $rel = &$this->m_linkInstance;

      return $rel->selectDb($this->m_name.".".$this->localKey()."=".$record[$this->m_ownerInstance->primaryKeyField()]);
    }

    /**
     * delete relational records..
     */
    function delete($record)
    {
      $classname = $this->m_name;
      $rel = &getNode($classname);

      $rel->deleteDb($this->localKey()."=".$record[$this->m_ownerInstance->primaryKeyField()]);
    }

  }

?>
