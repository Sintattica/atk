<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Db\Db;
use Sintattica\Atk\RecordList\CustomRecordList;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\RecordList\RecordList;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Ui\Ui;

/**
 * Handler for the 'export' action of a node. The export action is a
 * generic tool for exporting table data into CSV files.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ExportHandler extends ActionHandler
{
    /**
     * The action handler.
     */
    public function action_export()
    {
        global $ATK_VARS;

        // Intercept partial call
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);

            return;
        }

        // Intercept delete call
        if (array_key_exists('dodelete', $this->m_postvars) && ctype_digit($this->m_postvars['dodelete'])) {
            if (array_key_exists('confirmed', $this->m_postvars) && $this->m_postvars['confirmed'] == 'true') {
                $this->deleteSelection($this->m_postvars['dodelete']);
            }
        }

        //need to keep the postdata after a Attribute::AF_LARGE selection in the allfield
        if (!isset($this->m_postvars['phase']) && isset($ATK_VARS['atkformdata'])) {
            foreach ($ATK_VARS['atkformdata'] as $key => $value) {
                $this->m_postvars[$key] = $value;
            }
        }

        //need to keep the selected item after an exporterror
        $phase = Tools::atkArrayNvl($this->m_postvars, 'phase', 'init');
        if (!in_array($phase, array('init', 'process'))) {
            $phase = 'init';
        }

        switch ($phase) {
            case 'init':
                $this->doInit();
                break;
            case 'process':
                $this->doProcess();
                break;
        }

        return true;
    }

    /**
     * This function shows a form to configure the .csv.
     */
    public function doInit()
    {
        $content = $this->_getInitHtml();
        $page = $this->getPage();

        $page->register_scriptcode("
        function toggleSelectionName( fieldval )
        {
          if( fieldval == undefined )
          {
            fieldval = $( 'export_selection_options' ).value;
          }
          new Ajax.Updater('export_attributes', '".Tools::partial_url($this->m_postvars['atknodeuri'], 'export', 'export')."exportvalue='+fieldval+'&' );

          if( fieldval != 'none' )
          {
            if( fieldval != 'new' )
            {
              new Ajax.Updater('selection_interact', '".Tools::partial_url($this->m_postvars['atknodeuri'], 'export', 'selection_interact')."exportvalue='+fieldval+'&' );
              new Ajax.Updater('export_name', '".Tools::partial_url($this->m_postvars['atknodeuri'], 'export', 'selection_name')."exportvalue='+fieldval+'&' );
              $( 'selection_interact' ).style.display='';
              $( 'export_name' ).style.display='';
              $( 'export_save_button' ).style.display='';
            }
            else
            {
              $( 'selection_interact' ).style.display='none';
              $( 'export_name' ).style.display='';
              $( 'export_selection_name' ).value='';
              $( 'export_selection_options' ).selectedIndex=0;
              $( 'export_save_button' ).style.display='none';
            }
          }
          else
          {
            $( 'export_name' ).style.display='none';
            $( 'selection_interact' ).style.display='none';
            $( 'export_save_button' ).style.display='none';
            $( 'export_selection_name' ).value='';
          }
        }");

        $page->register_scriptcode("
        function confirm_delete()
        {
         var where_to = confirm('".Tools::atktext('confirm_delete')."');
         var dodelete = $( 'export_selection_options' ).value;

         if (where_to == true)
         {
           window.location= \"".Tools::dispatch_url($this->m_postvars['atknodeuri'], 'export', array('confirmed' => 'true')).'&dodelete="+dodelete;
         }
        }');

        $params = [];
        $params['title'] = $this->m_node->actionTitle('export');
        $params['content'] = $content;
        $content = $this->getUi()->renderBox($params);
        $output = $this->m_node->renderActionPage('export', $content);
        $page->addContent($output);

        return true;
    }

    /**
     * Handle partial request.
     *
     * @return string
     */
    public function partial_export()
    {
        $value = array_key_exists('exportvalue', $this->m_postvars) ? $this->m_postvars['exportvalue'] : null;

        return $this->getAttributeSelect($value);
    }

    /**
     * Partial fetches and displays the name of the selected value.
     *
     * @return string
     */
    public function partial_selection_name()
    {
        $selected = array_key_exists('exportvalue', $this->m_postvars) ? $this->m_postvars['exportvalue'] : null;
        $value = '';

        if ($selected) {
            $db = Db::getInstance();
            $rows = $db->getRows('SELECT name FROM atk_exportcriteria WHERE id = :id', [':id' => $selected]);
            if (Tools::count($rows) == 1) {
                $value = htmlentities($rows[0]['name']);
            }
        }

        return '<td>'.Tools::atktext('export_selections_name',
            'atk').': </td><td align="left"><input type="text" size="40" name="export_selection_name" id="export_selection_name" value="'.$value.'"></td>
              <input type="hidden" name="exportvalue" value="'.$this->m_postvars['exportvalue'].'" />';
    }

    /**
     * Partial displays a interaction possibilities with an export selection.
     *
     * @return string
     */
    public function partial_selection_interact()
    {
        $selected = array_key_exists('exportvalue', $this->m_postvars) ? $this->m_postvars['exportvalue'] : null;

        $url_delete = Tools::dispatch_url($this->m_node->m_module.'.'.$this->m_node->m_type, 'export', array('dodelete' => $selected));

        if ($selected) {
            $result = '<a href="'.$url_delete.'" title="'.Tools::atktext('delete_selection').'" onclick="confirm_delete();">'.Tools::atktext('delete_selection').'</a>';

            return $result;
        }
    }

    /**
     * Gets the HTML for the initial mode of the exporthandler.
     *
     * @return string The HTML for the screen
     */
    public function _getInitHtml()
    {
        $action = Tools::dispatch_url($this->m_node->m_module.'.'.$this->m_node->m_type, 'export');
        $sm = SessionManager::getInstance();

        $params = [];
        $params['formstart'] = '<form id="entryform" name="entryform" enctype="multipart/form-data" action="'.$action.'" method="post" class="form-horizontal">';
        $params['formstart'] .= $sm->formState();
        $params['formstart'] .= '<input type="hidden" name="phase" value="process"/>';
        if ($sm->atkLevel() > 0) {
            $params['buttons'][] = Tools::atkButton(Tools::atktext('cancel', 'atk'), '', SessionManager::SESSION_BACK);
        }
        $params['buttons'][] = '<input class="btn btn-primary" type="submit" value="'.Tools::atktext('export', 'atk').'"/>';
        $params['buttons'][] = '<input id="export_save_button" style="display:none;" value="'.Tools::atktext('save_export_selection',
                'atk').'" name="save_export" class="btn" type="submit" /> ';
        $params['content'] = '<b>'.Tools::atktext('export_config_explanation', 'atk', $this->m_node->m_type).'</b><br/><br/>';
        $params['content'] .= $this->_getOptions();
        $params['formend'] = '</form>';

        return Ui::getInstance()->renderAction('export', $params, $this->m_node->m_module);
    }

    /**
     * This function checks if there is enough information to export the date
     * else it wil shows a form to set how the file wil be exported.
     */
    public function doProcess()
    {
        // Update selection
        if (array_key_exists('exportvalue', $this->m_postvars) && array_key_exists('save_export',
                $this->m_postvars) && '' != $this->m_postvars['export_selection_name']
        ) {
            $this->updateSelection();
            $this->getNode()->redirect(Tools::dispatch_url($this->getNode(), 'export'));
        }

        // Save selection
        if (array_key_exists('export_selection_options', $this->m_postvars) && array_key_exists('export_selection_name',
                $this->m_postvars) && 'none' == $this->m_postvars['export_selection_options'] && '' != $this->m_postvars['export_selection_name']
        ) {
            $this->saveSelection();
        }

        // Export CVS
        if (!array_key_exists('save_export', $this->m_postvars)) {
            return $this->doExport();
        }
    }

    private function _getOptionsFormRow($rowAttributes, $label, $field) {
        $content = '';

        $content .= '<div class="row form-group"';
        if($rowAttributes) {
            foreach($rowAttributes as $k => $v) {
                $content .= ' '.$k.'="'.$v.'"';
            }
        }
        $content .= '>';

        $content .= '  <label class="col-sm-2 control-label">'.$label.'</label>';
        $content .= '  <div class="col-sm-10">'.$field.'</div>';
        $content .= '</div>';
        return $content;
    }

    /**
     * Get the options for the export.
     *
     * @return string html
     */
    public function _getOptions()
    {

        $content = '';

        // enable extended export options
        if (true === Config::getGlobal('enable_export_save_selection')) {
            $content .= $this->_getOptionsFormRow(
                null,
                Tools::atktext('export_selections', 'atk'),
                $this->getExportSelectionDropdown().'&nbsp;&nbsp;&nbsp;<a href="javascript:void(0);" onclick="toggleSelectionName(\'new\');return false;">'.Tools::atktext('new', 'atk')).'</a>';

            $content .= $this->_getOptionsFormRow(null, '', '<div id="selection_interact"></div>');

            $content .= $this->_getOptionsFormRow(
                ['id' => 'export_name', 'style'=>"display:none;"],
                Tools::atktext('export_selections_name', 'atk'),
                '<input type="text" size="40" id="export_selection_name" name="export_selection_name" value="" class="form-control">'
            );
        }

        $content .= $this->_getOptionsFormRow(
            null,
            Tools::atktext('delimiter', 'atk'),
            '<input type="text" class="form-control" size="2" name="delimiter" value='.Config::getGlobal('export_delimiter', ';').'>'
        );

        $content .= $this->_getOptionsFormRow(
            null,
            Tools::atktext('enclosure', 'atk'),
            '<input type="text" size="2" class="form-control" name="enclosure" value='.Config::getGlobal('export_enclosure', '&quot;').'>'
        );

        $content .= $this->_getOptionsFormRow(
            null,
            Tools::atktext('export_selectcolumns', 'atk'),
            '<div id="export_attributes">'.$this->getAttributeSelect().'</div>'
        );

        $content .= $this->_getOptionsFormRow(
            null,
            Tools::atktext('export_generatetitlerow'),
            '<input type="checkbox" name="generatetitlerow" class="atkcheckbox" value=1 '.(Config::getGlobal('export_titlerow_checked', true) ? 'checked' : '').'>'
        );

        return $content;
    }

    /**
     * Build the dropdown field to add the exportselections.
     *
     * @return string
     */
    private function getExportSelectionDropdown()
    {
        $html = '
        <select name="export_selection_options" id="export_selection_options" onchange="toggleSelectionName();return false;" class="form-control select-standard">
          <option value="none">'.Tools::atktext('none', 'atk').'</option>';

        $options = $this->getExportSelections();
        if (Tools::count($options)) {
            foreach ($options as $option) {
                $html .= '
           <option value="'.$option['id'].'">'.htmlentities($option['name']).'</option>';
            }
        }

        $html .= '</select>';

        return $html;
    }

    /**
     * Store selectiondetails in the database.
     */
    private function saveSelection()
    {
        $db = Db::getInstance();

        $user_id = 0;
        if ('none' !== strtolower(Config::getGlobal('authentication'))) {
            $user = SecurityManager::atkGetUser();
            $user_id = array_key_exists(Config::getGlobal('auth_userpk'), $user) ? $user[Config::getGlobal('auth_userpk')] : 0;
        }

        // first check if the combination of node, name and user_id doesn't already exist
        $query = $db->createQuery('atk_exportcriteria');
        $query->addCondition('nodetype = :nodetype', [':nodetype' => $this->m_postvars['atknodeuri']]);
        $query->addCondition('name = :name', [':name' => $this->m_postvars['export_selection_name']]);
        $query->addCondition('userid = :user', [':user' => $user_id]);
        if ($query->executeCount()) {
            return;
        }

        $query = $db->createQuery('atk_exportcriteria');
        $query->addFields([
            'nodetype' => $this->m_postvars['atknodeuri'],
            'name' => $this->m_postvars['export_selection_name'],
            'criteria' => serialize($this->m_postvars),
            'userid' => $user_id
        ]);
        $query->executeInsert();
    }

    /**
     * Update selectiondetails in the database.
     */
    private function updateSelection()
    {
        $db = Db::getInstance();

        $user_id = 0;
        if ('none' !== strtolower(Config::getGlobal('authentication'))) {
            $user = SecurityManager::atkGetUser();
            $user_id = array_key_exists(Config::getGlobal('auth_userpk'), $user) ? $user[Config::getGlobal('auth_userpk')] : 0;
        }

        // first check if the combination of node, name and user_id doesn't already exist
        $query = $db->createQuery('atk_exportcriteria');
        $query->addCondition('nodetype = :nodetype', [':nodetype' => $this->m_postvars['atknodeuri']]);
        $query->addCondition('name = :name', [':name' => $this->m_postvars['export_selection_name']]);
        $query->addCondition('userid = :user', [':user' => $user_id]);
        $query->addCondition('id != :id', [':id' => $this->m_postvars['exportvalue']]);
        if ($query->executeCount()) {
            return;
        }

        $query = $db->createQuery('atk_exportcriteria');
        $query->addCondition('id = :id', [':id' => $this->m_postvars['exportvalue']]);
        $query->addField('name', $this->m_postvars['export_selection_name']);
        $query->addField('criteria', serialize($this->m_postvars));

        $query->executeUpdate();
    }

    /**
     * Delete record.
     *
     * @param int $id
     */
    private function deleteSelection($id)
    {
        $query = Db::getInstance()->createQuery('atk_exportcriteria');
        $query->addCondition('id = :id', [':id' => $id]);
        $query->executeDelete();
    }

    /**
     * Determine the export selections that should be displayed.
     *
     * @return array
     */
    protected function getExportSelections()
    {
        $query = Db::getInstance()->createQuery('atk_exportcriteria');
        $query->addField('id');
        $query->addField('name');
        $query->addOrderBy('name');
        $query->addCondition('nodetype = :nodetype', [':nodetype' => $this->m_postvars['atknodeuri']]);

        // Filter by user if needed :
        if ('none' !== strtolower(Config::getGlobal('authentication'))) {
            $user = SecurityManager::atkGetUser();
            if (!SecurityManager::isUserAdmin($user)) {
                $query->addCondition($query->inCondition('userid', [0, $user[Config::getGlobal('auth_userpk')]]));
            }
        }

        return $query->executeSelect();
    }

    /**
     * Get all attributes to select for the export.
     * @param string $value
     * @return string HTML code with checkboxes for each attribute to select
     */
    public function getAttributeSelect($value = '')
    {
        $atts = $this->getUsableAttributes($value);
        $content = '<div class="container-fluid ExportHandler">';

        foreach ($atts as $tab => $group) {
            $content .= '<div class="row attributes-group">';

            if ($tab != 'default') {
                $content .= '<div class="col-sm-12 attributes-group-title">';
                $content .= Tools::atktext(["tab_$tab", $tab], $this->m_node->m_module, $this->m_node->m_type);
                $content .= '</div>';
            }

            foreach ($group as $item) {
                $checked = $item['checked']?'CHECKED':'';
                $content .= '<div class="col-xs-12 col-sm-4 col-md-3 col-lg-2 attributes-checkbox-container">';
                $content .= '<label><input type="checkbox" name="export_'.$item['name'].'" class="atkcheckbox" value="export_'.$item['name'].'" '.$checked.'> '.$item['text'].'</label>';
                $content .= '</div>';
            }

            $content .= '</div>';
        }

        $content .= '</div>';

        return $content;
    }

    /**
     * Gives all the attributes that can be used for the export.
     * @param string $value
     * @return array the attributes
     */
    public function getUsableAttributes($value = '')
    {
        $selected = ($value == 'new') ? false : true;

        $criteria = [];
        if (!in_array($value, array('new', 'none', ''))) {
            $query = Db::getInstance();
            $criteria = $db->getValue('SELECT criteria FROM atk_exportcriteria WHERE id = :id', [':id' => $value]);
            $criteria = unserialize($criteria);
        }

        $atts = [];
        $attriblist = $this->invoke('getExportAttributes');
        foreach ($attriblist as $key => $attr) {
            $flags = $attr->m_flags;
            $class = strtolower(get_class($attr));
            if ($attr->hasFlag(Attribute::AF_AUTOKEY) || $attr->hasFlag(Attribute::AF_HIDE_VIEW) || !(strpos($class, 'dummy') === false) || !(strpos($class,'image') === false) || !(strpos($class, 'tabbedpane') === false)
            ) {
                continue;
            }
            if (method_exists($this->m_node, 'getExportAttributeGroup')) {
                $group = $this->m_node->getExportAttributeGroup($attr->m_name);
            } else {
                $group = $attr->m_tabs[0];
            }
            if (in_array($group, $atts)) {
                $atts[$group] = [];
            }
            // selected options based on a new selection, or no selection
            if (empty($criteria)) {
                $atts[$group][] = array(
                    'name' => $key,
                    'text' => $attr->label(),
                    'checked' => $selected == true ? !$attr->hasFlag(Attribute::AF_HIDE_LIST) : false,
                );
            } // selected options based on a selection from DB
            else {
                $atts[$group][] = array(
                    'name' => $key,
                    'text' => $attr->label(),
                    'checked' => in_array('export_'.$key, $criteria) ? true : false,
                );
            }
        }

        return $atts;
    }

    /**
     * Return all attributes that can be exported.
     *
     * @return array Array with attribs that needs to be exported
     */
    public function getExportAttributes()
    {
        $attribs = $this->m_node->getAttributes();
        if (is_null($attribs)) {
            return [];
        } else {
            return $attribs;
        }
    }

    /**
     * the real import function
     * import the uploaded csv file for real.
     */
    public function doExport()
    {
        $enclosure = $this->m_postvars['enclosure'];
        $delimiter = $this->m_postvars['delimiter'];
        $source = $this->m_postvars;
        $list_includes = [];
        foreach ($source as $name => $value) {
            $pos = strpos($name, 'export_');
            if (is_integer($pos) and $pos == 0) {
                $list_includes[] = substr($name, strlen('export_'));
            }
        }
        $sm = SessionManager::getInstance();
        $sessionData = &SessionManager::getSession();
        $session_back = $sessionData['default']['stack'][$sm->atkStackID()][$sm->atkLevel() - 1];
        $atkorderby = isset($session_back['atkorderby'])?$session_back['atkorderby']:null;

        $node = $this->m_node;
        $node_bk = $node;
        $atts = &$node_bk->m_attribList;

        foreach ($atts as $name => $object) {
            $att = $node_bk->getAttribute($name);
            if (in_array($name, $list_includes) && $att->hasFlag(Attribute::AF_HIDE_LIST)) {
                $att->removeFlag(Attribute::AF_HIDE_LIST);
            } elseif (!in_array($name, $list_includes)) {
                $att->addFlag(Attribute::AF_HIDE_LIST);
            }
        }

        $rl = new CustomRecordList();
        $node_bk->m_postvars = $session_back;

        if (isset($session_back['atkdg']['admin']['atksearch'])) {
            $node_bk->m_postvars['atksearch'] = $session_back['atkdg']['admin']['atksearch'];
        }
        if (isset($session_back['atkdg']['admin']['atksearchmode'])) {
            $node_bk->m_postvars['atksearchmode'] = $session_back['atkdg']['admin']['atksearchmode'];
        }

        $condition = isset($session_back['atkselector'])?$node_bk->primaryKeyFromString($session_back['atkselector']):'1';
        $recordset = $node_bk->select($atkselector)->orderBy($atkorderby)->includes($list_includes)->mode('export')->fetchAll();
        if (method_exists($this->m_node, 'assignExportData')) {
            $this->m_node->assignExportData($list_includes, $recordset);
        }
        $recordset_new = [];

        foreach ($recordset as $row) {
            foreach ($row as $name => $value) {
                if (in_array($name, $list_includes)) {
                    $value = str_replace("\r\n", '\\n', $value);
                    $value = str_replace("\n", '\\n', $value);
                    $value = str_replace("\t", '\\t', $value);
                    $row[$name] = $value;
                }
            }
            $recordset_new[] = $row;
        }

        $filename = 'export_'.strtolower(str_replace(' ', '_', $this->getUi()->nodeTitle($node)));
        $rl->render($node_bk, $recordset_new, '', $enclosure, $enclosure, "\r\n", 1, '', '', array('filename' => $filename), 'csv', $source['generatetitlerow'],
            true, $delimiter);

        return true;
    }
}
