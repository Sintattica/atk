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
   * @version $Revision: 6323 $
   * $Id$
   */

  /**
   * Imports
   * @access private
   */
  atkimport("atk.wizard.atkwizardbase");
  atkimport("atk.wizard.atkwizardactionloader");
  atkimport("atk.wizard.atkwizardpanel");
  atkimport("atk.atknodetools");

  /**
   * atkWizard modees
   */
  define("WIZARD_MODE_ADD"     , 1);
  define("WIZARD_MODE_EDIT"    , 2);

  /**
   * atkWizard class which is capable of using atknodes as 
   * wizardpanels.
   *
   * This class makes the distinction between update/save and
   * navigation actions from the wizard and respondis correspondingly.
   * 
   * atkWizard's highest baseclass is atkController. When an atkWizard
   * is created and becomes the controller of the atkapp, it will stay
   * the controller untill we take a non wizard specific action.
   *
   * @author maurice <maurice@ibuildings.nl>
   * @package atk
   * @subpackage wizard
   *
   */
class atkWizard extends atkWizardBase
{
    /**
     * Array of panels
     * First item is first to show in wizard
     *
     * @access private
     * @var array
     */
  var $m_panelList = array();

    /**
     * Key/value array containing panel name as key and
     * index as value
     * 
     * @access private
     * @var array key/value panel name / index
     */
  var $m_panelIndex = array();

    /**
     * The mode in which the wizard is run (add/edit).
     * Default is add
     *
     * @access private
     * @var array
     */
  var $m_mode = WIZARD_MODE_ADD;

    /**
     * Key-value array which will be added as hidden value in edit/add forms
     *
     * @access private
     * @var array
     */
  var $m_hiddenParams = array();

    /**
     * Key-value array which will be url when redirecting to a new page
     *
     * @access private
     * @var array
     */
  var $m_redirectParams = array();

    /**
     * Index of panel which is currently shown or processed
     *
     * @access private
     * @var array
     */
  var $m_currentPanelIndex;

    /**
     * Value needed by atkPage to render the content of a page. Default
     * we need to render a complete html page, but it might be necesary to only 
     * render the content part.
     *
     * @var int define value
     */
  var $m_page_flags = HTML_ALL;

  /**
     * We use a reference to ourselfs because we pass data to the wizardpanel
     * which has a reference to its wizard parent.
     *
     * @access private
     * @var object
     */
  var $m_self;

  /**
     * Action to perform in the wizard. Actions can be: next, finish, saveandnext, 
     * saveandaddnew.
     *
     * @var string
     */
  var $m_wizardAction;

  /**
   * It can com in handy to know if we are at the start of the wizard and have
   * not performed any wizard actions. 
   *
   * @var bool
   */
  var $m_isWizardInitiated;
  
  /**
     * Constructor
     *
     * @return atkWizard
     */    
  function atkWizard()
  {
    atkdebug("atkWizard::constructor" . atkGetPostVar("atkcontroller"));
    
    $this->atkWizardBase();

    $this->collectWizardSessionData();
    $this->setHiddenParam("atkcontroller", $this->m_module_name.$this->m_name);
    
    $this->checkWizardInitiationStatus();

    $this->m_self = &$this;
  }
  
  /**
   * This function is called from the constructor to check if the wizard
   * goes through the initialisation. This means that the first panel is 
   * shown and no wizard actions have been taken.
   * 
   * We check this by saving searching the sessing for the var 
   * 'wizard_initiation_level'. If it does not exist we fill it with the
   * current atkLevel. 
   *
   */
  private function checkWizardInitiationStatus()
  {
    
    global $g_sessionManager;
    /* @var $g_sessionManager atkSessionManager*/
    $level = $g_sessionManager->getValue("wizard_initiation_level");
    if($level === null)
    {
      $g_sessionManager->globalVar("wizard_initiation_level", atkLevel());
      $this->m_isWizardInitiated = true;
    }
    else 
    {
      $this->m_isWizardInitiated = ($level === atkLevel());      
    }
     
  }

  /**
   * This function is called from the constructor. Its basic function is 
   * to collect wizard information from the session stack. To run the wizard
   * we need to be able to use atk's session stack. Default it is turned on,
   * but it might be turned off explicitly.
   *
   */
  private function collectWizardSessionData()
  {
    //Get session vars
    global $g_sessionManager;

    $this->m_wizardAction = AtkWizardActionLoader::getWizardAction($g_sessionManager->stackVar("atkwizardaction"));

    $this->m_currentPanelIndex = $g_sessionManager->stackVar("atkwizardpanelindex");
    if($this->m_currentPanelIndex == "")
      $this->m_currentPanelIndex = 0;

    global $g_sessionData;
    atk_var_dump($g_sessionData["default"]["stack"][atkStackID()], "SESSION DATA");
  }

  /**
   * Set the wizard mode. Options: WIZARD_MODE_ADD (default), WIZARD_MODE_EDIT
   *
   * @param integer $mode
   */
  public function setMode($mode = WIZARD_MODE_ADD)
  {
    $this->m_mode = $mode;
  }

  /**
   * Get the wizard mode
   *
   * @return define value
   */
  public function getMode()
  {
    return $this->m_mode;
  }

  /**
   * Add wizardpanel to the wizard. Only objects of type
   * atkWizardPanel can be added. The order of additions
   * to the wizard determines order in which the panels are
   * shown.
   *
   * @param object $atkWizardPanel
   */
  public function addPanel(&$atkWizardPanel)
  {
    $this->m_panelList[] = &$atkWizardPanel;
    $this->m_panelIndex[$atkWizardPanel->getPanelName()] = count($this->m_panelList)-1;
  }

  /**
   * Main execution function (start of the wizard). Every page load will go through
   * this function. This function overrides atkController::handleRequest but it always 
   * calls its parent in the end.
   *
   * @return String The html output
   */
  public function handleRequest()
  {
    atkdebug("atkwizard::handleRequest()");

    global $g_sessionManager, $ATK_VARS;

    if (!$this->m_isWizardInitiated && atkGetPostVar("atkaction") !== "" && atkGetPostVar("atkwizardaction") === "" )
    {
      //A not wizard related action as initiated, like an edit action
      //on an admin screen. Set the wizard mode to NULL to prevent showing a wizard
      //specific form. We do not use the wizard dispatcher but call the standard
      //handleRequest of atkController.
      $this->setMode(NULL);
    }
    else
    {
  
      // get our node
      $currentWizardPanel = &$this->getCurrentPanel();
      $node = $currentWizardPanel->getPanelNode();
  
      // check if user has filled in something
      if ((atkGetPostVar("atkaction") == 'save' || atkGetPostVar("atkaction") == "update" || atkGetPostVar("atkaction") == "delete") && $node->filledInForm())
      {
        //save form
        $this->save($this->m_wizardAction);
      }
      elseif ($this->m_wizardAction == 'finish')
      {
        //finish wizard
        $finishOutput = $this->finish();
        if($this->getReturnOutput())
         return $finishOutput;
        else 
        {
           $output = &atkOutput::getInstance();
           $output->output($finishOutput);        
           return "";
        }
      }
      else
      {        
        if(!((atkGetPostVar("atkaction") == 'save' || atkGetPostVar("atkaction") == "update" || atkGetPostVar("atkaction") == "delete") && $node->filledInForm()))
        {          
          $ATK_VARS['atkaction'] = "";
        }
        $this->wizardDispatch($this->m_wizardAction);
      }	    
    }
    
    $output = parent::handleRequest($ATK_VARS, $this->m_page_flags);
    return $output;
  }

  /**
   * Determine the wizardPanel to load and let the wizardpanel
   * do some dispatching too.
   *
   * @param String $atkWizardaction
   */
  protected function wizardDispatch($atkWizardaction)
  {
    if($atkWizardaction == 'next' || $atkWizardaction == 'saveandnext')
    {
      $wizardPanel = $this->m_panelList[$this->m_currentPanelIndex+1];
      $this->setHiddenParam("atkwizardpanelindex", $this->m_currentPanelIndex+1);

      $this->m_currentPanelIndex += 1;
    }
    elseif($atkWizardaction == 'saveandaddnew' || $atkWizardaction == 'delete')
    {
      //We saved the panel but we want to show the same panel again.
      $wizardPanel = $this->m_panelList[$this->m_currentPanelIndex];
      $this->setHiddenParam("atkwizardpanelindex", $this->m_currentPanelIndex);
    }
    else
    {
      //first panel
      $this->m_currentPanelIndex = 0;
      $wizardPanel = $this->m_panelList[0];
      $this->setHiddenParam("atkwizardpanelindex", 0);
    }
    
    //Save the hiddenparams as hiddenvars in the controller
    if(count($this->m_hiddenParams))
    {
      foreach($this->m_hiddenParams as $hiddenVarName => $hiddenVarValue)
      {
        $this->setHiddenVar($hiddenVarName, $hiddenVarValue);
      }
    }
    
    $wizardPanel->dispatchForm();
  }

  /**
   * We are saving a newly added record. On success or failure ATK will redirect.
   * On failure the session stack of the previous level is loaded to show the 
   * same wizardpanel again (now with error message). On succes we need to set
   * some redirect vars to make sure we go to the next wizardpanel.     
   *
   * @param string $atkwizardaction The wizard action for redirecting
   * @return bool on successfully executing this function
   */
  protected function save($atkwizardaction)
  {
    $this->setRedirectParam("atkwizardpanelindex", $this->m_currentPanelIndex);
    $this->setRedirectParam("atkwizardaction", $atkwizardaction);

    //Save redirect params as hidden vars in the controller.
    //TODO/Fixme Why the distinction between hiddenvars and redirect params.
    if(count($this->m_redirectParams))
    {
      foreach($this->m_redirectParams as $hiddenVarName => $hiddenVarValue)
        $this->setHiddenVar($hiddenVarName, $hiddenVarValue);
    }
    
    $currentWizardPanel = $this->getCurrentPanel();
    $currentWizardPanel->save();
    
    return true;
  }

  /**
     * Finish the wizard
     *
     * This function can be overriden in the your extended wizard class.
     *
     * TODO/FIXME It would be even more cool if wizardpanels could listen to
     * the finish action and be notified of this action/event.
     *
     */
  protected function finish()
  {
    $node = &$this->getNode();
      
    $content = "<br><br>".atktext("content_wizard_finished")."<br><br><br>";
    
    $page = &$node->getPage();  
    $page->addContent($this->genericPage(atktext("title_wizard_finished"), $content));
    return $page->render($this->getHtmlTitle(), $this->m_page_flags);
  }

  /**
   * Collect the variables which should be set as hidden input fields in forms. 
   * Save them in a key/value array.
   *
   * @param string $key
   * @param mixed $value
   */
  public function setHiddenParam($key, $value)
  {
    $this->m_hiddenParams[$key] = $value;
  }

  /**
   * Return the hidden params
   *
   * @return array
   */
  public function getHiddenParam()
  {
    return $this->m_hiddenParams;
  }

  /**
   * Collect the variables which should be added to the redirect url. 
   * Save them in a key/value array.
   *
   * @param string $key
   * @param mixed $value
   */
  public function setRedirectParam($key, $value)
  {
    $this->m_redirectParams[$key] = $value;
  }

  /**
   * Return the current active panel object. 
   *
   * @return object of type atkWizardPanel
   */
  public function &getCurrentPanel()
  {
    if(!is_object($this->m_panelList[$this->m_currentPanelIndex]))
    {
      atkerror("Panel could not be crated. Non existing panel index: " . $this->m_currentPanelIndex);
      return NULL;
    }
    return $this->m_panelList[$this->m_currentPanelIndex];
  }

  /**
   * Return if this panel is the last one in the wizard
   *
   * @param string $name
   * @return bool
   */
  public function isFinishPanel($name)
  {
    $count = count($this->m_panelList);

    if (!isset($this->m_panelIndex[$name])) return false;
    return (($count-1) == $this->m_panelIndex[$name]);
  }

  /**
   * Set a defined value needed by atkPage to render the content of a page. Default
   * we need to render a complete html (HTML_ALL) page, but it might be necesary to only 
   * render the content part.
   *
   * @param define $flag
   */
  public function setPageFlags($flag)
  {
    $this->m_page_flags = $flag;
  }

  /**
   * Set the value of the html meta tag. This is an override of the atkcontroller::setHtmlTitle.
   * We now want to show the wizard name, wizard panel and which step we are at. 
   *
   */
  public function setHtmlTitle()
  {
    $node = &$this->getNode();
    $ui = &$node->getUi();
    return atktext('app_shorttitle')." - ".$ui->getWizardTitle($this, $this->getCurrentPanel());
  }

  /**
     * Return the title to be show on top of an Action Page. This is an override of the atkController::actionPageTitle function.
     * We now want to show the wizard name, wizard panel and which step we are at. 
     *
     * @return string The title
     */
  public function actionPageTitle()
  {
    $node = $this->getNode();
    $ui = $node->getUi();
    $page = $node->getPage();
    $page->setTitle($ui->getWizardTitle($this, $this->getCurrentPanel(), $node->m_action));
  }
  
  /**
   * Return the current wizardaction
   *
   * @return string wizardaction (eg next, saveandnext, saveandaddnew, finish).
   */
  public function getWizardAction()
  {
    return $this->m_wizardAction;
  }
}
?>