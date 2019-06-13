<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\RecordList\RecordListCache;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Ui\Ui;

/**
 * Generic action handler base class.
 *
 * Action handlers are responsible for performing actions on nodes (for
 * example "add", "edit", "delete", or any other custom actions your
 * application might have).
 * An action from the default handler can be overridden by implementing a
 * method in your node with the name action_<actionname> where <actionname>
 * is the action you want to perform. The original handler is passed as a
 * parameter to the override.
 *
 * Custom action handlers should always be derived from atkActionHandler,
 * and should contain at least an implementation for the handle() method,
 * which is called by the framework to execute the action.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @abstract
 */
class ActionHandler
{
    /**
     * Some defines for return behaviour.
     */
    const ATK_ACTION_STAY = 0;
    const ATK_ACTION_BACK = 1;

    /**
     * action status flags
     * Note that these have binary numbers, even though an action could never have
     * two statusses at the same time.
     * This is done however, so the flags can be used as a mask in the setFeedback
     * function.
     */
    /**
     * The action is cancelled.
     *
     * action status flag
     */
    const ACTION_CANCELLED = 1;

    /**
     * The action failed to accomplish it's goal.
     *
     * action status flag
     */
    const ACTION_FAILED = 2;

    /**
     * The action is a success.
     *
     * action status flag
     */
    const ACTION_SUCCESS = 4;

    /**
     * @var Node $m_node
     * @access private
     */
    public $m_node;

    /* @access private */
    public $m_action = '';

    /* @access private */
    public $m_partial = null;

    /* @access private */
    public $m_renderBoxVars = [];

    /* @access private */
    public $m_rejecting = false;

    /* @access private */
    public $m_returnbehaviour = self::ATK_ACTION_STAY;

    public $m_postvars;

    protected $m_boxTemplate = 'box';

    /*
     * Render mode, defaults to "box"
     *
     * @var string
     * @access protected
     */
    public $m_renderMode = 'box';

    public function __construct()
    {
    }

    /**
     * The handle() method handles the action.
     *
     * The default implementation invokes an action_$action override (if
     * present) and stores the postvars. Custom handlers may override this
     * behavior. If there is no node action override and a partial is set
     * for the action we don't invoke the action_$action override but
     * instead let the partial method handle the action.
     *
     * @param Node $node The node on which the action should be performed.
     * @param string $action The action that is being performed.
     * @param array $postvars Any variables from the request
     */
    public function handle($node, $action, &$postvars)
    {
        $this->m_postvars = $postvars;
        $this->m_node = $node;
        $this->m_action = $action;
        $this->m_partial = $node->m_partial;

        $this->invoke('action_'.$action);

        // when we're finished, cleanup any atkrejects (that we haven't set ourselves).
        if (!$this->m_rejecting) {
            Tools::atkdebug('clearing the stuff');
            $this->getRejectInfo(); // this will clear it.
        }
    }

    /**
     * Returns the node object.
     *
     * @return Node
     */
    public function getNode()
    {
        return $this->m_node;
    }

    /**
     * Get the reject info from the session
     * This is used by the atkAddHandler and atkEditHandler to
     * show the validation errors.
     *
     * @return array The reject info
     */
    public function getRejectInfo()
    {
        $sm = SessionManager::getInstance();

        return $sm->stackVar('atkreject');
    }

    /**
     * Store the reject info in the session
     * This is used by the atkSaveHandler and atkUpdateHandler to
     * store the record if the record is not validated.
     *
     * @param array $data The reject information
     */
    public function setRejectInfo($data)
    {
        $sm = SessionManager::getInstance();
        $sm->stackVar('atkreject', $data, $sm->atkPrevLevel());
        $this->m_rejecting = true;
    }

    /**
     * Set the calling node of the current action.
     *
     * @param Node $node The node on which the action should be performed.
     */
    public function setNode($node)
    {
        $this->m_node = $node;
        $this->m_partial = $node->m_partial;
        $this->m_postvars = $node->m_postvars;
    }

    /**
     * Sets the current action.
     *
     * @param string $action The action name.
     */
    public function setAction($action)
    {
        $this->m_action = $action;
    }

    /**
     * Set postvars of the the calling node of the current action.
     *
     * @param array $postvars Postvars of the node on which the action should be performed.
     */
    public function setPostvars(&$postvars)
    {
        $this->m_postvars = &$postvars;
    }

    /**
     * Returns the render mode.
     *
     * @return string render mode
     */
    public function getRenderMode()
    {
        return $this->m_renderMode;
    }

    public function setBoxTemplate($tpl)
    {
        $this->m_boxTemplate = $tpl;
    }

    /**
     * Get the page instance for generating output.
     *
     * @return Page The active page instance.
     */
    public function getPage()
    {
        return $this->m_node->getPage();
    }

    /**
     * Get the ui instance for drawing and templating purposes.
     *
     * @return Ui An Ui instance for drawing and templating.
     */
    public function getUi()
    {
        return $this->m_node->getUi();
    }

    /**
     * Generic method invoker.
     *
     * Handler methods invoked with invoke() instead of directly, have a major
     * advantage: the handler automatically searches for an override in the
     * node. For example, If a handler calls its getSomething() method using
     * the invoke method, the node may implement its own version of
     * getSomething() and that method will then be called instead of the
     * original. The handler is passed by reference to the override function
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
     *   $handler->invoke("editPage", $record, $mode);
     * </code>
     *
     * This will call editPage(&$handler, $record, $mode) on your node class
     * if present, or editPage($record, $mode) in the handler if the node has
     * no override.
     *
     * @param string $methodname The name of the method to call.
     *
     * @return mixed The method returns the return value of the invoked
     *               method.
     */
    public function invoke($methodname)
    {
        $arguments = func_get_args(); // Put arguments in a variable (php won't let us pass func_get_args() to other functions directly.
        // the first argument is $methodname, which we already defined by name.
        array_shift($arguments);

        if ($this->m_node !== null && method_exists($this->m_node, $methodname)) {
            Tools::atkdebug("Invoking '$methodname' override on node");
            // We pass the original object as first parameter to the override.
            array_unshift($arguments, $this);
            $arguments[0] = &$this; // reference copy workaround;
            return call_user_func_array(array(&$this->m_node, $methodname), $arguments);
        } else {
            if (method_exists($this, $methodname)) {
                Tools::atkdebug("Invoking '$methodname' on ActionHandler for action ".$this->m_action);

                return call_user_func_array(array(&$this, $methodname), $arguments);
            }
        }
        Tools::atkerror("Undefined method '$methodname' in ActionHandler");

        return;
    }

    /**
     * Static factory method to get the default action handler for a certain
     * action.
     *
     * When no action handler class can be found for the action, a default
     * handler is instantiated and returned. The default handler assumes that
     * the node has an action_.... method, that will be called when the
     * actionhandler's handle() mehod is called.
     *
     * @param string $action
     *
     * @return ActionHandler
     */
    public static function getDefaultHandler($action)
    {
        $class = __NAMESPACE__.'\\'.ucfirst($action).'Handler';
        if (class_exists($class)) {
            return new $class();
        }

        return new self();
    }

    /**
     * Modify grid.
     *
     * @param DataGrid $grid grid
     * @param int $mode CREATE or RESUME
     */
    protected function modifyDataGrid(DataGrid $grid, $mode)
    {
        $method = 'modifyDataGrid';
        if (method_exists($this->getNode(), $method)) {
            $this->getNode()->$method($grid, $mode);
        }
    }

    /**
     * Get the cached recordlist.
     *
     * @return RecordListCache object
     */
    public function getRecordlistCache()
    {
        static $recordlistcache;
        if (!$recordlistcache) {
            $recordlistcache = new RecordListCache();
            $recordlistcache->setNode($this->m_node);
            $recordlistcache->setPostvars($this->m_postvars);
        }

        return $recordlistcache;
    }

    /**
     * Clear the recordlist cache.
     */
    public function clearCache()
    {
        if ($this->m_node->hasFlag(Node::NF_CACHE_RECORDLIST)) {
            $recordlistcache = $this->getRecordlistCache();
            if ($recordlistcache) {
                $recordlistcache->clearCache();
            }
        }
    }

    /**
     * Notify the node that an action has occured.
     *
     * @param string $action The action that occurred
     * @param array $record The record on which the action was performed
     */
    public function notify($action, $record)
    {
        $this->m_node->notify($action, $record);
    }

    /**
     * Add a variable to the renderbox.
     *
     * @param string $key
     * @param string $value
     */
    public function addRenderBoxVar($key, $value)
    {
        $this->m_renderBoxVars[$key] = $value;
    }

    /**
     * Set the returnbehaviour of this action.
     *
     * @param int $returnbehaviour The return behaviour (possible values: ActionHandler::ATK_ACTION_BACK and ActionHandler::ATK_ACTION_STAY)
     */
    public function setReturnBehaviour($returnbehaviour)
    {
        $this->m_returnbehaviour = $returnbehaviour;
    }

    /**
     * Get the returnbehaviour of this action.
     *
     * @return string the return behaviour
     */
    public function getReturnBehaviour()
    {
        return $this->m_returnbehaviour;
    }

    /**
     * Current action allowed on the given record?
     *
     * @param array $record record
     *
     * @return bool is action allowed on record?
     */
    public function allowed($record)
    {
        return $this->m_node->allowed($this->m_action, $record);
    }

    /**
     * Render access denied page.
     */
    public function renderAccessDeniedPage()
    {
        $page = $this->m_node->getPage();
        $page->addContent($this->_getAccessDeniedPage());
    }

    /**
     * Get the access denied page.
     *
     * @return string the HTML code of the access denied page
     */
    public function _getAccessDeniedPage()
    {
        $ui = $this->m_node->getUi();
        $content = '<br><br>'.Tools::atktext('error_node_action_access_denied', '', $this->m_node->getType()).'<br><br><br>';
        $blocks = [
            $ui->renderBox([
                'title' => Tools::atktext('access_denied'),
                'content' => $content,
            ]),
        ];

        return $ui->render('actionpage.tpl', ['blocks' => $blocks, 'title' => Tools::atktext('access_denied')]);
    }

    /**
     * Handle partial.
     *
     * @param string $partial full partial
     */
    public function partial($partial)
    {
        $parts = explode('.', $partial);
        $method = 'partial_'.$parts[0];

        if (!method_exists($this, $method)) {
            $content = '<span style="color: red; font-weight: bold">Invalid partial \''.htmlspecialchars($this->m_partial).'\'!</span>';
        } else {
            $content = $this->$method($partial);
        }

        $page = $this->getPage();
        $page->addContent($content);
    }

    /**
     * Get/generate CSRF token for the current session stack.
     *
     * http://www.owasp.org/index.php/Cross-Site_Request_Forgery_%28CSRF%29_Prevention_Cheat_Sheet
     *
     * @return string CSRF token
     */
    public function getCSRFToken()
    {
        // retrieve earlier generated token from the session stack
        $token = SessionManager::getInstance()->globalStackVar('ATK_CSRF_TOKEN');
        if ($token != null) {
            return $token;
        }

        // generate and store token in sesion stack
        $token = md5(uniqid(rand(), true));
        SessionManager::getInstance()->globalStackVar('ATK_CSRF_TOKEN', $token);

        return $token;
    }

    /**
     * Checks whatever the given CSRF token matches the one stored in the
     * session stack.
     *
     * @param string $token
     *
     * @return bool is valid CSRF token?
     */
    protected function isValidCSRFToken($token)
    {
        return $this->getCSRFToken() == $token;
    }
}
