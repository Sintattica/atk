<?php
  /**
   * The atkRelation class defines a relation to an attribute
   * (or a relation between two attributes?)
   * (maybe a source and destination node?)
   * (and should we distinguish between 1:1, 1:N, N:M relations?)
   *
   * Note        : A relation is a (very) special kind of attribute.
   * @author Ivo Jansch (ivo@achievo.com)
   * @version 0.6
   *
   * @changes
   *
   * 0.6:   - Configurable referential keys (Ivo)
   *
   * $Id$
   * $Log$
   * Revision 4.15  2001/06/26 13:04:40  sandy
   * fixed a bug for the copy function
   *
   * Revision 4.14  2001/06/26 12:39:21  ivo
   * Adjusted link text (it contained a dot, making the link untranslatable)
   *
   * Revision 4.13  2001/06/20 14:33:51  ivo
   * ManyToOneRelation now supports templated destinationfilters.
   *
   * Revision 4.12  2001/06/14 12:25:38  ivo
   * fixed bug: onetomanyrelation ignored NF_NO_ADD flag.
   *
   * Revision 4.11  2001/05/18 09:11:06  ivo
   * Fixed a bug: the wrong column in recordlists was suppressed.
   *
   * Revision 4.10  2001/05/14 10:20:19  ivo
   * Improved error reporting when database queries fail.
   * New feature: atk can now send a detailed error report by email.
   *
   * Revision 4.9  2001/05/10 08:31:01  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 4.8  2001/05/02 14:55:46  ivo
   * New cool feature: stacktrace in the topright screen (navigate through
   * pages you visited before)
   *
   * Revision 4.7  2001/05/02 13:06:23  sandy
   * fixed the AF_CASCADE_DELETE flag
   * CV: ----------------------------------------------------------------------
   *
   * Revision 4.6  2001/05/02 12:44:21  ivo
   * multilevel session_back feature.
   * new relational features: maxRecords for oneToManyRelation, filter for
   * onetomanyrelation.
   *
   * Revision 4.5  2001/05/01 09:15:51  ivo
   * Initial session based atk version.
   *
   * Revision 4.4  2001/04/25 08:46:08  ivo
   * Big optimisations: using references for attribute manipulation.
   *
   * Revision 4.3  2001/04/25 08:44:04  sandy
   * changed the order of the edit,delete functions (first edit, then delete)
   *
   * Revision 4.2  2001/04/25 07:29:40  sandy
   * fixed a bug in the data javascript for multilanguage
   * added the stickyvars to the onetomany relations urls
   *
   * Revision 4.1  2001/04/24 13:51:50  ivo
   * Fixed some small bugs, and updated the language files, improved the menu.
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:14  ivo
   * Added atk4 to repository.
   *
   * Revision 1.16  2001/04/10 13:11:57  ivo
   * Fixes and optimisations to all relations.
   *
   * Revision 1.15  2001/04/09 10:09:50  ivo
   * Link is no longer bold.
   *
   * Revision 1.14  2001/04/05 09:33:40  ivo
   * *** empty log message ***
   *
   * Revision 1.13  2001/04/05 09:27:08  ivo
   * Changed the link text again.
   *
   * Revision 1.12  2001/04/04 10:18:05  ivo
   * Changed text of add link.
   *
   * Revision 1.11  2001/04/04 10:02:08  ivo
   * Changed the text of the add link.
   *
   * Revision 1.10  2001/04/04 09:01:49  ivo
   * AF_READONLY now also affects add forms.
   * Implemented hide() method for manytoonerelation.
   *
   * Revision 1.9  2001/04/03 09:05:50  ivo
   * Fixed bug: Wrong table was used in onetomanyrelational joins.
   *
   * Revision 1.8  2001/04/02 12:13:34  ivo
   * Fixed a bug in onetomanyrelation: stickyvars got lost.
   *
   * Revision 1.7  2001/03/27 15:33:49  ivo
   * Fixed a bug: records were not loaded after using the 'save' button.
   *
   * Revision 1.6  2001/03/21 15:02:07  ivo
   * New feature: sticky vars (variables that 'stick' to all forms and
   * urls once you've set them.
   * Stripped the m_prefix function, since embedded forms have become
   * obsolete.
   *
   * Revision 1.5  2001/02/22 22:47:16  peter
   * changed search/edit API, some clean-up of code
   *
   * Revision 1.4  2001/02/22 12:25:36  ivo
   * Fixed a lot of bugs in atkOneToManyRelation
   *
   * Revision 1.3  2001/01/22 09:04:17  martinp
   * bug in onetomany relation gefixed,
   * statisch return adres eruit gehaald voor add optie.
   *
   * Revision 1.2  2001/01/10 23:07:15  ivo
   * Adding a new record to a relation is now done through a link instead of an embedded form (martinp)
   *
   * Revision 1.1.1.1  2001/01/10 13:58:00  sandy
   * Achievo Tool Kit
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
      $this->atkRelation($name, $destination, $flags|AF_HIDE_ADD); // 1toM Relations are NEVER edited when adding a rec.
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
        $keyfield = $this->m_ownerInstance->primaryKeyField();
        $myrecords = $this->load($notused, array($keyfield=>$record[$keyfield]));
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
        $strfilter = $this->m_refKey.".".$this->m_ownerInstance->primaryKeyField()."=".$record[$this->m_ownerInstance->primaryKeyField()];
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
      
      $where = $this->m_destInstance->m_table.".".$this->m_refKey."='".$record[$this->m_ownerInstance->primaryKeyField()]."'";
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
      atkdebug("O2M DELETE voor $classname: ".$this->m_refKey."=".$record[$this->m_ownerInstance->primaryKeyField()]);

      $rel->deleteDb($rel->m_table.'.'.$this->m_refKey."=".$record[$this->m_ownerInstance->primaryKeyField()]);
    }
    

    /**
     * Store values only when it's an array and not empty
     * @param $notused ??
     * @param $record Record
     */
    function store($notused, $record)
    { 
      $onetomanyrecs = $record[$this->fieldName()];
      if (is_array($onetomanyrecs)&&count($onetomanyrecs)>0)
      {
        $this->createDestination();
        for ($i=0;$i<count($onetomanyrecs);$i++)
        {
          $this->m_destInstance->addDb($onetomanyrecs[$i]);
        }
      }
    }
  }
?>