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
      
      $this->m_destInstance->m_records = $myrecords;

      if (count($myrecords)!=0)
      {
        $result = "<ul>";
        for ($this->m_destInstance->m_currentRec = 0; $this->m_destInstance->m_currentRec < count($this->m_destInstance->m_records);$this->m_destInstance->m_currentRec++)
        {
          $result.="<li>".$this->m_destInstance->descriptor();
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
    function edit($record="")
    {
      $GLOBALS['g_layout']->debug("edit called for ".$this->fieldName());

      $this->createDestination();
      $this->createOwner();

      $output =$GLOBALS['g_layout']->ret_table_simple(0,true);
      
      $myrecords = $record[$this->fieldName()];

      $this->m_ownerInstance->m_records[0] = $record;
      
      if (count($myrecords)==0)
      {
        // If someone pressed 'save', no records are loaded from the db.
        // since we don't post all records in this relation like we do with
        // other attributes, we have to load them at 'edit' time.
        $keyfield = $this->m_ownerInstance->primaryKeyField();
        $myrecords = $this->load($notused, Array($keyfield=>$record[$keyfield]));
      }
      
      if (count($myrecords)!=0)
      {
        // I *HATE* the fact that PHP does not support pointers.
        // How am I supposed to know that we were originally in edit mode of the
        // owner when this edit function is called? I just assume this now.
        // I could know it if I would pass like *everything* to this
        // attribute. But a simple pointer to the owner object would be enough.
        //
        // Sometimes, PHP sucks.
        // This is such a time.
        //
        // So the next line will only make sense if nobody does anything unexpected.
        //
        // So I also need the owner instance (which would also be a pointer if php supported it)
        $output.="<tr>";
        $cell="";        
        
        $returnurl=rawurlencode('dispatch.php?atknodetype='.$this->m_owner.'&atkaction=edit&atkselector='.rawurlencode($this->m_ownerInstance->primaryKey()));
        
        $actions = array();
        if (!$this->m_destInstance->hasFlag(NF_NO_DELETE))
        {
          $actions[] = url('dispatch.php?atknodetype='.$this->m_destination.'&atkaction=delete&atkselector=[pk]&atkreturnurl='.$returnurl,text("delete"));
        }
        if (!$this->m_destInstance->hasFlag(NF_NO_EDIT))
        {
          $actions[] = url('dispatch.php?atknodetype='.$this->m_destination.'&atkaction=edit&atkselector=[pk]&atkreturnurl='.$returnurl,text("edit"));
        }        
         
        $this->m_destInstance->m_records = $myrecords;
        
        // User may not search in embedded forms..        
        $this->m_destInstance->m_flags |= NF_NO_SEARCH;    
        
        $cell.=$this->m_destInstance->recordList($actions,false,Array($this->m_owner));
      
        $output.=$GLOBALS['g_layout']->ret_td($cell);
        $output.="</tr>";
      }

      //$returnurl = rawurlencode('dispatch.php?atknodetype='.$this->m_owner.'&atkaction=edit&atkselector='.$this->m_ownerInstance->primaryKey());
      $returnurl = rawurlencode($this->m_ownerInstance->stickyUrl('dispatch.php?atkaction=edit&atkselector='.rawurlencode($this->m_ownerInstance->primaryKey())));
      $add_url = 'dispatch.php?atknodetype='.$this->m_destination.'&atkaction=add&atkreturnurl='.$returnurl."&atkfilter=".rawurlencode($this->m_refKey.".".$this->m_ownerInstance->primaryKeyField()."=".$record[$this->m_ownerInstance->primaryKeyField()]);
      $output.="<tr><td><a href='".$add_url."'>".text("link_".$this->m_destination."_add")."</a></td></tr>";      
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
      $GLOBALS['g_layout']->debug("load called for ".$this->m_name);
      $this->createDestination();

      $this->createOwner();
      if ($this->m_refKey=="") $this->m_refKey=$this->m_owner;
      
      $where = $this->m_destInstance->m_table.".".$this->m_refKey."='".$record[$this->m_ownerInstance->primaryKeyField()]."'";      
      $this->m_destInstance->selectDb($where,"","",$this->m_destInstance->m_listExcludes);
      //var_dump($this->m_destInstance->m_records);
      return $this->m_destInstance->m_records;
    }

    /**
     * Store values
     * @param $notused ??
     * @param $record Record
     */
    function store($notused, $record)
    {
      if (is_array($record[$this->fieldName()]))
      {
        $this->createDestination();

        $this->m_destInstance->m_records = Array($record[$this->fieldName()]);
        $this->m_destInstance->addDb();
      }
    }
  }
?>
