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
  define("AF_ONETOMANY_ADD_DIALOG", AF_SPECIFIC_2);

  /**
   * Use embedded forms for adding records
   * @deprecated Replaced by AF_ONETOMANY_ADD_DIALOG.
   */
  define("AF_ONETOMANY_EMBED", AF_ONETOMANY_ADD_DIALOG);

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
     * Use destination filter for autolink add link?
     *
     * @access private
     * @var boolean
     */
    var $m_useFilterForAddLink = true;

    /**
     * Use destination filter for edit link (edit button)?
     *
     * @access private
     * @var boolean
     */
    var $m_useFilterForEditLink = true;

    /**
     * Function names for recordlist header/footer generation
     *
     * @access private
     * @var string
     */
    var $m_headerName = "";
    var $m_footerName = "";
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
     *                     In contrast with most other attributes, the name
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
     * Use destination filter for auto add link?
     *
     * @param boolean $useFilter use destnation filter for add link?
     */
    function setUseFilterForAddLink($useFilter)
    {
      $this->m_useFilterForAddLink = $useFilter;
    }

    /**
     * Use destination filter for edit link (edit button)?
     *
     * @param boolean $useFilter use destnation filter for edit link (edit button)?
     */
    function setUseFilterForEditLink($useFilter)
    {
      $this->m_useFilterForEditLink = $useFilter;
    }

    /**
     * Create the datagrid for the edit and display actions. The datagrid is
     * configured with the correct node filter, excludes etc.
     *
     * The datagrid uses for both the edit and display actions the partial_grid
     * method to update it's view.
     *
     * @param array  $record      the record
     * @param string  $mode       the mode
     * @param string  $action     the action
     * @param boolean $useSession use session?
     *
     * @return atkDataGrid grid
     */
    protected function createGrid($record, $mode, $action, $useSession=true)
    {
      $this->createDestination();

      atkimport('atk.datagrid.atkdatagrid');

      $grid = atkDataGrid::create($this->m_destInstance, null, null, true, $useSession);
      $grid->setMode($mode);

      $grid->removeFlag(atkDataGrid::EXTENDED_SEARCH);
      if ($mode == 'view')
      {
        $grid->removeFlag(atkDataGrid::MULTI_RECORD_ACTIONS);
        $grid->removeFlag(atkDataGrid::MULTI_RECORD_PRIORITY_ACTIONS);
        $grid->removeFlag(atkDataGrid::LOCKING);
      }

      $grid->setBaseUrl(partial_url($this->getOwnerInstance()->atkNodeType(), $action, 'attribute.'.$this->fieldName().'.grid'));

      $grid->setExcludes($this->m_refKey);

      $grid->addFilter($this->_getLoadWhereClause($record));
      if ($this->m_destinationFilter != '')
      {
        $grid->addFilter($this->parseFilter($this->m_destinationFilter, $record));
      }

      return $grid;
    }

    /**
     * Updates the datagrid for the edit and display actions.
     *
     * @return string grid html
     */
    public function partial_grid()
    {
      $this->createDestination();
      $node = $this->getDestination();
      atkimport('atk.datagrid.atkdatagrid');
      return atkDataGrid::resume($node)->render();
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * The atkOneToManyRelation displays a list of detail records in "view"
     * mode, in the form of a read-only data grid. In "list" mode, a plain
     * list of detail record descriptors is displayed.
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list"
     *                     for displaying in recordlists)
     * @return String HTML String
     */
    public function display($record, $mode="list")
    {
      // for the view mode we use the datagrid and load the records ourselves
      if ($mode == 'view')
      {
        $grid = $this->createGrid($record, $mode, 'view');
        $grid->loadRecords(); // load records early

        // no records
        if ($grid->getCount() == 0)
        {
          if (!in_array($mode, array("csv", "plain")))
          {
            return $this->text("none");
          }
          else
          {
            return '';
          }
        }

        $actions = array();
        if (!$this->m_destInstance->hasFlag(NF_NO_VIEW))
        {
          $actions['view'] = dispatch_url($this->m_destination, "view", array("atkselector" => "[pk]", "atkfilter" => $this->m_destinationFilter));
        }

        $grid->setDefaultActions($actions);
        return $grid->render();
      }

      // records should be loaded inside the load method
      $records = $record[$this->fieldName()];

      // no records
      if (count($records) == 0)
      {
        return $this->text('none');
      }

      // list mode
      if ($mode == "list")
      {
        $result = "<ul>";

        foreach ($records as $current)
        {
          $result .= sprintf("<li>%s</li>", $this->m_destInstance->descriptor($current));
        }

        $result .= "</ul>";

        return $result;
      }

      // cvs / plain mode
      else
      {
        $result = "";

        foreach ($records as $i => $current)
        {
          $result .= ($i > 0 ? ', ' : '').$this->m_destInstance->descriptor($current);
        }

        return $result;
      }
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
     *
     * @return String A piece of htmlcode for editing this attribute
     */
    public function edit($record="", $fieldprefix="", $mode='')
    {
      $grid = $this->createGrid($record, $mode, $mode);

      $params = array();
      if ($this->m_useFilterForEditLink && $this->m_destinationFilter != "")
      {
        $params["atkfilter"] = $this->m_destinationFilter;
      }

      $actions = $this->m_destInstance->defaultActions("relation", $params);
      $grid->setDefaultActions($actions);

      $grid->loadRecords(); // force early load of records

      $output =
        $this->editHeader($record, $grid->getRecords()).
        ($grid->getCount() > 0 ? $grid->render() : "").
        $this->editFooter($record, $grid->getRecords());

      if ($this->m_destInstance->allowed("add"))
      {
        $this->_addAddToEditOutput($output, $grid->getRecords(), $record);
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
     * Internal function to get the add link for a atkOneToManyRelation
     * @param Array $myrecords  The load of all attributes (see comment in edit() code)
     * @param Array $record     The record that holds the value for this attribute.
     * @return String  The link to add records to the onetomany
     */
    function _getAddLink($myrecords, $record,$saveform=true)
    {
      $filter = $this->getAddFilterString($record);

      $addorcopy = $this->hasFlag(AF_ONETOMANY_ADDORCOPY_DIALOG) ||
                   $this->m_destInstance->hasFlag(NF_ADDORCOPY_DIALOG);

      $add = $this->hasFlag(AF_ONETOMANY_ADD_DIALOG) ||
             $this->m_destInstance->hasFlag(NF_ADD_DIALOG);

      if ($addorcopy)
      {
        atkimport('atk.handlers.atkaddorcopyhandler');
        $showDialog = atkAddOrCopyHandler::hasCopyableRecords($this->m_destInstance, $filter);

        if ($showDialog)
          return $this->_getDialogAddLink($record, 'addorcopy');
      }

      if ($add)
      {
        return $this->_getDialogAddLink($record, 'add');
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
      if ($this->m_useFilterForAddLink && $this->m_destinationFilter!="")
      {
        $strfilter.=' AND '.$this->parseFilter($this->m_destinationFilter,$record);
      }

      return $strfilter;
    }

    function _getDialogAddLink($record, $action)
    {
      atkimport("atk.ui.atkdialog");

      $ui = &$this->m_ownerInstance->getUi();

      $params = array();

      $filter = $this->getAddFilterString($record);
      if (!empty($filter))
        $params['atkfilter'] = $filter;

      $dialog = new atkDialog($this->m_ownerInstance->atkNodeType(), 'edit', 'attribute.'.$this->fieldName().'.'.$action.'_dialog', $params);
      $title = $ui->title($this->m_destInstance->m_module, $this->m_destInstance->m_type, $action);
      $dialog->setTitle($title);
      $dialog->setModifierObject($this->m_destInstance);
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
      if (($this->m_maxRecords==0 || $this->m_maxRecords > count($myrecords)) && $this->createDestination() && !$this->m_destInstance->hasFlag(NF_NO_ADD))
      {
        $params = array();

        $filter = $this->getAddFilterString($record);
        if (!empty($filter))
          $params['atkfilter'] = $filter;

        $add_url = dispatch_url($this->m_destination,"add", $params);
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
      $filterelems = array();

      $ownerfields = $this->getOwnerFields();
      if ($this->destinationHasRelation())
      {
        // we need to set the filter of the record we are going to add.
        // The referential key must be set to the value of the current
        // primary key.
        $this->createDestination();
        for ($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
        {
          $primkeyattr = &$this->m_ownerInstance->m_attribList[$ownerfields[$i]];
          $filterelems[] = $this->m_destInstance->getTable().".".$this->m_refKey[$i]."='".$this->escapeSQL($primkeyattr->value2db($record))."'";
        }
      }
      else
      {
        for ($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
        {
          $filterelems[] = $this->m_destInstance->getTable().".".$this->m_refKey[$i]."='".$this->escapeSQL($record[$ownerfields[$i]])."'";
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
      if(!empty($this->m_headerName))
      {
        $methodname = $this->m_headerName;
        return $this->m_ownerInstance->$methodname($record, $childrecords, $this);
      }
      else return "";
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
      if(!empty($this->m_footerName))
      {
        $methodname = $this->m_footerName;
        return $this->m_ownerInstance->$methodname($record, $childrecords, $this);
      }
      else return "";
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

      $result = implode(" AND ", $whereelems);
      return $result == '' ? '1=0' : $result;
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
      $result = null;

      // for edit and view mode we don't load any records unless a display override exists
      // we use the grid to load records because it makes things easier
      if (($mode != 'add' && $mode != 'edit' && $mode != 'view') ||
          ($mode == 'view' && method_exists($this->getOwnerInstance(), $this->fieldName()."_display")))
      {
        $grid = $this->createGrid($record, $mode, $mode, false);
        $grid->setPostvar('atklimit', -1); // all records
        $grid->setCountHandler(create_function('', '')); // don't count
        $grid->loadRecords();
        $result = $grid->getRecords();
        $grid->destroy(); // clean-up
      }

      return $result;
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
              // original record
              $original = $onetomanyrecs[$i];
              $onetomanyrecs[$i]['atkorgrec'] = $original;

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
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * Note that not all modes may be supported by the database driver.
     * Compare this list to the one returned by the databasedriver, to
     * determine which searchmodes may be used.
     *
     * @return array List of supported searchmodes
     */
    function getSearchModes()
    {
      return array('substring');
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
       if ($this->createDestination())
       {
         $searchcondition = $this->getSearchCondition($query, $table, $value, $searchmode);

         if(!empty($searchcondition))
         {
           $query->addSearchCondition($searchcondition);

           // @todo: is this still needed?
           if ($this->m_ownerInstance->m_postvars["atkselector"])
           {
             $query->addTable($this->m_destInstance->m_table);
             $query->addCondition($this->translateSelector($this->m_ownerInstance->m_postvars['atkselector']));
           }
         }
       }
     }

     function getSearchCondition(&$query, $table, $value, $searchmode)
     {
       $usedfields = array();
       $searchconditions = array();

       if (!is_array($value))
       {
         foreach ($this->m_destInstance->descriptorFields() as $field)
         {
           if (!in_array($field, $usedfields))
           {
             $sc = $this->_callSearchConditionOnDestField($query, $this->m_destInstance->m_table, $value, $searchmode, $field, $table);
             if (!empty($sc))
              $searchconditions[] = $sc;
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
             $sc = $this->_callSearchConditionOnDestField($query, $this->m_destInstance->m_table, $val, $searchmode, $key, $table);
             if (!empty($sc))
              $searchconditions[] = $sc;
           }
         }
       }

       if (count($searchconditions)>0)
        return "(".implode(" OR ",$searchconditions).")";
       else
        return false;
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
     function _callSearchConditionOnDestField(&$query, $table, $value, $searchmode, $field, $reftable)
     {
       if ($this->createDestination())
       {
         $alias = $this->fieldName() . "_AE_" . $this->m_destInstance->m_table;
         $attr = &$this->m_destInstance->getAttribute($field);

         $query->addJoin($table, $alias, $this->getJoinCondition($query, $reftable, $alias), false);

         return $attr->getSearchCondition($query, $alias, $value, $searchmode);
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

    /**
     * Edit dialog.
     */
    function partial_edit_dialog()
    {
      $this->createDestination();
      $this->m_destInstance->m_partial = 'dialog';
      $handler = &$this->m_destInstance->getHandler('edit');
      $handler->m_postvars = $this->m_ownerInstance->m_postvars;
      $handler->setDialogSaveUrl(partial_url($this->m_ownerInstance->atkNodeType(), 'edit', 'attribute.'.$this->fieldName().'.edit_process'));
      $result = $handler->renderEditDialog();
      $page = &$this->m_ownerInstance->getPage();
      $page->addContent($result);
    }

    /**
     * Process edit dialog update action.
     */
    function partial_edit_process()
    {
      $this->createDestination();
      $handler = &$this->m_destInstance->getHandler('update');
      $handler->m_postvars = $this->m_ownerInstance->m_postvars;
      $handler->setDialogSaveUrl(partial_url($this->m_ownerInstance->atkNodeType(), 'edit', 'attribute.'.$this->fieldName().'.edit_process'));
      $handler->handleUpdate(partial_url($this->m_ownerInstance->atkNodeType(), 'edit', 'attribute.'.$this->fieldName().'.refresh'));
    }

    /**
     * Set header generation function name.
     * @param String $name The header generation function name.
     */
    function setHeader($name)
    {
      $this->m_headerName = $name;
  }

    /**
     * Set footer generation function name.
     * @param String $name The footder generation function name.
     */
    function setFooter($name)
    {
      $this->m_footerName = $name;
    }
  }
?>
