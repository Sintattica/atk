<?php
/**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   */

  /**
   * The atkController class
   *
   * @author Maurice Maas <maurice@ibuildings.nl>
   * @package atk
   */
  class atkController
  {
    /**
     * Reference to the instance of currently selected atkNode
     *
     * @var unknown_type
     */
    var $m_node = NULL;

    var $m_postvars = NULL;

    var $m_php_file = "";

    var $m_return_output = false;

    /**
     * List of actions that should give success/failure feedback
     * @access private
     * @var array
     */
    var $m_feedback = array();


    /**
     * Key/value Array containing which are be send als post or get vars
     * @access private
     * @var Array
     */
    var $m_hidden_vars = array();


    function atkController()
    {
    }

    function &_instance($class="", $replace=false)
    {
      static $s_object=NULL;
      if(!is_object($s_object) || $replace)
      {
        if (empty($class))
          $class = $_REQUEST['atkcontroller'];
        if (empty($class))
          $class = "atk.atkcontroller";
        atkdebug("atkcontroller::_instance() make new one ".$class);

        $s_object = atknew($class);
      }
      return $s_object;
    }

   /**
    * Return the one and only instance of the class
    *
    * @return unknown
    */
    function &getInstance()
    {
      atkdebug("atkcontroller::getInstance()");
      $object = &atkcontroller::_instance();
      atkdebug("atkcontroller::getInstance() return ".get_class($object));
      return $object;
    }


   /**
    * Return the one and only instance of the class
    *
    * @return unknown
    */
    function &createInstance($controller)
    {
      atkdebug("atkcontroller::createInstance() ".$controller);
      $controller = &atkController::_instance($controller, true);
      return $controller;
    }


   /**
    * handle HTTP requests
    *
    * TODO fixme Do we need this function?
    *
    * @param array $postvars
    */
    function handleRequest($postvars, $flags=NULL)
    {
      $this->m_postvars = $postvars;

      $this->invoke("dispatch", $postvars, $flags);
    }

    function setNode(&$node)
    {
      $this->m_node = &$node;
    }

    /**
     * Make instance of atkNode class
     *
     * @return reference to atknode
     */
    function &getNode()
    {
      if(is_object($this->m_node))
      {
        atkdebug("atkcontroller::getNode() <b>" . ($this->m_postvars["atknodetype"] != $this->m_node->m_module.".".$this->m_node->m_type ?
                                                   "postvars[atknodetype] not equal to the current node. postvar: ". $this->m_postvars["atknodetype"] . " current: ". $this->m_node->m_module.".".$this->m_node->m_type : "Returning node ".$this->m_node->m_module.".".$this->m_node->m_type )."</b>");
        return $this->m_node;
      }
      else
      {
        //if the object not yet exists, try to create it
        $fullclassname = $this->m_postvars["atknodetype"];
        if(isset($fullclassname) && $fullclassname != null)
        {
          $this->m_node = &getNode($fullclassname);
          if (is_object($this->m_node))
          {
            return $this->m_node;
          }
          else
          {
            atkError("No object '".$ATK_VARS["atknodetype"]."' created!!?!");
          }
        }
      }
    }


    /**
     * This is the wrapper method for all http requests on a node.
     *
     * The method looks at the atkaction from the postvars and determines what
     * should be done. If possible, it instantiates actionHandlers for
     * handling the actual action.
     *
     * @param array $postvars The request variables for the node.
     * @param boolean $fullpage If true (default), the dispatch function
     *                          renders a complete html page, including
     *                          <html><head><body> etc. If you set this to
     *                          false, plain output is generated. Warning: If
     *                          you set this to false, you are responsible for
     *                          loading all the required scripts, stylesheets
     *                          etc. (When $fullpage is true, these are
     *                          automatically included in the page header.
     */
    function dispatch($postvars, $flags=NULL)
    {
      atkdebug("atkcontroller::dispatch()");
      $this->m_postvars = $postvars;
      $node = &$this->getNode();
      if(!is_object($node)) return;

      $ui = $node->getUi();
      $page = &$node->getPage();
      atkdebug("atkcontroller::dispatch() call page->render");

      // backwards compatibility mode
      if ($flags == NULL)
      {
        $flags = array_key_exists("atkpartial", $postvars) ? HTML_PARTIAL : HTML_STRICT;
      }
      elseif (is_bool($flags))
      {
        $flags = $flags ? HTML_STRICT : HTML_HEADER|HTML_DOCTYPE;
      }

      if (hasFlag($flags, HTML_PARTIAL) && !$postvars["atkdebug"]["force"])
      {
        global $config_debug;
        $config_debug = 0;
      }

      // Use invoke to be backwards compatible with overrides
      // of loadDispatchPage in atknode.
      $this->invoke("loadDispatchPage", $postvars);

      $screen = '';
      if (!$page->isEmpty()) // Only output an html output if there is anything to output.
      {
        $screen = $page->render(atktext('app_shorttitle')." - ".$ui->title($this->m_module, $this->m_type,$this->m_postvars['atkaction']), $flags);
      }

      if(!$this->m_return_output)
      {
        $output = &atkOutput::getInstance();
        $output->output($screen);
      }

      // This is the end of all things for this page..
      // so we clean up some resources..
      $node = $this->getNode();
      $db = &$node->getDb();
      if (is_object($db)) $db->disconnect();
      atkdebug("disconnected from the database");

      if($this->m_return_output)
      {
        atkdebug("atkController::dispatch Return output ");
        return $screen;
      }
    }

    /**
     * Does the actual loading of the dispatch page
     * And adds it to the page for the dispatch() method to render.
     * @param array $postvars The request variables for the node.
     */
    function loadDispatchPage($postvars)
    {
      atkdebug("atkcontroller::loaddispatchpage ");
      $this->m_postvars = $postvars;
      $node = $this->getNode();
      if(!is_object($node)) return;

      $node->m_postvars = $postvars;
      $node->m_action = $postvars['atkaction'];
      $this->m_partial = $postvars["atkpartial"];

      atkdebug("atkcontroller::loaddispatchpage " . $node->m_action);

      $page = &$node->getPage();

      $page->register_script(atkconfig("atkroot")."atk/javascript/prototype/prototype.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/scriptaculous/scriptaculous.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/atkbusy.js");
      atkdebug("loaddispatchpage. action:" .$node->m_action) ;

      if ($node->allowed($node->m_action))
      {
        $secMgr = &atkGetSecurityManager();
        $secMgr->logAction($node->m_type, $node->m_action);
        $node->callHandler($node->m_action);

        list($selector,$id) = explode("=", atkarraynvl($node->m_postvars, "atkselector", "="));
        $page->register_hiddenvars(array("atknodetype"=>$node->m_module.".".$node->m_type, "atkselector"=>str_replace("'", "", $id)));
      }
      else
      {
        $page->addContent($this->genericPage(atktext('access_denied'), "<br><br>".atktext("error_node_action_access_denied", "", $node->m_type)."<br><br><br>"));
      }
    }

  /**
     * Render a generic page, with a box, title, stacktrace etc.
     * @param String $title The pagetitle and if $content is a string, also
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
      $node->addStyle("style.css");
      if (!is_array($content)) $content = array($title=>$content);
      $blocks = array();
      foreach($content as $itemtitle => $itemcontent)
        $blocks[] = $ui->renderBox(array("title"=>$itemtitle, "content"=>$itemcontent), 'dispatch');
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
     * @param String $action The action for which the page is rendered.
     * @param mixed $blocks Pieces of html content to be rendered. Can be a
     *                      single string with content, or an array with
     *                      multiple content blocks.
     * @return String Piece of HTML containing the given blocks and a pagetrail.
     */
    function renderActionPage($action, $blocks=array())
    {
      if(!is_array($blocks))
      {
        $blocks = ($blocks==""?array():array($blocks));
      }

      $node = $this->getNode();
      $ui = &$node->getUi();

      // todo: overridable action templates
      return $ui->render("actionpage.tpl", array("blocks"=>$blocks,
                                                 "title"=>$ui->title($node->m_module, $node->m_type, $action)));
    }

    /**
     * Use this function to enable feedback for one or more actions.
     *
     * When feedback is enabled, the action does not immediately return to the
     * previous screen, but first displays a message to the user. (e.g. 'The
     * record has been saved').
     *
     * @param mixed $action The action for which feedback is enabled. You can
     *                      either pass one action or an array of actions.
     * @param int $statusmask The status(ses) for which feedback is enabled.
     *                        If for example this is set to ACTION_FAILED,
     *                        feedback is enabled only when the specified
     *                        action failed. It is possible to specify more
     *                        than one status by concatenating with '|'.
     */
    function setFeedback($action, $statusmask)
    {
      if (is_array($action))
      {
        for ($i=0, $_i=count($action); $i<$_i; $i++)
        {
          $this->m_feedback[$action[$i]] = $statusmask;
        }
      }
      else
      {
        $this->m_feedback[$action] = $statusmask;
      }
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
     * @param String $action The action that was performed
     * @param int $status The status of the action.
     * @param array $record The record on which the action was performed.
     * @param String $message An optional message to pass to the feedbackpage,
     *                        for example to explain the reason why an action
     *                        failed.
     * @return String The feedback url.
     */
    function feedbackUrl($action, $status, $record="", $message="")
    {
      //Default we leave atkNodeType empty because the sessionmanager will determine
      //which is de atkNodeType
      $atkNodeType = "";
      if((isset($this->m_feedback[$action]) && hasFlag($this->m_feedback[$action], $status)) || $status==ACTION_FAILED)
      {
        $vars = array("atkaction"=>"feedback", "atkfbaction"=>$action, "atkactionstatus"=>$status,"atkfbmessage"=>$message);
        $node = $this->getNode();
        $sessionStatus = SESSION_REPLACE;
        $atkNodeType = $node->atkNodeType();
      }
      else $sessionStatus = SESSION_BACK;

      return (session_url($this->dispatchUrl($vars, $atkNodeType), $sessionStatus));
    }

    /**
     * Generate a dispatch menu URL for use with nodes
     * and their specific actions.
     * @param string $node the (module.)node name
     * @param string $action the atk action the link will perform
     * @param string $params: A key/value array with extra options for the url
     * @param string $phpfile The php file to use for dispatching, defaults to dispatch.php
     * @return string url for the node with the action
     */
    function dispatchUrl($params=array(), $atknodetype="", $file="")
    {
      //atkdebug("atkcontroller::dispatchurl");

      if (!is_array($params)) $params = array();
      $vars = array_merge($params, $this->m_hidden_vars);
      //atk_var_dump($vars, "atkcontroller::dispatchurl");
      if($file!="") $phpfile = $file;
      else $phpfile = $this->getPhpFile();

      //$dispatch_url = dispatch_url(($atknodetype=="" ? $node->atkNodeType() : $atknodetype), $vars["atkaction"], $vars, $phpfile);
      //When $atknodetype is empty this means that we use the atknodetype from session
      $dispatch_url = dispatch_url($atknodetype, $vars["atkaction"], $vars, $phpfile);

      return $dispatch_url;

    }

    /**
     * Returns the form buttons for a certain page.
     *
     * Can be overridden by derived classes to define custom buttons.
     * @param String $mode The action for which the buttons are retrieved.
     * @param array $record The record currently displayed/edited in the form.
     *                      This param can be used to define record specific
     *                      buttons.
     */
    function getFormButtons($mode, $record)
    {
      atkdebug("atkController::getFormButtons()");
      $result = array();

      // edit mode
      if ($mode == "edit")
      {
        // if atklevel is 0 or less, we are at the bottom of the session stack,
        // which means that 'saveandclose' doesn't close anyway, so we leave out
        // the 'saveandclose' button. Unless, a feedback screen is configured.
        if (atkLevel() >0 || hasFlag($this->m_feedback["update"], ACTION_SUCCESS))
        {
          $result[] = '<input type="submit" class="btn_saveandclose" name="atksaveandclose" value="'.atktext('saveandclose', "atk").'">';
        }
        $result[] = '<input type="submit" class="btn_save" name="atknoclose" value="'.atktext("save", "atk").'">';
        $result[] = '<input type="submit" class="btn_cancel" name="atkcancel" value="'.atktext("cancel", "atk").'">';
      }
      elseif ($mode == "add")
      {
        $node = $this->getNode();
        $result[] = '<input type="submit" class="btn_save" value="'.atktext("save", "atk").'">';
        // if action is admin, we don't show the cancelbutton or the add next button
        if ($node->hasFlag(NF_ADDAFTERADD) && !$node->hasFlag(NF_EDITAFTERADD))
          $result[] = '<input type="submit" class="btn_saveandnext" name="atksaveandnext" value="'.atktext("saveandnext", "atk").'">';
        $result[] = '<input type="submit" class="btn_cancel" name="atkcancel" value="'.atktext("cancel", "atk").'">';
      }
      elseif ($mode == "view")
      {
        $node = $this->getNode();
        if(atkLevel() > 0)
          $result[] = '<input type="submit" class="btn_cancel" name="atkback" value="&lt;&lt; '.atktext('back').'">';
        // if appropriate, display an edit button.
        if (!$node->hasFlag(NF_NO_EDIT)&&$node->allowed("edit", $record))
        {
          $result[] = '<input type="hidden" name="atkaction" value="edit">'.
                      '<input type="hidden" name="atknodetype" value="'.$node->atkNodeType().'">'.
                      '&nbsp;<input type="submit" class="btn_save" value="'.atktext('edit').'">&nbsp;';
        }
      }

      return $result;
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
     * Return php_file if not set returns PHP_SELF
     *
     * @return string
     */
    function getPhpFile()
    {
      $theme = &atkinstance('atk.ui.atktheme');

      if ($this->m_php_file != "") return $this->m_php_file;
      elseif ($theme->getAttribute('dispatcher') != '') return $theme->getAttribute('dispatcher');
      elseif (atkconfig("dispatcher") != '') return atkconfig("dispatcher");
      else return $_SERVER['PHP_SELF'];
    }

    /**
     * Return m_hidden_vars as html input types.
     *
     * @return string
     */
    function getHiddenVarsString()
    {
      if (count($this->m_hidden_vars) == 0) return "";
      foreach($this->m_hidden_vars as $hiddenVarName => $hiddenVarValue)
      {
        $varString .= '<input type="hidden" name="'.$hiddenVarName.'" value="'.$hiddenVarValue.'">';
      }
      return $varString;
    }

    function setReturnOutput($returnOutput)
    {
      atkdebug("atkcontroller::setReturnOutput() Setting return output to ". ($returnOutput ? "true":"false"));
      $this->m_return_output = $returnOutput;
    }


    function getReturnOutput()
    {
      return $this->m_return_output;
    }

    /**
     * Return a reference to the atkPage object. This object
     * is used to render output as an html page.
     *
     * @return object reference
     */
    function &getPage()
    {
      $page = &atkinstance("atk.ui.atkpage");
      return $page;
    }


    /**
     * Get the ui instance for drawing and templating purposes.
     *
     * @return atkUi An atkUi instance for drawing and templating.
     */
    function &getUi()
    {
      $ui = &atkinstance("atk.ui.atkui");
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
     * This will call dispatch(&$handler, $postvars, $fullpage) on your node class
     * if present, or dispatch($record, $postvars, $fullpage) in the handler if the node has
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
      $node = $this->getNode();
      if ($node!==NULL && method_exists($node, $methodname))
      {
        atkdebug("atkcontroller::invoke() Invoking '$methodname' override on node");
        // We pass the original object as last parameter to the override.
        array_push($arguments, $this);
        return call_user_func_array(array(&$node, $methodname), $arguments);
      }
      else if (method_exists($this, $methodname))
      {
        atkdebug("atkcontroller::invoke() Invoking '$methodname' on controller");
        return call_user_func_array(array(&$this, $methodname), $arguments);
      }
      atkerror("atkcontroller::invoke() Undefined method '$methodname' in atkController");
    }

  }
?>