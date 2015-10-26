<?php namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Module;
use Sintattica\Atk\Wizard\Wizard;
use Sintattica\Atk\Wizard\WizardPanel;

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
     * Theme instance, initialised by constructor
     * @access private
     * @var Theme
     */
    var $m_theme = null;

    /**
     * Ui constructor, initialises Smarty and Theme instance
     */
    function __construct()
    {
        $this->m_theme = Theme::getInstance();
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
     * @param String $action the action for which to render the template
     * @param array $vars the template variables
     * @param string $module the name of the module requesting to render a template
     * @return String the rendered template
     */
    function renderAction($action, $vars, $module = "")
    {
        // todo.. action specific templates
        $tpl = "action_$action.tpl";
        if ($this->m_theme->tplPath($tpl) == "") { // no specific theme for this action
            $tpl = "action.tpl";
        }
        return $this->render($tpl, $vars, $module);
    }

    /**
     * Renders a list template
     * @param String $action not used (deprecated?)
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
        if ($name && file_exists($this->m_theme->tplPath($name . ".tpl"))) {
            return $this->render($name . ".tpl", $vars);
        }
        return $this->render("box.tpl", $vars, $module);
    }

    /**
     * Renders the insides of a dialog.
     *
     * @param array $vars template variables
     * @param string $module the name of the module requesting to render a template
     * @return string rendered dialog
     */
    function renderDialog($vars, $module = "")
    {
        return $this->render("dialog.tpl", $vars, $module);
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
     * @param String $name the name of the template to render
     * @param array $vars the variables with which to render the template
     * @param String $module the name of the module requesting to render a template
     *
     * @return String rendered template
     */
    public function render($name, $vars = array(), $module = "")
    {
        $path = $this->templatePath($name, $module);
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
        $this->m_smarty->clear_all_assign();

        // Then set some defaults that we need in all templates.
        $this->m_smarty->assign("themedir", $this->m_theme->themeDir());

        $this->m_smarty->assign("atkroot", Config::getGlobal("atkroot"));
        $this->m_smarty->assign("application_dir", Config::getGlobal("application_dir"));

        $this->m_smarty->assign($vars);

        // Smarty fetches templates relative from the template_dir setting.
        // Since that is an application directory, and themes reside in
        // a different directory, we have to hack the template_dir
        // setting.
        $old = $this->m_smarty->template_dir;

        // disable smarty caching for ui purposes.
        $old_caching = $this->m_smarty->caching;
        $this->m_smarty->caching = false;
        $this->m_smarty->template_dir = "./"; // current dir, because tplname already contains full relative path.
        $res = $this->m_smarty->fetch($path);
        $this->m_smarty->template_dir = $old;
        $this->m_smarty->caching = $old_caching;

        return $res;
    }

    /**
     * This function returns a complete themed path for a given template.
     * This is a convenience method, which calls the tplPath method on
     * the theme instance. However, if the template name contains a '/',
     * we assume the full template path is already given and we simply
     * return it.
     *
     * @param String $template The filename (without path) of the template
     *                          for which you want to complete the path.
     * @param String $module The name of the module requesting to render a template
     * @return String the template path
     */
    function templatePath($template, $module = "")
    {
        if (strpos($template, "/") === false) {
            // lookup template in theme.
            $template = $this->m_theme->tplPath($template, $module);
        }

        return $template;
    }

    /**
     * This function returns a complete themed path for a given stylesheet.
     * This is a convenience method, which calls the stylePath method on
     * the theme instance.
     *
     * @param String $style The filename (without path) of the stylesheet for
     *                      which you want to complete the path.
     * @param String $module the name of the module requesting the style path
     * @return String the path of the style
     */
    function stylePath($style, $module = "")
    {
        return $this->m_theme->stylePath($style, $module);
    }

    /**
     * Return the title to render
     *
     * @param String $module the module in which to look
     * @param String $nodetype the nodetype of the action
     * @param String $action the action that we are trying to find a title for
     * @param bool $actiononly wether or not to return a name of the node
     *                          if we couldn't find a specific title
     * @return String the title for the action
     */
    function title($module, $nodetype, $action = null, $actiononly = false)
    {
        if ($module == null || $nodetype == null) {
            return "";
        }
        return $this->nodeTitle(Module::atkGetNode($module . '.' . $nodetype), $action, $actiononly);
    }

    /**
     * This function returns a suitable title text for an action.
     * Example: echo $ui->title("users", "employee", "edit"); might return:
     *          'Edit an existing employee'
     * @param Node $node the node to get the title from
     * @param String $action the action that we are trying to find a title for
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

    /**
     * This function returns a suitable title text for an Wizardpanel.
     * Example: echo $ui->title("departmentwizard", "employee", "add"); might return:
     *          'Departmen wizard - Add employees - Step 2 of 3'
     * @param Wizard $wizard the wizard object
     * @param WizardPanel $panel the panel object
     * @param String $action the atk action that we are trying execute in the panel
     * @return String the title for this wizardpanel
     */
    function getWizardTitle(Wizard $wizard, WizardPanel $panel, $action = null)
    {
        if ($wizard == null) {
            return "";
        }

        $module = $wizard->getModuleName();
        $wizardName = $wizard->getName();
        $panelName = $panel->getPanelName();

        $keys = array(
            'title_' . $module . '_' . $wizardName,
            'title_' . $wizardName
        );
        $wizardTitle = Tools::atktext($keys, null, "", "", true);

        $keys = array(
            'title_' . $module . '_' . $panelName . '_' . $action,
            'title_' . $panelName . '_' . $action
        );

        $panelTitle = Tools::atktext($keys, null, "", "", true);

        if ($wizard->getWizardAction() !== 'finish') {
            $status = Tools::atktext("Step") . " " . ($wizard->m_currentPanelIndex + 1) . " " . Tools::atktext("of") . " " . count($wizard->m_panelList);
        } else {
            $status = Tools::atktext("finished");
        }
        $label = $wizardTitle . " - " . $panelTitle . " - " . $status;

        return $label;
    }

}


