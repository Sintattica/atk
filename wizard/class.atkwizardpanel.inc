<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage wizard
 *
 * @author maurice <maurice@ibuildings.nl>
 *
 * @copyright (c) 2006 Ibuildings.nl BV
 * @license see doc/LICENSE
 *
 * @version $Revision: 6309 $
 * $Id$
 */

  /**
   * Imports
   * @access private
   */
  atkimport("atk.handlers.atkactionhandler");
  atkimport("atk.handlers.atkaddhandler");
  atkimport("atk.handlers.atksavehandler");
  atkimport("atk.wizard.atkwizardcontroller");

  /**
   * atkWizardPanel button definitions.
   */
  define("FINISH_BUTTON_DEFAULT", 1);
  define("FINISH_BUTTON_SHOW", 2);
  define("FINISH_BUTTON_DONT_SHOW", 3);

  /**
   * Wizardpanel which binds to a atknode. This class makes
   * it possible to add atkNodeListeners and override actionhandlers.
   *
   * @author maurice <maurice@ibuildings.nl>
   * @package atk
   * @subpackage wizard
   *
   */
  class atkWizardPanel
  {
    /**
     * Reference to the parent wizard
     *
     * @var object reference
     */
    var $m_wizard;

    /**
     * Panel name
     *
     * @var string
     */
    var $m_panelName;

    /**
     * Name representation of the atkNode
     *
     * @var string
     */
    var $m_nodeName;

    /**
     * Reference to the atkNode
     *
     * @var object reference
     */
    var $m_node;

    /**
     * Array of Actionlisteners which listen
     * to the atkNode
     *
     * @var array
     */
    var $m_listeners;

    /**
     * Array of ActionHandlers which are binded
     * to the atkNode
     *
     * @var array
     */
    var $m_actionHandlers = array();

    /**
     * The default atkNode action for this wizardpanel
     *
     * @var string
     */
    var $m_defaultAction;

    /**
     * Explicity set the finish button on this panel
     *
     * @var boolean
     */
    var $m_showFinishButton = false;

    /**
     * atkWizardPanel constructor
     *
     * We don't construct the node here for performance reasons (if any?). We
     * probably don't need the node in most pageloads because we only use one
     * wizardpanel at a time.
     *
     * @param atkWizard $wizard The wizard
     * @param string $panelName The panelname
     * @param string $nodeName The nodename
     * @param string $defaultAction The default action
     * @param integer $finishButton The finish button to use
     * @return atkWizardPanel
     */
    function atkWizardPanel(&$wizard, $panelName, $nodeName, $defaultAction="", $finishButton=FINISH_BUTTON_DEFAULT)
    {
      $this->m_wizard = &$wizard;
      $this->m_panelName = $panelName;
      $this->m_nodeName = $nodeName;
      $this->m_defaultAction = $defaultAction;
      $this->m_showFinishButton = $finishButton;
    }

    /**
     * Return the atkNode object for this panel. If it didn't exist yet, this method 
     * will create it and add actionlistener or actionhandlers. The session will also
     * be manipulated so that the controller knows which atknodetype to render.
     *
     * @return object of type atknode
     */
    public function getPanelNode()
    {
      if (!is_object($this->m_node))
      {
        atkdebug("atkWizardPanel::getPanelNode() create node. Node name: " . $this->m_nodeName);

        $this->m_node = &getNode($this->m_nodeName);
        if(!is_object($this->m_node))
        {
          atkerror("atkWizardPanel::getPanelNode() Node could not be created. Node name: " . $this->m_nodeName);
          return NULL;
        }

        //Add listeners to node
        for($i=0, $_i=count($this->m_listeners); $i<$_i; $i++)
        {
          $this->m_node->addListener($this->m_listeners[$i]);
        }

        //Add actionhandlers to node
        for($i=0, $_i=count($this->m_actionHandlers); $i<$_i; $i++)
        {
          $handlerName = $this->m_actionHandlers[$i]['name'];
          $handlerAction = $this->m_actionHandlers[$i]['action'];

          $handler = &new $handlerName();
          atkRegisterNodeHandler($this->m_node->m_type, $handlerAction, $handler);
        }

        //All nodes should return the output and not try to fill the screen themselves
        $controller = &atkController::getInstance();
        $controller->setNode($this->m_node);
             
        //Make session aware of the fact that we are rendering a node which has been
        //created by a wizard panel and is not posted as a variable
        global $g_sessionManager;
        /* @var $g_sessionManager atkSessionManager  */
        $g_sessionManager->stackVar("atknodetype", $this->m_nodeName);
        $g_sessionManager->stackVar("atkaction", $this->m_defaultAction);
                   
        //We set how we want the atk page to be returned
        $this->m_wizard->setPageFlags(HTML_ALL);                        
      }
      return $this->m_node;
    }

    /**
     * Return the panel name
     *
     * @return unknown
     */
    public function getPanelName()
    {
      return $this->m_panelName;
    }

    /**
     * Add an atkActionListener to the atkWizardPanel.
     *
     * Listeners are added to the atkNode when the
     * node is created.
     *
     * @param atkActionListener $listener
     */
    function addListener(&$listener)
    {
      $this->m_listeners[] = &$listener;
    }


    /**
     * Add an atkActionHandler to the atkWizardPanel.
     *
     * Handlers are created and added to the atkNode when the
     * node is created.
     *
     * @param string $handlerName Name of the handler to add
     * @param string $handlerAction The action
     */
    public function addActionHandler($handlerName, $handlerAction)
    {
      $this->m_actionHandlers[] = array("name" => $handlerName, "action" => $handlerAction);
    }

    /**
     * Do some session manipulations
     *
     * @return unknown
     */
    function dispatchForm()
    {
      $node = &$this->getPanelNode();
      if(!is_object($node)) return "";

      /* @var $g_sessionManager atkSessionManager  */
      global $ATK_VARS, $g_sessionManager;      
	    
	    if(!isset($ATK_VARS['atkaction']) || $ATK_VARS['atkaction'] == "")
	    {
	      if(!isset($this->m_defaultAction) || $this->m_defaultAction == "")
	      {
	        $ATK_VARS['atkaction'] = "add";
	        $g_sessionManager->stackVar("atkaction", "add");
	      }
	      else
	      {
	        $ATK_VARS['atkaction'] = $this->m_defaultAction;
	        $g_sessionManager->stackVar("atkaction", $this->m_defaultAction);
	      }
	    }

	    //Load some smarty vars
	    if($ATK_VARS['atkaction'] == 'add' || $ATK_VARS['atkaction'] == 'edit' || $ATK_VARS['atkaction'] == 'admin')
	    {
	      $this->loadRenderBoxVars($ATK_VARS['atkaction'] );
	      if($ATK_VARS['atkaction'] == 'admin') $this->loadRenderBoxVars("add");
	    }
    }

    /**
     * We are saving a newly added record. On success or failure ATK will redirect.
     * On failure the session stack of the previous level is loaded to show the 
     * same wizardpanel again (now with error message). On succes we need to set
     * some redirect vars to make sure we go to the next wizardpanel.     
     *
     * @return bool on successfully executing this function
     */    
    function save()
    {
      $node = &$this->getPanelNode();
      if(!is_object($node)) return;

      //Little dirty, we have to do this to make sure that in case
      //of an error the paneltitle and intro text are known
      //in the template
      //$this->loadRenderBoxVars("add");
      //if($node->m_action == 'admin') $this->loadRenderBoxVars("admin");
      
      return true;
    }

    /**
     * Return the setting which specifies if the button
     * should be shown, not shown or should depend on the
     * default behaviour. Default behaviour is that the last
     * panel in the wizard will have a finish button.
     *
     * @return int FINISH_BUTTON_DEFAULT
     *             FINISH_BUTTON_SHOW
     *             FINISH_BUTTON_DONT_SHOW
     */
    function showFinishButton()
    {
      return $this->m_showFinishButton;
    }

    /**
     * Check to see if this wizardpanel is the last panel before the finish screen.
     *
     * @return bool
     */
    function isFinishPanel()
    {
      return $this->m_wizard->isFinishPanel($this->getPanelName());
    }

    /**
     * Return a translation from the language file with a fallback option.
     *
     * @param string $key
     * @param string $fallbackKey
     * @return string
     */
    function _getText($key, $fallbackKey)
    {

      $panelNode = $this->getPanelNode();
      $text = atktext($key,
                     $panelNode->m_module,
                     "",
                     "",
                     "",
                     true);
      
      if($text != "") return $text;
      
      return  atktext($fallbackKey,
                     $panelNode->m_module,
                     "",
                     "",
                     "",
                     true);
    }

    /**
     * Add smarty vars to actionhandler
     *
     * @param string $action
     */
    function loadRenderBoxVars($action)
    {
      $handler = &$this->m_node->getHandler($action);
      $handler->addRenderBoxVar("paneltitle", $this->_getText($this->getPanelName()."_".$action, $this->getPanelName()));
      $handler->addRenderBoxVar("intro", $this->_getText($this->getPanelName()."_".$action."_intro", $this->getPanelName()."_intro"));
      atkdebug("loadRenderBoxVars actionhandler: ". get_class($handler)." action: ".$action);
    }

  }
?>
