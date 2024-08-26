<?php

namespace Sintattica\Atk\Handlers;

use Exception;
use Sintattica\Atk\Attributes\DummyAttribute;
use Sintattica\Atk\Attributes\TabbedPane;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\RecordList\CustomRecordList;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Ui\Ui;

/**
 * Handler for the 'import' action of a node. The import action is a
 * generic tool for importing CSV files into a table.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ExportHandler extends ActionHandler
{
    const TO_HIDE_CLASSES = [DummyAttribute::class, TabbedPane::class];

    public function action_export()
    {
        global $ATK_VARS;

        // intercept partial call
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);
            return;
        }

        // need to keep the postdata after a Attribute::AF_LARGE selection in the allfield
        if (!isset($this->m_postvars['phase']) && isset($ATK_VARS['atkformdata'])) {
            foreach ($ATK_VARS['atkformdata'] as $key => $value) {
                $this->m_postvars[$key] = $value;
            }
        }

        // need to keep the selected item after an exporterror
        $phase = Tools::atkArrayNvl($this->m_postvars, 'phase', 'init');
        if (!in_array($phase, ['init', 'process'])) {
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
    public function doInit(): bool
    {
        $content = $this->_getInitHtml();
        $page = $this->getPage();

        $page->register_scriptcode("
        
         function onSelectAllTabClick(tabId){
           const allTabFields = getAllFieldsOfTab(tabId);
           
           for (let i=0; i<allTabFields.length; i++){
                const field = allTabFields[i];
                $(field).prop('checked', true); 
           }
          
         }
                      
         function onSelectNoneTabClick(tabId){
           const allTabFields = getAllFieldsOfTab(tabId);
           
           for (let i=0; i<allTabFields.length; i++){
                const field = allTabFields[i];
                $(field).prop('checked', false); 
           }
          
          console.log('clicked none on ', allTabFields)
         }
         
         function getAllFieldsOfTab(tabId){
            const selector = '.' + tabId +' .atkcheckbox';
            return document.querySelectorAll(selector)
         }
        
        ");

        $params = [];
        $params['title'] = $this->m_node->actionTitle('export');
        $params['content'] = $content;
        $content = $this->getUi()->renderBox($params);
        $output = $this->m_node->renderActionPage('export', $content);
        $page->addContent($output);

        return true;
    }


    /**
     * Gets the HTML for the initial mode of the exporthandler.
     *
     * @return string The HTML for the screen
     * @throws Exception
     */
    public function _getInitHtml(): string
    {
        $action = Tools::dispatch_url($this->m_node->m_module . '.' . $this->m_node->m_type, 'export');
        $sm = SessionManager::getInstance();

        $params = [];
        $params['formstart'] = '<form id="entryform" name="entryform" enctype="multipart/form-data" action="' . $action . '" method="post" class="form-horizontal">';
        $params['formstart'] .= $sm->formState();
        $params['formstart'] .= '<input type="hidden" name="phase" value="process"/>';
        if ($sm->atkLevel() > 0) {
            $params['buttons'][] = Tools::atkButton(Tools::atktext('cancel', 'atk'), '', SessionManager::SESSION_BACK);
        }

        $exportText = Tools::atktext('export', 'atk');
        $params['buttons'][] = '<button class="btn btn-primary" type="submit" value="' . $exportText . '">' . $exportText . '</button>';

        $saveExportText = Tools::atktext('save_export_selection', 'atk');
        $params['buttons'][] = '<button id="export_save_button" style="display:none;" value="' . $saveExportText . '" name="save_export" class="btn" type="submit" >' . $saveExportText . '</button>';

        $params['content'] = '<b>' . Tools::atktext('export_config_explanation', 'atk', $this->m_node->m_type) . '</b><br/><br/>';
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

        // Export CVS
        if (!array_key_exists('save_export', $this->m_postvars)) {
            return $this->doExport();
        }
    }

    private function _getOptionsFormRow($rowAttributes, $label, $field): string
    {
        $content = '<div class="row form-group"';
        if ($rowAttributes) {
            foreach ($rowAttributes as $k => $v) {
                $content .= ' ' . $k . '="' . $v . '"';
            }
        }
        $content .= '>';

        $content .= '  <label class="col-sm-2 control-label">' . $label . '</label>';
        $content .= '  <div class="col-sm-10">' . $field . '</div>';
        $content .= '</div>';
        return $content;
    }

    /**
     * Get the options for the export.
     *
     * @return string html
     * @throws Exception
     */
    public function _getOptions(): string
    {

        $content = $this->_getOptionsFormRow(
            null,
            Tools::atktext('delimiter', 'atk'),
            '<input type="text" class="form-control form-control-sm" size="2" name="delimiter" value=' . Config::getGlobal('export_delimiter', ';') . '>'
        );

        $content .= $this->_getOptionsFormRow(
            null,
            Tools::atktext('enclosure', 'atk'),
            '<input type="text" size="2" class="form-control form-control-sm" name="enclosure" value=' . Config::getGlobal('export_enclosure', '&quot;') . '>'
        );

        $content .= $this->_getOptionsFormRow(
            null,
            Tools::atktext('export_selectcolumns', 'atk'),
            '<div id="export_attributes">' . $this->getAttributeSelect() . '</div>'
        );

        $content .= $this->_getOptionsFormRow(
            null,
            Tools::atktext('export_generatetitlerow'),
            '<input type="checkbox" name="generatetitlerow" class="atkcheckbox" value=1 ' . (Config::getGlobal('export_titlerow_checked', true) ? 'checked' : '') . '>'
        );

        return $content;
    }

    /**
     * Get all attributes to select for the export.
     * @param string|null $value
     * @return string HTML code with checkboxes for each attribute to select
     * @throws Exception
     */
    public function getAttributeSelect(string $value = ''): string
    {
        $atts = $this->getUsableAttributes($value);
        $content = '<div class="container-fluid ExportHandler d-flex  flex-wrap m-0 p-0 justify-content-center">';

        $content .= '<div class="row no-gutters"></div>';


        foreach ($atts as $tab => $group) {


            $tabId = 'tab-' . str_replace(' ', '_', $tab);
            $content .= '<div id=' . $tabId . ' class="card card-outline card-secondary m-1 attributes-group flex-grow-1 ' . $tabId . '" style="min-width: 300px">';


            $tabTitle = "<div class='d-flex flex-grow-1 my-auto px-1 text-bold'>" . ($tab != 'default' ? Tools::atktext(["tab_$tab", $tab], $this->m_node->m_module, $this->m_node->m_type) : Tools::atktext("menu_main", $this->m_node->m_module, $this->m_node->m_type)) . "</div>";


            $navButtons = '<div class="d-flex" >
                               <div class="my-auto px-1">' . Tools::atktext("select", $this->m_node->m_module, $this->m_node->m_type) . '</div>        
                               <div class="btn-group" role="group">                   
                                <button type="button" class="btn btn-xs btn-default px-2" onclick="onSelectAllTabClick(\'' . $tabId . '\')">' . Tools::atktext("pf_check_all", $this->m_node->m_module, $this->m_node->m_type) . '</button>
                                <button type="button" class="btn btn-xs btn-default px-2" onclick="onSelectNoneTabClick(\'' . $tabId . '\')">' . Tools::atktext("pf_check_none", $this->m_node->m_module, $this->m_node->m_type) . '</button>
                            </div>
                            </div>';


            $content .= '<div class="card-header text-sm d-flex justify-content-between">' . $tabTitle . $navButtons . '</div>';


            $content .= '<div class="card-body d-flex justify-content-start flex-wrap">';

            foreach ($group as $item) {
                $checked = $item['checked'] ? 'CHECKED' : '';
                $content .= '<div class="attributes-checkbox-container mx-1">';
                $content .= '<label class="text-nowrap"><input type="checkbox" name="export_' . $item['name'] . '" class="atkcheckbox" value="export_' . $item['name'] . '" ' . $checked . '> ' . $item['text'] . '</label>';
                $content .= '</div>';
            }

            $content .= "</div>";


            $content .= '</div>';

        }

        $content .= '</div></div>';

        return $content;
    }

    /**
     * Gives all the attributes that can be used for the import.
     * @param string $value
     * @return array the attributes
     * @throws Exception
     */
    public function getUsableAttributes(string $value = ''): array
    {
        $selected = $value != 'new';

        $attributes = [];
        /** @var Attribute[] $attributesList */
        $attributesList = $this->invoke('getExportAttributes');
        foreach ($attributesList as $key => $attr) {

            $skipAttribute = false;
            foreach (self::TO_HIDE_CLASSES as $toHideClass) {
                if ($attr instanceof $toHideClass && !$attr->isForceExport()) {
                    $skipAttribute = true;
                    break;
                }
            }

            $class = strtolower(get_class($attr));
            if ($skipAttribute || $attr->hasFlag(Attribute::AF_AUTOKEY) || $attr->hasFlag(Attribute::AF_HIDE_VIEW) || !(strpos($class, 'image') === false)) {
                continue;
            }

            if (method_exists($this->m_node, 'getExportAttributeGroup')) {
                $group = $this->m_node->getExportAttributeGroup($attr->m_name);
            } else {
                $group = $attr->m_tabs[0];
            }
            if (in_array($group, $attributes)) {
                $attributes[$group] = [];
            }
            // selected options based on a new selection, or no selection

            $attributes[$group][] = [
                'name' => $key,
                'text' => $attr->label(),
                'checked' => $selected == true && !$attr->hasFlag(Attribute::AF_HIDE_LIST),
            ];
        }

        return $attributes;
    }

    /**
     * Return all attributes that can be exported.
     *
     * @return array Array with attribs that needs to be exported
     */
    public function getExportAttributes(): array
    {
        $attribs = $this->m_node->getAttributes();
        return is_null($attribs) ? [] : $attribs;
    }

    /**
     * the real import function
     * import the uploaded csv file for real.
     */
    public function doExport(): bool
    {
        $enclosure = $this->m_postvars['enclosure'];
        $delimiter = $this->m_postvars['delimiter'];
        $source = $this->m_postvars;
        $listIncludes = [];
        foreach ($source as $name => $value) {
            $pos = strpos($name, 'export_');
            if (is_integer($pos) and $pos == 0) {
                $listIncludes[] = substr($name, strlen('export_'));
            }
        }
        $sm = SessionManager::getInstance();
        $sessionData = &SessionManager::getSession();
        $sessionBack = $sessionData['default']['stack'][$sm->atkStackID()][$sm->atkLevel() - 1];
        $atkOrderBy = $sessionBack['atkorderby'] ?? null;

        $node = $this->m_node;
        $nodeBk = $node;
        $nodeBkAttributes = &$nodeBk->m_attribList;

        foreach ($nodeBkAttributes as $name => $object) {
            $att = $nodeBk->getAttribute($name);
            if (in_array($name, $listIncludes) && $att->hasFlag(Attribute::AF_HIDE_LIST)) {
                $att->removeFlag(Attribute::AF_HIDE_LIST);
            } elseif (!in_array($name, $listIncludes)) {
                $att->addFlag(Attribute::AF_HIDE_LIST);
            }
        }

        $customRecordList = new CustomRecordList();
        $nodeBk->m_postvars = $sessionBack;

        if (isset($sessionBack['atkdg']['admin']['atksearch'])) {
            $nodeBk->m_postvars['atksearch'] = $sessionBack['atkdg']['admin']['atksearch'];
        }
        if (isset($sessionBack['atkdg']['admin']['atksearchmode'])) {
            $nodeBk->m_postvars['atksearchmode'] = $sessionBack['atkdg']['admin']['atksearchmode'];
        }

        $atkFilter = Tools::atkArrayNvl($source, 'atkfilter', '');

        $atkSelector = $sessionBack[Node::PARAM_ATKSELECTOR] ?? '';
        $condition = $atkSelector . ($atkSelector != '' && $atkFilter != '' ? ' AND ' : '') . $atkFilter;
        $recordSet = $nodeBk->select($condition)->orderBy($atkOrderBy)->includes($listIncludes)->mode('export')->getAllRows();

        if ($nodeBk->hasNestedAttributes()) {
            $nestedAttributesList = $nodeBk->getNestedAttributesList();
            foreach ($recordSet as &$record) {
                foreach ($nestedAttributesList as $nestedAttributeField => $nestedAttributeNames) {
                    foreach ($nestedAttributeNames as $nestedAttributeName) {
                        if (in_array($nestedAttributeName, $listIncludes) && isset($record[$nestedAttributeField])) {
                            $record[$nestedAttributeName] = $record[$nestedAttributeField][$nestedAttributeName];
                        } else {
                            $record[$nestedAttributeName] = null;
                        }
                    }
                }
            }
        }

        if (method_exists($this->m_node, 'assignExportData')) {
            $this->m_node->assignExportData($listIncludes, $recordSet);
        }

        $recordSetNew = [];

        foreach ($recordSet as $row) {
            foreach ($row as $name => $value) {
                if (in_array($name, $listIncludes)) {
                    $value = str_replace("\r\n", '\\n', $value);
                    $value = str_replace("\n", '\\n', $value);
                    $value = str_replace("\t", '\\t', $value);
                    $row[$name] = $value;
                }
            }
            $recordSetNew[] = $row;
        }

        $customRecordList->render(
            $nodeBk,
            $recordSetNew,
            '',
            $enclosure,
            $enclosure,
            "\r\n",
            1,
            '',
            '',
            ['filename' => $this->getNode()->exportFileName()],
            'csv',
            $source['generatetitlerow'],
            true,
            $delimiter
        );

        return true;
    }
}
