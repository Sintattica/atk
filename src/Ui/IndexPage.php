<?php

namespace Sintattica\Atk\Ui;

use ReflectionException;
use Sintattica\Atk\Core\AdminLTE;
use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Menu\Item;
use Sintattica\Atk\Core\Menu\Menu;
use Sintattica\Atk\Core\Menu\MenuBase;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;

/**
 * Class that generates an index page.
 */
class IndexPage
{
    /*
     * @var Page
     */
    public $m_page;

    /*
     * @var Ui
     */
    public $m_ui;

    /*
     * @var Output
     */
    public $m_output;

    /*
     * @var array
     */
    public $m_user;

    public $m_title;
    public $m_extrabodyprops;
    public $m_extraheaders;
    public $m_username;
    public $m_defaultDestination;
    public $m_flags;

    private $atk;

    private $m_adminLte;

    /**
     * Constructor
     * @param $atk Atk
     */
    public function __construct(Atk $atk)
    {
        global $ATK_VARS;
        $this->atk = $atk;
        $this->m_page = Page::getInstance();
        $this->m_ui = Ui::getInstance();
        $this->m_output = Output::getInstance();
        $this->m_user = SecurityManager::atkGetUser();
        $this->m_adminLte = AdminLTE::getInstance();
        $this->m_flags = array_key_exists('atkpartial', $ATK_VARS) ? Page::HTML_PARTIAL : Page::HTML_STRICT;
    }

    /**
     * Does the IndexPage has this flag?
     *
     * @param int $flag The flag
     *
     * @return bool
     */
    public function hasFlag($flag): bool
    {
        return Tools::hasFlag($this->m_flags, $flag);
    }

    /**
     * Generate the page.
     *
     * @throws ReflectionException
     */
    public function generate()
    {
        if (!$this->hasFlag(Page::HTML_PARTIAL)) {
            /** @var Menu $menuClass */
            $menuClass = Config::getGlobal('menu');
            $menuObj = $menuClass::getInstance();
            $username = $this->m_username ?: $this->m_user['name'];

            if (Config::getGlobal('menu_show_user') && $username) {
                $this->addUsernameNavbarLink($menuObj, $username);
            }

            if (Config::getGlobal('menu_show_logout_link') && $username) {
                $this->addLogoutNavbarLink($menuObj);
            }

            $this->m_adminLte->setNavBarDevMode($_SERVER['APP_ENV'] !== 'prod');

            // render navbar
            $navbarTpl = Config::getGlobal('menu_hide_sidebar') === true ? 'navbar_without_sidebar' : 'navbar';
            $navbar = $this->m_ui->render("menu/$navbarTpl.tpl", [
                'menu' => $menuObj->getMenu(), // formatted smarty menu as ['left', 'right','sidebar']
                'main_header_classes' => $this->m_adminLte->getMainHeaderClasses(),
                'dispatcher' => Config::getGlobal('dispatcher'),
                'brand_logo' => Config::getGlobal('brand_logo')
            ]);

            $this->m_page->addContent($navbar);

            // render sidebar
            $sidebar = $this->m_ui->render("menu/sidebar.tpl", [
                'title' => ($this->m_title != '' ?: Tools::atktext('app_title')),
                'app_title' => Tools::atktext('app_title'),
                'menu' => $menuObj->getMenu(), // formatted smarty menu as ['left', 'right','sidebar']
                'nav_sidebar_classes' => $this->m_adminLte->getNavSidebarClasses(),
                'sidebar_classes' => $this->m_adminLte->getSidebarClasses(),
                'brand_text_style' => $this->m_adminLte->getBrandTextStyle(),
                'menu_accordion' => $this->m_adminLte->isMenuAccordion(),
                'collapse_sidebar' => $this->m_adminLte->isCollapsedSidebar(),
                'dispatcher' => Config::getGlobal('dispatcher'),
                'brand_logo' => Config::getGlobal('brand_logo')
            ]);

            $this->m_page->addContent($sidebar);

            if (Config::getGlobal('session_autorefresh')) {
                $this->m_page->register_scriptcode(SessionManager::getSessionAutoRefreshJs());
            }
        }

        $this->atkGenerateDispatcher();

        $title = $this->m_title != '' ?: null;
        $bodyprops = $this->m_extrabodyprops != '' ?: null;
        $headers = $this->m_extraheaders != '' ?: null;

        $content = $this->m_page->render($title, $this->m_flags, $bodyprops, $headers, $this->m_adminLte->getGeneralBodyClasses());

        $this->m_output->output($content);
        $this->m_output->outputFlush();
    }

    /**
     * Set the title of the page.
     *
     * @param string $title
     */
    public function setTitle(string $title)
    {
        $this->m_title = $title;
    }

    /**
     * Set the extra body properties of the page.
     *
     * @param string $extrabodyprops
     */
    public function setBodyprops(string $extrabodyprops)
    {
        $this->m_extrabodyprops = $extrabodyprops;
    }

    /**
     * Set the extra headers of the page.
     *
     * @param string $extraheaders
     */
    public function setExtraheaders(string $extraheaders)
    {
        $this->m_extraheaders = $extraheaders;
    }

    /**
     * Set the username.
     *
     * @param string $username
     */
    public function setUsername(string $username)
    {
        $this->m_username = $username;
    }

    /**
     * Generate the dispatcher.
     */
    public function atkGenerateDispatcher()
    {
        global $ATK_VARS;
        $session = &SessionManager::getSession();

        if ($session['login'] != 1) {
            // no nodetype passed, or session expired

            $destination = '';
            if (isset($ATK_VARS['atknodeuri']) && isset($ATK_VARS['atkaction'])) {
                $destination = '&atknodeuri=' . $ATK_VARS['atknodeuri'] . '&atkaction=' . $ATK_VARS['atkaction'];
                if (isset($ATK_VARS[Node::PARAM_ATKSELECTOR])) {
                    $destination .= '&' . Node::PARAM_ATKSELECTOR . '=' . $ATK_VARS[Node::PARAM_ATKSELECTOR];
                }
            }

            $box = $this->m_ui->renderBox(array(
                'title' => Tools::atktext('title_session_expired'),
                'content' => '<br><br>' . Tools::atktext('explain_session_expired') . '<br><br><br><br>
                                           <a href="' . Config::getGlobal('dispatcher') . '?atklogout=true' . $destination . '" target="_top">' . Tools::atktext('relogin') . '</a><br><br>',
            ));

            $this->m_page->addContent($box);

            $this->m_output->output($this->m_page->render(Tools::atktext('title_session_expired'), true));

        } else {

            // Create node
            if (isset($ATK_VARS['atknodeuri'])) {
                $node = $this->atk->atkGetNode($ATK_VARS['atknodeuri']);
                $this->loadDispatchPage($ATK_VARS, $node);
            } else {
                if (is_array($this->m_defaultDestination)) {
                    // using dispatch_url to redirect to the node
                    $isIndexed = array_values($this->m_defaultDestination) === $this->m_defaultDestination;
                    if ($isIndexed) {
                        $destination = Tools::dispatch_url($this->m_defaultDestination[0], $this->m_defaultDestination[1],
                            $this->m_defaultDestination[2] ? $this->m_defaultDestination[2] : array());
                    } else {
                        $destination = Tools::dispatch_url($this->m_defaultDestination['atknodeuri'], $this->m_defaultDestination['atkaction'],
                            $this->m_defaultDestination[0] ? $this->m_defaultDestination[0] : array());
                    }
                    header('Location: ' . $destination);
                    exit;
                } else {
                    $this->renderContent();
                }
            }
        }
    }

    public function renderContent()
    {
        $content = $this->getContent();
        $this->m_page->addContent($content);
    }

    public function getContent()
    {
        return $this->m_ui->renderBox([
            'title' => Tools::atktext('app_shorttitle'),
            'content' => Tools::atktext('app_description'),
            'footer' => Footer::getInstance()->render(),
        ], 'index_page');
    }

    /**
     * Set the default destination.
     *
     * @param array $destination The default destination
     */
    public function setDefaultDestination(array $destination)
    {
        $this->m_defaultDestination = $destination;
    }

    /**
     * Does the actual loading of the dispatch page
     * And adds it to the page for the dispatch() method to render.
     *
     * @param array $postvars The request variables for the node.
     * @param Node $node
     */
    public function loadDispatchPage($postvars, Node $node)
    {
        $node->m_postvars = $postvars;
        $node->m_action = $postvars['atkaction'];
        if (isset($postvars['atkpartial'])) {
            $node->m_partial = $postvars['atkpartial'];
        }

        $page = $node->getPage();
        $page->setTitle(Tools::atktext('app_shorttitle') . ' - ' . $node->getUi()->title($node->m_module, $node->m_type, $node->m_action));

        if ($node->allowed($node->m_action)) {
            $secMgr = SecurityManager::getInstance();
            $secMgr->logAction($node->m_type, $node->m_action);
            $node->callHandler($node->m_action);
            $id = '';

            if (isset($node->m_postvars[Node::PARAM_ATKSELECTOR]) && is_array($node->m_postvars[Node::PARAM_ATKSELECTOR])) {
                $atkSelectorDecoded = [];

                foreach ($node->m_postvars[Node::PARAM_ATKSELECTOR] as $rowIndex => $selector) {
                    list(, $pk) = explode('=', $selector);
                    $atkSelectorDecoded[] = $pk;
                    $id = implode(',', $atkSelectorDecoded);
                }
            } else {
                list(, $id) = explode('=', Tools::atkArrayNvl($node->m_postvars, Node::PARAM_ATKSELECTOR, '='));
            }

            $page->register_hiddenvars(array(
                'atknodeuri' => $node->m_module . '.' . $node->m_type,
                Node::PARAM_ATKSELECTOR => str_replace("'", '', $id),
            ));
        } else {
            $page->addContent($this->accessDeniedPage($node->getType()));
        }
    }

    /**
     * Render a generic access denied page.
     *
     * @param string $nodeType
     * @return string A complete html page with generic access denied message.
     */
    private function accessDeniedPage(string $nodeType): string
    {
        $content = '<br><br>' . Tools::atktext('error_node_action_access_denied', '', $nodeType) . '<br><br><br>';

        $blocks = [
            $this->m_ui->renderBox([
                'title' => Tools::atktext('access_denied'),
                'content' => $content,
            ]),
        ];

        return $this->m_ui->render('actionpage.tpl', [
            'blocks' => $blocks,
            'title' => Tools::atktext('access_denied'),
            'footer' => Footer::getInstance()->render()
        ]);
    }

    /**
     * @return Page|null
     */
    public function getPage(): ?Page
    {
        return $this->m_page;
    }

    /**
     * @return Ui|null
     */
    public function getUi(): ?Ui
    {
        return $this->m_ui;
    }

    /**
     * @throws ReflectionException
     */
    private function addUsernameNavbarLink(MenuBase $menuObj, string $username)
    {
        $url = $this->generateUserUrl();
        $menuObj->addMenuItem(MenuBase::ATK_MENU_USERNAME_PREFIX . $username,
            $url, 'main', true, 0, '', '', true, MenuBase::MENU_NAV_RIGHT,
            null, false, null, '', null, false,
            Tools::atktext('user'), Item::TOOLTIP_PLACEMENT_BOTTOM,
            null, '', true
        );
    }

    /**
     * @throws ReflectionException
     */
    private function addLogoutNavbarLink(MenuBase $menuObj)
    {
        $menuObj->addMenuItem(Tools::atktext('logout'),
            Config::getGlobal('dispatcher') . '?atklogout=1', 'main', true, 0,
            '', '', false, MenuBase::MENU_NAV_RIGHT, null, false, MenuBase::ICON_LOGOUT,
            '', null, true, Tools::atktext('logout'), Item::TOOLTIP_PLACEMENT_BOTTOM,
            null, ''
        );
    }

    private function generateUserUrl(): string
    {
        $url = '';
        if (Config::getGlobal('menu_enable_user_link')) {
            $userTable = Config::getGlobal('auth_usertable');
            $userPk = Config::getGlobal('auth_userpk');
            if (!isset($this->m_user[$userPk])) {
                // logged user is atk administrator
                return '';
            }
            $userId = $this->m_user[$userPk];
            $userAtkSelector = "$userTable.$userPk=$userId";
            $userNode = Atk::getInstance()->atkGetNode(Config::getGlobal('auth_usernode'));
            $userRecord = $userNode->select($userAtkSelector)->getFirstRow();
            $action = $userNode->allowed('edit', $userRecord) ? 'edit' : 'view';
            $url = SessionManager::getInstance()->sessionUrl(
                Tools::dispatch_url($userNode->atkNodeUri(), $action, [Node::PARAM_ATKSELECTOR => $userAtkSelector]),
                SessionManager::SESSION_NEW
            );
        }
        return $url;
    }
}
