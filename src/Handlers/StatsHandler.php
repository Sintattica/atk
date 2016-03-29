<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Session\SessionManager;
use Khill\Lavacharts\Lavacharts;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Core\Tools;

class StatsHandler extends ActionHandler
{

    /** @var Attribute[] array  */
    private $attribs = [];

    public function action_stats()
    {
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);

            return;
        }
        $page = $this->getPage();
        $page->register_script(Config::getGlobal('assets_url').'javascript/formsubmit.js');

        $record = $this->m_postvars['atksearch'];

        $page->addContent($this->m_node->renderActionPage('stats', $this->invoke('statsPage', $record)));
    }

    public function statsPage($record = null)
    {
        $res = [];
        $res[] = $this->invoke('statsFormPage', $record);
        $res[] = $this->invoke('statsGraphPage', $record);

        return $res;
    }

    public function statsFormPage($record = null)
    {
        $node = $this->m_node;
        $page = $this->getPage();
        $ui = $this->getUi();

        $page->register_script(Config::getGlobal('assets_url').'javascript/tools.js');
        $sm = SessionManager::getInstance();
        $params = [];
        $params['formstart'] = '<form name="entryform" action="'.Config::getGlobal('dispatcher').'" method="get">';
        $params['formstart'] .= $sm->formState(SessionManager::SESSION_REPLACE);
        $params['formstart'] .= '<input type="hidden" name="atkaction" value="stats">';
        $params['formstart'] .= '<input type="hidden" name="atknodeuri" value="'.$node->atkNodeUri().'">';
        $params['content'] = $this->invoke('statsForm', $record);
        $params['buttons'] = $node->getFormButtons('search');
        $params['formend'] = '</form>';

        return $ui->renderBox([
            'title' => $node->actionTitle('stats'),
            'content' => $ui->renderAction('search', $params),
        ]);
    }

    public function statsGraphPage($record = null)
    {
        $ui = $this->getUi();
        $vars = array(
            'title' => 'Stats',
            'content' => $this->invoke('renderStatsGraphPage', $record),
        );

        return $ui->renderBox($vars);
    }

    public function statsForm($record = null)
    {
        $params = [];
        $params['fields'] = [];
        $ui = $this->getUi();
        $node = $this->getNode();
        $node->setAttribSizes();

        foreach ($node->getStatsAttributes() as &$p_attrib) {
            /** @var Attribute $p_attrib */
            $p_attrib->m_owner = $node->getType();
            $p_attrib->setOwnerInstance($node);
            $p_attrib->init();
            $p_attrib->addToSearchformFields($params['fields'], $node, $record, '');
            $this->attribs[] = $p_attrib;
        }

        return $ui->render($node->getTemplate('stats', $record), $params);

    }

    public function renderStatsGraphPage($record = null)
    {
        $query = $this->getNode()->getDb()->createQuery();
        $ret = [];
        foreach($this->attribs as &$attrib){
            $value = $attrib->getSearchCondition($query, $attrib->getOwnerInstance()->getTable(), $record[$attrib->fieldName()], '');
            $ret[] = $value;
        }

        return implode('<br />', $ret);
       // return print_r($record, true);
    }

    /**
     * Attribute handler.
     *
     * @param string $partial full partial
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
