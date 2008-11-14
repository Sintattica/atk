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
 * @copyright (c)2007 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * Imports
 * @access private
 */
atkimport('atk.utils.atkjson');
atkimport('atk.relations.controls.atkshuttlecontrol');
atkimport('atk.relations.controls.atkshuttlefilter');

/**
 * Shuttle relation with widget extensions.
 * @author Tjeerd Bijlsma <tjeerd@ibuildings.nl>
 * @package atk
 * @subpackage relations
 */
class atkExtendableShuttleRelation extends atkManyToManyRelation
{
  protected $m_controlsBySection = array();
  protected $m_selectedFields = null;
  protected $m_availableFields = null;
  protected $m_limit = null;
  protected $m_descriptor_tooltip_template = null;

  public function __construct($name, $link, $destination, $flags=0)
  {
    parent::__construct($name, $link, $destination, $flags);
    $this->m_controlsBySection[atkShuttleControl::AVAILABLE] = array();
    $this->m_controlsBySection[atkShuttleControl::SELECTED] = array();
  }

  public function addControl($control, $section)
  {
    $control->setSection($section);
    $control->setShuttle($this);
    $this->m_controlsBySection[$section][$control->getName()] = $control;
    $control->init();
  }

  /**
   * Re-renders the section contents based on the current filter values.
   *
   * Using $this->getOwnerInstance()->updateRecord() the current "record" can
   * be retrieved. $record[$this->fieldName()] contains the following entries:
   *
   * - "section" => section ("available" or "selected")
   * - "controls" => control values (see atkShuttleControl::getValue)
   * - "selected" => currently selected records (keys)
   * - "available" => currently available records (keys) (should not be used by this method!)
   */
  public function partial_filter()
  {
    $redraw = false;
    $record = $this->getOwnerInstance()->updateRecord();
    $mode   = $ATK_VARS["atkaction"];
    $prefix = $this->getOwnerInstance()->m_postvars['atkfieldprefix'];
    
    foreach ($this->m_controlsBySection[$record[$this->fieldName()]["section"]] as $control)
    {
      if (is_a($control, 'atkShuttleFilter'))
      {
        $redraw = true;

        $filter = $control->getFilter($record);
        if (!empty($filter))
        {
          // add filter immediately because we are only refreshing a single section
          $this->createDestination();          
          $this->getDestination()->addFilter($filter);
        } 
        
        $limit = $control->getLimit();
        if ($limit!==null) $this->m_limit = $limit;
      }
    }

    $res = "<script language=\"text/javascript\">";
    foreach ($this->m_controlsBySection[$record[$this->fieldName()]["section"]] as $control)
    {
      if ($control->needsRefresh('filter', $record))
      {
        $res .= "$('".$control->getFormName($prefix)."').innerHTML = ".atkJSON::encode($control->render($record, $mode, $prefix)).";";
      }
    }

    if ($redraw)
    {
      $res .= "$('".$this->getHtmlId($prefix)."_".$record[$this->fieldName()]["section"]."').innerHTML = ".atkJSON::encode($this->renderSelectBoxes($record[$this->fieldName()]["section"],$record,$mode,$prefix)).";";
    }
    $res .= "</script>";

    return $res;
  }

  private function renderSelectBoxes($side,$record,$mode,$prefix)
  {
    $this->loadFieldData($record, $mode);
    
    if ($side=='available')
    {
      $rs = $this->getAvailableFields($record, $mode);
      $name  = $this->getAvailableSelectName($prefix);
      $opp   = $this->getSelectedSelectName($prefix);
      $sel   = 0;
    }
    else
    {
      $rs = $this->getSelectedFields($record);
      $opp   = $this->getAvailableSelectName($prefix);
      $name  = $this->getSelectedSelectName($prefix);
      $sel   = 1;
    }

    return $this->_renderSelect($name,$rs,$opp,$prefix,$sel);
  }
  
  public function getSelectedSelectName($prefix)
  {
    return $this->getHtmlId($prefix).'[cselected][]['.$this->getRemoteKey().']';
  }
  
  public function getAvailableSelectName($prefix)
  {
    return $this->getHtmlId($prefix)."[available]";
  }

  /**
   * A new selection has been made. Allows some controls to re-render
   * themselves based on the new selection.
   *
   * Using $this->getOwnerInstance()->updateRecord() the current "record" can
   * be retrieved. $record[$this->fieldName()] contains the following entries:
   *
   * - "action" => "add" or "delete"
   * - "item" => added or deleted record (key)
   * - "controls" => control values (see atkShuttleControl::getValue)
   * - "selected" => currently selected records (keys)
   * - "available" => currently available records (keys)
   */
  public function partial_selection()
  {
    $record = $this->getOwnerInstance()->updateRecord();
    $mode   = $ATK_VARS["atkaction"];
    $prefix = $this->getOwnerInstance()->m_postvars['atkfieldprefix'];


    $res = "<script language=\"text/javascript\">";
    foreach ($this->m_controlsBySection[atkShuttleControl::AVAILABLE] as $control)
    {
      if ($control->needsRefresh('selection', $record))
      {
        $res .= "$('".$control->getFormName($prefix)."').innerHTML = ".atkJSON::encode($control->render($record, $mode, $prefix)).";";
      }
    }
    foreach ($this->m_controlsBySection[atkShuttleControl::SELECTED] as $control)
    {
      if ($control->needsRefresh('selection', $record))
      {
        $res .= "$('".$control->getFormName($prefix)."').innerHTML = ".atkJSON::encode($control->render($record, $mode, $prefix)).";";
      }
    }
    $res .= "</script>";

    return $res;
  }

  public function edit($record, $fieldprefix, $mode)
  {
    // Add onchange handler
    $mode == "add" ? "add" : "edit";
    $url = addslashes(partial_url($this->m_ownerInstance->atkNodeType(), $mode, "attribute.".$this->getHtmlId($fieldprefix).".selection", array("atkfieldprefix" => $fieldprefix)));
    $this->addOnChangeHandler("shuttle_refresh('$url', '".$this->getHtmlId($fieldprefix).'[cselected][]['.$this->getRemoteKey().']'."', '".$prefix.$this->fieldName()."[section]', el);");
    $this->_renderChangeHandler($fieldprefix);
    
    $filtersBySection = array();
    foreach (array(atkShuttleControl::AVAILABLE, atkShuttleControl::SELECTED) as $section)
    {
      foreach ($this->m_controlsBySection[$section] as $control)
      {
        if (is_a($control, 'atkShuttleFilter'))
        {
          $filter = $control->getFilter($record);
          if (!empty($filter)) 
          {
            $filtersBySection[$section][] = $filter;
          }
          
          $limit = $control->getLimit();
          if ($limit!==null) $this->m_limit = $limit;
        }
      }
    }   

    $availableFilter = '';
    if (count($filtersBySection[atkShuttleControl::AVAILABLE]) > 0)
      $availableFilter = '('.implode(') AND (', $filtersBySection[atkShuttleControl::AVAILABLE]).')';

    $selectedFilter = '';
    if (count($filtersBySection[atkShuttleControl::SELECTED]) > 0)
      $selectedFilter = '('.implode(') AND (', $filtersBySection[atkShuttleControl::SELECTED]).')';
      
    $this->loadFieldData($record, $mode, $availableFilter, $selectedFilter);
    
    // Get controls for 'available' side
    foreach ($this->m_controlsBySection[atkShuttleControl::AVAILABLE] as $control)
    {
      $ava_controls[] = $control->render($record, $mode, $fieldprefix);
    }

    // Get controls for 'selected' side
    foreach ($this->m_controlsBySection[atkShuttleControl::SELECTED] as $control)
    {
      $sel_controls[] = $control->render($record, $mode, $fieldprefix);
    }

    // Get selected records
    $right = $this->getSelectedFields($record);
    for($i=0, $_i=count($right); $i<$_i; $i++)
    {
      $selected_options[$right[$i][$this->m_destInstance->primaryKeyField()]] = $this->m_destInstance->descriptor($right[$i]);
    }

    // Get available records
    $left  = $this->getAvailableFields($record, $mode);
    for($i=0, $_i=count($left); $i<$_i; $i++)
    {
      $available_options[$left[$i][$this->m_destInstance->primaryKeyField()]] = $this->m_destInstance->descriptor($left[$i]);
    }

    $leftname  = $this->getHtmlId($fieldprefix)."[available]";
    $rightname = $this->getHtmlId($fieldprefix).'[cselected][]['.$this->getRemoteKey().']';
    $name      = $this->getHtmlId($fieldprefix).'[selected][]['.$this->getRemoteKey().']';

    // Build jsonned value for selected fields
    foreach ($right as $fld)
    {
      $vals[] = $fld[$this->m_destInstance->primaryKeyField()];
    }
    $value = atkJSON::encode($vals);
    if ($value=="null") $value="[]";

    // on submit, we must select all items in the right selector, as unselected items will not be posted.
    $page = &$this->m_ownerInstance->getPage();
    $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkextendableshuttlerelation.js");
    $page->register_submitscript("shuttle_selectAll('".$rightname."');");

    $ui = ATKUI::getInstance();
    $result = $ui->render('extendableshuttle.tpl',array(
                                'leftname'=>$leftname,
                                'rightname'=>$rightname,
                                'name'=>$name,
                                'htmlid'=>$this->getHtmlId($fieldprefix),
                                'remotekey'=>$this->getRemoteKey(),
                                'value'=>$value,
                                'ava_controls'=>$ava_controls,
                                'sel_controls'=>$sel_controls,
                                'available_options'=>$available_options,
                                'selected_options'=>$selected_options,
    ));

    return $result;
  }

  public function load($notused, $record)
  {
    $res = parent::load($notused, $record);
    $ret['selected'] = $res;
    return $ret;
  }

  public function fetchValue($postvars)
  {
    $ret = array();
    $vals = atkJSON::decode($postvars[$this->fieldName()]['selected'][0][$this->getRemoteKey()],true);
    if (is_array($vals))
    {
      foreach ($vals as $val)
      {
        $ret[][$this->getRemoteKey()] = $val;
      }
    }
    $postvars[$this->fieldName()]['selected'] = $ret;
    return $postvars[$this->fieldName()];
  }

  public function store($notused, &$record, $mode)
  {
    $rec = $record[$this->fieldName()];
    $record[$this->fieldName()] = $record[$this->fieldName()]['selected'];
    $res = parent::store($notused, $record, $mode);
    $record[$this->fieldName()]["selected"] = $rec["selected"];
    return $res;
  }

  public function display($record, $mode="")
  {
    $record[$this->fieldName()] = $record[$this->fieldName()]['selected'];
    return parent::display($record,$mode);
  }

  /**
   * Render the multiselect list control
   * @access private
   * @param String $name The name of the list control
   * @param array $recordset The list of records to render in the control
   * @param String $opposite The name of the list control connected to this list control for shuttle actions
   * @param String $prefix The prefix which is needed for determining the correct JS name
   * @param bool $isSelected Whether or not this is the selectbox with the selectedItems (needed for onchangecode)
   * @return String piece of html code
   */
  private function _renderSelect($name, $recordset, $opposite, $prefix, $isSelected)
  {
    if ($isSelected) {
      $onchangecode = $this->getHtmlId($prefix).'_onChange(\'selected\');';
      $action = "del";
    } else {
      $onchangecode = $this->getHtmlId($prefix).'_onChange(\'available\');';
      $action = "add";
    }

    $valName = $this->getHtmlId($fieldprefix).'[selected][]['.$this->getRemoteKey().']';
    $result = '<select class="shuttle_select" id="'.$name.'" name="'.$name.'" multiple size="10" onDblClick="shuttle_move(\''.$name.'\', \''.$opposite.'\',\''.$action.'\',\''.$valName.'\');'.$onchangecode.'">';
    
    $parser = null;
    // Only import the stringparser once.
    if (isset($this->m_descriptor_tooltip_template)) 
    {
      atkimport("atk.utils.atkstringparser");
      $parser = new atkStringParser($this->m_descriptor_tooltip_template);
    }
    
    for($i=0, $_i=count($recordset); $i<$_i; $i++)
    {
      $title = $this->m_destInstance->descriptor($recordset[$i]);
      $ttip = isset($this->m_descriptor_tooltip_template) ? $parser->parse($recordset[$i]) : $title; 
      
      $ttip = str_replace('\r\n',' ',strip_tags($ttip));
      
      $result.= '<option value="'.$recordset[$i][$this->m_destInstance->primaryKeyField()].'" title="'.$ttip.'">'.atk_htmlentities($title).'</option>';
    }
    $result.= '</select>';
    return $result;
  }
  
  public function setDescriptorTooltipTemplate($template)
  { 
    $this->m_descriptor_tooltip_template = $template;
  }

  /**
   * Get array with all selected fields from record
   * @access private
   * @param array $record The record with the currently selected fields
   * @return array available records
   */
  public function getSelectedFieldsFromRecord($record)
  {
    $selectedPk = array();
    $this->createLink();
    $this->createDestination();

    if (isset($record[$this->m_name]["selected"]) && is_array($record[$this->m_name]["selected"]))
    {
      foreach ($record[$this->m_name]["selected"] as $rec)
      {
        if(is_array($rec[$this->getRemoteKey()]))
          $selectedPk[] = $this->m_destInstance->primaryKey($rec[$this->getRemoteKey()]);
        else
          $selectedPk[] = $this->m_destInstance->primaryKey(array($this->m_destInstance->primaryKeyField()=>$rec[$this->getRemoteKey()]));
      }
    }

    return $selectedPk;
  }
      
  /**
   * A small performance boost method to load the available- and selectedfields
   * into two arrays. This prevents the foreach loops in both the getSelectedFields()
   * and in getAvailableFields() to be executed.
   *
   * @param Array $record The record
   * @param String $mode The current mode.
   */
  private function loadFieldData($record, $mode, $availableFilter='', $selectedFilter='')
  {        
    $selectedFields = $this->getSelectedFieldsFromRecord($record);    
    
    // available fields
    if (!empty($availableFilter))
    {
      $this->getDestination()->addFilter($availableFilter);  
      $recs = $this->_getSelectableRecords($record, $mode, true);
      $this->getDestination()->removeFilter($availableFilter);
    }
    else 
    {
      $recs = $this->_getSelectableRecords($record, $mode, true);
    }

    $this->m_availableFields = array();    
    foreach ($recs as $rec)
    {
      if (!in_array($this->m_destInstance->primaryKey($rec),$selectedFields))
      {
        $this->m_availableFields[] = $rec;
      }
    }  

    // selected fields
    if ($availableFilter != $selectedFilter)
    {
      if (count($selectedFields)>0)
      {
        if (empty($selectedFilter))
          $selectedFilter = " (".implode(') OR (',$selectedFields).")  "; 

        $this->getDestination()->addFilter($selectedFilter);
        $recs = $this->_getSelectableRecords($record, $mode, true);
        $this->getDestination()->removeFilter($selectedFilter);
      }
      else
      {
        $recs = array();
      }
    }
    $this->m_selectedFields = array();    
    foreach ($recs as $rec)
    {
      if (in_array($this->m_destInstance->primaryKey($rec),$selectedFields))
      {
        $this->m_selectedFields[] = $rec;
      }
    }
  }

  /**
   * Get array with all selected fields
   * @param array $record The record with the currently selected fields
   * @param String $mode for which mode we are rendering
   * @return array selected records
   */
  public function getSelectedFields($record, $mode='add')
  {
    if ($this->m_selectedFields === null)
    {
      $this->loadFieldData($record, $mode);
    }
    
    return $this->m_selectedFields;
  }

  /**
   * Get array with all available fields (which are not already selected)
   * @param array $record The record with the currently selected fields
   * @param String $mode for which mode we are rendering
   * @return array available records
   */
  public function getAvailableFields($record, $mode='add')
  {
    if ($this->m_availableFields === null)
    {
      $this->loadFieldData($record, $mode);
    }
            
    return $this->m_availableFields;
  }
  
  /**
   * Returns the selected field count.
   *
   * @param array $record record
   * 
   * @return int selected field count
   */
  public function getSelectedFieldCount($record)
  {
    $selectedFields = $this->getSelectedFieldsFromRecord($record);
    return count($selectedFields);
  }
  
  /**
   * Returns the available field count.
   *
   * @param array $record record
   * 
   * @return int available field count
   */  
  public function getAvailableFieldCount($record, $mode='add')
  {
    $count = $this->_getSelectableRecordCount($record, $mode);
    return $count;
  }
}