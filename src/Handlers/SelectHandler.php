<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Utils\StringParser;
use Sintattica\Atk\Core\Node;
use Exception;

/**
 * Handler class for the select action of a node. The handler draws a
 * generic select form for searching through the records and selecting
 * one of the records.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
class SelectHandler extends ActionHandler
{
    /**
     * The action handler method.
     */
    public function action_select()
    {
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);

            return;
        }

        $output = $this->invoke('selectPage');

        if ($output != '') {
            $this->getPage()->addContent($this->getNode()->renderActionPage('select', $output));
        }
    }

    /**
     * This method returns an html page containing a recordlist to select
     * records from. The recordlist can be searched, sorted etc. like an
     * admin screen.
     *
     * @return string The html select page.
     */
    public function selectPage()
    {
        $node = $this->getNode();

        $grid = DataGrid::create($node, 'select');
        $actions = array('select' => Tools::atkurldecode($grid->getPostvar('atktarget')));
        $grid->removeFlag(DataGrid::MULTI_RECORD_ACTIONS);
        $grid->removeFlag(DataGrid::MULTI_RECORD_PRIORITY_ACTIONS);
        $grid->setDefaultActions($actions);

        $this->modifyDataGrid($grid, DataGrid::CREATE);

        if ($this->autoSelectRecord($grid)) {
            return '';
        }

        $sm = SessionManager::getInstance();

        $params = [];
        $params['header'] = $node->text('title_select');
        $params['list'] = $grid->render();
        $params['footer'] = '';

        if ($sm->atkLevel() > 0) {
            $backUrl = $sm->sessionUrl(Config::getGlobal('dispatcher').'?atklevel='.$sm->newLevel(SessionManager::SESSION_BACK));
            $params['footer'] = '<br><div style="text-align: center"><input type="button" class="btn btn-default" onclick="window.location=\''.$backUrl.'\';" value="'.$this->getNode()->text('cancel').'"></div>';
        }

        $output = $this->getUi()->renderList('select', $params);

        $vars = array('title' => $this->m_node->actionTitle('select'), 'content' => $output);
        $output = $this->getUi()->renderBox($vars);

        return $output;
    }

    /**
     * Update the admin datagrid.
     *
     * @return string new grid html
     */
    public function partial_datagrid()
    {
        try {
            $grid = DataGrid::resume($this->getNode());

            $this->modifyDataGrid($grid, DataGrid::RESUME);
        } catch (Exception $e) {
            $grid = DataGrid::create($this->getNode());

            $this->modifyDataGrid($grid, DataGrid::RESUME);
        }

        return $grid->render();
    }

    /**
     * If the auto-select flag is set and only one record exists we immediately
     * return with the selected record.
     *
     * @param DataGrid $grid data grid
     *
     * @return bool auto-select active?
     */
    protected function autoSelectRecord($grid)
    {
        $node = $this->getNode();
        if (!$node->hasFlag(Node::NF_AUTOSELECT)) {
            return false;
        }

        $grid->loadRecords();
        if ($grid->getCount() != 1) {
            return false;
        }

        $sm = SessionManager::getInstance();

        if ($sm->atkLevel() > 0 && $grid->getPostvar('atkprevlevel', 0) > $sm->atkLevel()) {
            $backUrl = $sm->sessionUrl(Config::getGlobal('dispatcher').'?atklevel='.$sm->newLevel(SessionManager::SESSION_BACK));
            $node->redirect($backUrl);
        } else {
            $records = $grid->getRecords();

            // There's only one record and the autoselect flag is set, so we
            // automatically go to the target.
            $parser = new StringParser(rawurldecode(Tools::atkurldecode($grid->getPostvar('atktarget'))));

            $target = $parser->parse($records[0], true);

            $node->redirect($sm->sessionUrl($target, SessionManager::SESSION_NESTED));
        }

        return true;
    }
}
