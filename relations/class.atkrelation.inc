<?php
  /**
   * The atkRelation class defines a relation to another node
   *   
   * Note        : A relation is a (very) special kind of attribute.
   *
   * TODO: relations with a referential key that consist of more than one field don't work.
   *
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * @version 0.6
   *
   * <b>Changes</b>:
   * 
   * 0.6    - NF_NO_FILTER of destination is now set if attribute has AF_NO_FILTER (Ivo)
   *
   * $Id$
   * $Log$
   * Revision 4.12  2001/10/08 16:26:50  peter
   * Added NF_TREE_NO_ROOT_ADD flag for atkTreeNode(). This makes it possible to
   * prevent the user from adding new root elements in a tree. Also added an
   * isEmpty() method for the atkRelation() class because the atkAttribute()
   * isEmpty() does not work properly in case of relationships.
   *
   * Revision 4.11  2001/09/26 12:05:52  maurice
   * Code optimalisations for error handling
   *
   * Revision 4.10  2001/09/26 07:05:13  maurice
   * New error handling. Errors are saved in $record variable
   *
   * Revision 4.9  2001/08/27 07:38:58  ivo
   * changed validate() api: atk now passes record to the function instead of
   * just one value. Also added a mode parameter that indicates whether we
   * are in add or update mode.
   *
   * Revision 4.8  2001/07/15 16:37:19  ivo
   * New atk.inc includefile in skel.
   * New feature: extended search.
   * Fixed a bug in session management; style.php and other files that get
   * loaded between two dispatches could corrupt a session.
   *
   * Revision 4.7  2001/06/20 14:33:51  ivo
   * ManyToOneRelation now supports templated destinationfilters.
   *
   * Revision 4.6  2001/05/10 08:31:01  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 4.5  2001/05/07 15:13:49  ivo
   * Put config_atkroot in all files.
   *
   * Revision 4.4  2001/05/02 12:44:21  ivo
   * multilevel session_back feature.
   * new relational features: maxRecords for oneToManyRelation, filter for
   * onetomanyrelation.
   *
   * Revision 4.3  2001/05/01 09:49:49  ivo
   * Replaced all require() and include() calls by require_once() and
   * include_once() calls. The if(!DEFINED)... inclusion protection in files
   * is now obsolete.
   *
   * Revision 4.2  2001/04/25 08:46:08  ivo
   * Big optimisations: using references for attribute manipulation.
   *
   * Revision 4.1  2001/04/23 13:21:22  peter
   * Introduction of module support. An ATK application can now have zero
   * or more modules which can, but don't have to, contain ATK nodes.
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:14  ivo
   * Added atk4 to repository.
   *
   * Revision 1.4  2001/04/05 15:10:12  ivo
   * AAAAAAAAARG. previous fix was wrong. now it's right. I hope. Finally.
   *
   * Revision 1.3  2001/04/05 15:06:54  ivo
   * undid the last modification (caused stack overflows)
   *
   * Revision 1.2  2001/04/05 15:02:19  peter
   * Optimised relations.
   *
   * Revision 1.1.1.1  2001/01/10 13:58:00  sandy
   * Achievo Tool Kit
   *   
   */
  class atkRelation extends atkAttribute
  {

   /**
    * Destination 
    */
    var $m_destination;

   /**
    * Destination Instance 
    */
    var $m_destInstance="";
    
   /**
    * Filter
    */
   var $m_destinationFilter="";
    
   /**
    * Constructor
    * @param $name ?
    * @param $destination ?
    * @param $flags Flags for the relation
    */
    function atkRelation($name, $destination, $flags=0)
    {
      $this->atkAttribute($name, $flags);
      $this->m_destination = $destination;
            
    }

    /**
    * Create Destination
    */
    function createDestination()
    {
      if ($this->m_destInstance == "")
      {
        $this->m_destInstance = getNode($this->m_destination);
        if ($this->hasFlag(AF_NO_FILTER)) $this->m_destInstance->m_flags |= NF_NO_FILTER;
//        if ($this->m_destinationFilter!="")
  //      {
    //      $this->m_destInstance->addFilter($this->m_destinationFilter);
      //  }
      }      
    }
    
    /**
    * Return a displayable string for this value.
    * @return a displayable string for this value.
    */
    function display($record)
    {
      return $record[$this->fieldName()];
    }

    /**
    * Dummy function
    */
    function validate(&$record, $mode)
    {
    }

    /**
     * Check if the attribute is empty
     * @return true if it's empty
     */
    function isEmpty($postvars)
    {
      $this->createDestination();
      return ($postvars[$this->fieldName()][$this->m_destInstance->primaryKeyField()] == "");
    }
  }  
?>