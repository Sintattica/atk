<?php namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Menu;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Node;

/**
 * Class that generates an index page.
 * @package atk
 * @subpackage ui
 */
class IndexPage
{
    /**
     * @var Page
     */
    var $m_page;

    /**
     * @var Ui
     */
    var $_uim;

    /**
     * @var Output
     */
    var $m_output;

    /**
     * @var array
     */
    var $m_user;
    var $m_topsearchpiece;
    var $m_topcenterpiecelinks;
    var $m_title;
    var $m_extrabodyprops;
    var $m_extraheaders;
    var $m_username;
    var $m_defaultDestination;
    var $m_flags;

    private $atk;

    /**
     * Hide top / menu?
     *
     * @var boolean
     */
    private $m_noNav;

    /**
     * Constructor
     * $atk Atk
     * @return IndexPage
     */
    function __construct(Atk $atk)
    {
        global $ATK_VARS;
        $this->atk = $atk;
        $this->m_page = Page::getInstance();
        $this->m_ui = Ui::getInstance();
        $this->m_output = Output::getInstance();
        $this->m_user = SecurityManager::atkGetUser();
        $this->m_flags = array_key_exists("atkpartial", $ATK_VARS) ? Page::HTML_PARTIAL : Page::HTML_STRICT;
        $this->m_noNav = isset($ATK_VARS['atknonav']);
        $this->m_extraheaders = $this->m_ui->render('index_meta.tpl');
    }

    /**
     * Does the IndexPage has this flag?
     *
     * @param integer $flag The flag
     * @return Boolean
     */
    function hasFlag($flag)
    {
        return Tools::hasFlag($this->m_flags, $flag);
    }

    /**
     * Generate the indexpage
     *
     */
    function generate()
    {
        if (!$this->hasFlag(Page::HTML_PARTIAL) && !$this->m_noNav) {
            $this->atkGenerateTop();
        }

        $this->atkGenerateDispatcher();

        $this->m_output->output(
            $this->m_page->render(
                $this->m_title != "" ? $this->m_title : null, $this->m_flags, $this->m_extrabodyprops != ""
                ? $this->m_extrabodyprops : null, $this->m_extraheaders != ""
                ? $this->m_extraheaders : null
            )
        );
        $this->m_output->outputFlush();
    }

    /**
     * Generate the menu
     *
     */
    function atkGenerateMenu()
    {
        /* general menu stuff */
        /* load menu layout */
        $menu = Menu::getInstance();

        if (is_object($menu)) {
            $this->m_page->addContent($menu->getMenu());
        } else {
            Tools::atkerror("no menu object created!");
        }
    }

    /**
     * Generate the top with login text, logout link, etc.
     *
     */
    function atkGenerateTop()
    {
        $logoutLink = Config::getGlobal('dispatcher') . '?atklogout=1';


        /* load menu layout */
        $menuObj = Menu::getInstance();
        $menu = null;

        if (is_object($menuObj)) {
            $menu = $menuObj->getMenu();
        }

        $top = $this->m_ui->renderBox(array(
            "logintext" => Tools::atktext("logged_in_as"),
            "logouttext" => ucfirst(Tools::atktext("logout", "atk")),
            "logoutlink" => $logoutLink,
            "logouttarget" => "_top",
            "centerpiece_links" => $this->m_topcenterpiecelinks,
            "searchpiece" => $this->m_topsearchpiece,
            "title" => ($this->m_title != "" ? $this->m_title : Tools::atktext("app_title")),
            "app_title" => Tools::atktext("app_title"),
            "user" => ($this->m_username ? $this->m_username : $this->m_user["name"]),
            "fulluser" => $this->m_user,
            "menu" => $menu
        ), "top");
        $this->m_page->addContent($top);
    }

    /**
     * Set the top center piece links
     *
     * @param string $centerpiecelinks
     */
    function setTopCenterPieceLinks($centerpiecelinks)
    {
        $this->m_topcenterpiecelinks = $centerpiecelinks;
    }

    /**
     * Set the top search piece
     *
     * @param string $searchpiece
     */
    function setTopSearchPiece($searchpiece)
    {
        $this->m_topsearchpiece = $searchpiece;
    }

    /**
     * Set the title of the page
     *
     * @param string $title
     */
    function setTitle($title)
    {
        $this->m_title = $title;
    }

    /**
     * Set the extra body properties of the page
     *
     * @param string $extrabodyprops
     */
    function setBodyprops($extrabodyprops)
    {
        $this->m_extrabodyprops = $extrabodyprops;
    }

    /**
     * Set the extra headers of the page
     *
     * @param string $extraheaders
     */
    function setExtraheaders($extraheaders)
    {
        $this->m_extraheaders = $extraheaders;
    }

    /**
     * Set the username
     *
     * @param string $username
     */
    function setUsername($username)
    {
        $this->m_username = $username;
    }

    /**
     * Generate the dispatcher
     *
     */
    function atkGenerateDispatcher()
    {
        global $ATK_VARS;
        $session = &SessionManager::getSession();


        if ($session["login"] != 1) {
            // no nodetype passed, or session expired

            $destination = "";
            if (isset($ATK_VARS['atknodeuri']) && isset($ATK_VARS["atkaction"])) {
                $destination = "&atknodeuri=" . $ATK_VARS['atknodeuri'] . "&atkaction=" . $ATK_VARS["atkaction"];
                if (isset($ATK_VARS["atkselector"])) {
                    $destination .= "&atkselector=" . $ATK_VARS["atkselector"];
                }
            }

            $box = $this->m_ui->renderBox(array(
                "title" => Tools::atktext("title_session_expired"),
                "content" => '<br><br>' . Tools::atktext("explain_session_expired") . '<br><br><br><br>
                                           <a href="' . Config::getGlobal('dispatcher') . '?atklogout=true' . $destination . '" target="_top">' . Tools::atktext("relogin") . '</a><br><br>'
            ));

            $this->m_page->addContent($box);

            $this->m_output->output($this->m_page->render(Tools::atktext("title_session_expired"), true));
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
                        $destination = Tools::dispatch_url($this->m_defaultDestination[0],
                            $this->m_defaultDestination[1],
                            $this->m_defaultDestination[2] ? $this->m_defaultDestination[2] : array());
                    } else {
                        $destination = Tools::dispatch_url($this->m_defaultDestination["atknodeuri"],
                            $this->m_defaultDestination["atkaction"],
                            $this->m_defaultDestination[0] ? $this->m_defaultDestination[0] : array());
                    }
                    header('Location: ' . $destination);
                    exit;
                } else {
                    $box = $this->m_ui->renderBox(array(
                        "title" => Tools::atktext("app_shorttitle"),
                        "content" => Tools::atktext("app_description")
                    ));

                    $box = '<div class="container-fluid">' . $box . '</div>';

                    $this->m_page->addContent($box);
                }
            }
        }
    }

    /**
     * Set the default destination
     *
     * @param string $destination The default destination
     */
    function setDefaultDestination($destination)
    {
        if (is_array($destination)) {
            $this->m_defaultDestination = $destination;
        }
    }

    /**
     * Does the actual loading of the dispatch page
     * And adds it to the page for the dispatch() method to render.
     * @param array $postvars The request variables for the node.
     * @param Node $node
     */
    function loadDispatchPage($postvars, Node $node)
    {

        $node->m_postvars = $postvars;
        $node->m_action = $postvars['atkaction'];
        if (isset($postvars["atkpartial"])) {
            $node->m_partial = $postvars["atkpartial"];
        }

        $page = $node->getPage();
        $page->setTitle(Tools::atktext('app_shorttitle') . " - " . $node->getUi()->title($node->m_module,
                $node->m_type, $node->m_action));

        if ($node->allowed($node->m_action)) {
            $secMgr = SecurityManager::getInstance();
            $secMgr->logAction($node->m_type, $node->m_action);
            $node->callHandler($node->m_action);
            $id = '';

            if (isset($node->m_postvars["atkselector"]) && is_array($node->m_postvars["atkselector"])) {
                $atkSelectorDecoded = array();

                foreach ($node->m_postvars["atkselector"] as $rowIndex => $selector) {
                    list($selector, $pk) = explode("=", $selector);
                    $atkSelectorDecoded[] = $pk;
                    $id = implode(',', $atkSelectorDecoded);
                }
            } else {
                list(, $id) = explode("=", Tools::atkArrayNvl($node->m_postvars, "atkselector", "="));
            }

            $page->register_hiddenvars(array(
                "atknodeuri" => $node->m_module . "." . $node->m_type,
                "atkselector" => str_replace("'", "", $id)
            ));
        } else {
            $page->addContent($this->accessDeniedPage($node->getType()));
        }
    }

    /**
     * Render a generic access denied page.
     * @param string $nodeType
     * @return string A complete html page with generic access denied message.
     */
    function accessDeniedPage($nodeType)
    {

        $content = "<br><br>" . Tools::atktext("error_node_action_access_denied", "", $nodeType) . "<br><br><br>";

        $blocks = [
            $this->m_ui->renderBox(array(
                "title" => Tools::atktext('access_denied'),
                "content" => $content
            ), 'dispatch')
        ];

        return $this->m_ui->render("action.tpl", array("blocks"=>$blocks, "title"=> Tools::atktext('access_denied')));
    }
}
