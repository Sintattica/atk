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
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   atkimport("atk.relations.atkrelation");   

  /**
   * Many to many relation. Should not be used directly.
   *
   * This class is used as base class for special kinds of manytomany
   * relations, like the manyboolrelation. Note that most many-to-many
   * relationships can be normalized to a combination of one-to-many and
   * many-to-one relations.
   *
   * @abstract
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage relations
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
     * @param String $name The name of the relation
     * @param String $link The full name of the node that is used as
     *                     intermediairy node.
     * @param String $destination The full name of the node that is the other
     *                            end of the relation.
     * @param int $flags Flags for the relation.
     */
    function atkManyToManyRelation($name, $link, $destination, $flags=0)
    {
      $this->m_link = $link;
      $this->atkRelation($name, $destination, $flags|AF_CASCADE_DELETE);

      $this->m_localKey = substr (strrchr ($this->m_owner, '.'), 1 );
      $this->m_remoteKey = substr (strrchr ($this->m_destination, '.'), 1 );
    }

    /**
     * Create instance of the intermediairy link node.
     *
     * If succesful, the instance is stored in the m_linkInstance member
     * variable.
     * @return boolean True if successful, false if not.
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
      if ($this->createDestination())
      {
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
      return "&nbsp;";
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
      if ($this->createLink())
      {
        $rel = &$this->m_linkInstance;
        return $rel->selectDb($this->m_linkInstance->m_table.".".$this->localKey()."=".$record[$this->m_ownerInstance->primaryKeyField()]);
      }
      return array();
    }

    /**
     * delete relational records..
     */
    function delete($record)
    {
      if ($this->createLink())
      {
        $rel = &$this->m_linkInstance;
        return $rel->deleteDb($this->localKey()."=".$record[$this->m_ownerInstance->primaryKeyField()]);
      }
      return false;
    }

  }

?>
