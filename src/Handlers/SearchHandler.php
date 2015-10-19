<?php namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Controller;
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
 * @package atk
 * @subpackage handlers
 *
 */
class SearchHandler extends AbstractSearchHandler
{

    /**
     * The action handler method.
     */
    function action_search()
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
            $url = Tools::dispatch_url($this->getPreviousNode(), $this->getPreviousAction());
            $url = Tools::session_url($url, SessionManager::atkLevel() > 0 ? SessionManager::SESSION_BACK : SessionManager::SESSION_REPLACE);

            $this->m_node->redirect($url);
        }

        $page = $this->getPage();
        $searcharray = array();

        // load criteria
        if (isset($this->m_postvars['load_criteria'])) {
            if (!empty($name)) {
                $criteria = $this->loadCriteria($name);
                $searcharray = $criteria['atksearch'];
            }
        } elseif (isset($this->m_postvars["atksearch"])) {
            $searcharray = $this->m_postvars["atksearch"];
        }
        $page->addcontent($this->m_node->renderActionPage("search", $this->invoke("searchPage", $searcharray)));
    }

    /**
     * Redirect to search results based on the given criteria.
     */
    function redirectToResults()
    {
        $url = Tools::dispatch_url($this->getPreviousNode(), $this->getPreviousAction(), $this->fetchCriteria(),
            Tools::atkSelf());
        $url = Tools::session_url($url, SessionManager::atkLevel() > 0 ? SessionManager::SESSION_BACK : SessionManager::SESSION_REPLACE);

        $this->m_node->redirect($url);
    }

    /**
     * Returns the node from which the search action was called
     *
     * @return string previous node
     */
    function getPreviousNode()
    {
        return SessionManager::atkLevel() > 0 ? SessionManager::getSessionManager()->stackVar('atknodetype',
            '', SessionManager::atkLevel() - 1)
            : $this->m_node->atkNodeType();
    }

    /**
     * Returns the action from which the search action was called
     *
     * @return string previous action
     */
    function getPreviousAction()
    {
        return SessionManager::atkLevel() > 0 ? SessionManager::getSessionManager()->stackVar('atkaction',
            '', SessionManager::atkLevel() - 1)
            : 'admin';
    }

    /**
     * Attribute handler.
     *
     * @param string $partial full partial
     */
    function partial_attribute($partial)
    {
        list($type, $attribute, $partial) = explode('.', $partial);

        $attr = $this->m_node->getAttribute($attribute);
        if ($attr == null) {
            Tools::atkerror("Unknown / invalid attribute '$attribute' for node '" . $this->m_node->atkNodeType() . "'");
            return '';
        }

        return $attr->partial($partial, 'add');
    }

    /**
     * This method returns an html page that can be used as a search form.
     * @param array $record A record containing default values that will be
     *                      entered in the searchform.
     * @return String The html search page.
     */
    function searchPage($record = null)
    {
        $node = $this->m_node;

        $node->addStyle("style.css");
        $controller = Controller::getInstance();
        $controller->setNode($this->m_node);

        $page = $this->getPage();
        $page->register_script(Config::getGlobal("assets_url") . "javascript/tools.js");
        $page->register_script(Config::getGlobal("assets_url") . "javascript/formfocus.js");
        $page->register_loadscript("placeFocus();");
        $ui = $this->getUi();
        if (is_object($ui)) {
            $params = array();
            $params["formstart"] = '<form name="entryform" action="' . $controller->getPhpFile() . '?' . SID . '" method="post">';

            $params["formstart"] .= Tools::session_form(SessionManager::SESSION_REPLACE);
            $params["formstart"] .= '<input type="hidden" name="atkaction" value="search">';

            $params["formstart"] .= '<input type="hidden" name="atknodetype" value="' . $node->atknodetype() . '">';
            $params["formstart"] .= '<input type="hidden" name="atkstartat" value="0">'; // start at first page after new search

            $params["content"] = $this->invoke("searchForm", $record);

            $params["buttons"] = $controller->getFormButtons('search');

            $params["formend"] = '</form>';

            $output = $ui->renderAction("search", $params);

            $total = $ui->renderBox(array(
                "title" => $node->actionTitle('search'),
                "content" => $output
            ));

            return $total;
        } else {
            Tools::atkerror("ui object failure");
        }
        return '';
    }

    /**
     * This method returns a form that the user can use to search records.
     *
     * @param array $record A record containing default values to put into
     *                      the search fields.
     * @return String The searchform in html form.
     */
    function searchForm($record = null)
    {
        $node = $this->m_node;
        $ui = $this->getUi();

        if (is_object($ui)) {
            $node->setAttribSizes();

            $criteria = $this->fetchCriteria();
            $name = $this->handleSavedCriteria($criteria);

            $params = array();
            $params['searchmode_title'] = Tools::atktext("search_mode", "atk");
            $params['searchmode_and'] = '<input type="radio" name="atksearchmethod" class="atkradio" value="AND" checked>' . Tools::atktext("search_and",
                    "atk");
            $params['searchmode_or'] = '<input type="radio" name="atksearchmethod" class="atkradio" value="OR">' . Tools::atktext("search_or",
                    "atk");
            $params['saved_criteria'] = $this->getSavedCriteria($name);

            $params["fields"] = array();

            foreach ($node->getAttributeNames() as $attribname) {
                $p_attrib = &$node->m_attribList[$attribname];

                if (!$p_attrib->hasFlag(Attribute::AF_HIDE_SEARCH)) {
                    $p_attrib->addToSearchformFields($params["fields"], $node, $record, "",
                        $this->m_postvars['atksearchmode']);
                }
            }
            return $ui->render($node->getTemplate("search", $record), $params);
        } else {
            Tools::atkerror("ui object error");
        }
    }

    /**
     * Fetch posted criteria.
     *
     * @return Array fetched criteria
     */
    function fetchCriteria()
    {
        return array(
            'atksearchmethod' => array_key_exists('atksearchmethod', $this->m_postvars)
                ? $this->m_postvars['atksearchmethod'] : '',
            'atksearch' => array_key_exists('atksearch', $this->m_postvars) ? $this->m_postvars['atksearch']
                : '',
            'atksearchmode' => array_key_exists('atksearchmode', $this->m_postvars)
                ? $this->m_postvars['atksearchmode'] : ''
        );
    }

}


