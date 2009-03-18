<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage handlers
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
  
  /**
   * Some defines for return behaviour
   */
  define("ATK_ACTION_STAY", 0);
  define("ATK_ACTION_BACK", 1);

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
   * @package atk
   * @subpackage handlers
   * @abstract
   */
  class atkActionHandler
  {
    /**
     * @var atkNode 
     * @access private 
     */
    var $m_node = NULL;

    /** @access private */
    var $m_action = "";

    /** @access private */
    var $m_partial = NULL;
    
    /** @access private */
    var $m_renderBoxVars = array();
    
    /** @access private */
    var $m_rejecting = false;
    
    /** @access private */
    var $m_returnbehaviour = ATK_ACTION_STAY;
    
    /**
     * Render mode, defaults to "box" but can be changed to "dialog".
     * 
     * @var string
     * @access protected
     */
    var $m_renderMode = 'box';

    /**
     * Default constructor.
     */
    function atkActionHandler()
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
     * @param atkNode $node The node on which the action should be performed.
     * @param String $action The action that is being performed.
     * @param array $postvars Any variables from the request
     *
     */
    function handle(&$node, $action, &$postvars)
    {
      $this->m_postvars = &$postvars;
      $this->m_node = &$node;
      $this->m_action = $action;
      $this->m_partial = $node->m_partial;
      
      $this->invoke("action_".$action);
           
      // when we're finished, cleanup any atkrejects (that we haven't set ourselves).
      if (!$this->m_rejecting)
      {
        atkdebug("clearing the stuff");
        $this->getRejectInfo(); // this will clear it.
      }
    }
    
    /**
     * Returns the node object.
     *
     * @return atkNode
     */
    public function getNode()
    {
      return $this->m_node;
    }
    
    /**
     * Get the reject info from the session
     * This is used by the atkAddHandler and atkEditHandler to
     * show the validation errors
     *
     * @return Array The reject info
     */
    function getRejectInfo()
    {      
      global $atklevel;
      atkdebug("looking for rejectinfo");
      $sm = &atkGetSessionManager();
      $session = &$sm->getSession();
      $stackid = atkStackID(($atklevel==-1));
      if (isset($session["atkreject"][$sm->m_namespace][$stackid][atkLevel()]))
      {
        atkdebug("rejectinfo found");        
        $result = $session["atkreject"][$sm->m_namespace][$stackid][atkLevel()];        
        unset($session["atkreject"][$sm->m_namespace][$stackid][atkLevel()]);
        return $result;
      }
      return NULL;
    }
    
    /**
     * Store the reject info in the session
     * This is used by the atkSaveHandler and atkUpdateHandler to 
     * store the record if the record is not validated
     *
     * @param array $data The reject information
     */
    function setRejectInfo($data)
    {
      global $atklevel;
      atkdebug("setting rejectinfo");
      $sm = &atkGetSessionManager();
      $session = &$sm->getSession();
      $stackid = atkStackID(($atklevel==-1));
      $session["atkreject"][$sm->m_namespace][$stackid][atkPrevLevel()] = $data;
      $this->m_rejecting = true;
    }

    /**
     * Set the calling node of the current action.
     * @param atkNode $node The node on which the action should be performed.
     */
    function setNode(&$node)
    {
      $this->m_node = &$node;
      $this->m_partial = $node->m_partial;  
      $this->m_postvars = &$node->m_postvars;    
    }

    /**
     * Sets the current action.
     * @param string $action The action name.
     */
    function setAction($action)
    {
      $this->m_action = $action;
    }

    /**
     * Set postvars of the the calling node of the current action.
     * @param array $postvars Postvars of the node on which the action should be performed.
     */
    function setPostvars(&$postvars)
    {
      $this->m_postvars = &$postvars;
    }
    
    /**
     * Sets the render mode ("box" or "dialog").
     * 
     * @param string $mode render mode
     */
    function setRenderMode($mode)
    {
      $this->m_renderMode = $mode;
    }
    
    /**
     * Returns the render mode.
     * 
     * @return string render mode
     */
    function getRenderMode()
    {
      return $this->m_renderMode;
    }

    /**
     * Get the page instance for generating output.
     *
     * @return atkPage The active page instance.
     */
    function &getPage()
    {
      return $this->m_node->getPage();
    }

    /**
     * Get the ui instance for drawing and templating purposes.
     *
     * @return atkUi An atkUi instance for drawing and templating.
     */
    function &getUi()
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
     * @param String $methodname The name of the method to call.
     * @return mixed The method returns the return value of the invoked
     *               method.
     */
    function invoke($methodname)
    {
      $arguments = func_get_args(); // Put arguments in a variable (php won't let us pass func_get_args() to other functions directly.
      // the first argument is $methodname, which we already defined by name.
      array_shift($arguments);

      if ($this->m_node!==NULL && method_exists($this->m_node, $methodname))
      {
        atkdebug("Invoking '$methodname' override on node");
        // We pass the original object as first parameter to the override. 
        array_unshift($arguments, $this);
        $arguments[0] = &$this; // reference copy workaround;
        return call_user_func_array(array(&$this->m_node, $methodname), $arguments);
      }
      else if (method_exists($this, $methodname))
      {
        atkdebug("Invoking '$methodname' on actionhandler for action ".$this->m_action);
        return call_user_func_array(array(&$this, $methodname), $arguments);
      }
      atkerror("Undefined method '$methodname' in atkActionHandler");
    }

    /**
     * Static factory method to get the default action handler for a certain
     * action.
     *
     * When no action handler class can be found for the action, a default
     * handler is instantiated and returned. The default handler assumes that
     * the node has an action_.... method, that will be called when the
     * actionhandler's handle() mehod is called.
     * @static
     *
     * @param String $action The action for which an action handler should be
     *                       retrieved.
     */
    function &getDefaultHandler($action)
    {
      // The next if statement checks for 'known' actions. All unknown actions
      // are handled the backwardscompatible default way (invoking action_$action on the node)
      $filename = atkconfig("atkroot")."atk/handlers/class.atk".$action."handler.inc";
      if (file_exists($filename))
      {
        return atknew("atk.handlers.atk".$action."handler");
      }
      else
      {
        // We don't have handlers yet for other actions.
        $actionhandler = new atkActionHandler(); // The default handler will automatically
                                                 // invoke the node methods.
        return $actionhandler;
      }
    }

    /**
     * Get the cached recordlist
     *
     * @return atkRecordListCache object
     */
    function getRecordlistCache()
    {
      static $recordlistcache;
      if (!$recordlistcache)
      {
        $recordlistcache = &atknew("atk.recordlist.atkrecordlistcache");
        $recordlistcache->setNode($this->m_node);
        $recordlistcache->setPostvars($this->m_postvars);
      }
      return $recordlistcache;
    }

    /**
     * Clear the recordlist cache
     *
     */
    function clearCache()
    {
      if ($this->m_node->hasFlag(NF_CACHE_RECORDLIST))
      {
        $recordlistcache = $this->getRecordlistCache();
        if ($recordlistcache)
          $recordlistcache->clearCache($this->m_node->atkNodeType());
      }
    }

    /**
     * Notify the node that an action has occured
     *
     * @param string $action The action that occurred
     * @param array $record The record on which the action was performed
     */
    function notify($action, $record)
    {
      $this->m_node->notify($action, $record);
    }
    
    /**
     * Add a variable to the renderbox
     *
     * @param string $key
     * @param string $value
     */
    function addRenderBoxVar($key, $value)
    {
      $this->m_renderBoxVars[$key] = $value;
    }
    
    /**
     * Set the returnbehaviour of this action
     *
     * @param Integer $returnbehaviour The return behaviour (possible values: ATK_ACTION_BACK and ATK_ACTION_STAY)
     */
    function setReturnBehaviour($returnbehaviour)
    {
      $this->m_returnbehaviour = $returnbehaviour;
    }
    
    /**
     * Get the returnbehaviour of this action
     *
     * @return String the return behaviour
     */
    function getReturnBehaviour()
    {
      return $this->m_returnbehaviour;
    }
    
    /**
     * Current action allowed on the given record?
     *
     * @param array $record record
     * @return boolean is action allowed on record?
     */
    function allowed($record)
    {
      return $this->m_node->allowed($this->m_action, $record);
    }

    /**
     * Render access denied page.
     */
    function renderAccessDeniedPage()
    {
      $page = &$this->m_node->getPage();
      $page->addContent($this->_getAccessDeniedPage());
    }
    
    /**
     * Get the access denied page
     *
     * @return String the HTML code of the access denied page
     */
    function _getAccessDeniedPage()
    {
    	$controller = &atkController::getInstance();
      $controller->setNode($this->m_node);
      return $controller->accessDeniedPage();
    }
    
    /**
     * Render access denied dialog contents.
     * 
     * @return String The access denied page in a dialog
     */
    function renderAccessDeniedDialog()
    {
      $message = 
        $this->m_node->text('access_denied').
        "<br><br>".
        $this->m_node->text("error_node_action_access_denied");
        
      return $this->renderMessageDialog($message);
    }
    
    /**
     * Render message dialog contents.
     * 
     * @param String $message The message to render in a dialog
     * @return String The message dialog
     */
    function renderMessageDialog($message)
    {
      atkimport('atk.ui.atkdialog');
      $ui = &$this->m_node->getUi();      
  	
      $params = array();    	
      $params["content"] = "<br />".$message."<br />";
      $params["buttons"][] = '<input type="button" class="btn_cancel" value="'.$this->m_node->text('close').'" onClick="'.atkDialog::getCloseCall().'" />';
      $content = $ui->renderAction($this->m_action, $params);
      
      $params = array();
      $params["title"] = $this->m_node->actionTitle($this->m_action);
      $params["content"] = $content;  
      $content = $ui->renderDialog($params);     
      
      return $content;
    }
    
    /**
     * Outputs JavaScript for updating the existing dialog contents.
     *
     * @param string $content
     */
    function updateDialog($content)
    {
      atkimport('atk.ui.atkdialog');
      $script = atkDialog::getUpdateCall($content, false);
      $page = &$this->getPage();
      $page->register_loadscript($script);
    }
    
    /**
     * Output JavaScript to close the dialog.
     */
    function closeDialog()
    {
      atkimport('atk.ui.atkdialog');
      $script = atkDialog::getCloseCall();
      $page = &$this->getPage();
      $page->register_loadscript($script);
    }
    
    /**
     * Handle partial.
     *
     * @param string $partial full partial
     */
    function partial($partial)
    {
      $parts = explode(".", $partial);
      $method = "partial_".$parts[0];

      if (!method_exists($this, $method))
      {
        $content = '<span style="color: red; font-weight: bold">Invalid partial \''.$this->m_partial.'\'!</span>';
      }
      else 
      {
        $content = $this->$method($partial);
      }
      
      $page = &$this->getPage();      
      $page->addContent($content);
    }     
  }
?>