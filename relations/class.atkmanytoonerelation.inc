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
/**
   * flag(s) specific for atkManyToOneRelation
   */
  define("AF_RELATION_AUTOLINK", AF_SPECIFIC_1);     // create edit/view links for the items in a manytoonerelation dropdown.
  define("AF_RELATION_NO_NULL_ITEM", AF_SPECIFIC_2); // do not add null option ever
  define("AF_RELATION_AUTOCOMPLETE", AF_SPECIFIC_3); // use auto-completition instead of drop-down / selection page
  define("AF_MANYTOONE_LAZY", AF_SPECIFIC_4); // lazy load

  /**
   * @internal include base class
   */
  userelation("atkrelation");

  /**
   * A N:1 relation between two classes.
   *
   * For example, projects all have one coordinator, but one
   * coordinator can have multiple projects. So in the project
   * class, there's a ManyToOneRelation to a coordinator.
   *
   * This relation essentially creates a dropdown box, from which
   * you can select from a set of records.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage relations
   *
   */
  class atkManyToOneRelation extends atkRelation
  {
    /**
     * By default, we do a left join. this means that records that don't have
     * a record in this relation, will be displayed anyway. NOTE: set  this to
     * false only if you know what you're doing. When in doubt, 'true' is
     * usually the best option.
     * @var boolean
     */
    var $m_leftjoin = true;

    /**
     * The array of referential key fields.
     * @access private
     * @var array
     */
    var $m_refKey = array();

    /**
     * SQL statement with extra filter for the join that retrieves the
     * selected record.
     * @var String
     */
    var $m_joinFilter = "";

    /**
     * Hide the relation when there are no records to select.
     * @access private
     * @var boolean
     */
    var $m_hidewhenempty = false;

    /**
     * Variable for caching records
     * @access private
     * @var array
     */
    var $m_selectableRecords = array();

    /**
     * List columns.
     * @Access private
     * @var Array
     */
    var $m_listColumns = array();

    /**
     * Always show list columns?
     * @access private
     * @var boolean
     */
    var $m_alwaysShowListColumns = false;

    /**
     * Label to use for the 'none' option.
     *
     * @access private
     * @var String
     */
    var $m_noneLabel = NULL;

    /**
     * Minimum number of character a user needs to enter before auto-completion kicks in.
     *
     * @access private
     * @var int
     */
    var $m_autocomplete_minchars = 2;

    /**
     * An array with the fieldnames of the destination node in which the autocompletion must search
     * for results.
     *
     * @access private
     * @var array
     */
    var $m_autocomplete_searchfields = "";

    /**
     * The search mode of the autocomplete fields. Can be 'startswith', 'exact' or 'contains'.
     *
     * @access private
     * @var String
     */
    var $m_autocomplete_searchmode = "contains";

    /**
     * Value determines wether the search of the autocompletion is case-sensitive.
     *
     * @var boolean
     */
    var $m_autocomplete_search_case_sensitive = false;

    /**
     * Value determines if select link for autocomplete should use atkSubmit or not (for use in admin screen for example)
     *
     * @var boolean
     */
    var $m_autocomplete_saveform = true;

    /**
     * Set the minimal number of records for showing the automcomplete. If there are less records
     * the normal dropdown is shown
     *
     * @access private
     * @var integer
     */
    var $m_autocomplete_minrecords = -1;

    // override onchangehandler init
    var $m_onchangehandler_init = "newvalue = el.options[el.selectedIndex].value;\n";

    /**
     * Constructor.
     * @param String $name The name of the attribute. This is the name of the
     *                     field that is the referential key to the
     *                     destination.
     *                     For relations with more than one field in the
     *                     foreign key, you should pass an array of
     *                     referential key fields. The order of the fields
     *                     must match the order of the primary key attributes
     *                     in the destination node.
     * @param String $destination The node we have a relationship with.
     * @param int $flags Flags for the relation
     */
    function atkManyToOneRelation($name, $destination, $flags=0)
    {
      if (atkconfig("manytoone_autocomplete_default", false))
        $flags |= AF_RELATION_AUTOCOMPLETE;

      if (atkconfig("manytoone_autocomplete_large", true) && hasFlag($flags, AF_LARGE))
        $flags |= AF_RELATION_AUTOCOMPLETE;

      $this->m_autocomplete_minchars = atkconfig("manytoone_autocomplete_minchars", 2);
      $this->m_autocomplete_searchmode = atkconfig("manytoone_autocomplete_searchmode", "contains");
      $this->m_autocomplete_search_case_sensitive = atkconfig("manytoone_autocomplete_search_case_sensitive", false);

      if (is_array($name))
      {
        $this->m_refKey = $name;

        // ATK can't handle an array as name, so we initialize the
        // underlying attribute with the first name of the referential
        // keys.
        // Languagefiles, overrides, etc should use this first name to
        // override the relation.
        $this->atkRelation($name[0], $destination, $flags);
      }
      else
      {
        $this->m_refKey[] = $name;
        $this->atkRelation($name, $destination, $flags);
      }

      if ($this->hasFlag(AF_MANYTOONE_LAZY) && (count($this->m_refKey) > 1 || $this->m_refKey[0] != $this->fieldName()))
      {
        atkerror("AF_MANYTOONE_LAZY flag is not supported for multi-column reference key or a reference key that uses another column.");
      }
    }

    /**
     * Set the searchfields for the autocompletion.
     *
     * @param array $searchfields
     */
    function setAutoCompleteSearchFields($searchfields)
    {
      $this->m_autocomplete_searchfields = $searchfields;
    }

    /**
     * Set the searchmode for the autocompletion:
     * exact, startswith(default) or contains.
     *
     * @param array $searchmode
     */
    function setAutoCompleteSearchMode($mode)
    {
      $this->m_autocomplete_searchmode = $mode;
    }

    /**
     * Set the case-sensitivity for the autocompletion search (true or false).
     *
     * @param array $case_sensitive
     */
    function setAutoCompleteCaseSensitive($case_sensitive)
    {
      $this->m_autocomplete_search_case_sensitive = $case_sensitive;
    }

    /**
     * Sets the minimum number of characters before auto-completion kicks in.
     *
     * @param int $chars
     */
    function setAutoCompleteMinChars($chars)
    {
      $this->m_autocomplete_minchars = $chars;
    }

    /**
     * Set if the select link should save form (atkSubmit) or not (for use in admin screen for example)
     *
     * @param boolean $saveform
     */
    function setAutoCompleteSaveForm($saveform=true)
    {
      $this->m_autocomplete_saveform = $saveform;
    }

    /**
     * Set the minimal number of records for the autocomplete to show
     * If there are less records the normal dropdown is shown
     *
     * @param integer $minrecords
     */
    function setAutoCompleteMinRecords($minrecords)
    {
      $this->m_autocomplete_minrecords = $minrecords;
    }

    /**
     * Add list column. An attribute of the destination node
     * that (only) will be displayed in the recordlist.
     */
    function addListColumn($attr)
    {
      $this->m_listColumns[] = $attr;
    }

    /**
     * Add multiple list columns. Attributes of the destination node
     * that (only) will be displayed in the recordlist.
     */
    function addListColumns()
    {
      $attrs = func_get_args();
      foreach ($attrs as $attr)
        $this->m_listColumns[] = $attr;
    }


    /**
     * Always show list columns in list view,
     * even if the attribute itself is hidden?
     *
     * @param $value always show list columns?
     */
    function setAlwaysShowListColumns($value)
    {
      $this->m_alwaysShowListColumns = $value;
    }

    /**
     * Convert value to DataBase value
     * @param array $rec Record to convert
     * @return int Database safe value
     */
    function value2db($rec)
    {
      if ($this->isEmpty($rec))
      {
        atkdebug($this->fieldName()." IS EMPTY!");
        return NULL;
      }
      else
      {
        if ($this->createDestination())
        {
          if (is_array($rec[$this->fieldName()]))
          {
            $pkfield = $this->m_destInstance->m_primaryKey[0];
            $pkattr = &$this->m_destInstance->getAttribute($pkfield);
            return $pkattr->value2db($rec[$this->fieldName()]);
          }
          else
          {
            return $rec[$this->fieldName()];
          }
        }
      }
      // This never happens, does it?
      return "";
    }

    /**
     * Fetch value out of record
     * @param $postvars Postvars
     * @return decoded value
     */
    function fetchValue($postvars)
    {
      if ($this->isPosted($postvars))
      {
        // Split the primary key of the selected record into its
        // referential key elements.
        $keyelements = decodeKeyValueSet($postvars[$this->fieldName()]);

        $result = array();

        foreach ($keyelements as $key=>$value)
        {
          // Tablename must be stripped out because it is in the way..
          if (strpos($key,'.')>0)
          {
            $field = substr($key,strrpos($key,'.')+1);
          }
          else
          {
            $field = $key;
          }
          $result[$field] = $value;
        }

        // add descriptor fields, this means they can be shown in the title
        // bar etc. when updating failed for example
        $record = array($this->fieldName() => $result);
        $this->populate($record);
        $result = $record[$this->fieldName()];

        return $result;
      }
      return NULL;
    }


    /**
     * Converts DataBase value to normal value
     * @param $rec Record
     * @return decoded value
     */
    function db2value($rec)
    {
      $this->createDestination();

      if (isset($rec[$this->fieldName()]) &&
          is_array($rec[$this->fieldName()]) &&
          (!isset($rec[$this->fieldName()][$this->m_destInstance->primaryKeyField()]) ||
           empty($rec[$this->fieldName()][$this->m_destInstance->primaryKeyField()])))
      {
        return NULL;
      }

      if (isset($rec[$this->fieldName()]))
      {
        $myrec = $rec[$this->fieldName()];
        if (is_array($myrec))
        {
          $result = array();
          if ($this->createDestination())
          {
            foreach (array_keys($this->m_destInstance->m_attribList) as $attrName)
            {
              $attr = &$this->m_destInstance->m_attribList[$attrName];
              $result[$attrName] = $attr->db2value($myrec);
            }
          }

          return $result;
        }
        else
        {
          // if the record is not an array, probably only the value of the primary key was loaded.
          // This workaround only works for single-field primary keys.
          if ($this->createDestination())
            return array($this->m_destInstance->primaryKeyField() => $myrec);
        }
      }
    }

    /**
     * Set none label.
     */
    function setNoneLabel($label)
    {
      $this->m_noneLabel = $label;
    }

    /**
     * Get none label.
     */
    function getNoneLabel()
    {
      if ($this->m_noneLabel !== NULL)
        return $this->m_noneLabel;
      $nodename = $this->m_destInstance->m_type;
      $modulename = $this->m_destInstance->m_module;;
      return atktext('select_none', $modulename, $nodename);
    }

    /**
     * Populate the record with the destination record data.
     *
     * @param array $record        record
     * @param mixed $fullOrFields  load all data, only the given fields or only the descriptor fields?
     */
    function populate(&$record, $fullOrFields=false)
    {
      if ($record[$this->fieldName()] == "") return;

      atkdebug("Delayed loading of ".($fullOrFields || is_array($fullOrFields) ? "" : "descriptor ")."fields for ".$this->m_name);
      $this->createDestination();

      $includes = "";
      if (is_array($fullOrFields))
        $includes = array_merge($this->m_destInstance->m_primaryKey, $fullOrFields);
      else if (!$fullOrFields)
        $includes = $this->m_destInstance->descriptorFields();

      list($record[$this->fieldName()]) =
        $this->m_destInstance->selectDb($this->m_destInstance->primaryKey($record[$this->fieldName()]), "", "", "", $includes);
    }

    /**
     * Returns a displayable string for this value.
     * @param $record Record
     * @return a displayable string
     */
    function display($record, $mode='list')
    {
      if ($this->createDestination())
      {
        if (count($record[$this->fieldName()])==count($this->m_refKey))
          $this->populate($record);

        if(!$this->isEmpty($record))
        {
          $title = $this->m_destInstance->descriptor($record[$this->fieldName()]);
        }
        else
        {
          $title = ""; // no record
        }

        $result = $title; // default result is just the title.

        if ($this->hasFlag(AF_RELATION_AUTOLINK) && (!in_array($mode, array("csv", "plain")))) // create link to edit/view screen
        {
          if (($this->m_destInstance->allowed("view"))&&!$this->m_destInstance->hasFlag(NF_NO_VIEW)&&$title!="")
          {
            $result = href("dispatch.php?atknodetype=".$this->m_destination."&atkaction=view&atkselector=".rawurlencode($this->m_destInstance->primaryKey($record[$this->fieldName()])), $title, SESSION_NESTED);
          }
        }
        return $result;
      }
      else
      {
       atkdebug("Can't create destination! ($this->m_destination");
      }
      return "";
    }

    /**
     * Creates HTML for the selection and auto links.
     *
     * @param string $id attribute id
     * @param array $record record
     * @return string
     */
    function createSelectAndAutoLinks($id, $record)
    {
      $links = array();

      $newsel = $id;
      $filter = $this->parseFilter($this->m_destinationFilter, $record);
      $links[] = $this->_getSelectLink($newsel, $filter);

      if ($this->hasFlag(AF_RELATION_AUTOLINK)) // auto edit/view link
      {
        if ($this->m_destInstance->allowed("add"))
        {
          $links[] = href("dispatch.php?atknodetype=".$this->m_destination."&atkaction=add&atkpkret=".$id."_newsel".($filter!=""?"&atkfilter=".rawurlencode($filter):""),atktext("new"),SESSION_NESTED,true);
        }

        if ($this->m_destInstance->allowed("edit") && $record[$this->fieldName()] != NULL)
        {
          //we laten nu altijd de edit link zien, maar eigenlijk mag dat niet, want
          //de app crasht als er geen waarde is ingevuld.
          $editUrl = session_url("dispatch.php?atknodetype=".$this->m_destination."&atkaction=edit&atkselector=REPLACEME",SESSION_NESTED);
          $links[] = "<span id=\"".$id."_edit\" style=\"\"><a href='javascript:atkSubmit(mto_parse(\"".atkurlencode($editUrl)."\", document.entryform.".$id.".value))'>".atktext('edit')."</a></span>";
        }
      }

      return implode("&nbsp;", $links);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param array  $record       The edit record
     * @param String $fieldprefix  Prefix for the fieldname
     * @return Piece of html code that can  be used in a form to edit this
     */
    function edit($record, $fieldprefix="", $mode="edit")
    {
      // Two variances.. one with a dropdownbox containing all records,
      // and one with a dropdownbox that contains the last 10 used records,
      // with a link to a selector with all records. This second one is for
      // relations with large tables, so is only used when AF_LARGE is set.

      // check if a normal dropdown should be shown or the autocomplete field
      if ($this->hasFlag(AF_RELATION_AUTOCOMPLETE) && $this->m_autocomplete_minrecords > -1)
      {
        $recordset = $this->_getSelectableRecords($record, 'select');
      }

      if ($this->hasFlag(AF_RELATION_AUTOCOMPLETE) && (is_object($this->m_ownerInstance)) && is_array($recordset) && count($recordset) > $this->m_autocomplete_minrecords)
      {
        $this->createDestination();
        $page = &$this->m_ownerInstance->getPage();

        $id = $this->getHtmlid($fieldprefix);

        $afterUpdateFunc = "";
        if (count($this->m_onchangecode))
        {
          $afterUpdateFunc = "{$id}_callChangeHandler";

          $js = "
            function {$afterUpdateFunc}()
            {
              {$id}_onChange(\$('{$id}_selection'));
            }
          ";

          $this->m_onchangehandler_init = "newvalue = el.value;\n";
          $page->register_scriptcode($js);
          $this->_renderChangeHandler($fieldprefix);
        }

        $url = partial_url($this->m_ownerInstance->atkNodeType(), $mode, 'attribute.'.$this->fieldName().'.autocomplete');

        if (!$this->_isSelectableRecord($record, 'select'))
          $record[$this->fieldName()] = NULL;

        $afterUpdate = ($afterUpdateFunc!="") ? $afterUpdateFunc : "null";
        $code = "ATK.ManyToOneRelation.completeEdit('{$id}_search', '{$id}_result', '{$id}_selection', '{$id}_selectionbox', '$id', '$url', $afterUpdate, 1);";

        $page->register_script(atkconfig("atkroot").'atk/javascript/prototype/prototype.js');
        $page->register_script(atkconfig("atkroot").'atk/javascript/scriptaculous/scriptaculous.js');
        $page->register_script(atkconfig('atkroot').'atk/javascript/class.atkmanytoonerelation.js');
        $page->register_loadscript($code);

        $this->m_ownerInstance->addStyle("atkmanytoonerelation.css");

        if ($record[$this->fieldName()] != NULL)
        {
          $selectedDisplay = '';
          $selectedValue = $this->m_destInstance->primaryKey($record[$this->fieldName()]);
        }
        else
        {
          $selectedDisplay = 'display: none';
          $selectedValue = '';
        }

        $theme = &atkinstance("atk.ui.atktheme");
        $resetIcon = sprintf('<img width="16" height="16" border="0" valign="absmiddle" src="%1$s" alt="%2$s" title="%2$s">',
                        $theme->iconPath('delete',"recordlist", $this->m_node->m_module),
                        atktext("reset", $this->m_node->m_module, $this->m_node->m_type));

        $links = $this->createSelectAndAutoLinks($id, $record);

        return
          '<input type="text" id="'.$id.'_search" class="atkmanytoonerelation_search" size="40" />
           <div id="'.$id.'_result" style="display: none" class="atkmanytoonerelation_result"></div>
           <input type="hidden" id="'.$id.'" name="'.$id.'" value="'.$selectedValue.'" />&nbsp;'.$links.'<br />
           <span id="'.$id.'_selectionbox" class="atkmanytoonerelation_selection" style="'.$selectedDisplay.'">'.
             '<span id="'.$id.'_selection">'.$this->display($record).'</span>&nbsp;'.
             '<a href="#" onClick="ATK.ManyToOneRelation.clear(\''.$id.'\', '.$afterUpdate.');">'.$resetIcon.'</a>'.
           '</span>';
      }

      if ($this->createDestination())
      {
        $id = $fieldprefix.$this->fieldName();
        $filter=$this->parseFilter($this->m_destinationFilter,$record);
        $autolink = $this->getRelationAutolink($id, $filter);
        $editflag = true;

        if (!$this->hasFlag(AF_LARGE))
        {
          // normal dropdown..
          $value=NULL;
          if(isset($record[$this->fieldName()]))
            $value = $record[$this->fieldName()];
          $currentPk = $this->m_destInstance->primaryKey($value);
          $recordset = $this->_getSelectableRecords($record, 'select');
          if(count($recordset)==0) $editflag = false;

          $onchange='';
          if (count($this->m_onchangecode))
          {
            $onchange = 'onChange="'.$id.'_onChange(this);"';
            $this->_renderChangeHandler($fieldprefix);
          }

          // autoselect if there is only one record (if obligatory is not set,
          // we don't autoselect, since user may wist to select 'none' instead
          // of the 1 record.
          $result = "";
          if (count($recordset) == 0)
          {
            $result .= $this->getNoneLabel();
          }
          else if ((count($recordset)==1 || $this->_checkPrimaryKeysForSelector($recordset))&&$this->hasFlag(AF_OBLIGATORY))
          {
            $result.= $this->m_destInstance->descriptor($recordset[0])."&nbsp;&nbsp;";
            $result.= $this->hide(array($this->fieldName()=>$recordset[0]), $fieldprefix);
          }
          else
          {
            $this->registerKeyListener($id, KB_CTRLCURSOR|KB_LEFTRIGHT);
            $result.= '<select id="'.$id.'" name="'.$id.'" class="atkmanytoonerelation" '.$onchange.'>';
            if ((!$this->hasFlag(AF_OBLIGATORY) && !$this->hasFlag(AF_RELATION_NO_NULL_ITEM)) || (atkconfig("list_obligatory_null_item")&&!is_array($value)))
            {
              // Relation may be empty, so we must provide an empty selectable..
              $result.= '<option value="">'.$this->getNoneLabel();
            }
            // Newly added check
            $newlyadded = "";
            if (is_object($this->m_ownerInstance) && isset($this->m_ownerInstance->m_postvars[$id."_ret"]))
            {
              $newlyadded = $this->m_ownerInstance->m_postvars[$id."_ret"];
            }

            for ($i=0, $_i=count($recordset);$i<$_i;$i++)
            {
              $pk = $this->m_destInstance->primaryKey($recordset[$i]);
              if (($newlyadded!="" && $newlyadded==$pk) || ($newlyadded=="" && $pk==$currentPk)) $sel = "selected"; else $sel = "";
              $result.= '<option value="'.$pk.'" '.$sel.'>'.$this->m_destInstance->descriptor($recordset[$i]);
            }
            $result.='</select>';
          }
        }
        else
        {
          // Large mode
          // TODO: Fill a dropdown box with recent records..

          $newlyadded = $this->m_ownerInstance->m_postvars[$id."_ret"];

          $destrecord = $this->_getDestRecord($id."_newsel", $newlyadded, $record, $editflag);

          if (count($destrecord))
          {
            $result.= '<span id="'.$id.'_current" >'.$this->m_destInstance->descriptor($destrecord)."&nbsp;&nbsp;";
            if (!$this->hasFlag(AF_OBLIGATORY))
            {
              $result.= '<a href="#" onClick="document.getElementById(\''.
                                    $id.'\').value=\'\'; document.getElementById(\''.$id.'_current\').style.display=\'none\'">'.atktext("unselect").'</a>&nbsp;&nbsp;';
            }
            $result.= '</span>';
          }

          $result.=$this->hide($record, $fieldprefix);
          $newsel = $id."_newsel";
          $result.=$this->_getSelectLink($newsel, $filter);
        }

        if($editflag) $result.= isset($autolink['edit']) ? $autolink['edit'] : "";
        $result.= isset($autolink['add']) ? $autolink['add'] : "";
        return $result;
      }

      return atkerror("Could not create destination for destination: $this->m_destination!");
    }


    function _getSelectLink($selname, $filter)
    {
      $result = "";
      // we use the current level to automatically return to this page
      // when we come from the select..
      $atktarget = rawurlencode("dispatch.php?atklevel=".atkLevel()."&".$selname."=[atkprimkey]");
      $linkname = atktext("link_select_".getNodeType($this->m_destination), getNodeModule($this->m_destination),getNodeType($this->m_destination),'','',true);
      if (!$linkname) $linkname = atktext("select_a").' '.strtolower(atktext(getNodeType($this->m_destination), getNodeModule($this->m_destination),getNodeType($this->m_destination)));
      if ($this->m_destinationFilter!="")
      {
        $result.= href("dispatch.php?atknodetype=".$this->m_destination.
                       "&atkaction=select&atkfilter=".$filter."&atktarget=".$atktarget,
                       $linkname,
                       SESSION_NESTED,
                       $this->m_autocomplete_saveform,'class="atkmanytoonerelation"');
      }
      else
      {
        $result.= href("dispatch.php?atknodetype=".$this->m_destination.
                       "&atkaction=select&atktarget=".$atktarget,
                       $linkname,
                       SESSION_NESTED,
                       $this->m_autocomplete_saveform,'class="atkmanytoonerelation"');
      }
      return $result;
    }

    /**
     * Get the destination record for the edit action
     *
     * @param String $selname    The name that the selected variable uses in the URL
     * @param String $newlyadded The name that the selected variable uses in the postvars
     * @param Array $record      The original record
     * @param bool $editflag     The edit flag (?)
     * @return Array The destination record
     */
    function _getDestRecord($selname, $newlyadded, &$record, &$editflag)
    {
      global $ATK_VARS;
      if ($ATK_VARS[$selname]!="" || $newlyadded!="")
      // new record selected
      {
        // $ATK_VARS[$selname] contains full primary key of selected record.
        $recset = $this->m_destInstance->selectDb(($ATK_VARS[$selname]?$ATK_VARS[$selname]:$newlyadded),"","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));
        $destrecord = $recset[0];
        $record[$this->fieldName()] = $destrecord; // put destrecord inside original record.
      }
      elseif (is_array($record[$this->fieldName()]) && count($record[$this->fieldName()]) == count($this->m_destInstance->m_primaryKey))
      // old record present, but probably only primary key fields are loaded (this happens after pressing 'save' on a record)
      {
        $currentPk = $this->m_destInstance->primaryKey($record[$this->fieldName()]);
        $recset = $this->m_destInstance->selectDb($currentPk,"","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));
        $destrecord = $recset[0];
        $record[$this->fieldName()] = $destrecord; // put destrecord inside original record.
      }
      elseif (is_array($record[$this->fieldName()]) && !$this->isEmpty($record))
      // complete old record present
      {
        $destrecord = $record[$this->fieldName()];
      }
      else
      // no record yet, empty value.
      {
        $destrecord = array();
        $editflag = false;
      }
      return $destrecord;
    }

    /**
     * Creates and returns the auto edit/view links
     * @param String $id          The field id
     * @param String $filter      Filter that we want to apply on the destination node
     * @return array              The HTML code for the autolink links
     */
    function getRelationAutolink($id, $filter)
    {
      $autolink = array();
      if ($this->hasFlag(AF_RELATION_AUTOLINK)) // auto edit/view link
      {
        $page = &atkPage::getInstance();
        $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkmanytoonerelation.js");

        if ($this->m_destInstance->allowed("edit"))
        {
          $editlink = session_url("dispatch.php?atknodetype=".$this->m_destination."&atkaction=edit&atkselector=REPLACEME",SESSION_NESTED);
          $autolink['edit'] = "&nbsp;<a href='javascript:atkSubmit(mto_parse(\"".atkurlencode($editlink)."\", document.entryform.".$id.".value))'>".atktext('edit')."</a>";
        }
        if ($this->m_destInstance->allowed("add"))
        {
          $autolink['add'] = "&nbsp;".href("dispatch.php?atknodetype=".$this->m_destination."&atkaction=add&atkpkret=".$id."_ret".($filter!=""?"&atkfilter=".rawurlencode($filter):""),atktext("new"),SESSION_NESTED,true);
        }
      }
      return $autolink;
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="", $fieldprefix="")
    {
      if ($this->createDestination())
      {
        $currentPk = "";
        if (count($record[$this->fieldName()]))
        {
          if(is_array($record[$this->fieldName()]))
          {
            $currentPk = $this->m_destInstance->primaryKey($record[$this->fieldName()]);
          }
          else
          {
            $newrecord=array($this->m_destInstance->primaryKeyField()=>$record[$this->fieldName()]);
            $currentPk = $this->m_destInstance->primaryKey($newrecord);
          }
        }
      $result = '<input type="hidden" id="'.$fieldprefix.$this->formName().'" name="'.$fieldprefix.$this->formName().
      '" value="'.$currentPk.'">';
        return $result;
      }
      return "";
    }


    /**
     * Returns a piece of html code that can be used in a form to search
     * @param $record Record
     * @param $size Size of the editbox
     * @param $maxsize Maxsize of the editbox
     * @return Piece of html code that can  be used in a form to edit this
     */
    function search($record=array(), $extended=false, $fieldprefix="")
    {
      // ugly workaround in addToListArrayHeader we define the prefix with
      // _AE_ at the end, this is correct, unfortuanately the search field needs
      // it at the beginning to work properly, so we reverse it here
      if (substr($fieldprefix, -4) == "_AE_")
        $fieldprefix = "_AE_".substr($fieldprefix, 0, -4);

      if (!$this->hasFlag(AF_LARGE) && !$this->hasFlag(AF_RELATION_AUTOCOMPLETE))
      {
        if ($this->createDestination())
        {
          if ($this->m_destinationFilter!="")
          {
            $filter = $this->parseFilter($this->m_destinationFilter,$record);
            $this->m_destInstance->addFilter($filter);
          }

          $recordset = $this->_getSelectableRecords($record, 'search');

          $result = '<select class="'.get_class($this).'" ';
          if ($extended)
          {
            $result.='multiple size="'.min(5,count($recordset)+1).'"';
          }

          if (strstr($fieldprefix,"]["))
            $result.='name="atksearch['. $fieldprefix.$this->fieldName().'][]">';
          else
            $result.='name="atksearch'.$fieldprefix.'['. $this->fieldName().'][]">';

          $pkfield = $this->m_destInstance->primaryKeyField();

          $result.= '<option value="">'.atktext('search_all');
          if (!$this->hasFlag(AF_OBLIGATORY))
            $result.= '<option value="__NONE__"'.(isset($record[$this->fieldName()]) && atk_in_array('__NONE__', $record[$this->fieldName()]) ? ' selected="selected"' : '').'>'.atktext('search_none');

          for ($i=0;$i<count($recordset);$i++)
          {
            $pk = $recordset[$i][$pkfield];

            if (is_array($record)&&isset($record[$this->fieldName()])&&
                       atk_in_array($pk, $record[$this->fieldName()])) $sel = "selected"; else $sel = "";
            $result.= '<option value="'.$pk.'" '.$sel.'>'.$this->m_destInstance->descriptor($recordset[$i]);
          }
          $result.='</select>';
          return $result;
        }
        return "";
      }
      else
      {
        if (strstr($fieldprefix,"]["))
          $id ='atksearch['.$fieldprefix.$this->fieldName().']';
        else $id ='atksearch'.$fieldprefix.'['. $this->fieldName().']';

        $this->registerKeyListener($id, KB_CTRLCURSOR|KB_UPDOWN);
        $result = '<input type="text" id="'.$id.'" class="'.get_class($this).'" name="'.$id.'" value="'.$record[$this->fieldName()].'"'.
          ($this->hasFlag(AF_RELATION_AUTOCOMPLETE) ? ' onchange=""' : '').
          ($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').
          ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';

        if ($this->hasFlag(AF_RELATION_AUTOCOMPLETE))
        {
          $page = &$this->m_ownerInstance->getPage();
          $url = partial_url($this->m_ownerInstance->atkNodeType(), $this->m_ownerInstance->m_action, 'attribute.'.$this->fieldName().'.autocomplete_search');
          $code = "ATK.ManyToOneRelation.completeSearch('{$id}', '{$id}_result', '{$url}', {$this->m_autocomplete_minchars});";
          $this->m_ownerInstance->addStyle("atkmanytoonerelation.css");
          $page->register_script(atkconfig("atkroot").'atk/javascript/prototype/prototype.js');
          $page->register_script(atkconfig("atkroot").'atk/javascript/scriptaculous/scriptaculous.js');
          $page->register_script(atkconfig('atkroot').'atk/javascript/class.atkmanytoonerelation.js');
          $page->register_loadscript($code);
          $result .= '<div id="'.$id.'_result" style="display: none" class="atkmanytoonerelation_result"></div>';
        }

        return $result;
      }
    }

    /**
     * Retrieve the default searchmode of an attribute
     *
     * @param boolean $extended Whether extended search is being used
     * @return String the default searchmode for this attribute.
     */
    function getSearchMode($extended = false)
    {
      if ($this->hasFlag(AF_LARGE))
      {
        return parent::getSearchMode($extended);
      }
      return "exact"; // only support exact search when searching with dropdowns
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
     * Creates an search condition for a given search value
     * @param  atkQuery $query      query to hook ourselves in to
     * @param  string   $table      the table name
     * @param  string   $value      the search value
     * @param  string   $searchmode the searchmode for the search (e.g. substring)
     * @return string          a piece of the SQL where clause
     */
    function searchCondition(&$query, $table, $value, $searchmode, $fieldaliasprefix='')
    {
      $searchcondition = $this->getSearchCondition($query, $table, $fieldaliasprefix, $value, $searchmode);
      if (!empty($searchcondition))
        $query->addSearchCondition($searchcondition);
    }

    function getSearchCondition(&$query, $table, $fieldaliasprefix, $value, $searchmode)
    {
      if (!$this->createDestination()) return;

    	if (!$this->hasFlag(AF_LARGE) && !$this->hasFlag(AF_RELATION_AUTOCOMPLETE))
    	{
    		// We only support 'exact' matches.
    		// But you can select more than one value, which we search using the IN() statement,
    		// which should work in any ansi compatible database.
    		if (is_array($value)) // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.
    		{
  		    foreach ($this->m_listColumns as $attr)
  		    {
  		      $attrValue = $value[$attr];
  		      if (!empty($attrValue))
  		      {
  		        $p_attrib = &$this->m_destInstance->m_attribList[$attr];
  		        if (!$p_attrib == NULL)
  		          $p_attrib->searchCondition($query, $this->fieldName(), $attrValue, $searchmode);
  		      }
  		    }

  		    $value = isset($value[0]) ? $value : $value[$this->fieldName()];

  		    if (!is_array($value)) return '';

    			if (count($value)==1) // exactly one value
    			{
    			  if ($value[0] == "__NONE__")
    			  {
    			    return $query->nullCondition($table.".".$this->fieldName(), true);
    			  }
    			  elseif ($value[0] != "")
            {
              return $query->exactCondition($table.".".$this->fieldName(),$this->escapeSQL($value[0]));
            }
          }
          else // search for more values using IN()
          {
            return $table.".".$this->fieldName()." IN ('".implode("','",$value)."')";
          }
        }
        else
        {
          $cond= $query->exactCondition($table.".".$this->fieldName(),$this->escapeSQL($value));
          atkdebug($cond, "condition");
          return $cond;
        }
      }
      else // AF_LARGE || AF_RELATION_AUTOCOMPLETE
      {
        // ask the destination node for it's search condition
        $alias = $fieldaliasprefix . $this->fieldName();
        return $this->m_destInstance->getSearchCondition($query, $alias, $fieldaliasprefix, $value, $searchmode);
      }
    }

    /**
     * Function to add values to query
     * @param $query
     * @param $tablename Table name
     * @param $fieldaliasprefix Field alias prefix
     * @param $level Level
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
    {
      if ($this->hasFlag(AF_MANYTOONE_LAZY))
      {
        parent::addToQuery($query, $tablename, $fieldaliasprefix, $rec, $level, $mode);
        return;
      }

      if ($this->createDestination())
      {
        if ($mode != "update" && $mode != "add")
        {
          $alias = $fieldaliasprefix . $this->fieldName();
          $query->addJoin($this->m_destInstance->m_table,
                          $alias,
                          $this->getJoinCondition($query, $tablename, $alias),
                          $this->m_leftjoin);
          $this->m_destInstance->addToQuery($query, $alias, $level+1, false, $mode, $this->m_listColumns);
        }
        else
        {
          for ($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
          {
            if ($rec[$this->fieldName()]==NULL)
            {
              $query->addField($this->m_refKey[$i],"NULL","","",false);
            }
            else
            {
              $value = $rec[$this->fieldName()];
              if (is_array($value))
              {
                $fk = &$this->m_destInstance->getAttribute($this->m_destInstance->m_primaryKey[$i]);
                $value = $fk->value2db($value);
              }

              $query->addField($this->m_refKey[$i],$value,"","",!$this->hasFlag(AF_NO_QUOTES));
            }
          }
        }
      }
    }


    /**
     * Retrieve detail records from the database.
     *
     * Called by the framework to load the detail records.
     *
     * @param atkDb $db The database used by the node.
     * @param array $record The master record
     * @param String $mode The mode for loading (admin, select, copy, etc)
     *
     * @return array Recordset containing detailrecords, or NULL if no detail
     *               records are present. Note: when $mode is edit, this
     *               method will always return NULL. This is a framework
     *               optimization because in edit pages, the records are
     *               loaded on the fly.
     */
    function load(&$db, $record, $mode)
    {
      return $this->_getSelectedRecord($record, $mode);
    }

    /**
     * Determine the load type of this attribute.
     *
     * With this method, the attribute tells the framework whether it wants
     * to be loaded in the main query (addToQuery) or whether the attribute
     * has its own load() implementation.
     * For the atkOneToOneRelation, this depends on the presence of the
     * AF_ONETOONE_LAZY flag.
     *
     * Framework method. It should not be necesary to call this method
     * directly.
     *
     * @param String $mode The type of load (view,admin,edit etc)
     *
     * @return int Bitmask containing information about load requirements.
     *             POSTLOAD|ADDTOQUERY when AF_ONETOONE_LAZY is set.
     *             ADDTOQUERY when AF_ONETOONE_LAZY is not set.
     */
    function loadType($mode)
    {
      if ($this->hasFlag(AF_MANYTOONE_LAZY))
      {
        return POSTLOAD|ADDTOQUERY;
      }
      else
      {
        return ADDTOQUERY;
      }
    }

    /**
     * Validate if the record we are referring to really exists.
     *
     * @param array $record
     * @param string $mode
     */
    function validate(&$record, $mode)
    {
      if (!$this->_isSelectableRecord($record))
      {
        triggerError($record, $this->fieldName(), 'error_integrity_violation');
      }
    }

    function equal($recA, $recB)
    {
      if ($this->createDestination())
      {
        return (($recA[$this->fieldName()][$this->m_destInstance->primaryKeyField()]
                 ==
                 $recB[$this->fieldName()][$this->m_destInstance->primaryKeyField()])
               ||
                ($this->isEmpty($recA)&&$this->isEmpty($recB)));
             // we must also check empty values, because empty values need not necessarily
             // be equal (can be "", NULL or 0.
      }
      return false;
    }

    function dbFieldType()
    {
      // The type of field that we need to store the foreign key, is equal to
      // the type of field of the primary key of the node we have a
      // relationship with.
      if ($this->createDestination())
      {
        if(count($this->m_refKey)>1)
        {
          $keys = array();
          for($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
          {
            $keys [] = $this->m_destInstance->m_attribList[$this->m_destInstance->m_primaryKey[$i]]->dbFieldType();
          }
          return $keys;
        }
        else
          return $this->m_destInstance->m_attribList[$this->m_destInstance->primaryKeyField()]->dbFieldType();
      }
      return "";
    }

    function dbFieldSize()
    {
      // The size of the field we need to store the foreign key, is equal to
      // the size of the field of the primary key of the node we have a
      // relationship with.
      if ($this->createDestination())
      {
        if(count($this->m_refKey)>1)
        {
          $keys = array();
          for($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
          {
            $keys [] = $this->m_destInstance->m_attribList[$this->m_destInstance->m_primaryKey[$i]]->dbFieldSize();
          }
          return $keys;
        }
        else
          return $this->m_destInstance->m_attribList[$this->m_destInstance->primaryKeyField()]->dbFieldSize();
      }
      return 0;
    }

    /**
     * Returns the selected record for this many-to-one relation. Uses
     * the owner instance $this->fieldName()."_selected" method if it exists.
     */
    function _getSelectedRecord($record=array(), $mode="select")
    {
      $method = $this->fieldName()."_selected";
      if (method_exists($this->m_ownerInstance, $method))
        return $this->m_ownerInstance->$method($record, $mode);
      else return $this->getSelectedRecord($record, $mode);
    }

    /**
     * Returns the currently selected record.
     */
    function getSelectedRecord($record=array(), $mode="select")
    {
      $this->createDestination();
      $condition = $this->m_destInstance->m_table.'.'.$this->m_destInstance->primaryKeyField().
                   "='".$record[$this->fieldName()][$this->m_destInstance->primaryKeyField()]."'";
      list($record) = $this->m_destInstance->selectDb($condition, "", "", "", "", $mode);
      return $record;
    }

    /**
     * Returns the selectable records for this many-to-one relation. Uses
     * the owner instance $this->fieldName()."_selection" method if it exists.
     */
    function _getSelectableRecords($record=array(), $mode="select")
    {
      $method = $this->fieldName()."_selection";
      if (method_exists($this->m_ownerInstance, $method))
        return $this->m_ownerInstance->$method($record, $mode);
      else return $this->getSelectableRecords($record, $mode);
    }

    /**
     * Is selectable record? Uses the owner instance $this->fieldName()."_selectable"
     * method if it exists.
     */
    function _isSelectableRecord($record=array(), $mode="select")
    {
      $method = $this->fieldName()."_selectable";
      if (method_exists($this->m_ownerInstance, $method))
        return $this->m_ownerInstance->$method($record, $mode);
      else return $this->isSelectableRecord($record, $mode);
    }

    /**
     * Create the destination filter for the given record.
     *
     * @param array $record
     * @return string filter
     */
    function createFilter($record)
    {
      if ($this->m_destinationFilter != "")
      {
        atkimport("atk.utils.atkstringparser");
        $parser = new atkStringParser($this->m_destinationFilter);
        return $parser->parse($record);
      }
      else
      {
        return "";
      }
    }

    /**
     * Is selectable record?
     *
     * Use this one from your selectable override when needed.
     */
    function isSelectableRecord($record=array(), $mode="select")
    {
      if ($record[$this->fieldName()] == NULL) return false;

      $this->createDestination();

      $selectedKey = $this->m_destInstance->primaryKey($record[$this->fieldName()]);
      if ($selectedKey == NULL) return false;

      // If custom selection method exists we use this one, although this is
      // way more inefficient, so if you create a selection override you should
      // also think about creating a selectable override!
      $method = $this->fieldName()."_selection";
      if (method_exists($this->m_ownerInstance, $method))
      {
        $rows = $this->m_ownerInstance->$method($record, $mode);
        foreach ($rows as $row)
        {
          $key = $this->m_destInstance->primaryKey($row);
          if ($key == $selectedKey) return true;
        }

        return false;
      }

      // No selection override exists, simply add the record key to the selector.
      $filter = $this->createFilter($record);
      $selector = "($selectedKey)".($filter != NULL ? " AND ($filter)" : "");
      list($result) = $this->m_destInstance->selectDb($selector,false,"","",$this->m_destInstance->m_primaryKey,$mode);

      return $result != NULL;
    }

    /**
     * Returns the selectable records.
     *
     * Use this one from your selection override when needed.
     */
    function getSelectableRecords($record=array(), $mode="select")
    {
      $this->createDestination();

      $selector = $this->createFilter($record);
      $result = $this->m_destInstance->selectDb($selector,"","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey),$mode);

      return $result;
    }

    /**
     * If the primary key of one of the records is the current selected selector
     * we should only be able to select that record so we set selectablerecords to that record
     * and return true.
     *
     * @return bool Wether one of the records has the current selector or not
     */
    function _checkPrimaryKeysForSelector(&$recordset)
    {
      $sessionm = &atkGetSessionManager();
      $atkselector = $sessionm->stackVar('atkselector');
      if ($atkselector)
      {
        foreach ($recordset as $key=>$record)
        {
          if ($atkselector==$record['atkprimkey'])
          {
            $recordset = array($record);
            return true;
          }
        }
      }
      return false;
    }


   /**
    * Returns the condition (SQL) that should be used when we want to join a relation's
    * owner node with the parent node.
    * @param String $tablename=""  the tablename on which
    * @param String $fieldalias=""
    * @return String SQL string for joining the owner with the destination.
    *                Returns false when impossible (f.e. attrib is not a relation).
    */
    function getJoinCondition(&$query, $tablename="",$fieldalias="")
    {
      if (!$this->createDestination()) return false;

      if ($tablename!="") $realtablename=$tablename;
      else $realtablename = $this->m_ownerInstance->m_table;
      $joinconditions = array();

      for ($i=0, $_i=count($this->m_refKey); $i<$_i; $i++)
      {
        $joinconditions[] = $realtablename.".".$this->m_refKey[$i].
        "=".
        $fieldalias.".".$this->m_destInstance->m_primaryKey[$i];
      }

      if ($this->m_joinFilter!="")
      {
        atkimport('atk.utils.atkstringparser');
        $parser = new atkStringParser($this->m_joinFilter);
        $filter = $parser->parse(array('table' => $realtablename, 'owner' => $realtablename, 'destination' => $fieldalias));
        $joinconditions[] = $filter;
      }
      return implode(" AND ",$joinconditions);
    }

    /**
     * Make this relation hide itself from the form when there are no items to select
     * @param boolean $hidewhenempty true - hide when empty, false - always show
     */
    function setHideWhenEmpty($hidewhenempty)
    {
      $this->m_hidewhenempty = $hidewhenempty;
    }

    function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
      if ($this->createDestination())
      {
        // check if destination table is empty
        // only check if hidewhenempty is set to true
        if ($this->m_hidewhenempty)
        {
          $recs = $this->_getSelectableRecords($defaults, 'select');
          if (count($recs)==0) return $this->hide($defaults, $fieldprefix);
        }
      }
      return parent::addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);
    }

    function getOrderByStatement($extra='')
    {
      if (!$this->createDestination())
        return parent::getOrderByStatement();

      if (!empty($extra) && in_array($extra, $this->m_listColumns))
      {
        return $this->fieldName().".".$extra;
      }

      $order = $this->m_destInstance->getOrder();
      if (!empty($order))
      {
        // realias if destination order contains a tablename.
        if (strpos($order, $this->m_destInstance->m_table.'.') !== FALSE)
          $order = str_replace($this->m_destInstance->m_table.'.', $this->fieldName().'.', $order);
        else $order = $this->fieldName().".".$order;
        return $order;
      }
      else
      {
        $fields = $this->m_destInstance->descriptorFields();
        if (count($fields) == 0)
          $fields = array($this->m_destInstance->primaryKeyField());

        $order = "";
        foreach ($fields as $field)
          $order .= (empty($order) ? '' : ', ').$this->fieldName().".".$field;

        return $order;
      }
    }


    /**
     * Adds the attribute / field to the list header. This includes the column name and search field.
     *
     * Framework method. It should not be necessary to call this method directly.
     *
     * @param String $action the action that is being performed on the node
     * @param array  $arr reference to the the recordlist array
     * @param String $fieldprefix the fieldprefix
     * @param int    $flags the recordlist flags
     * @param array  $atksearch the current ATK search list (if not empty)
     * @param String $atkorderby the current ATK orderby string (if not empty)
     * @see atkNode::listArray
     */
    function addToListArrayHeader($action, &$arr, $fieldprefix, $flags, $atksearch, $atkorderby)
    {
      $prefix = $fieldprefix.$this->fieldName()."_AE_";

      // regular behaviour.
      parent::addToListArrayHeader($action, $arr, $prefix, $flags, $atksearch[$this->fieldName()], $atkorderby);

      // only add extra columns when needed
      if ($this->hasFlag(AF_HIDE_LIST) && !$this->m_alwaysShowListColumns) return;
      if (!$this->createDestination() || count($this->m_listColumns) == 0) return;

      foreach ($this->m_listColumns as $attribname)
      {
        $p_attrib = &$this->m_destInstance->getAttribute($attribname);
        $p_attrib->m_flags |= AF_HIDE_LIST;
        $p_attrib->m_flags ^= AF_HIDE_LIST;
        $p_attrib->addToListArrayHeader($action, $arr, $prefix, $flags, $atksearch[$this->fieldName()], $atkorderby);

        $needle = $prefix.$attribname;
        foreach (array_keys($arr['heading']) as $key)
        {
          // fix order by clause
          if (strpos($key, $needle) === 0)
          {
            $order = $arr['heading'][$key]['order'];
            if (!empty($order))
            {
              $order = $this->fieldName().'.'.$order;

              if (is_object($atkorderby) &&
                  isset($atkorderby->m_colcfg[$this->fieldName()]) &&
                  $atkorderby->m_colcfg[$this->fieldName()]['extra'] == $attribname)
              {
                $direction = $atkorderby->getDirection($this->fieldName());
                if ($direction=="asc") $order.=" desc";
              }

              $arr['heading'][$key]['order'] = $order;
            }
          }

          // fix search box
          $fieldPointer = &$arr["search"][$key];
          $fieldPointer =
            str_replace(
              "atksearch[".$p_attrib->fieldName()."]",
              "atksearch_".$prefix."[" . $p_attrib->fieldName()."]",
              $fieldPointer
            );
        }
      }
    }

    /**
     * Adds the attribute / field to the list row. And if the row is totalisable also to the total.
     *
     * Framework method. It should not be necessary to call this method directly.
     *
     * @param String $action the action that is being performed on the node
     * @param array  $arr reference to the the recordlist array
     * @param int    $nr the current row number
     * @param String $fieldprefix the fieldprefix
     * @param int    $flags the recordlist flags
     * @see atkNode::listArray
     */
    function addToListArrayRow($action, &$arr, $nr, $fieldprefix, $flags)
    {
      $prefix = $fieldprefix.$this->fieldName()."_AE_";

      parent::addToListArrayRow($action, $arr, $nr, $prefix, $flags);

      // only add extra columns when needed
      if ($this->hasFlag(AF_HIDE_LIST) && !$this->m_alwaysShowListColumns) return;
      if (!$this->createDestination() || count($this->m_listColumns) == 0) return;

      // small trick, the destination record is in a subarray. The destination
      // addToListArrayRow will not expect this though, so we have to modify the
      // record a bit before passing it to the detail columns.
      $backup = $arr["rows"][$nr]["record"];
      $arr["rows"][$nr]["record"] = $arr["rows"][$nr]["record"][$this->fieldName()];

      foreach ($this->m_listColumns as $attribname)
      {
        $p_attrib = &$this->m_destInstance->getAttribute($attribname);
        $p_attrib->m_flags |= AF_HIDE_LIST;
        $p_attrib->m_flags ^= AF_HIDE_LIST;
        $p_attrib->addToListArrayRow($action, $arr, $nr, $prefix, $flags);
      }

      $arr["rows"][$nr]["record"] = $backup;
    }


    /**
     * Overridden method; in the integrated version, we should let the destination
     * attributes hook themselves into the fieldlist instead of hooking the relation
     * in it.
     * For original documentation for this method, please see the atkAttribute class
     */
    function addToSearchformFields(&$fields, &$node, &$record, $fieldprefix = "")
    {
      $prefix = "_AE_".$fieldprefix.$this->fieldName();

      parent::addToSearchformFields($fields,$node,$record, $prefix);

      // only add extra columns when needed
      if ($this->hasFlag(AF_HIDE_LIST) && !$this->m_alwaysShowListColumns) return;
      if (!$this->createDestination() || count($this->m_listColumns) == 0) return;

      foreach ($this->m_listColumns as $attribname)
      {
        $p_attrib = &$this->m_destInstance->m_attribList[$attribname];
        $p_attrib->m_flags |= AF_HIDE_LIST;
        $p_attrib->m_flags ^= AF_HIDE_LIST;

        if (!$p_attrib->hasFlag(AF_HIDE_SEARCH))
        {
          // TODO: This is an ugly way to make sure date attributes are changed as well
          //       Furthermore it does nothing to save the javascript functionality
          //       (which completely breaks) in searching a onetoone something like a 'searchname' should
          //       be implemented and used.
          if (method_exists($p_attrib, "atkDateAttribute"))
            $p_attrib->addToSearchformFields($fields,$node,$record[$this->fieldName()], "{$prefix}_AE_");
          else
            $p_attrib->addToSearchformFields($fields,$node,$record[$this->fieldName()], $prefix);
        }
      }
    }

    function listHeaderSortOrder(&$columnConfig)
    {
      $order = $this->fieldName();

      // only add desc if not one of the listColumns is used for the sorting
      if (isset($columnConfig->m_colcfg[$order]) && empty($columnConfig->m_colcfg[$order]['extra']))
      {
        $direction = $columnConfig->getDirection($order);
        if ($direction=="asc") $order.=" desc";
      }

      return $order;
    }

    /**
     * Auto-complete partial.
     *
     * @param $mode add/edit mode?
     */
    function partial_autocomplete($mode)
    {
      $searchvalue = $this->m_ownerInstance->m_postvars['value'];
      if (strlen($searchvalue) < $this->m_autocomplete_minchars)
      {
        return '<ul><li class="minimum_chars">'.sprintf($this->text('autocomplete_minimum_chars'), $this->m_autocomplete_minchars).'</li></ul>';
      }

      $this->createDestination();

      $fieldprefix = $this->m_ownerInstance->m_postvars['atkfieldprefix'];
      $searchvalue = $this->escapeSQL($searchvalue);
      $record = $this->m_ownerInstance->updateRecord();

      $filter = $this->createSearchFilter($searchvalue);
      $this->addDestinationFilter($filter);

      $records = $this->_getSelectableRecords($record, 'select');

      if (count($records) == 0)
      {
        if(in_array($this->m_autocomplete_searchmode,array("exact","startswith","contains")))
          $str = $this->text('autocomplete_no_results_'.$this->m_autocomplete_searchmode);
        else
          $str = $this->text('autocomplete_no_results');

        return '<ul><li class="no_results">'.$str.'</li></ul>';
      }

      $result = '';
      foreach ($records as $rec)
      {
        $option = $this->m_destInstance->descriptor($rec);
        $value = $this->m_destInstance->primaryKey($rec);
        $result .= '
          <li title="'.atk_htmlentities($option).'">
            '.atk_htmlentities($option).'
            <span class="selection" style="display: none">'.atk_htmlentities($option).'</span>
            <span class="value" style="display: none">'.$value.'</span>
          </li>';
      }

      return "<ul>$result</ul>";
    }

    /**
     * Auto-complete search partial.
     */
    function partial_autocomplete_search()
    {
      $this->createDestination();

      $searchvalue = $this->m_ownerInstance->m_postvars['value'];
      $searchvalue = $this->escapeSQL($searchvalue);
      $filter = $this->createSearchFilter($searchvalue);
      $this->addDestinationFilter($filter);

      $record = array();
      $records = $this->_getSelectableRecords($record, 'search');

      $result = '';
      foreach ($records as $rec)
      {
        $option = $this->m_destInstance->descriptor($rec);
        $value = $this->m_destInstance->primaryKey($rec);
        $result .= '
          <li title="'.htmlentities($option).'">'.htmlentities($option).'</li>';
      }

      return "<ul>$result</ul>";
    }

    /**
     * Creates a search filter with the given search value on the given
     * descriptor fields
     *
     * @param String $searchvalue A searchstring
     * @return String a search string (WHERE clause)
     */
    function createSearchFilter($searchvalue)
    {
      if($this->m_autocomplete_searchfields=="")
        $searchfields = $this->m_destInstance->descriptorFields();
      else
        $searchfields = $this->m_autocomplete_searchfields;

      $parts = preg_split('/\s+/', $searchvalue);

      $mainFilter = array();
      foreach ($parts as $part)
      {
        $filter = array();
        foreach($searchfields as $attribname)
        {
          if(!$this->m_autocomplete_search_case_sensitive)
            $tmp = "LOWER(".$this->m_destInstance->m_table.".".$attribname.")";
          else
            $tmp = $this->m_destInstance->m_table.".".$attribname;

          switch($this->m_autocomplete_searchmode)
          {
            case "exact":
              if(!$this->m_autocomplete_search_case_sensitive)
                $tmp.= " = LOWER('{$part}')";
              else
                $tmp.= " = '{$part}'";
              break;
            case "startswith":
              if(!$this->m_autocomplete_search_case_sensitive)
                $tmp.= " LIKE LOWER('{$part}%')";
              else
                $tmp.= " LIKE '{$part}%'";
              break;
            case "contains":
              if(!$this->m_autocomplete_search_case_sensitive)
                $tmp.= " LIKE LOWER('%{$part}%')";
              else
                $tmp.= " LIKE '%{$part}%'";
              break;
            default:
              $tmp.= " = LOWER('{$part}')";
          }

          $filter[] = $tmp;
        }

        if (count($filter) > 0)
          $mainFilter[] = "(".implode(") OR (", $filter).")";
      }

      if (count($mainFilter) > 0)
        return "(".implode(") AND (", $mainFilter).")";
      else return "";
    }
  }
?>
