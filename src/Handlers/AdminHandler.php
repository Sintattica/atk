<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\Json;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Session\SessionManager;
use Exception;

/**
 * Handler for the 'admin' action of a node. It displays a recordlist with
 * existing records, and links to view/edit/delete them (or custom actions
 * if present), and an embedded addform or a link to an addpage (depending
 * on the presence of the Node::NF_ADD_LINK).
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class AdminHandler extends ActionHandler
{
    public $m_actionSessionStatus = SessionManager::SESSION_NESTED;

    /**
     * The action method.
     */
    public function action_admin()
    {
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);

            return;
        }

        $page = $this->getPage();
        $page->register_script(Config::getGlobal('assets_url').'javascript/formsubmit.js');
        $res = $this->renderAdminPage();
        $page->addContent($this->m_node->renderActionPage('admin', $res));
    }

    /**
     * Sets the action session status for actions in the recordlist.
     * (Defaults to SessionManager::SESSION_NESTED).
     *
     * @param int $sessionStatus The sessionstatus (for example SessionManager::SESSION_REPLACE)
     */
    public function setActionSessionStatus($sessionStatus)
    {
        $this->m_actionSessionStatus = $sessionStatus;
    }

    /**
     * Render the adminpage, including addpage if necessary.
     *
     * @return array with result of adminPage and addPage
     */
    public function renderAdminPage()
    {
        $res = [];
        if ($this->m_node->hasFlag(Node::NF_NO_ADD) == false && $this->m_node->allowed('add')) {
            if (!$this->m_node->hasFlag(Node::NF_ADD_LINK)) { // otherwise, in adminPage, an add link will be added.
                // we could get here because of a reject.
                $record = $this->getRejectInfo();

                $res[] = $this->invoke('addPage', $record);
            }
        }
        $res[] = $this->invoke('adminPage');

        return $res;
    }

    /**
     * Draws the form for adding new records.
     *
     * The implementation delegates drawing of the form to the atkAddHandler.
     *
     * @param array $record The record
     *
     * @return string A box containing the add page.
     */
    public function addPage($record = null)
    {
        // Reuse the atkAddHandler for the addPage.
        $atk = Atk::getInstance();
        $node = $atk->atkGetNode($this->invoke('getAddNodeType'));

        $handler = $node->getHandler('add');
        $handler->setNode($node);
        $handler->setReturnBehaviour(self::ATK_ACTION_STAY); // have the save action stay on the admin page
        return $handler->invoke('addPage', $record);
    }

    /**
     * Admin page displays records and the actions that can be performed on
     * them (edit, delete).
     *
     * @param array $actions The list of actions displayed next to each
     *                       record. Nodes can implement a
     *                       recordActions($record, &$actions, &$mraactions)
     *                       method to add/remove record-specific actions.
     *
     * @return string A box containing the admin page (without the add form,
     *                which is added later.
     */
    public function adminPage($actions = array())
    {
        $ui = $this->getUi();

        $vars = array(
            'title' => $this->m_node->actionTitle($this->getNode()->m_action),
            'content' => $this->renderAdminList(),
        );

        $output = $ui->renderBox($vars);

        return $output;
    }

    /**
     * Renders the recordlist for the admin mode.
     *
     * @param array $actions An array with the actions for the admin mode
     *
     * @return string The HTML for the admin recordlist
     */
    public function renderAdminList($actions = '')
    {
        $grid = DataGrid::create($this->getNode(), 'admin');

        if (is_array($actions)) {
            $grid->setDefaultActions($actions);
        }

        $this->modifyDataGrid($grid, DataGrid::CREATE);

        if ($this->redirectToSearchAction($grid)) {
            return '';
        }

        $params = [];
        $params['header'] = $this->invoke('adminHeader').$this->getHeaderLinks();
        $params['list'] = $grid->render();
        $params['footer'] = $this->invoke('adminFooter');
        $output = $this->getUi()->renderList('admin', $params);

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

            $this->modifyDataGrid($grid, DataGrid::CREATE);
        }

        if ($this->redirectToSearchAction($grid)) {
            return '';
        }

        return $grid->render();
    }

    /**
     * If a search action has been defined and a search only returns one result
     * the user will be automatically redirected to the search action.
     *
     * @param DataGrid $grid data grid
     *
     * @return bool redirect active?
     */
    protected function redirectToSearchAction($grid)
    {
        $node = $this->getNode();
        $search = $grid->getPostvar('atksearch');

        // check if we are searching and a search action has been defined
        if (!is_array($search) || Tools::count($search) == 0 || !is_array($node->m_search_action) || Tools::count($node->m_search_action) == 0) {
            return false;
        }

        // check if there is only a single record in the result
        $grid->loadRecords();
        if ($grid->getCount() != 1) {
            return false;
        }

        $records = $grid->getRecords();

        foreach ($node->m_search_action as $action) {
            if (!$node->allowed($action, $records[0])) {
                continue;
            }

            // reset search so we can back to the normal admin screen if we want
            $grid->setPostvar('atksearch', array());
            $sm = SessionManager::getInstance();

            $url = $sm->sessionUrl(Tools::dispatch_url($node->atkNodeUri(), $action, array('atkselector' => $node->primaryKeyString($records[0]))),
                SessionManager::SESSION_NESTED);

            if ($grid->isUpdate()) {
                $script = 'document.location.href = '.Json::encode($url).';';
                $node->getPage()->register_loadscript($script);
            } else {
                $node->redirect($url);
            }

            return true;
        }

        return false;
    }

    /**
     * Function that is called when creating an adminPage.
     *
     * The default implementation returns an empty string, but developers can
     * override this function in their custom handlers or directly in the
     * node class.
     *
     * @return string A string that is displayed above the recordlist.
     */
    public function adminHeader()
    {
        return '';
    }

    /**
     * Function that is called when creating an adminPage.
     *
     * The default implementation returns an empty string, but developers can
     * override this function in their custom handlers or directly in the
     * node class.
     *
     * @return string A string that is displayed below the recordlist.
     */
    public function adminFooter()
    {
        return '';
    }

    /**
     * Get the importlink to add to the admin header.
     *
     * @return string HTML code with link to the import action of the node (if allowed)
     */
    public function getImportLink()
    {
        $link = '';
        if ($this->m_node->allowed('add') && !$this->m_node->hasFlag(Node::NF_READONLY) && $this->m_node->hasFlag(Node::NF_IMPORT)) {
            $cssClass = 'class="btn btn-default admin_link admin_link_import"';
            $link .= Tools::href(Tools::dispatch_url($this->m_node->atkNodeUri(), 'import'), Tools::atktext('import', 'atk', $this->m_node->m_type),
                SessionManager::SESSION_NESTED, false, $cssClass);
        }

        return $link;
    }

    /**
     * Get the exportlink to add to the admin header.
     *
     * @return string HTML code with link to the export action of the node (if allowed)
     */
    public function getExportLink()
    {
        $link = '';
        if ($this->m_node->allowed('view') && $this->m_node->allowed('export') && $this->m_node->hasFlag(Node::NF_EXPORT)) {
            $cssClass = 'class="btn btn-default admin_link admin_link_export"';

            $link .= Tools::href(Tools::dispatch_url($this->m_node->atkNodeUri(), 'export'),
                Tools::atktext('export', 'atk', $this->m_node->m_type), SessionManager::SESSION_NESTED, false, $cssClass);
        }

        return $link;
    }

    /**
     * This function returns the nodetype that should be used for creating
     * the add form or add link above the admin grid. This defaults to the
     * node for this handler. Override this method in your handler or directly
     * in your node to set a custom nodetype.
     */
    public function getAddNodeType()
    {
        return $this->m_node->atkNodeUri();
    }

    /**
     * Get the add link to add to the admin header.
     *
     * @return string HTML code with link to the add action of the node (if allowed)
     */
    public function getAddLink()
    {
        $atk = Atk::getInstance();
        $node = $atk->atkGetNode($this->invoke('getAddNodeType'));

        if (!$node->hasFlag(Node::NF_NO_ADD) && $node->allowed('add')) {

            $label = $node->text('link_'.$node->m_type.'_add', null, '', '', true);
            if (empty($label)) {
                // generic text
                $label = Tools::atktext('add', 'atk');
            }

            if ($node->hasFlag(Node::NF_ADD_LINK)) {
                $addurl = $this->invoke('getAddUrl', $node);

                $cssClass = 'class="btn btn-default admin_link admin_link_add"';
                return Tools::href($addurl, $label, SessionManager::SESSION_NESTED, false, $cssClass);
            }
        }

        return '';
    }

    /**
     * This function renders the url that is used by
     * AdminHandler::getAddLink().
     *
     * @return string The url for the add link for the admin page
     */
    public function getAddUrl()
    {
        $atk = Atk::getInstance();
        $node = $atk->atkGetNode($this->invoke('getAddNodeType'));

        return Config::getGlobal('dispatcher').'?atknodeuri='.$node->atkNodeUri().'&atkaction=add';
    }

    /**
     * Get all links to add to the admin header.
     *
     * @return string String with the HTML code of the links (each link separated with |)
     */
    public function getHeaderLinks()
    {
        $links = [];
        $addlink = $this->getAddLink();
        if ($addlink != '') {
            $links[] = $addlink;
        }
        $importlink = $this->getImportLink();
        if ($importlink != '') {
            $links[] = $importlink;
        }
        $exportlink = $this->getExportLink();
        if ($exportlink != '') {
            $links[] = $exportlink;
        }
        $result = implode(' ', $links);

        if (strlen(trim($result)) > 0) {
            $result .= '<br/>';
        }

        return $result;
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

        return $attr->partial($partial, 'admin');
    }
}
