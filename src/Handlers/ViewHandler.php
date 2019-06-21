<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Session\SessionManager;

/**
 * Handler class for a readonly view action. Similar to the edit handler,
 * but all fields are displayed readonly.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ViewHandler extends ViewEditBase
{
    public $m_buttonsource = null;

    /**
     * The action handler method.
     *
     * @param bool $renderbox Render this action in a renderbox or just output the HTML
     */
    public function action_view($renderbox = true)
    {
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);

            return;
        }

        $record = $this->getRecord();

        // allowed to view record?
        if (!$record || !$this->allowed($record)) {
            $this->renderAccessDeniedPage();

            return;
        }

        $page = $this->getPage();
        $page->register_script(Config::getGlobal('assets_url').'javascript/formsubmit.js');
        $this->notify('view', $record);
        $page->addContent($this->m_node->renderActionPage('admin', $this->invoke('viewPage', $record, $this->m_node, $renderbox)));
    }

    /**
     * Returns the view record.
     */
    public function getRecordFromDb()
    {
        return $this->m_node->select($this->m_postvars['atkselector'])->excludes($this->m_node->m_viewExcludes)->mode('view')->getFirstRow();
    }

    /**
     * Get the start of the form.
     *
     * @return string HTML The forms' start
     */
    public function getFormStart($record = null)
    {
        $sm = SessionManager::getInstance();
        $formstart = '<form id="entryform" name="entryform" action="'.Config::getGlobal('dispatcher').'" method="get" onsubmit="return ATK.globalSubmit(this,false)">';
        $formstart .= $sm->formState(SessionManager::SESSION_NESTED);
        $formstart .= '<input type="hidden" name="atkselector" value="'.$this->getNode()->primaryKey($record).'">';
        $formstart .= '<input type="hidden" class="atksubmitaction" />';

        return $formstart;
    }

    /**
     * Returns an htmlpage displaying all displayable attributes.
     *
     * @param array $record The record to display.
     * @param Node $node The node for which a viewPage is displayed.
     * @param bool $renderbox Render this action in a renderbox or just output the HTML
     *
     * @return string The html page with a reaonly view of relevant fields.
     */
    public function viewPage($record, $node, $renderbox = true)
    {
        $ui = $this->getUi();

        if (is_object($ui)) {
            $params = $node->getDefaultActionParams();

            $params['header'] = $this->invoke('viewHeader', $record);
            $params['title'] = $node->actionTitle($this->m_action, $record);
            $params['content'] = $this->viewForm($record, 'view');

            $params['formstart'] = $this->getFormStart($record);
            $params['buttons'] = $this->getFormButtons($record);
            $params['formend'] = '</form>';

            $output = $ui->renderAction('view', $params);

            if (!$renderbox) {
                return $output;
            }

            $this->getPage()->setTitle(Tools::atktext('app_shorttitle').' - '.$node->actionTitle($this->m_action, $record));

            $vars = array('title' => $node->actionTitle($this->m_action, $record), 'content' => $output);

            $total = $ui->renderBox($vars, $this->m_boxTemplate);

            return $total;
        } else {
            Tools::atkerror('ui object error');
        }
    }

    /**
     * Get the buttons for the current action form.
     *
     * @param array $record
     *
     * @return array Array with buttons
     */
    public function getFormButtons($record = null)
    {
        // If no custom button source is given, get the default
        if ($this->m_buttonsource === null) {
            $this->m_buttonsource = $this->m_node;
        }

        return $this->m_buttonsource->getFormButtons('view', $record);
    }

    /**
     * Overrideable function to create a header for view mode.
     * Similar to the admin header functionality.
     */
    public function viewHeader()
    {
        return '';
    }

    /**
     * Get the view page.
     *
     * @param array $record The record
     * @param string $mode The mode we're in (defaults to "view")
     * @param string $template The template to use for the view form
     *
     * @return string HTML code of the page
     */
    public function viewForm($record, $mode = 'view', $template = '')
    {
        $node = $this->m_node;

        // get data, transform into form, return
        $data = $node->viewArray($mode, $record);

        $fields = $this->fieldsWithTabsAndSections($data['fields']);
        // get active tab
        $tab = $this->getActiveTab();
        // get all tabs of current mode
        $tabs = $this->getTabs($fields);

        $ui = $this->getUi();

        $tabTpl = $this->_getTabTpl($node, $tabs, $mode, $record);

        if ($template) {
            $innerform = $ui->render($template, ['fields' => $fields]);
        } else {
            if (Tools::count(array_unique($tabTpl)) > 1) {
                $tabForm = $this->_renderTabs($fields, $tabTpl);
                $innerform = implode(null, $tabForm);
            } else {
                $innerform = $ui->render($node->getTemplate('view', $record, $tab), ['fields' => $fields]);
            }
        }

        return $this->tabulate('view', $fields, $innerform);
    }
}
