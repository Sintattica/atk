<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Tools;

class StatsHandler extends SearchHandler
{
    public function action_stats()
    {
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);

            return;
        }
        $page = $this->getPage();
        $page->register_script(Config::getGlobal('assets_url').'javascript/formsubmit.js');



        $page->addContent($this->m_node->renderActionPage('stats', $this->invoke('statsPage')));
    }

    public function statsPage($record = null)
    {
        $result = $this->getStatsPage($record);
        if ($result !== false) {
            return $result;
        }

        return;
    }

    public function getStatsPage($record = null)
    {
        if ($record == null) {
            $record = $this->getNode()->updateRecord('', null, null, true);
        }

        $page = $this->getPage();
        $page->register_script(Config::getGlobal('assets_url').'javascript/tools.js');

        $res = [];
        $res[] = $this->invoke('statsFormPage', $record);
        $res[] = $this->invoke('statsGraphPage', $record);

        return $res;
    }

    public function statsFormPage($record = null)
    {
        $node = $this->getNode();
        $ui = $this->getUi();

        $params = [];
        $params['formstart'] = $this->getFormStart();
        $params['content'] = $this->invoke('statsForm', $record);
        $params['buttons'] = $node->getFormButtons('stats');
        $params['formend'] = '</form>';

        return $ui->renderBox([
            'title' => $node->actionTitle('stats'),
            'content' => $ui->renderAction('stats', $params),
        ]);
    }

    public function getFormStart()
    {
        $sm = SessionManager::getInstance();

        $formstart = '<form name="entryform" action="'.Config::getGlobal('dispatcher').'" method="get">';
        $formstart .= $sm->formState(SessionManager::SESSION_REPLACE);
        $formstart .= '<input type="hidden" name="atkaction" value="stats">';
        $formstart .= '<input type="hidden" name="atknodeuri" value="'.$this->getNode()->atkNodeUri().'">';

        return $formstart;
    }

    public function statsGraphPage($record)
    {
        $ui = $this->getUi();
        $vars = array(
            'title' => 'Stats',
            'content' => $this->invoke('renderStatsGraphPage', $record),
        );

        return $ui->renderBox($vars);
    }

    public function statsForm($record)
    {
        $params = [];
        $params['fields'] = [];
        $ui = $this->getUi();
        $node = $this->getNode();

        /** @var EditHandler $edithandler */
        $edithandler = $node->getHandler('edit');
        $form = $edithandler->editForm('stats', $record, [], '', $node->getEditFieldPrefix(), $node->getTemplate('stats', $record), true);
        return $form;
    }

    public function renderStatsGraphPage($record)
    {
        return print_r($record, 1);
    }


    /**
     * Handler for partial actions on an stats page.
     *
     * @param string $partial full partial name
     *
     * @return string
     */
    public function partial_attribute($partial)
    {
        list($type, $attribute, $partial) = explode('.', $partial);

        $attr = $this->m_node->getAttribute($attribute);
        if ($attr == null) {
            Tools::atkerror("Unknown / invalid attribute '$attribute' for node '".$this->m_node->atkNodeUri()."'");

            return '';
        }

        return $attr->partial($partial, 'add');
    }
}
