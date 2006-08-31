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
    var $m_postvars = array();

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
     * behavior.
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
      $this->invoke("action_".$action);
    }

    /**
     * Set the calling node of the current action.
     * @param atkNode $node The node on which the action should be performed.
     */
    function setNode(&$node)
    {
      $this->m_node = &$node;
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

    function clearCache()
    {
      if ($this->m_node->hasFlag(NF_CACHE_RECORDLIST))
      {
        $recordlistcache = $this->getRecordlistCache();
        if ($recordlistcache)
          $recordlistcache->clearCache($this->m_node->atkNodeType());
      }
    }

    function notify($action, $record)
    {
      $this->m_node->notify($action, $record);
    }
  }

?>