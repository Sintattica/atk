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
   
  userelation("atkrelation");
  class atkOneToManyRelation extends atkRelation
  {
    /*** Member variables ***/

    /**
     * The referential key in the target node
     */
    var $m_refKey=array();

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
      $this->atkRelation($name, $destination, $flags|AF_HIDE_ADD|AF_HIDE_SEARCH|AF_NO_SORT); 
      // 1toM Relations are NEVER edited when adding a rec. And for now, we 
      // cannot search or sort them.
                                                                                 
      if (is_array($refKey))
      {
        $this->m_refKey = $refKey;
      }
      else
      {
        $this->m_refKey[] = $refKey;
      }
    }

    /**
     * Returns a displayable string for this value.
     * @param $record record
     * @param $mode display mode (list or view)
     * @return a displayable string
     */
    function display($record, $mode="list")
    {      

      $myrecords = $record[$this->fieldName()];
      
      if (!is_array($myrecords))
      {
        $myrecords = $this->load(NULL, $record, "admin");
      }
      
      $result = text("none"); // default result

      if ($this->createDestination())
      {

        /* record list */
        if (count($myrecords) > 0)
        {
          $result = "";
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
            $recordlist = &atknew("atk.recordlist.atkrecordlist");
            $result = $recordlist->render($this->m_destInstance, $myrecords, $actions, RL_NO_SORT|RL_NO_SEARCH, $this->m_refKey)."<br>";
          }
        }
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
      if ($this->createDestination())
      {
      
        // If someone pressed 'save', no records are loaded from the db.
        // since we don't post all records in this relation like we do with
        // other attributes, we have to load them at 'edit' time.
        // we pass 'admin' as mode, since we show a recordlist, and we need
        // all fields that we also get when we would be on an admin screen.
        $myrecords = $this->load(NULL, $record, "admin");

        $output.=$this->editHeader($record, $myrecords);
     
        if (count($myrecords)!=0)
        {     
          $actions = array();
          $params = array();
          
          if ($this->m_destinationFilter!="")
          {
            $params["atkfilter"]=$this->m_destinationFilter;
          }
                    
          $actions = $this->m_destInstance->defaultActions("relation", $params);   

          $recordlist = &atknew("atk.recordlist.atkrecordlist");          
          $flags = RL_EMBED;
          $flags |= $this->m_destInstance->hasFlag(NF_MRA) ? RL_MRA : 0;
          $flags |= $this->m_destInstance->hasFlag(NF_MRPA) ? RL_MRPA : 0;          
          $output.=$recordlist->render($this->m_destInstance, $myrecords, $actions, $flags, $this->m_refKey, "entryform");
          
        }
        if (($this->m_maxRecords==0||$this->m_maxRecords>count($myrecords))&&!$this->m_destInstance->hasFlag(NF_NO_ADD))
        {
          //$strfilter = $this->m_refKey.".".$this->m_ownerInstance->primaryKeyField()."=".$record[$this->m_ownerInstance->primaryKeyField()];
          //echo $record["atkprimkey"];
          if ($this->destinationHasRelation())
          {
            // we need to set the filter of the record we are going to add. 
            // The referential key must be set to the value of the current 
            // primary key. 
            for ($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
            {             
              $primkeyattr = &$this->m_ownerInstance->m_attribList[$this->m_ownerInstance->m_primaryKey[$i]];            
              
              $filterelems[] = $this->m_refKey[0].".".$this->m_ownerInstance->m_primaryKey[$i]."=".$primkeyattr->value2db($record);
              
            }            
          }
          else
          {
            for ($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
            {
              $filterelems[] = $this->m_refKey[$i]."=".$record[$this->m_ownerInstance->m_primaryKey[$i]];
            }
          }
          $strfilter = implode(" AND ", $filterelems);
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
          $output.= href($add_url,$label,SESSION_NESTED,true);
        }
        
        $output.=$this->editFooter($record, $myrecords);
        $output.="<br>";
        return $output;
      }
      return "";
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
      if ($mode=="edit")
      {
        // when editing a record with a onetomanyrelation, we don't
        // need to load anything, since that is done in the edit()
        // function.
        return NULL;
      }
      else
      {
        if ($this->createDestination())
        {
          if (count($this->m_refKey)==0||$this->m_refKey[0]=="") $this->m_refKey[0]=$this->m_owner;
          
          for ($i=0, $_i = count($this->m_refKey); $i<$_i; $i++)
          {
            $primkeyattr = &$this->m_ownerInstance->m_attribList[$this->m_ownerInstance->m_primaryKey[$i]];
            $whereelems[] = $this->m_destInstance->m_table.".".$this->m_refKey[$i]."='".$primkeyattr->value2db($record)."'";
          }          
          
          $where = implode(" AND ", $whereelems);

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
          $recordset = $this->m_destInstance->selectDb($where,"","",$excludes,"",$mode);

          return $recordset;
        }
      }
      return NULL;
    }


  /**
     * delete relational records..
     */
    function delete($record)
    {
      $classname = $this->m_destination;
      $rel = &getNode($classname);
      //atkdebug("O2M DELETE voor $classname: ".$this->m_refKey."='".$record[$this->m_ownerInstance->primaryKeyField()]."'");
      
      for ($i=0, $_i = count($this->m_refKey); $i<$_i; $i++)
      {
         $primkeyattr = &$this->m_ownerInstance->m_attribList[$this->m_ownerInstance->m_primaryKey[$i]]; 
         $whereelems[] = $rel->m_table.".".$this->m_refKey[$i]."='".$primkeyattr->value2db($record)."'";
      }         
      $where = implode(" AND ", $whereelems);

      if ($where!="") // double check, so we never by accident delete the entire db
      {
        return $rel->deleteDb($where);
      }
      return true;
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
      if ($mode=="copy") // when copying, mode is "copy".
      {
        $onetomanyrecs = $record[$this->fieldName()];
        if (is_array($onetomanyrecs)&&count($onetomanyrecs)>0)
        {
          if ($this->createDestination())
          {
            for ($i=0;$i<count($onetomanyrecs);$i++)
            {
              // the referential key of the onetomanyrecs could be wrong, if we
              // are called for example from a copy function. So just in case,
              // we reset the correct key.
              if(!$this->destinationHasRelation())
              {
                atkdebug("hmm.. there's no relation back. fixing the value of refKey...");
                for ($j=0, $_j = count($this->m_refKey); $j<$_j; $j++)
                {
                  $onetomanyrecs[$i][$this->m_refKey[$j]] = $record[$this->m_ownerInstance->m_primaryKey[$j]];
                }
              }  
              else
              {
                for ($j=0, $_j = count($this->m_refKey); $j<$_j; $j++)
                {
                   $onetomanyrecs[$i][$this->m_refKey[0]][$this->m_ownerInstance->m_primaryKey[$j]] = $record[$this->m_ownerInstance->m_primaryKey[$j]];
                }                              
              }        
              
              if (!$this->m_destInstance->addDb($onetomanyrecs[$i], true, $mode))
              {
                // error
                return false;
              }

            }
          }
        }
      }
      return true;
    }
    
    function hide($record="", $fieldprefix="")
    {    
      //Nothing to hide..
      return "";
    }
    
    /**
    * currently we cannot search one2many relations
    */
    function search()
    {
      return FALSE;
    }

    /**
    * Check if destination node has a relation to this node, or a 'regular'
    * atkattribute to store it's referential key.
    */
    function destinationHasRelation()
    {
      if ($this->createDestination())
      {
        // If there's a relation back, it's in the destination node under the name of the first refkey element.
        $attrib = $this->m_destInstance->m_attribList[$this->m_refKey[0]];
        if (is_object($attrib) && strpos(get_class($attrib), "elation")!==false) return true;
      }
      return false;
    }
  }
?>