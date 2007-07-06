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

  /**
   * @internal Include base class.
   */
  userelation("atkrelation");

  /**
   * Only allow deletion of master item when there are no child records
   */
  define("AF_RESTRICTED_DELETE", AF_SPECIFIC_1);

  /**
   * Use pop-up dialogs for adding records
   */
  define("AF_ONETOMANY_ADD_DIALOG", AF_SPECIFIC_5);

  /**
   * Use embedded forms for adding records
   * @deprecated Replaced by AF_ONETOMANY_ADD_DIALOG.
   */
  define("AF_ONETOMANY_EMBED", AF_SPECIFIC_2);

  /**
   * Use pop-up dialog for whatever a new record must be copied or must be added.
   */
  define("AF_ONETOMANY_ADDORCOPY_DIALOG", AF_SPECIFIC_3);

  /**
   * Implementation of one-to-many relationships.
   *
   * Can be used to create one to many relations ('1 library has N books').
   * A common term for this type of relation is a master-detail relationship.
   * The detailrecords can be edited inline.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage relations
   *
   */
  class atkOneToManyRelation extends atkRelation
  {
    var $m_recordlist;

    /**
     * Instance of atk.recordlist.atkrecordlistcache
     * @access private
     * @var Object
     */
    var $m_recordlistcache;

    /**
     * List of keys from the destination node that refer to the master record.
     * @access private
     * @var array
     */
    var $m_refKey=array();

    /**
     * The maximum number of detail records. If the number of detail records
     * exceeds this maximum, the link for adding new details disappears.
     * @access private
     * @var int
     */
    var $m_maxRecords = 0;


    /**
     * The load method might build a custom filter. When it does, we might want
     * to use it again in other methods.
     * @access private
     * @var string
     */
    var $m_loadFilter = "";

    /**
     * The field that the foreign key in the destination points to.
     * Is set to the primary key if no value is provided.
     * @access private
     * @var array;
     */
    var $m_ownerFields = array();

    /**
     * Default constructor.
     *
     * <b>Example: </b> Suppose a department has many employees. To edit the
     * list of employees in a department, this relationship can be built like
     * this, in the department node:
     * <code>
     * $this->add(new atkOneToManyRelation("employees", "mymod.employee", "department_id"));
     * </code>
     *
     * @param String $name The unique name of this relation within a node.
     *                     In contrast with moth other attributes, the name
     *                     does not correspond to a database field. (Because
     *                     in one2many relations, the databasefield that
     *                     stores the link, is in the destination node and not
     *                     in the owner node).
     * @param String $destination The node to which the relationship is made
     *                            (in module.nodename notation).
     * @param mixed $refKey For regular oneToMany relationships, $refKey is
     *                      name of the referential key in the destination
     *                      node. In the case of multi-foreign key
     *                      relationships, $refKey can be an array of fields.
     * @param int $flags Attribute flags that influence this attributes'
     *                   behavior.
     */
    function atkOneToManyRelation($name, $destination, $refKey="", $flags=0)
    {
      $page = &atkinstance('atk.ui.atkpage');
      $page->register_script(atkconfig("atkroot")."atk/javascript/tools.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkonetomanyrelation.js");

      $this->atkRelation($name, $destination, $flags|AF_HIDE_ADD|AF_NO_SORT);
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

    function setOwnerFields($ownerfields)
    {
      $this->m_ownerFields = $ownerfields;
    }

    function getOwnerFields()
    {
      if (is_array($this->m_ownerFields) && count($this->m_ownerFields)>0)
      {
        return $this->m_ownerFields;
      }
      return $this->m_ownerInstance->m_primaryKey;
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * The atkOneToManyRelation displays a list of detail records in "view"
     * mode, in the form of a readonly recordlist. In "list" mode, a plain
     * list of detail record descriptors is displayed.
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list"
     *                     for displaying in recordlists)
     * @return String HTML String
     */
    function display($record, $mode="list")
    {
      $myrecords = $record[$this->fieldName()];

      if (!is_array($myrecords))
      {
        $myrecords = $this->load(atkGetDb(), $record, "admin",true);
      }

      $result = atktext("none"); // default result

      if ($this->createDestination())
      {
        // record list
        if (count($myrecords) > 0)
        {
          // display in list mode
          if ($mode == "list")
          {
            $result = "<ul>";
            for ($i = 0; $i < count($myrecords);$i++)
              $result .= sprintf("<li>%s</li>", $this->m_destInstance->descriptor($myrecords[$i]));
            $result .= "</ul>";
          }

          // display in csv/plain mode
          elseif (in_array($mode, array("csv", "plain")))
          {
            $result = "";
            for ($i = 0; $i < count($myrecords);$i++)
              $result .= ($i > 0 ? ', ' : '').$this->m_destInstance->descriptor($myrecords[$i]);
          }

          // display in view mode
          else
          {
            if (!empty($this->m_destinationFilter)) $filter =$this->m_destinationFilter;
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
     *
     * The atkOneToManyRelation's edit method returns a recordlist in which
     * detail records can be removed, added and edited.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record="", $fieldprefix="")
    {
      if ($this && $this->createDestination())
      {
        if ($this->m_destInstance->hasFlag(NF_CACHE_RECORDLIST))
        {
          $this->_setRecordlistCache();
          $cache = $this->m_recordlistcache->getCache();
          if ($cache)
          {
            atkdebug("Using recordlist cache: {$this->m_recordlistcache->m_cacheid}");
            return $cache;
          }
        }

        $output = $this->_getEditOutput($record,$fieldprefix);
        if ($this->m_recordlistcache) $this->m_recordlistcache->writeCache($output, $this->m_recordlist->m_actionloader);
        return $output;
      }

      return "";
    }

    function _getEditOutput($record="", $fieldprefix="")
    {
      $myrecords = $this->_getRecords($record,true);

      $output=$this->editHeader($record, $myrecords);

      $output.= $this->_getRecordList($myrecords);

      $output.=$this->editFooter($record, $myrecords);
      $this->_fetchNavigationLinks($output);

      if($this->m_destInstance->allowed("add"))
      {
        $this->_addAddToEditOutput($output, $myrecords, $record);
      }
      return $output;
    }

    /**
     * Adds the 'add' option to the onetomany, either integrated or as a link
     *
     * @param String $output   The HTML output of the edit function
     * @param Array $myrecords The records that are loaded into the recordlist
     * @param array $record The master record that is being edited.
     */
    function _addAddToEditOutput(&$output, $myrecords, $record)
    {
      if (atkconfig("onetomany_addlink_position","bottom") == "top")
      {
        $output = $this->_getAddLink($myrecords, $record)."<br />".$output;
      }
      else if (atkconfig("onetomany_addlink_position","bottom") == "bottom")
      {
        $output .= $this->_getAddLink($myrecords, $record)."<br />";
      }
    }

    /**
     * Get the buttons for the embedded mode of the onetomany relation.
     * @todo Move this to a template
     * @return String The HTML buttons
     */
    function _getEmbeddedButtons()
    {
      $fname = $this->fieldName();
      $output.='<input type="submit" class="otm_add" name="'.$fname.'_save" value="'.atktext("add").'">';
      return $output.'<input type="button" onClick="toggleAddForm(\''.$fname."_integrated',
                                                               '".$fname."_integrated_link');\"
                                       class=\"otm_add\" name=\"".$fname."_cancel\" value=\"".atktext("cancel").'">';
    }

    /**
     * If someone pressed 'save', no records are loaded from the db.
     * Since we don't post all records in this relation like we do with
     * other attributes, we have to load them at 'edit' time.
     * We pass 'admin' as mode, since we show a recordlist, and we need
     * all fields that we also get when we would be on an admin screen.
     * Also, because the load looks to the owner for it's orderby but we have special orderbys
     * for each onetomany, we fake it.
     *
     * @param array $record the current master record
     & @param bool $paging divide the result on multiple pages (based on $config_recordsperpage)
     * @return array records from database
     */
    function _getRecords($record,$paging=false)
    {
      $myrecords = $this->load(atkGetDb(), $record, "admin", $paging);
      return $myrecords;
    }

    /**
     * Gets the recordlist.
     * @param Array $myrecords The records that are loaded into the recordlist
     * @return String The generated recordlist
     */
    function _getRecordList($myrecords)
    {
      if (count($myrecords)!=0 || isset($this->m_ownerInstance->m_postvars['atksearch']))
      {
        $actions = array();
        $params = array();

        $embedprefix = "{$this->m_name}";

        if ($this->m_destinationFilter!="")
        {
          $params["atkfilter"]=$this->m_destinationFilter;
        }

        $actions = $this->m_destInstance->defaultActions("relation", $params);

        $recordlist = &atknew("atk.recordlist.atkrecordlist");

        $flags = $this->_getRecordListFlags();

        if (isset($this->m_ownerInstance->m_postvars['atkorderby'.$this->fieldName()]))
          $this->m_destInstance->m_postvars['atkorderby'] =     $this->m_ownerInstance->m_postvars['atkorderby'.$this->fieldName()];

        if (isset($this->m_ownerInstance->m_postvars['atksearch'][$this->fieldName()]))
          $this->m_destInstance->m_postvars['atksearch'] =      $this->m_ownerInstance->m_postvars['atksearch'][$this->fieldName()];

        if (isset($this->m_ownerInstance->m_postvars['atksearchmode']))
          $this->m_destInstance->m_postvars['atksearchmode'] =  $this->m_ownerInstance->m_postvars['atksearchmode'];


        $output = $recordlist->render($this->m_destInstance, $myrecords,
                                    $actions,              $flags,
                                    $this->m_refKey,       "entryform",
                                    array(),               $embedprefix);
        $this->m_recordlist = $recordlist;
        return $output;
      }
      return "";
    }

    /**
     * Determine the flags that the embedded recordlist should have.
     * @return int The flags for the recordlist.
     */
    function _getRecordListFlags()
    {
      // for now, we don't support extended search in embedded one2many recordlists.
      atkimport("atk.recordlist.atkrecordlist");
      $flags  = atkRecordList::convertFlags($this->m_destInstance->getFlags());
      $flags |= RL_EMBED|RL_NO_EXTENDED_SEARCH|RL_NO_SORT|RL_NO_SEARCH;
      return $flags;
    }

    /**
     * Internal function to get the add link for a atkOneToManyRelation
     * @param Array $myrecords  The load of all attributes (see comment in edit() code)
     * @param Array $record     The record that holds the value for this attribute.
     * @return String  The link to add records to the onetomany
     */
    function _getAddLink($myrecords, $record,$saveform=true)
    {
      if ($this->hasFlag(AF_ONETOMANY_ADD_DIALOG) ||
          $this->hasFlag(AF_ONETOMANY_ADDORCOPY_DIALOG) ||
          $this->m_destInstance->hasFlag(NF_ADD_DIALOG) ||
          $this->m_destInstance->hasFlag(NF_ADDORCOPY_DIALOG))
      {
        return $this->_getDialogAddLink($record);
      }
      else
      {
        return $this->_getNestedAddLink($myrecords, $record, $saveform);
      }
    }

    /**
     * Uses the given record to create an add filter string.
     *
     * @param array $record
     * @return string filter string
     */
    function getAddFilterString($record)
    {
      $filterelems = $this->_getFilterElements($record);
      $strfilter = implode(" AND ", $filterelems);
      if ($this->m_destinationFilter!="")
      {
        $strfilter.=' AND '.$this->parseFilter($this->m_destinationFilter,$record);
      }

      return $strfilter;
    }

    function _getDialogAddLink($record)
    {
      atkimport("atk.ui.atkdialog");

      $filter = $this->getAddFilterString($record);

      $action = $this->hasFlag(AF_ONETOMANY_ADDORCOPY_DIALOG) || $this->m_destInstance->hasFlag(NF_ADDORCOPY_DIALOG) ? 'addorcopy' : 'add';

      $ui = &$this->m_ownerInstance->getUi();

      $dialog = new atkDialog($this->m_ownerInstance->atkNodeType(), 'edit', 'attribute.'.$this->fieldName().'.'.$action.'_dialog', array('atkfilter' => $filter));
      $title = $ui->title($this->m_destInstance->m_module, $this->m_destInstance->m_type, $action);
      $dialog->setTitle($title);
      $onClick = $dialog->getCall();

      return '<a href="javascript:void(0)" onclick="'.$onClick.'" class="valignMiddle">'.$this->getAddLabel().'</a>';
    }

    /**
     * Internal function to get the add link for a atkOneToManyRelation.
     *
     * @param Array $myrecords  The load of all attributes (see comment in edit() code)
     * @param Array $record     The record that holds the value for this attribute.
     * @return String  The link to add records to the onetomany
     */
    function _getNestedAddLink($myrecords, $record,$saveform=true)
    {
      $url = "";
      if (($this->m_maxRecords==0||$this->m_maxRecords>count($myrecords))&&$this->createDestination() && !$this->m_destInstance->hasFlag(NF_NO_ADD))
      {
        $strfilter = $this->getAddFilterString($record);
        $add_url = dispatch_url($this->m_destination,"add",array("atkfilter"=>$strfilter));
        $label = $this->getAddLabel();

        if (count($this->m_onchangecode))
        {
          $onchange = 'onChange="'.$this->fieldName().'_onChange(this);"';
          $this->_renderChangeHandler($fieldprefix);
        }
        else
        {
          $onchange = '';
        }
        $url = href($add_url,$label,SESSION_NESTED,$saveform,$onchange.' class="atkonetomanyrelation"');
      }
      return $url;
    }

    function _getFilterElements($record)
    {
      $ownerfields = $this->getOwnerFields();
      if ($this->destinationHasRelation())
      {
        // we need to set the filter of the record we are going to add.
        // The referential key must be set to the value of the current
        // primary key.
        for ($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
        {
          $primkeyattr = &$this->m_ownerInstance->m_attribList[$ownerfields[$i]];
          $filterelems[] = $this->m_refKey[0].".".$ownerfields[$i]."=".$primkeyattr->value2db($record);
        }
      }
      else
      {
        for ($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
        {
          $filterelems[] = $this->m_refKey[$i]."=".$record[$ownerfields[$i]];
        }
      }
      return $filterelems;
    }

    /**
     * Attempts to get a translated label which can be used when composing an "add" link
     *
     * @return String Localised "add" label
     */
    function getAddLabel()
    {
      $key = "link_".$this->fieldName()."_add";
      $label = atktext($key,$this->m_ownerInstance->m_module, $this->m_ownerInstance->m_type, "", "", true);
      if ($label=="")
      {
        $key = "link_".$this->fieldName()."_add";
        $label = atktext($key,$this->m_destInstance->m_module, "", "", "", true);
        if ($label=="")
        {
          $key = "link_".getNodeType($this->m_destination)."_add";
          $label = atktext($key,$this->m_destInstance->m_module, "", "", "", true);
          if ($label=="")
          {
            $label = atktext(getNodeType($this->m_destination),$this->m_destInstance->m_module)." ".strtolower(atktext("add", "atk"));
          }
        }
      }
      return $label;
    }

    /**
     * Retrieve header for the recordlist.
     *
     * The regular atkOneToManyRelation has no implementation for this method,
     * but it may be overridden in derived classes to add extra information
     * (text, links, whatever) to the top of the attribute, right before the
     * recordlist. This is similar to the adminHeader() method in atkNode.
     *
     * @param array $record The master record that is being edited.
     * @param array $childrecords The childrecords in this master/detail
     *                            relationship.
     * @return String a String to be added to the header of the recordlist.
     */
    function editHeader($record=NULL, $childrecords=NULL)
    {
      return "";
    }

    /**
     * Retrieve footer for the recordlist.
     *
     * The regular atkOneToManyRelation has no implementation for this method,
     * but it may be overridden in derived classes to add extra information
     * (text, links, whatever) to the bottom of the attribute, just after the
     * recordlist. This is similar to the adminFooter() method in atkNode.
     *
     * @param array $record The master record that is being edited.
     * @param array $childrecords The childrecords in this master/detail
     *                            relationship.
     * @return String a String to be added at the bottom of the recordlist.
     */
    function editFooter($record=NULL, $childrecords=NULL)
    {
      return "";
    }

    /**
     * Create the where clause for the referential key that is used to
     * retrieve the destination records.
     * @access private
     *
     * @param array $record The master record
     * @return String SQL where clause
     */
    function _getLoadWhereClause($record)
    {
      $whereelems = array();

      if (count($this->m_refKey)==0||$this->m_refKey[0]=="") $this->m_refKey[0]=$this->m_owner;
      $ownerfields = $this->getOwnerFields();

      for ($i=0, $_i = count($this->m_refKey); $i<$_i; $i++)
      {
        $primkeyattr = &$this->m_ownerInstance->m_attribList[$ownerfields[$i]];

        if (!$primkeyattr->isEmpty($record))
        {
          $whereelems[] = $this->m_destInstance->m_table.".".$this->m_refKey[$i]."='".$primkeyattr->value2db($record)."'";
        }
      }

      return implode(" AND ", $whereelems);
    }

    /**
     * Retrieve detail records from the database.
     *
     * Called by the framework to load the detail records.
     *
     * @param atkDb $db The database used by the node.
     * @param array $record The master record
     * @param String $mode The mode for loading (admin, select, copy, etc)
     * @param bool $paging divide the result records on multiple pages ($config_recordsperpage)
     *
     * @return array Recordset containing detailrecords, or NULL if no detail
     *               records are present. Note: when $mode is edit, this
     *               method will always return NULL. This is a framework
     *               optimization because in edit pages, the records are
     *               loaded on the fly.
     */
    function load(&$db, $record, $mode="", $paging=false)
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
          $where = $this->_getLoadWhereClause($record);

          if ($where!="")
          {
            if ($this->m_destinationFilter!="")
            {
              $this->m_destInstance->addFilter($this->parseFilter($this->m_destinationFilter,$record));
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
            if (isset($this->m_ownerInstance->m_postvars['atksearch'][$this->fieldName()]))
            {
              $this->m_destInstance->m_postvars['atksearch'] = $this->m_ownerInstance->m_postvars['atksearch'][$this->fieldName()];
            }
            if (isset($this->m_ownerInstance->m_postvars['atksearchmode']))
            {
              $this->m_destInstance->m_postvars['atksearchmode'] = $this->m_ownerInstance->m_postvars['atksearchmode'];
            }
            ($paging==true)?$limitarray=$this->fetchLimitArray():$limitarray="";

            $stmt = "";
            if(isset($this->m_ownerInstance->m_postvars['atkorderby'.$this->fieldName()]))
            {
              $atkorderby = $this->m_ownerInstance->m_postvars['atkorderby'.$this->fieldName()];
              // EVIL BAD SMELL, sorting and recordlist embedding is handled in a very cumbersome way (with prefixes and embedprefixes
              // that are hardly understandable
              $elems = explode(" ", $atkorderby);
              if (isset($this->m_destInstance->m_attribList[$elems[0]]))
              {
                $attr = &$this->m_destInstance->getAttribute($elems[0]);
                $stmt = $attr->getOrderByStatement();

                if (count($elems)>1)
                {
                  // probably asc or desc
                  $stmt.=" ".$elems[1];
                }
              }
              else
              {
                // not an attribute, probably custom order by
                $stmt = $atkorderby;
              }
            }

            $recordset = $this->m_destInstance->selectDb($where,$stmt,$limitarray,$excludes,"",$mode);
            $this->m_loadFilter = $where;
            return $recordset;
          }
        }
      }
      return NULL;
    }

    /**
     * Override isEmpty function - in a oneToMany relation we should check if the
     * relation contains any records. When there aren't any, the relation is empty,
     * otherwise it isn't
     *
     * @param  array  &$record The record to check
     * @return bool true if a destination record is present. False if not.
     */
    function isEmpty($record)
    {
      if (!isset($record[$this->fieldName()])
              || (is_array($record[$this->fieldName()]) && count($record[$this->fieldName()])==0))
      {
        // empty. It might be that the record has not yet been fetched. In this case, we do
        // a forced load to see if it's really empty.
        $recs = $this->load($this->m_ownerInstance->getDb(), $record);
        return (count($recs)==0);
      }
      return false;
    }

    /**
     * Returns an array to pass as limit parameter to the selectdb function
     *
     * @return array an array to pass to the selectdb function
     */
    function fetchLimitArray()
    {
      global $ATK_VARS;

      // limitVarPostfix was introduced to make a difference when
      // there is more than one item with pagenumbers on the page
      $postfix = $this->fieldName();

      $retArray = array();

      if (!isset($ATK_VARS["atkstartat" . $postfix]))
      {
        $retArray["offset"] = 0;
      }
      else
      {
        $retArray["offset"] = $ATK_VARS["atkstartat" . $postfix];
      }

      // If we didn't pass along our own limit (atklimit var with postfix)
      // we'll use the recordsperpage config value and if THAT fails,
      // we use 25
      if (!isset($ATK_VARS["atklimit" . $postfix]))
      {
        $retArray["limit"] = atkconfig("recordsperpage", 25);
      }
      else
      {
        $retArray["limit"] = $ATK_VARS["atklimit" . $postfix];
      }
      return $retArray;
    }

    /**
     * The delete method is called by the framework to inform the attribute
     * that the master record is deleted.
     *
     * Note that the framework only calls the method when the
     * AF_CASCADE_DELETE flag is set. When calling this method, all detail
     * records belonging to the master record are deleted.
     *
     * @param array $record The record that is deleted.
     * @return boolean true if cleanup was successful, false otherwise.
     */
    function delete($record)
    {
      $classname = $this->m_destination;
      $cache_id = $this->m_owner.".".$this->m_name;
      $rel = &getNode($classname,$cache_id);
      $ownerfields = $this->getOwnerFields();

      for ($i=0, $_i = count($this->m_refKey); $i<$_i; $i++)
      {
         $primkeyattr = &$this->m_ownerInstance->m_attribList[$ownerfields[$i]];
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
     * Store detail records in the database.
     *
     * For onetomanyrelation, this function does not have much use, since it
     * stores records using its 'add link'. Thehe copyDb function however uses
     * this method to clone detail records.
     * If mode is anything other than "copy" this method does not do anything.
     *
     * @param atkDb $db The database used by the node.
     * @param array $record The master record which has the detail records
     *                      embedded.
     * @return boolean true if store was successful, false otherwise.
     */
    function store(&$db, $record, $mode)
    {
      if ($mode=="copy") // when copying, mode is "copy".
      {
        $onetomanyrecs = $record[$this->fieldName()];
        if (is_array($onetomanyrecs)&&count($onetomanyrecs)>0)
        {
          if ($this->createDestination())
          {
            $ownerfields = $this->getOwnerFields();
            for ($i=0;$i<count($onetomanyrecs);$i++)
            {
              // the referential key of the onetomanyrecs could be wrong, if we
              // are called for example from a copy function. So just in case,
              // we reset the correct key.
              if(!$this->destinationHasRelation())
              {
                for ($j=0, $_j = count($this->m_refKey); $j<$_j; $j++)
                {
                  $onetomanyrecs[$i][$this->m_refKey[$j]] = $record[$ownerfields[$j]];
                }
              }
              else
              {
                for ($j=0, $_j = count($this->m_refKey); $j<$_j; $j++)
                {
                   $onetomanyrecs[$i][$this->m_refKey[0]][$ownerfields[$j]] = $record[$ownerfields[$j]];
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

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form.
     *
     * Because the oneToMany has nothing to hide, we override the default
     * hide() implementation with a dummy method.
     *
     * @return String An empty string.
     */
    function hide()
    {
      //Nothing to hide..
      return "";
    }

    /**
     * Returns a piece of html code that can be used to get search terms input
     * from the user.
     *
     * The framework calls this method to display the searchbox
     * in the search bar of the recordlist, and to display a more extensive
     * search in the 'extended' search screen.
     * The regular atkAttributes returns a simple text input box for entering
     * a keyword to search for.
     *
     * @param array   $record   Array with values
     * @param boolean $extended if set to false, a simple search input is
     *                          returned for use in the searchbar of the
     *                          recordlist. If set to true, a more extended
     *                          search may be returned for the 'extended'
     *                          search page. The atkAttribute does not
     *                          make a difference for $extended is true, but
     *                          derived attributes may reimplement this.
     *
     * @return String A piece of html-code
     */
    function search($record="", $extended=false, $fieldprefix="")
    {
      $id = "atksearch[".$fieldprefix.$this->formName()."]";
      $this->registerKeyListener($id, KB_CTRLCURSOR|KB_UPDOWN);
      $result= '<input type="text" id="'.$id.'" class="'.get_class($this).'" name="'.$id.'" value="'.$record[$this->fieldName()].'"'.
                ($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').
                ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';
      return $result;
    }


    /**
     * Returns the condition (SQL) that should be used when we want to join an owner
     * node with the destination node of the atkOneToManyRelation.
     *
     * @param atkQuery $query      The query object.
     * @param String   $ownerAlias The owner table alias.
     * @param String   $destAlias  The destination table alias.
     *
     * @return String SQL string for joining the owner with the destination.
     */
    function getJoinCondition(&$query, $ownerAlias="", $destAlias="")
    {
      if (!$this->createDestination()) return false;

      if ($ownerAlias == "")
        $ownerAlias = $this->m_ownerInstance->m_table;

      if ($destAlias == "")
        $destAlias = $this->m_destInstance->m_table;

      $conditions = array();
      $ownerfields = $this->getOwnerFields();

      for ($i = 0, $_i = count($this->m_refKey); $i < $_i; $i++)
      {
        $conditions[] = $destAlias.".".$this->m_refKey[$i]."=".$ownerAlias.".".$ownerfields[$i];
      }

      return implode(" AND ", $conditions);
    }

    /**
     * Creates a smart search condition for a given search value, and adds it
     * to the query that will be used for performing the actual search.
     *
     * @param Integer  $id         The unique smart search criterium identifier.
     * @param Integer  $nr         The element number in the path.
     * @param Array    $path       The remaining attribute path.
     * @param atkQuery $query      The query to which the condition will be added.
     * @param String   $ownerAlias The owner table alias to use.
     * @param Mixed    $value      The value the user has entered in the searchbox.
     * @param String   $mode       The searchmode to use.
     */
    function smartSearchCondition($id, $nr, $path, &$query, $ownerAlias, $value, $mode)
    {
      // one-to-many join means we need to perform a distinct select
      $query->setDistinct(true);

      if (count($path) > 0)
      {
        $this->createDestination();

        $destAlias = "ss_{$id}_{$nr}_".$this->fieldName();

        $query->addJoin(
          $this->m_destInstance->m_table, $destAlias,
          $this->getJoinCondition($query, $ownerAlias, $destAlias),
          false
        );

        $attrName = array_shift($path);
        $attr = &$this->m_destInstance->getAttribute($attrName);

        if (is_object($attr))
        {
          $attr->smartSearchCondition($id, $nr + 1, $path, $query, $destAlias, $value, $mode);
        }
      }
      else
      {
        $this->searchCondition($query, $ownerAlias, $value, $mode);
      }
    }

    /**
     * Adds a search condition for a given search value
     * @param $table the table name
     * @param $value the search value
     * @return a piece of the SQL where clause
     */
     function searchCondition(&$query, $table, $value, $searchmode)
     {
       if ($this->createDestination()) {
         $searchcondition = $this->getSearchCondition($query, $table, $value, $searchmode);
         if(isset($searchcondition) && !empty($searchcondition)) {
           if ($this->m_ownerInstance->m_postvars["atkselector"]) {
             $query->addTable($this->m_destInstance->m_table);
             $query->addCondition($this->translateSelector($this->m_ownerInstance->m_postvars['atkselector']));
             $query->addSearchCondition($searchcondition);
           }
         }
       }
     }

     function getSearchCondition(&$query, $table, $value, $searchmode)
     {
       $usedfields = array();
       if (!is_array($value))
       {
         foreach ($this->m_destInstance->descriptorFields() as $field)
         {
           if (!in_array($field, $usedfields))
           {
             $this->_callSearchConditionOnDestField($query, $this->m_destInstance->m_table, $value, $searchmode, $field);
             $usedfields[] = $field;
           }
         }
       }
       else
       {
         foreach ($value as $key=>$val)
         {
           if ($val)
           {
             $this->_callSearchConditionOnDestField($query, $this->m_destInstance->m_table, $val, $searchmode, $key);
           }
         }
       }
     }

     /**
      * Calls searchCondition on an attribute in the destination
      * To hook the destination attribute on the query
      * @param atkQuery &$query     The query object
      * @param String   $table      The table to search on
      * @param mixed    $value      The value to search
      * @param mixed    $searchmode The mode used when searching
      * @param String   $field      The name of the attribute
      */
     function _callSearchConditionOnDestField(&$query, $table, $value, $searchmode, $field)
     {
       if ($this->createDestination())
       {
         $alias = $this->fieldName() . "_AE_" . $this->m_destInstance->m_table;
         $attr = &$this->m_destInstance->getAttribute($field);
         $attr->searchCondition($query, $alias, $value, $searchmode);

         $condition = array();
         $ownerfields = $this->getOwnerFields();
         for ($i = 0; $i < sizeof($ownerfields); $i++)
         {
            $condition[] = $this->m_ownerInstance->m_table . "." . $ownerfields[$i] . "=". $alias . "." . $this->m_refKey[$i];
         }

         $query->addJoin($table,$alias,implode(" AND ",$condition),false);
       }
     }

    /**
     * Determine the type of the foreign key on the other side.
     *
     * On the other side of a oneToManyRelation (in the destination node),
     * there may be a regular atkAttribute for the referential key, or an
     * atkManyToOneRelation pointing back at the source. This method discovers
     * which of the 2 cases we are dealing with.
     * @return boolean True if the foreign key on the other side is a
     *                 relation, false if not.
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

    function deleteAllowed()
    {
      if ($this->hasFlag(AF_RESTRICTED_DELETE))
      {
        // Get the destination node
        $classname = $this->m_destination;
        $cache_id = $this->m_owner.".".$this->m_name;
        $rel = &getNode($classname,$cache_id);
        // Get the current atkselector
        $where = $this->translateSelector($this->m_ownerInstance->m_postvars['atkselector']);
        if ($where)
        {
          $childrecords = $rel->selectDb($where);
          if (!empty($childrecords)) return atktext("restricted_delete_error");
        }
        else return;
      }
      return true;
    }

    /**
     * Here we check if the selector is on the owner or on the destination
     * if it's on the destination, we leave it alone.
     * Otherwise we translate it back to the destination.
     *
     * @todo when we translate the selector, we get the last used refKey
     *       but how do we know what is the right one?
     * @param string $selector the selector we have to translate
     * @return string the new selector
     */
    function translateSelector($selector)
    {
      // All standard SQL operators
      $sqloperators = array('=','<>','>','<','>=','<=','BETWEEN','LIKE','IN');
      $this->createDestination();

      // Check the filter for every SQL operators
      for ($counter=0;$counter<count($sqloperators);$counter++)
      {
        if ($sqloperators[$counter])
        {
          list($key, $value) = explode($sqloperators[$counter], $selector);

          // if the operator is in the filter
          if ($value)
          {
            // check if it's on the destination
            for ($refkeycount=0;$refkeycount<count($this->m_refKey);$refkeycount++)
            {
              $destinationkey = $this->m_destInstance->m_table.".".$this->m_refKey[$refkeycount];

              // if the selector is on the destination, we pass it back
              if ($key ==  $destinationkey || $key == $this->m_refKey[$refkeycount])
              {
                return $selector;
              }
            }
            // otherwise we set it on the destination
            return $destinationkey.$sqloperators[$counter].$value;
          }
        }
      }
      // We never found a value, something is wrong with the filter
      return "";
    }

   /**
    * Retrieves navigation links (prev/pagenumbers/next) and puts it
    * on the top and the bottom of the output string
    *
    * @param string &$output Pointer to the output string
    */
    function _fetchNavigationLinks(&$output)
    {
      global $ATK_VARS;

      // create temporary node and set the right parameters
      $this->createDestination();

      $limitArray = $this->fetchLimitArray();
      $this->m_destInstance->m_postvars["atklimit"]     = $limitArray["limit"];
      $this->m_destInstance->m_postvars["atkstartat"]   = $limitArray["offset"];
      $this->m_destInstance->m_postvars["limitPostfix"] = $this->fieldName();
      $this->m_destInstance->m_action                   = $ATK_VARS["atkaction"];

      // If we build a custom loadFilter (in the load method), use this filter
      if ($this->m_loadFilter) $destfilter=$this->m_loadFilter;
      // If not, a destinationFilter takes precedense over all other filters
      elseif ($this->m_destinationFilter) $destfilter = $this->m_destinationFilter;
      // If we have a selector, we need to make sure the navigator filters on this
      elseif ($ATK_VARS['atkselector'])
      {
        // if the filter is valid on the destination (highly unlikely) we just use it
        if ($this->m_destInstance->validateFilter($ATK_VARS['atkselector']))
        {
          $destfilter = $ATK_VARS['atkselector'];
        }
        // if the filter is on the owner, we translate it back to the destination
        else if ($this->m_ownerInstance->validateFilter($ATK_VARS['atkselector']))
        {
          $destfilter = $this->m_destInstance->validateFilter($this->translateSelector($ATK_VARS['atkselector']));
        }
        // otherwise we just don't set a filter
      }

      if ($destfilter)
      {
        // create a navigator and let it create the navigation links using the temporary node
        $navigator  = &atknew("atk.utils.atknavigator");
        $navigation = $navigator->getNavigationUrls($this->m_destInstance,"admin", $destfilter);
        $navstr     = $navigator->buildNavigation($this->m_destInstance,"admin",$navigation, true);
      }

      // add the navigation to the top and bottom of the page
      if ($navstr) $output = $navstr . "<br />" . $output . $navstr."<br />";
      else return $output;
    }

    function _setRecordlistCache()
    {
      if (!$this->m_recordlistcache && $this->createDestination())
      {
        $this->m_recordlistcache = &atknew("atk.recordlist.atkrecordlistcache");
        $this->m_recordlistcache->setNode($this->m_destInstance);
        $postvars = $this->m_destInstance->m_postvars;
        $postvars['atksearch'] = $this->m_ownerInstance->m_postvars['atksearch'][$this->fieldName()];
        $postvars['atksearchmode'] = $this->m_ownerInstance->m_postvars['atksearchmode'];
        $postvars['atkorderby'] = $this->m_ownerInstance->m_postvars['atkorderby'];
        $this->m_recordlistcache->setPostvars($postvars);
        $this->m_recordlistcache->addCacheIdentifier(array('key'=>"embeded"));
      }
      return true;
    }

    /**
     * Add dialog.
     */
    function partial_add_dialog()
    {
       $this->createDestination();
      $this->m_destInstance->m_partial = 'dialog';
      $handler = &$this->m_destInstance->getHandler('add');
      $handler->m_postvars = $this->m_ownerInstance->m_postvars;
      $handler->setDialogSaveUrl(partial_url($this->m_ownerInstance->atkNodeType(), 'edit', 'attribute.'.$this->fieldName().'.add_process'));
      $result = $handler->renderAddDialog();
      $page = &$this->m_ownerInstance->getPage();
      $page->addContent($result);
    }

    /**
     * Process add dialog save action.
     */
    function partial_add_process()
    {
      $this->createDestination();
      $handler = &$this->m_destInstance->getHandler('save');
      $handler->m_postvars = $this->m_ownerInstance->m_postvars;
       $handler->setDialogSaveUrl(partial_url($this->m_ownerInstance->atkNodeType(), 'edit', 'attribute.'.$this->fieldName().'.add_process'));
      $handler->handleSave(partial_url($this->m_ownerInstance->atkNodeType(), 'edit', 'attribute.'.$this->fieldName().'.refresh'));
    }

    /**
     * Add or copy dialog.
     */
    function partial_addorcopy_dialog()
    {
      $this->createDestination();
      $this->m_destInstance->addFilter($this->m_ownerInstance->m_postvars['atkfilter']);
      $handler = &$this->m_destInstance->getHandler('addorcopy');
      $handler->setProcessUrl(partial_url($this->m_ownerInstance->atkNodeType(), 'edit', 'attribute.'.$this->fieldName().'.addorcopy_process', array('atkfilter' => $this->m_ownerInstance->m_postvars['atkfilter'])));
      $handler->handleDialog();
    }

    /**
     * Process add or copy action.
     */
    function partial_addorcopy_process()
    {
      $this->createDestination();
      $addOrCopy = $this->m_ownerInstance->m_postvars['addorcopy'];

      // user has choosen to copy an existing record, let the copy action
      // be handled by the normal addorcopy handler
      if ($addOrCopy == 'copy')
      {
        $handler = &$this->m_destInstance->getHandler('addorcopy');
        $handler->m_postvars = $this->m_ownerInstance->m_postvars;
        $handler->handleCopy(partial_url($this->m_ownerInstance->atkNodeType(), 'edit', 'attribute.'.$this->fieldName().'.refresh'));
      }

      // user has choosen to add a new record, depending on whatever the AF_ONETOMANY_ADD_DIALOG
      // or the destination instance NF_ADD_DIALOG flags has been set we either show the user an
      // add dialog or redirect him/her to the add page (using an atkSubmit)
      else
      {
        atkimport("atk.ui.atkdialog");

        $script = atkDialog::getCloseCall();

        if ($this->hasFlag(AF_ONETOMANY_ADD_DIALOG) || $this->m_destInstance->hasFlag(NF_ADD_DIALOG))
        {
          $ui = &$this->m_ownerInstance->getUi();
          $filter = $this->m_ownerInstance->m_postvars['atkfilter'];
          $dialog = new atkDialog($this->m_ownerInstance->atkNodeType(), 'edit', 'attribute.'.$this->fieldName().'.add_dialog', array('atkfilter' => $filter));
          $title = $ui->title($this->m_destInstance->m_module, $this->m_destInstance->m_type, 'add');
          $dialog->setTitle($title);
          $script .= $dialog->getCall(true, false);
        }
        else
        {
          $url = dispatch_url($this->m_destInstance->atkNodeType(), 'add');
          $script .= "atkSubmit('".atkurlencode(session_url($url, SESSION_NESTED))."');";
        }

        $page = &$this->m_ownerInstance->getPage();
        $page->register_loadscript($script);
      }
    }
  }
?>