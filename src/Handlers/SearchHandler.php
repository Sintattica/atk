<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Attributes\Attribute;

/**
 * Handler class for the search action of a node. The handler draws a
 * generic search form for the given node.
 *
 * The actual search is not performed by this handler. The search values are
 * stored in the default atksearch variables, which the admin page uses to
 * perform the actual search. The search form by default redirects to
 * the adminpage to display searchresults.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @author Sandy Pleyte <sandy@achievo.org>
 */
class SearchHandler extends AbstractSearchHandler
{
    /**
     * The action handler method.
     */
    public function action_search()
    {
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);

            return;
        }

        // save criteria
        $criteria = $this->fetchCriteria();
        $name = $this->handleSavedCriteria($criteria);

        // redirect to search results and return
        $doSearch = isset($this->m_postvars['atkdosearch']);
        if ($doSearch) {
            $this->redirectToResults();

            return;
        } elseif (!empty($this->m_postvars['atkcancel'])) {
            $sm = SessionManager::getInstance();
            $url = Tools::dispatch_url($this->getPreviousNode(), $this->getPreviousAction());
            $url = $sm->sessionUrl($url, $sm->atkLevel() > 0 ? SessionManager::SESSION_BACK : SessionManager::SESSION_REPLACE);

            $this->m_node->redirect($url);
        }

        $page = $this->getPage();
        $searcharray = [];

        // load criteria
        if (isset($this->m_postvars['load_criteria'])) {
            if (!empty($name)) {
                $criteria = $this->loadCriteria($name);
                $searcharray = $criteria['atksearch'];
            }
        } elseif (isset($this->m_postvars['atksearch'])) {
            $searcharray = $this->m_postvars['atksearch'];
        }
        $page->addContent($this->m_node->renderActionPage('search', $this->invoke('searchPage', $searcharray)));
    }

    /**
     * Redirect to search results based on the given criteria.
     */
    public function redirectToResults()
    {
        $sm = SessionManager::getInstance();
        $url = Tools::dispatch_url($this->getPreviousNode(), $this->getPreviousAction(), $this->fetchCriteria());
        $url = $sm->sessionUrl($url, $sm->atkLevel() > 0 ? SessionManager::SESSION_BACK : SessionManager::SESSION_REPLACE);
        $this->m_node->redirect($url);
    }

    /**
     * Returns the node from which the search action was called.
     *
     * @return string previous node
     */
    public function getPreviousNode()
    {
        $sm = SessionManager::getInstance();

        return $sm->atkLevel() > 0 ? $sm->stackVar('atknodeuri', '', $sm->atkLevel() - 1) : $this->m_node->atkNodeUri();
    }

    /**
     * Returns the action from which the search action was called.
     *
     * @return string previous action
     */
    public function getPreviousAction()
    {
        $sm = SessionManager::getInstance();

        return $sm->atkLevel() > 0 ? $sm->stackVar('atkaction', '', $sm->atkLevel() - 1) : 'admin';
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

    /**
     * This method returns an html page that can be used as a search form.
     *
     * @param array $searcharray An array with conditions from a search form
     *                           submission. Values will be entered in the
     *                           searchform.
     *
     * @return string The html search page.
     */
    public function searchPage($searcharray = null)
    {
        $node = $this->m_node;
        $ui = $this->getUi();

        if (is_object($ui)) {
            $sm = SessionManager::getInstance();
            $params = [];
            $params['formstart'] = '<form id="entryform" name="entryform" action="'.Config::getGlobal('dispatcher').'" method="post">';

            $params['formstart'] .= $sm->formState(SessionManager::SESSION_REPLACE);
            $params['formstart'] .= '<input type="hidden" name="atkaction" value="search">';

            $params['formstart'] .= '<input type="hidden" name="atknodeuri" value="'.$node->atkNodeUri().'">';
            $params['formstart'] .= '<input type="hidden" name="atkstartat" value="0">'; // start at first page after new search

            $params['content'] = $this->invoke('searchForm', $searcharray);

            $params['buttons'] = $node->getFormButtons('search');

            $params['formend'] = '</form>';

            $output = $ui->renderAction('search', $params);

            $total = $ui->renderBox(array(
                'title' => $node->actionTitle('search'),
                'content' => $output,
            ));

            return $total;
        } else {
            Tools::atkerror('ui object failure');
        }

        return '';
    }

    /**
     * This method returns a form that the user can use to search records.
     *
     * @param array $searcharray A record containing default values to put into
     *                      the search fields.
     *
     * @return string The searchform in html form.
     */
    public function searchForm($searcharray = null)
    {
        $node = $this->m_node;
        $ui = $this->getUi();

        if (is_object($ui)) {
            $node->setAttribSizes();

            $criteria = $this->fetchCriteria();
            $name = $this->handleSavedCriteria($criteria);

            $params = [];
            $params['searchmode_title'] = Tools::atktext('search_mode', 'atk');
            $params['searchmode_and'] = '<input type="radio" name="atksearchmethod" class="atkradio" value="AND" checked>'.Tools::atktext('search_and', 'atk');
            $params['searchmode_or'] = '<input type="radio" name="atksearchmethod" class="atkradio" value="OR">'.Tools::atktext('search_or', 'atk');
            $params['saved_criteria'] = $this->getSavedCriteria($name);

            $params['fields'] = [];

            foreach ($node->getAttributeNames() as $attribname) {
                $p_attrib = $node->m_attribList[$attribname];

                if (!$p_attrib->hasFlag(Attribute::AF_HIDE_SEARCH)) {
                    $p_attrib->addToSearchformFields($params['fields'], $node, $searcharray, '', true);
                }
            }

            return $ui->render($node->getTemplate('search', $searcharray), $params);
        } else {
            Tools::atkerror('ui object error');
        }
    }

    /**
     * Fetch posted criteria.
     *
     * @return array fetched criteria
     */
    public function fetchCriteria()
    {
        return array(
            'atksearchmethod' => array_key_exists('atksearchmethod', $this->m_postvars) ? $this->m_postvars['atksearchmethod'] : '',
            'atksearch' => array_key_exists('atksearch', $this->m_postvars) ? $this->m_postvars['atksearch'] : '',
            'atksearchmode' => array_key_exists('atksearchmode', $this->m_postvars) ? $this->m_postvars['atksearchmode'] : '',
        );
    }
}
