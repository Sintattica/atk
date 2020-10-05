<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\StringParser;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Config;

/**
 * Handler class for the select action of a node. The handler draws a
 * generic select form for searching through the records and selecting
 * multiple records.
 *
 * @author Lineke Kerckhoffs-Willems <lineke@ibuildings.nl>
 */
class MultiselectHandler extends AdminHandler
{
    /**
     * The action handler method.
     */
    public function action_multiselect()
    {
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);

            return;
        }

        if (isset($this->m_postvars['atkselector'])) {
            $output = $this->invoke('handleMultiselect');
        } else {
            $output = $this->invoke('multiSelectPage');
        }

        if ($output != '') {
            $page = $this->getPage();
            $page->addContent($this->m_node->renderActionPage('multiselect', $output));
        }
    }

    /**
     * Parse atkselectors in postvars into atktarget using atktargetvartpl and atktargetvar
     * Then redirect to atktarget.
     */
    public function handleMultiselect()
    {
        $node = $this->getNode();
        $columnConfig = $node->getColumnConfig();
        $recordset = $node->select($node->primaryKeyFromString($this->m_postvars['atkselector']))->orderBy($columnConfig->getOrderByStatement())->excludes($node->m_listExcludes)->mode('multiselect')->fetchAll();

        // loop recordset to parse atktargetvar
        $atktarget = Tools::atkurldecode($node->m_postvars['atktarget']);
        $atktargetvar = $node->m_postvars['atktargetvar'];
        $atktargettpl = $node->m_postvars['atktargetvartpl'];

        for ($i = 0; $i < Tools::count($recordset); ++$i) {
            if ($i == 0 && strpos($atktarget, '&') === false) {
                $atktarget .= '?';
            } else {
                $atktarget .= '&';
            }
            $atktarget .= $atktargetvar.'[]='.$this->parseString($atktargettpl, $recordset[$i]);
        }
        $node->redirect($atktarget);
    }

    /**
     * Parse the target string.
     *
     * @param string $string The string to parse
     * @param array $recordset The recordset to use for parsing the string
     *
     * @return string The parsed string
     */
    public function parseString($string, $recordset)
    {
        $parser = new StringParser($string);

        $output = $parser->parse($recordset, true);

        return $output;
    }

    /**
     * This method returns an html page containing a recordlist to select
     * records from. The recordlist can be searched, sorted etc. like an
     * admin screen.
     *
     * @return string The html select page.
     */
    public function multiSelectPage()
    {
        // add the postvars to the form
        global $g_stickyurl;
        $sm = SessionManager::getInstance();
        $g_stickyurl[] = 'atktarget';
        $g_stickyurl[] = 'atktargetvar';
        $g_stickyurl[] = 'atktargetvartpl';
        $GLOBALS['atktarget'] = $this->getNode()->m_postvars['atktarget'];
        $GLOBALS['atktargetvar'] = $this->getNode()->m_postvars['atktargetvar'];
        $GLOBALS['atktargetvartpl'] = $this->getNode()->m_postvars['atktargetvartpl'];

        $params['header'] = Tools::atktext('title_multiselect', $this->getNode()->m_module, $this->getNode()->m_type);

        $actions['actions'] = [];
        $actions['mra'][] = 'multiselect';

        $grid = DataGrid::create($this->getNode(), 'multiselect');
        /*
         * At first the changes below looked like the solution for the error
         * on the contact multiselect page. Except this is not the case, because
         * the MRA actions will not be shown, which is a must.
         */
        if (is_array($actions['actions'])) {
            $grid->setDefaultActions($actions['actions']);
        } else {
            $grid->setDefaultActions($actions);
        }

        $grid->removeFlag(DataGrid::EXTENDED_SEARCH);
        $grid->addFlag(DataGrid::MULTI_RECORD_ACTIONS);
        $params['list'] = $grid->render();

        if ($sm->atkLevel() > 0) {
            $backlinkurl = $sm->sessionUrl(Config::getGlobal('dispatcher').'?atklevel='.$sm->newLevel(SessionManager::SESSION_BACK));
            $params['footer'] = '<br><div style="text-align: center"><input type="button" class="btn btn-default" onclick="window.location=\''.$backlinkurl.'\';" value="'.Tools::atktext('cancel').'"></div>';
        }

        $output = $this->getUi()->renderList('multiselect', $params);

        return $this->getUi()->renderBox(array(
            'title' => $this->getNode()->actionTitle('multiselect'),
            'content' => $output,
        ));
    }
}
