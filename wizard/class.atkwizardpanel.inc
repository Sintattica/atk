<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 *
 * @author maurice <maurice@ibuildings.nl>
 *
 * @copyright (c) 2006 Ibuildings.nl BV
 * @license see doc/LICENSE
 *
 * @version $Revision$
 * $Id$
 */

atkimport("atk.handlers.atkactionhandler");
atkimport("atk.handlers.atkaddhandler");
atkimport("atk.handlers.atksavehandler");
atkimport("atk.wizard.atkwizardcontroller");


define(FINISH_BUTTON_DEFAULT, 1);
define(FINISH_BUTTON_SHOW, 2);
define(FINISH_BUTTON_DONT_SHOW, 3);

/**
 * Wizardpanel which binds to a atknodes
 *
 * Full description of the class
 *
 * @author maurice <maurice@ibuildings.nl>
 * @package atk
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
     * @param unknown_type $wizard
     * @param unknown_type $panelName
     * @param unknown_type $nodeName
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

    function getPanelNode()
    {
      if (!is_object($this->m_node))
      {
        atkdebug("atkWizardPanel::getPanelNode() create node. Node name: " . $this->m_nodeName);

        $this->m_node = &getNode($this->m_nodeName);
        if(!is_object($this->m_node))
        {
          atkerror("atkWizardPanel::getPanelNode() Node could not be created. Node name: " . $this->m_nodeName);
          return;
        }

        //Set wizard specific controller
        atkdebug("atkWizardPanel::getPanelNode() Register controller for. Node: " . getNodeType($this->m_nodeName));
        //$wizardcontrollerclass = $this->m_wizard->m_wizardcontroller;
        //registerNodeController(getNodeType($this->m_nodeName), new $wizardcontrollerclass($this->m_node, $this));

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

          atkdebug("atkWizardPanel::getPanelNode() registerNodeHandler for. Node: " . getNodeType($this->m_nodeName). " name: ".$handlerName. " action ". $handlerAction);

          $handler = &new $handlerName();
          registerNodeHandler($this->m_node->m_type, $handlerAction, &$handler);
        }

        //todo: not sure if this needed
        //Add postvars to node manually
        //$this->m_node->m_postvars = atkGetPostVar();

        //All nodes should return the output and not try to fill the screen themselves
        $controller = &atkController::getInstance();
        $controller->setNode($this->m_node);
        $controller->setReturnOutput(true);
        $controller->setPhpFile("index.php");
        //We set how we want the atk page to be returned
        atkdebug("getPanelNode flags" . $this->m_wizard->m_page_flags . " controller ".$controller->m_page_flags);
        $this->m_wizard->setPageFlags(HTML_ALL);
        atkdebug("getPanelNode flags" . $this->m_wizard->m_page_flags . " controller ".$controller->m_page_flags);

      }
      return $this->m_node;
    }

    function getPanelName()
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
      $this->m_listeners[] = $listener;
    }


    /**
     * Add an atkActionHandler to the atkWizardPanel.
     *
     * Handlers are created and added to the atkNode when the
     * node is created.
     *
     * @param atkActionHandler $listener
     */
    function addActionHandler($handlerName, $handlerAction)
    {
      $this->m_actionHandlers[] = array("name" => $handlerName, "action" => $handlerAction);
    }

    function dispatchForm($params)
    {
      atkdebug("atkWizardPanel::dispatchForm() panel=".$this->getPanelName() . " atknode=".$this->m_nodeName);
      $node = &$this->getPanelNode();
      if(!is_object($node)) return;

      $controller = &atkController::getInstance();

      if(count($params))
      {
        foreach($params as $hiddenVarName => $hiddenVarValue)
        {
          $controller->setHiddenVar($hiddenVarName, $hiddenVarValue);
        }
      }

	    global $ATK_VARS;
	    if(!isset($ATK_VARS['atkaction']) || $ATK_VARS['atkaction'] == "")
	    {
	      if(!isset($this->m_defaultAction) || $this->m_defaultAction == "")
	        $ATK_VARS['atkaction'] = 'add';
	      else
	        $ATK_VARS['atkaction'] = $this->m_defaultAction;
	    }

	    //Load some smarty vars
	    if($ATK_VARS['atkaction'] == 'add' || $ATK_VARS['atkaction'] == 'edit' || $ATK_VARS['atkaction'] == 'admin')
	    {
	      $this->loadRenderBoxVars($ATK_VARS['atkaction'] );
	      if($ATK_VARS['atkaction'] == 'admin') $this->loadRenderBoxVars("add");

	      //TODO Fixme: Somehow we can't save the Renderboxvars in this object
	      //why doesn't this work for override classes. Some kind of reference problem
	      //$handler = &$this->m_node->getHandler($ATK_VARS['atkaction']);
        //atk_var_dump($handler->m_renderBoxVars, "dispatchForm m_renderBoxVars");
	    }

	    //If atk renders the page itself the return value is empty
	    $output = $controller->dispatch($ATK_VARS, false);

      return $output;
    }

    //Rename to more generic name
    function save()
    {
      atkdebug("atkWizardPanel::save node: " . $this->m_nodeName);
      atk_var_dump(atkGetPostVar(), "atkgetpostvar");
      $node = &$this->getPanelNode();
      if(!is_object($node)) return;

      $controller = &atkController::getInstance();
      $hiddenVars = $controller->m_redirectParams;
      atk_var_dump($hiddenVars, "atkWizardPanel::save");
      if(count($hiddenVars))
      {
        foreach($hiddenVars as $hiddenVarName => $hiddenVarValue)
          $controller->setHiddenVar($hiddenVarName, $hiddenVarValue);
      }

      //Little dirty, we have to do this to make sure that in case
      //of an error the paneltitle and intro text are known
      //in the template
      $this->loadRenderBoxVars("add");
      if($node->m_action = 'admin') $this->loadRenderBoxVars("admin");

      //Save, on succes dispatch() will redirect
      //else it will return an 'add form' with error messages
      atkdebug("atkwizardpanel::save call controller_dispatch()");
      return $controller->dispatch(atkGetPostVar(), false);
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

    function isFinishPanel()
    {
      return $this->m_wizard->isFinishPanel($this->getPanelName());
    }

    function _getText($key, $fallbackKey)
    {

      $panelNode = $this->getPanelNode();
      $text = atktext($key,
                     $panelNode->m_module,
                     "",
                     "",
                     "",
                     true);
      atkdebug("_getText key: " .$key." text: ".$text);
      if($text != "") return $text;
      atkdebug("_getText key: " .$key." fallbackKey: ".$fallbackKey);

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
      atk_var_dump($handler->m_renderBoxVars, "loadRenderBoxVars m_renderBoxVars");
    }

  }
?>
