<?php

namespace Sintattica\Atk\Ui;

use Exception;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Atk;

/**
 * Utility class for rendering boxes, lists, tabs or other templates.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class Ui
{
    const ACTION_FORM_BUTTON_POSITION_LEFT = 'left';
    const ACTION_FORM_BUTTON_POSITION_CENTER = 'center';
    const ACTION_FORM_BUTTON_POSITION_RIGHT = 'right';

    /*
     * Smarty instance, initialised by constructor
     * @access private
     * @var SmartyProvider
     */
    public $m_smarty = null;

    /**
     * Ui constructor, initialises Smarty and Theme instance.
     * @throws Exception
     */
    public function __construct()
    {
        $this->m_smarty = SmartyProvider::getInstance();
    }

    /**
     * get a singleton instance of the Ui class.
     *
     * @return Ui
     */
    public static function getInstance(): ?Ui
    {
        static $s_instance = null;

        if ($s_instance == null) {
            Tools::atkdebug('Creating a new Ui instance');
            $s_instance = new self();
        }

        return $s_instance;
    }

    /**
     * Renders action templates
     * Currently only the view action is implemented.
     *
     * @param string $action the action for which to render the template
     * @param array $vars the template variables
     * @param string $module the name of the module requesting to render a template
     *
     * @return string the rendered template
     * @throws \Smarty\Exception
     */
    public function renderAction($action, $vars, $module = ''): string
    {
        $tpl = "action_$action.tpl";
        if (!$this->m_smarty->templateExists($tpl)) {
            // no specific tpl for this action
            $tpl = "action.tpl";
        }

        $this->addActionFormButtonsPositionClassVar($vars);

        return $this->render($tpl, $vars, $module);
    }

    /**
     * Renders a list template.
     *
     * @param string $action not used (deprecated?)
     * @param array $vars the variables with which to parse the list template
     * @param string $module the name of the module requesting to render a template
     *
     * @return string rendered list
     */
    public function renderList($action = '', $vars = [], $module = ''): string
    {
        return $this->render('list.tpl', $vars, $module);
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
     *
     * @return string rendered box
     */
    public function renderBox($vars, $name = '', $module = ''): string
    {
        if ($name) {
            return $this->render($name . '.tpl', $vars);
        }

        return $this->render('box.tpl', $vars, $module);
    }

    /**
     * Renders a tabulated template
     * Registers some scriptcode for dhtml tab.
     *
     * @param array $vars the variables with which to render the template
     * @param string $module the name of the module requesting to render a template
     *
     * @return string the rendered template
     */
    public function renderTabs(array $vars, $module = ''): string
    {
        return $this->render('tabs.tpl', $vars, $module);
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
     * @return string rendered template
     * @throws \Smarty\Exception
     */
    public function render(string $name, array $vars = [], string $module = ''): string
    {
        $result = $this->renderSmarty($name, $vars);

        if (Config::getGlobal('debug') >= 3) {
            $result = "\n<!-- START [$name] -->\n" . $result . "\n<!-- END [$name] -->\n";
        }

        return $result;
    }

    /**
     * Return the title to render.
     *
     * @param string $module the module in which to look
     * @param string $nodetype the nodetype of the action
     * @param string $action the action that we are trying to find a title for
     * @param bool $actiononly wether or not to return a name of the node
     *                           if we couldn't find a specific title
     *
     * @return string the title for the action
     */
    public function title($module, $nodetype, $action = null, $actiononly = false): string
    {
        if ($module == null || $nodetype == null) {
            return '';
        }
        $atk = Atk::getInstance();

        return $atk->atkGetNode($module . '.' . $nodetype)->nodeTitle($action, $actiononly);
    }

    public function addActionFormButtonsPositionClassVar(array &$tplVars)
    {
        $actionFormButtonPosition = Config::getGlobal('action_form_buttons_position');
        switch ($actionFormButtonPosition) {
            case self::ACTION_FORM_BUTTON_POSITION_LEFT:
                $actionFormButtonPositionClass = 'justify-content-end';
                break;
            case self::ACTION_FORM_BUTTON_POSITION_CENTER:
                $actionFormButtonPositionClass = 'justify-content-center';
                break;
            default:
                $actionFormButtonPositionClass = 'justify-content-start';
        }
        $tplVars['action_form_buttons_position_class'] = $actionFormButtonPositionClass;
    }

    /**
     * Render Smarty-based template.
     *
     * @param string $path template path
     * @param array $vars template variables
     *
     * @return string rendered template
     * @throws \Smarty\Exception
     */
    private function renderSmarty(string $path, array $vars = []): string
    {
        // First clear any existing smarty var.
        $this->m_smarty->clearAllAssign();

        $this->m_smarty->assign($vars);
        return $this->m_smarty->fetch($path);
    }
}
