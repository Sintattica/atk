<?php
  /**
   * Can be used to create on to many relations ('1 library has N books').
   * A common term for this type of relation is a master-detail relationship.
   * The detailrecords can be edited inline.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
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
      $this->atkRelation($name, $destination, $flags|AF_HIDE_ADD|AF_HIDE_SEARCH); // 1toM Relations are NEVER edited when adding a rec.
                                                                                 // and for now, we cannot search them.
      $this->m_refKey = $refKey;
    }

    /**
     * Returns a displayable string for this value.
     * @param $record record
     * @param $mode display mode (list or view)
     * @return a displayable string
     */
    function display($record, $mode="list")
    {
      global $g_layout;

      $myrecords = $record[$this->fieldName()];
      $this->createDestination();

      /* record list */
      if (count($myrecords) > 0)
      {
        /* display in list mode */
        if ($mode == "list")
          for ($i = 0; $i < count($myrecords);$i++)
            $result .= ($i > 0 ? "<br>" : "").$this->m_destInstance->descriptor($myrecords[$i]);

        /* display in view mode */
        else
        {
          if (!empty($this->m_destinationFilter)) $filter = rawurlencode($this->m_destinationFilter);
          if (!$this->m_destInstance->hasFlag(NF_NO_VIEW))
            $actions = array("view" => dispatch_url($this->m_destination, "view", array("atkselector" => "[pk]", "atkfilter" => $filter)));
          else $actions = array();
          $result = $this->m_destInstance->normalRecordList($myrecords, $actions, RL_NO_SORT|RL_NO_SEARCH, array($this->m_refKey))."<br>";
        }
      }

      /* no records */
      else $result = text("none");

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
      global $PHP_SELF;

      $this->createDestination();
      
      // If someone pressed 'save', no records are loaded from the db.
      // since we don't post all records in this relation like we do with
      // other attributes, we have to load them at 'edit' time.
      // we pass 'admin' as mode, since we show a recordlist, and we need
      // all fields that we also get when we would be on an admin screen.
      $myrecords = $this->load(NULL, $record, "admin");

      $output.=$this->editHeader($record, $myrecords);
      $output =$GLOBALS['g_layout']->ret_table_simple(0,true);

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
        $actions["edit"] = $PHP_SELF.'?atknodetype='.$this->m_destination.'&atkaction=edit&atkselector=[pk]'.$strfilter;
        }

        if (!$this->m_destInstance->hasFlag(NF_NO_DELETE))
        {
          $actions["delete"] = $PHP_SELF.'?atknodetype='.$this->m_destination.'&atkaction=delete&atkselector=[pk]'.$strfilter;
        }
        if($this->m_destInstance->hasFlag(NF_COPY))
        {
          $actions["copy"] = $PHP_SELF.'?atknodetype='.$this->m_destination.'&atkaction=copy&atkselector=[pk]';
        }

        $flags = RL_EMBED;
        $flags |= $this->m_destInstance->hasFlag(NF_MRA) ? RL_MRA : 0;
        $flags |= $this->m_destInstance->hasFlag(NF_MRPA) ? RL_MRPA : 0;
        $cell.=$this->m_destInstance->normalRecordList($myrecords, $actions, $flags, array($this->m_refKey), "entryform");

        $output.=$GLOBALS['g_layout']->ret_td($cell);
        $output.="</tr>";
      }

      if (($this->m_maxRecords==0||$this->m_maxRecords>count($myrecords))&&!$this->m_destInstance->hasFlag(NF_NO_ADD))
      {
        //$strfilter = $this->m_refKey.".".$this->m_ownerInstance->primaryKeyField()."=".$record[$this->m_ownerInstance->primaryKeyField()];
      //echo $record["atkprimkey"];
        // we need to set the filter of the record we are going to add. The referential key must
        // be set to the value of the current primary key. However, the current primary key
        // contains the table name and a dot. So we strip that.
        list($tmp,$refkeyvalue) = explode(".",$record["atkprimkey"]);
        $strfilter = $this->m_refKey.".".$refkeyvalue;

        if ($this->m_destinationFilter!="")
        {
          $strfilter.=' AND '.$this->m_destinationFilter;
        }
        $add_url = 'dispatch.php?atknodetype='.$this->m_destination.'&atkaction=add&atkfilter='.rawurlencode($strfilter);
        $key = "link_".$this->fieldName()."_add";
        $label = text($key);
        if ($label==""||strtolower($label)==str_replace('_',' ',strtolower($key)))
        {
          $key = "link_".getNodeType($this->m_destination)."_add";
          $label = text($key);
          if ($label==""||strtolower($label)==str_replace('_',' ',strtolower($key)))
          {
            $key = text(getNodeType($this->m_destination))." ".strtolower(text("add"));
            $label = text($key);
          }
        }
        $output.="<tr><td>".href($add_url,$label,SESSION_NESTED,true)."</td></tr>";
      }
      $output.="</table>";
      $output.=$this->editFooter($record, $myrecords);
      $output.="<br>";
      return $output;
    }
    
    /**
          * This function can be implemented in derived classes to add extra information
          * (text, links, whatever) to the top of the attribute, right before the recordlist
          *
          * @param $record The master record that is being edited.
          * @param $childrecords The childrecords in this master/detail relationship.
          */
    function editHeader($record=NULL, $childrecords=NULL)
    {
      return "";
    }
    
    /**
          * This function can be implemented in derived classes to add extra information
          * (text, links, whatever) to the bottom of the attribute, right after the add link.
          *
          * @param $record The master record that is being edited.
          * @param $childrecords The childrecords in this master/detail relationship.
          */
    function editFooter($record=NULL, $childrecords=NULL)
    {
      return "";
    }

    /**
     * load values
     * @param $db pointer to the database
     * @param $record Record
     * @param $mode Mode for loading (admin, select, copy, etc)

     * @return ??
     */
    function load($db, $record, $mode="")
    {
      atkdebug("load called for ".$this->m_name." (mode $mode)");

      if ($mode=="edit")
      {
        // when editing a record with a onetomanyrelation, we don't
        // need to load anything, since that is done in the edit()
        // function.
        return NULL;
      }
      else
      {
        $this->createDestination();
        if ($this->m_refKey=="") $this->m_refKey=$this->m_owner;

        $key = decodeKeyValueSet($record["atkprimkey"]);
        // we assume singular referential keys, which means, there's only one value in $keyvalue;
        list($tmp,$keyvalue) = each($key);

        $where = $this->m_destInstance->m_table.".".$this->m_refKey."='".$keyvalue."'";

        if ($this->m_destinationFilter!="")
        {
          $this->m_destInstance->addFilter($this->m_destinationFilter);
        }

        if ($mode=="admin" || $mode=="select" || $mode=="view")
        {
          // we only need fields that are displayed in the descriptor.
          $excludes = $this->m_destInstance->m_listExcludes;
        }
        else
        {
          // we probably need everything
          $excludes = "";
        }
        $recordset = $this->m_destInstance->selectDb($where,"","",$excludes);

        return $recordset;
      }
    }


  /**
     * delete relational records..
     */
    function delete($record)
    {
      $classname = $this->m_destination;
      $rel = &getNode($classname);
      atkdebug("O2M DELETE voor $classname: ".$this->m_refKey."='".$record[$this->m_ownerInstance->primaryKeyField()]."'");

      $rel->deleteDb($rel->m_table.'.'.$this->m_refKey."='".$record[$this->m_ownerInstance->primaryKeyField()]."'");
    }


    /**
     * Store values only when it's an array and not empty
     * For onetomanyrelation, this function does not have much use, since it
     * stores records using it's 'add link'. But the copyDb function however
     * might use this.
     * To prevent double storage when clicking the "save" button on a form
     * we only store something when in add mode. In 'normal' add forms, we
     * don't have a oneToManyRelation, so this add is only called when
     * doing 'abnormal' adds, like a copy function.
     *
     * @param $notused ??
     * @param $record Record
     */
    function store($notused, $record, $mode)
    {
      if ($mode=="add") // when copying, mode is "add".
      {
        $onetomanyrecs = $record[$this->fieldName()];
        if (is_array($onetomanyrecs)&&count($onetomanyrecs)>0)
        {
          $this->createDestination();
          for ($i=0;$i<count($onetomanyrecs);$i++)
          {
            // the referential key of the onetomanyrecs could be wrong, if we
            // are called for example from a copy function. So just in case,
            // we reset the correct key.
            $onetomanyrecs[$i][$this->m_refKey][$this->m_ownerInstance->primaryKeyField()] = $record[$this->m_ownerInstance->primaryKeyField()];

            $this->m_destInstance->addDb($onetomanyrecs[$i]);
          }
        }
      }
    }
  }
?>
