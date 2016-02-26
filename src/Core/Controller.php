<?php namespace Sintattica\Atk\Core;

use Sintattica\Atk\Handlers\ActionHandler;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Ui\Ui;
use Sintattica\Atk\Ui\Output;

/**
 * The Controller class
 *
 * @author Maurice Maas <maurice@ibuildings.nl>
 * @package atk
 */
class Controller
{
    /**
     * The name of the wizard.
     * @access protected
     * @var String
     */
    var $m_name;

    /**
     * The module of the wizard.
     * @access protected
     * @var String
     */
    var $m_module_name;

    /**
     * Reference to the instance of currently selected Node
     *
     * @var Node
     */
    var $m_node = null;

    /**
     * The postvars in this pageload
     *
     * @var array Key/value
     */
    var $m_postvars = null;

    /**
     * The file to use when creating url's
     *
     * @var string filename
     */
    var $m_php_file = "";

    /**
     * By this property is determined if the output of the
     * handleRequest method should be returned as a string
     * or the output should be outputted by Output.
     *
     * @var Bool
     */
    var $m_return_output = false;

    /**
     * Key/value Array containing which are be send als post or get vars
     * @access private
     * @var array
     */
    var $m_hidden_vars = array();

    /**
     * Constructor of Controller
     *
     * @return Controller object
     */
    function __construct()
    {
        $sessionManager = SessionManager::getInstance();
        if (is_object($sessionManager)) {
            $ControllerClass = $sessionManager->stackVar("atkcontroller");


            //Its not so nice to use the getNodeModule and getNodeType functions,
            //because the name suggests they work with atkNodes. But they also do
            //the job when using other class names.
            $this->m_name = Module::getNodeType($ControllerClass);
            $this->m_module_name = Module::getNodeModule($ControllerClass);
        }
    }

    /**
     * Create instance of controller (determined by class variable) if not yet created, return instance of Controller
     *
     * @access private
     * @param string $class
     * @param boolean $replace
     * @return Controller instance of controller
     */
    public static function &_instance($class = "", $replace = false)
    {
        static $s_object = null;
        if (!is_object($s_object) || $replace) {
            global $ATK_VARS;
            if (empty($class) && isset($ATK_VARS['atkcontroller'])) {
                $class = $ATK_VARS['atkcontroller'];
            }
            if (empty($class)) {
                $class = __CLASS__;
            }


            //We save the controller in stack, so the controller constructor
            //can store the Controller name and module. It is also saved for other
            //atk levels if we move down the stack.
            $sessionManager = SessionManager::getInstance();
            if (is_object($sessionManager)) {
                $sessionManager->stackVar("atkcontroller", $class);
            }

            $s_object = new $class();
        }
        return $s_object;
    }

    /**
     * Return the one and only instance of the class
     *
     * @return Controller
     */
    public static function &getInstance()
    {
        $object = Controller::_instance();
        return $object;
    }

    /**
     * Return the one and only instance of the class
     *
     * @param string $controller The class of the controller to instanciate
     * @return object
     */
    function &createInstance($controller)
    {
        Tools::atkdebug("Controller::createInstance() " . $controller);
        //First check if another controller is active. If so make sure this
        //controller will use Output to return output
        $currentController = Controller::getInstance();
        if (is_object($currentController)) {
            $currentController->setReturnOutput(true);
        }

        //Now create new controller
        $controller = Controller::_instance($controller, true);
        return $controller;
    }

    /**
     * This is the wrapper method for all http requests on a node.
     *
     * The method looks at the atkaction from the postvars and determines what
     * should be done. If possible, it instantiates actionHandlers for
     * handling the actual action.
     *
     * @param array $postvars The request variables for the node.
     * @param string $flags Render flags (see class Page).
     * @return string
     */
    function handleRequest($postvars, $flags = null)
    {
        // we set the m_postvars variable of the controller for backwards compatibility reasons (when using $obj->dispatch in the dispatch.php)
        $this->m_postvars = $postvars;

        $node = $this->getNode();
        $node->m_postvars = $postvars;
        if (!is_object($node) || !method_exists($node, 'getUi')) {
            return "";
        }

        $page = $node->getPage();

        // backwards compatibility mode
        if ($flags == null) {
            $flags = array_key_exists("atkpartial", $postvars) ? Page::HTML_PARTIAL : Page::HTML_STRICT;
        } elseif (is_bool($flags)) {
            $flags = $flags ? Page::HTML_STRICT : Page::HTML_HEADER | Page::HTML_DOCTYPE;
        }

        // Use invoke to be backwards compatible with overrides
        // of loadDispatchPage in atknode.
        $this->invoke("loadDispatchPage", $postvars);

        $screen = '';
        if (!$page->isEmpty() || Tools::hasFlag($flags, Page::HTML_PARTIAL)
        ) { // Only output an html output if there is anything to output.
            $screen = $page->render(null, $flags);
        }

        if (!$this->m_return_output) {
            $output = Output::getInstance();
            $output->output($screen);
        }

        // This is the end of all things for this page..
        // so we clean up some resources..
        $db = $node->getDb();
        if (is_object($db)) {
            $db->disconnect();
        }
        Tools::atkdebug("disconnected from the database");

        if ($this->m_return_output) {
            return $screen;
        }
        return "";
    }

    /**
     * Return the html title for the content frame. Default we show node name and action.
     */
    protected function getHtmlTitle()
    {
        $node = $this->getNode();
        $ui = $node->getUi();
        return Tools::atktext('app_shorttitle') . " - " . $ui->title($node->m_module, $node->m_type,
            $node->m_postvars['atkaction']);
    }

    /**
     * This method is a wrapper for calling the node dispatch function
     * Therefore each node can define it's own dispatch function
     * The default dispatch function of the Node will call the handleRequest function of the controller
     *
     * @param array $postvars
     * @param integer $flags
     */
    function dispatch($postvars, $flags = null)
    {
        $this->m_postvars = $postvars;
        $node = $this->getNode();
        return $node->dispatch($postvars, $flags);
    }

    /**
     * Set m_node variable of this class
     *
     * @param object $node
     */
    function setNode(&$node)
    {
        $this->m_node = &$node;
    }

    /**
     * Get m_node variable or if not set make instance of Node class (determined by using the postvars)
     *
     * @return Node reference to Node
     */
    function &getNode()
    {
        if (is_object($this->m_node)) {
            return $this->m_node;
        } else {
            //if the object not yet exists, try to create it
            $fullclassname = $this->m_postvars['atknodeuri'];
            if (isset($fullclassname) && $fullclassname != null) {
                $this->m_node = Module::atkGetNode($fullclassname);
                if (is_object($this->m_node)) {
                    return $this->m_node;
                } else {
                    global $ATK_VARS;
                    Tools::atkerror("No object '" . $ATK_VARS['atknodeuri'] . "' created!!?!");
                }
            }
        }
        $res = null;
        return $res; // prevent notice
    }

    /**
     * Does the actual loading of the dispatch page
     * And adds it to the page for the dispatch() method to render.
     * @param array $postvars The request variables for the node.
     */
    function loadDispatchPage($postvars)
    {
        $this->m_postvars = $postvars;
        $node = $this->getNode();

        if (!is_object($node)) {
            return;
        }



        $node->m_postvars = $postvars;
        $node->m_action = $postvars['atkaction'];
        if (isset($postvars["atkpartial"])) {
            $node->m_partial = $postvars["atkpartial"];
        }

        $page = $node->getPage();
        $page->setTitle(Tools::atktext('app_shorttitle') . " - " . $this->getUi()->title($node->m_module,
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
                list(,$id) = explode("=", Tools::atkArrayNvl($node->m_postvars, "atkselector", "="));
            }

            $page->register_hiddenvars(array(
                "atknodeuri" => $node->m_module . "." . $node->m_type,
                "atkselector" => str_replace("'", "", $id)
            ));
        } else {
            $page->addContent($this->accessDeniedPage());
        }
    }

    /**
     * Render a generic access denied page.
     *
     * @return String A complete html page with generic access denied message.
     */
    function accessDeniedPage()
    {
        $node = $this->getNode();

        $content = "<br><br>" . Tools::atktext("error_node_action_access_denied", "",
                $node->m_type) . "<br><br><br>";


        return $this->genericPage(Tools::atktext('access_denied'), $content);
    }

    /**
     * Render a generic page, with a box, title, stacktrace etc.
     * @param string $title The pagetitle and if $content is a string, also
     *                      the boxtitle.
     * @param mixed $content The content to display on the page. This can be:
     *                       - A string which will be the content of a single
     *                         box on the page.
     *                       - An associative array of $boxtitle=>$boxcontent
     *                         pairs. Each pair will be rendered as a seperate
     *                         box.
     * @return String A complete html page with the desired content.
     */
    function genericPage($title, $content)
    {
        $node = $this->getNode();
        $ui = &$node->getUi();
        if (!is_array($content)) {
            $content = array($title => $content);
        }
        $blocks = array();
        foreach ($content as $itemtitle => $itemcontent) {
            $blocks[] = $ui->renderBox(array(
                "title" => $itemtitle,
                "content" => $itemcontent
            ), 'dispatch');
        }

        /**
         * @todo Don't use renderActionPage here because it tries to determine
         *       it's own title based on the title which is passed as action.
         *       Instead use something like the commented line below:
         */
        //return $ui->render("actionpage.tpl", array("blocks"=>$blocks, "title"=>$title));
        return $this->renderActionPage($title, $blocks);
    }

    /**
     * Render a generic action.
     *
     * Renders actionpage.tpl for the desired action. This includes the
     * given block(s) and a pagetrial, but not a box.
     * @param string $action The action for which the page is rendered.
     * @param mixed $blocks Pieces of html content to be rendered. Can be a
     *                      single string with content, or an array with
     *                      multiple content blocks.
     * @return String Piece of HTML containing the given blocks and a pagetrail.
     */
    function renderActionPage($action, $blocks = array())
    {
        if (!is_array($blocks)) {
            $blocks = ($blocks == "" ? array() : array($blocks));
        }
        $node = $this->getNode();
        $ui = &$node->getUi();

        // todo: overridable action templates
        return $ui->render("actionpage.tpl", array(
            "blocks" => $blocks,
            "title" => $this->actionPageTitle()
        ));
    }

    /**
     * Return the title to be show on top of an Action Page
     *
     * @return string The title
     */
    function actionPageTitle()
    {
        $node = $this->getNode();
        $ui = &$node->getUi();
        return $ui->title($node->m_module, $node->m_type);
    }

    /**
     * Determine the url for the feedbackpage.
     *
     * Output is dependent on the feedback configuration. If feedback is not
     * enabled for the action, this method returns an empty string, so the
     * result of this method can be passed directly to the redirect() method
     * after completing the action.
     *
     * The $record parameter is ignored by the default implementation, but
     * derived classes may override this method to perform record-specific
     * feedback.
     * @param string $action The action that was performed
     * @param int $status The status of the action.
     * @param array $record The record on which the action was performed.
     * @param string $message An optional message to pass to the feedbackpage,
     *                        for example to explain the reason why an action
     *                        failed.
     * @param integer $levelskip The number of levels to skip
     * @return String The feedback url.
     */
    function feedbackUrl($action, $status, $record = array(), $message = "", $levelskip = null)
    {
        $node = $this->getNode();
        $sm = SessionManager::getInstance();
        if ((isset($node->m_feedback[$action]) && Tools::hasFlag($node->m_feedback[$action], $status)) || $status == ActionHandler::ACTION_FAILED) {
            $vars = array(
                "atkaction" => "feedback",
                "atkfbaction" => $action,
                "atkactionstatus" => $status,
                "atkfbmessage" => $message
            );
            $atkNodeUri = $node->atkNodeUri();
            $sessionStatus = SessionManager::SESSION_REPLACE;

            // The level skip given is based on where we should end up after the
            // feedback action is shown to the user. This means that the feedback
            // action should be shown one level higher in the stack, hence the -1.
            // Default the feedback action is shown on the current level, so in that
            // case we have a simple SessionManager::SESSION_REPLACE with a level skip of null.
            $levelskip = $levelskip == null ? null : $levelskip - 1;
        } else {
            // Default we leave atkNodeUri empty because the sessionmanager will determine which is de atkNodeUri
            $vars = array();
            $atkNodeUri = "";
            $sessionStatus = SessionManager::SESSION_BACK;
        }
        return ($sm->sessionUrl($this->dispatchUrl($vars, $atkNodeUri), $sessionStatus, $levelskip));
    }

    /**
     * Generate a dispatch menu URL for use with nodes
     * and their specific actions.
     * @param string $params : A key/value array with extra options for the url
     * @param string $atknodeuri The atknodeuri (modulename.nodename)
     * @param string $file The php file to use for dispatching, defaults to dispatch.php
     * @return string url for the node with the action
     */
    function dispatchUrl($params = array(), $atknodeuri = "", $file = "")
    {
        if (!is_array($params)) {
            $params = array();
        }
        $vars = array_merge($params, $this->m_hidden_vars);
        if ($file != "") {
            $phpfile = $file;
        } else {
            $phpfile = $this->getPhpFile();
        }

        // When $atknodeuri is empty this means that we use the atknodeuri from session
        $dispatch_url = Tools::dispatch_url($atknodeuri, Tools::atkArrayNvl($vars, "atkaction", ""), $vars,
            $phpfile);

        return $dispatch_url;
    }

    /**
     * Returns the form buttons for a certain page.
     *
     * Can be overridden by derived classes to define custom buttons.
     * @param string $mode The action for which the buttons are retrieved.
     * @param array $record The record currently displayed/edited in the form.
     *                      This param can be used to define record specific
     *                      buttons.
     * @return array
     */
    function getFormButtons($mode, $record = array())
    {
        $result = array();
        $node = $this->getNode();
        $page = &$node->getPage();
        $page->register_script(Config::getGlobal("assets_url") . "javascript/tools.js");
        $sm = SessionManager::getInstance();

        // edit mode
        if ($mode == "edit") {
            if ($sm->atkLevel() > 0 || Tools::hasFlag(Tools::atkArrayNvl($node->m_feedback,
                    "update", 0), ActionHandler::ACTION_SUCCESS)
            ) {
                $result[] = $this->getButton('saveandclose', true);
            }

            $result[] = $this->getButton('save');

            // if atklevel is 0 or less, we are at the bottom of the session stack,
            // which means that 'saveandclose' doesn't close anyway, so we leave out
            // the 'saveandclose' and 'cancel' button. Unless, a feedback screen is configured.
            if ($sm->atkLevel() > 0 || Tools::hasFlag(Tools::atkArrayNvl($node->m_feedback,
                    "update", 0), ActionHandler::ACTION_CANCELLED)
            ) {
                $result[] = $this->getButton('cancel');
            }


        } elseif ($mode == "add") {

            if ($node->hasFlag(Node::NF_EDITAFTERADD) === true) {
                if ($node->allowed('edit')) {
                    $result[] = $this->getButton('saveandedit', true);
                } else {
                    Tools::atkwarning("Node::NF_EDITAFTERADD found but no 'edit' privilege.");
                }
            } else {
                $result[] = $this->getButton('saveandclose', true);

                if ($node->hasFlag(Node::NF_ADDAFTERADD)) {
                    $result[] = $this->getButton('saveandnext', false);
                }
            }


            if ($sm->atkLevel() > 0 || Tools::hasFlag(Tools::atkArrayNvl($node->m_feedback,
                    "save", 0), ActionHandler::ACTION_CANCELLED)
            ) {
                $result[] = $this->getButton('cancel');
            }

        } elseif ($mode == "view") {
            // if appropriate, display an edit button.
            if (!$node->hasFlag(Node::NF_NO_EDIT) && $node->allowed("edit", $record)) {
                $result[] = '<input type="hidden" name="atkaction" value="edit">' .
                    '<input type="hidden" name="atknodeuri" value="' . $node->atkNodeUri() . '">' .
                    $this->getButton('edit');
            }

            if ($sm->atkLevel() > 0) {
                $result[] = $this->getButton('back', false, Tools::atktext('cancel'));
            }

        } elseif ($mode == "delete") {
            $result[] = '<input name="cancel" type="submit" class="btn btn-default btn_cancel" value="' . $node->text('no') . '">';
            $result[] = '<input name="confirm" type="submit" class="btn btn-default btn_ok" value="' . $node->text('yes') . '">';
        } elseif ($mode == "search") {
            // (don't change the order of button)
            $result[] = $this->getButton('search', true);
            $result[] = $this->getButton('cancel');
        }

        return $result;
    }

    /**
     * Create a button.
     *
     * @param string $action
     * @param Bool $default Add the atkdefaultbutton class?
     * @return String HTML
     */
    function getButton($action, $default = false, $label = null)
    {
        $valueAttribute = "";

        switch ($action) {
            case "save":
                $name = "atknoclose";
                $class = "btn_save";
                break;
            case "saveandclose":
                $name = "atksaveandclose";
                $class = "btn_saveandclose";
                break;
            case "cancel":
                $name = "atkcancel";
                $class = "btn_cancel";
                break;
            case "saveandedit":
                $name = "atksaveandcontinue";
                $class = "btn_saveandcontinue";
                break;
            case "saveandnext":
                $name = "atksaveandnext";
                $class = "btn_saveandnext";
                break;
            case "back":
                $name = "atkback";
                $class = "btn_cancel";
                $value = '<< ' . Tools::atktext($action, 'atk');
                break;
            case "edit":
                $name = "atkedit";
                $class = "btn_save";
                break;
            case "search":
                $name = "atkdosearch";
                $class = "btn_search";
                break;
            default:
                $name = $action;
                $class = "atkbutton";
        }

        if (!isset($value)) {
            $value = $this->m_node->text($action);
        }
        if (isset($label)) {
            $value = $label;
        }
        $value = htmlentities($value);

        $class = trim('btn ' . $class);

        if ($default) {
            $class .= (!empty($class) ? ' ' : '') . 'atkdefaultbutton btn-primary';
        } else {
            $class .= (!empty($class) ? ' ' : '') . 'btn-default';
        }

        if ($class != "") {
            $class = "class=\"$class\" ";
        }

        if ($value != "") {
            $valueAttribute = "value=\"{$value}\" ";
        }

        if ($name != "") {
            $name = "name=\"" . $this->m_node->getEditFieldPrefix() . "{$name}\" ";
        }

        return '<button type="submit" ' . $class . $name . $valueAttribute . '>' . $value . '</button>';
    }


    /**
     * Set Key/value pair in m_hidden_vars array. Saved pairs are
     * send as post or get vars in the next page load
     *
     * @param string $name the reference key
     * @param string $value the actual value
     */
    function setHiddenVar($name, $value)
    {
        $this->m_hidden_vars[$name] = $value;
    }

    /**
     * Return m_hidden_vars array.
     *
     * @return array
     */
    function getHiddenVars()
    {
        return $this->m_hidden_vars;
    }

    /**
     * Set php_file member variable
     *
     * @param string $phpfile
     */
    function setPhpFile($phpfile)
    {
        $this->m_php_file = $phpfile;
    }

    /**
     * Return php_file.
     *
     * @return string The name of the file to which subsequent requests should be posted.
     */
    function getPhpFile()
    {
        return $this->m_php_file;
    }

    /**
     * Return m_hidden_vars as html input types.
     *
     * @return string
     */
    function getHiddenVarsString()
    {
        if (count($this->m_hidden_vars) == 0) {
            return "";
        }

        $varString = '';

        foreach ($this->m_hidden_vars as $hiddenVarName => $hiddenVarValue) {
            $varString .= '<input type="hidden" name="' . $hiddenVarName . '" value="' . $hiddenVarValue . '">';
        }
        return $varString;
    }

    /**
     * Configure if you want the html returned or leave it up to Output.
     *
     * @param bool $returnOutput
     */
    function setReturnOutput($returnOutput)
    {
        $this->m_return_output = $returnOutput;
    }

    /**
     * Return the setting for returning output
     *
     * @return bool
     */
    function getReturnOutput()
    {
        return $this->m_return_output;
    }

    /**
     * Return a reference to the Page object. This object
     * is used to render output as an html page.
     *
     * @return object reference
     */
    function &getPage()
    {
        $page = Page::getInstance();
        return $page;
    }

    /**
     * Get the ui instance for drawing and templating purposes.
     *
     * @return Ui An Ui instance for drawing and templating.
     */
    function &getUi()
    {
        $ui = Ui::getInstance();
        return $ui;
    }

    /**
     * Generic method invoker (copied from class.atkactionhandler.inc).
     *
     * Controller methods invoked with invoke() instead of directly, have a major
     * advantage: the controller automatically searches for an override in the
     * node. For example, If a controller calls its getSomething() method using
     * the invoke method, the node may implement its own version of
     * getSomething() and that method will then be called instead of the
     * original. The controller is passed by reference to the override function
     * as first parameter, so if necessary, you can call the original method
     * from inside the override.
     *
     * The function accepts a variable number of parameters. Any parameter
     * that you would pass to the method, can be passed to invoke(), and
     * invoke() will pass the parameters on to the method.
     *
     * There is one limitation: you can't pass parameters by reference if
     * you use invoke().
     *
     * <b>Example:</b>
     *
     * <code>
     *   $controller->invoke("dispatch", $postvars, $fullpage);
     * </code>
     *
     * This will call dispatch(&$handler, $postvars, $flags) on your node class
     * if present, or dispatch($postvars, $flags) in the handler if the node has
     * no override.
     *
     * @param string $methodname The name of the method to call.
     * @return mixed The method returns the return value of the invoked
     *               method.
     */
    function invoke($methodname)
    {
        $arguments = func_get_args(); // Put arguments in a variable (php won't let us pass func_get_args() to other functions directly.
        // the first argument is $methodname, which we already defined by name.
        array_shift($arguments);
        $node = $this->getNode();
        if ($node !== null && method_exists($node, $methodname)) {
            Tools::atkdebug("Controller::invoke() Invoking '$methodname' override on node");
            // We pass the original object as last parameter to the override.
            array_push($arguments, $this);
            return call_user_func_array(array(&$node, $methodname), $arguments);
        } else {
            if (method_exists($this, $methodname)) {
                Tools::atkdebug("Controller::invoke() Invoking '$methodname' on controller");
                return call_user_func_array(array(&$this, $methodname), $arguments);
            }
        }
        Tools::atkerror("Controller::invoke() Undefined method '$methodname' in Controller");
        return false;
    }

    /**
     * Return module name of controller
     *
     * @return string module name
     */
    function getModuleName()
    {
        return $this->m_module_name;
    }

    /**
     * Return controller name
     *
     * @return string controller name
     */
    function getName()
    {
        return $this->m_name;
    }
}
