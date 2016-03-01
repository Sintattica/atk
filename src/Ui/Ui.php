<?php namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Atk;

/**
 * Utility class for rendering boxes, lists, tabs or other templates.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage ui
 */
class Ui
{
    /**
     * Smarty instance, initialised by constructor
     * @access private
     * @var SmartyProvider
     */
    var $m_smarty = null;


    /**
     * Ui constructor, initialises Smarty and Theme instance
     */
    function __construct()
    {
        $this->m_smarty = SmartyProvider::getInstance();
    }

    /**
     * get a singleton instance of the Ui class.
     *
     * @return Ui
     */
    static public function &getInstance()
    {
        static $s_instance = null;

        if ($s_instance == null) {
            Tools::atkdebug("Creating a new Ui instance");
            $s_instance = new self();
        }

        return $s_instance;
    }

    /**
     * Renders action templates
     * Currently only the view action is implemented
     * @param string $action the action for which to render the template
     * @param array $vars the template variables
     * @param string $module the name of the module requesting to render a template
     * @return String the rendered template
     */
    function renderAction($action, $vars, $module = "")
    {
        return $this->render("action_$action.tpl", $vars, $module);
    }

    /**
     * Renders a list template
     * @param string $action not used (deprecated?)
     * @param array $vars the variables with which to parse the list template
     * @param string $module the name of the module requesting to render a template
     * @return string rendered list
     */
    function renderList($action = '', $vars, $module = "")
    {
        return $this->render("list.tpl", $vars, $module);
    }

    /**
     * Renders a box with Smarty template.
     * Call with a $name variable to provide a
     * better default than "box.tpl".
     *
     * For instance, calling renderBox($smartyvars, "menu")
     * will make it search for a menu.tpl first and use that
     * if it's available, otherwise it will just use box.tpl
     *
     * @param array $vars the variables for the template
     * @param string $name The name of the template
     * @param string $module the name of the module requesting to render a template
     * @return string rendered box
     */
    function renderBox($vars, $name = "", $module = "")
    {
        if ($name) {
            return $this->render($name . ".tpl", $vars);
        }
        return $this->render("box.tpl", $vars, $module);
    }

    /**
     * Renders a tabulated template
     * Registers some scriptcode for dhtml tab
     * @param array $vars the variables with which to render the template
     * @param string $module the name of the module requesting to render a template
     * @return String the rendered template
     */
    function renderTabs($vars, $module = "")
    {
        $page = Page::getInstance();
        $page->register_script(Config::getGlobal("assets_url") . "javascript/tools.js");
        return $this->render("tabs.tpl", $vars, $module);
    }

    /**
     * Renders the given template.
     *
     * If the name ends with ".php" PHP will be used to render the template. If
     * the name ends with ".tpl" and a file with the extension ".tpl.php" exists
     * PHP will be used, otherwise Smarty will be used to render the template.
     *
     * @param string $name the name of the template to render
     * @param array $vars the variables with which to render the template
     * @param string $module the name of the module requesting to render a template
     *
     * @return String rendered template
     */
    public function render($name, $vars = array(), $module = "")
    {
        $path = Config::getGlobal('template_dir') . $name;
        $result = $this->renderSmarty($path, $vars);

        if (Config::getGlobal('debug') >= 3) {
            $result = "\n<!-- START [{$path}] -->\n" .
                $result .
                "\n<!-- END [{$path}] -->\n";
        }

        return $result;
    }

    /**
     * Render Smarty-based template.
     *
     * @param string $path template path
     * @param array $vars template variables
     *
     * @return string rendered template
     */
    private function renderSmarty($path, $vars)
    {
        // First clear any existing smarty var.
        $this->m_smarty->clearAllAssign();

        $this->m_smarty->assign($vars);
        $res = $this->m_smarty->fetch($path);
        return $res;
    }


    /**
     * Return the title to render
     *
     * @param string $module the module in which to look
     * @param string $nodetype the nodetype of the action
     * @param string $action the action that we are trying to find a title for
     * @param bool $actiononly wether or not to return a name of the node
     *                          if we couldn't find a specific title
     * @return String the title for the action
     */
    function title($module, $nodetype, $action = null, $actiononly = false)
    {
        if ($module == null || $nodetype == null) {
            return "";
        }
        $atk = Atk::getInstance();
        return $this->nodeTitle($atk->atkGetNode($module . '.' . $nodetype), $action, $actiononly);
    }

    /**
     * This function returns a suitable title text for an action.
     * Example: echo $ui->title("users", "employee", "edit"); might return:
     *          'Edit an existing employee'
     * @param Node $node the node to get the title from
     * @param string $action the action that we are trying to find a title for
     * @param bool $actiononly wether or not to return a name of the node
     *                          if we couldn't find a specific title
     * @return String the title for the action
     */
    function nodeTitle($node, $action = null, $actiononly = false)
    {
        if ($node == null) {
            return "";
        }

        $nodetype = $node->m_type;
        $module = $node->m_module;

        if ($action != null) {
            $keys = array(
                'title_' . $module . '_' . $nodetype . '_' . $action,
                'title_' . $nodetype . '_' . $action,
                'title_' . $action
            );

            $label = $node->text($keys, null, "", "", true);
        } else {
            $label = "";
        }

        if ($label == "") {
            $actionKeys = array(
                'action_' . $module . '_' . $nodetype . '_' . $action,
                'action_' . $nodetype . '_' . $action,
                'action_' . $action,
                $action
            );

            if ($actiononly) {
                return $node->text($actionKeys);
            } else {
                $keys = array('title_' . $module . '_' . $nodetype, 'title_' . $nodetype, $nodetype);
                $label = $node->text($keys);
                if ($action != null) {
                    $label .= " - " . $node->text($actionKeys);
                }
            }
        }
        return $label;
    }
}


