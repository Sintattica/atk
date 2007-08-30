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
   * @version $Revision$
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
   * atkWizard class which is capable of using atknodes
   *
   * This class makes the distinction between update/save and
   * navigation actions and respondis correspondingly.
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
     * return the html for the current panel or let the node display itself
     *
     * @access public
     * @var boolean
     */
    //var $m_returnOutput;


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
     * Constructor
     *
     * @param boolean $returnOutput Indicate if this wizard should return output or not
     * @return atkWizard
     */
    function atkWizard()
    {
      atkdebug("atkWizard::constructor");
      $this->atkWizardBase();
      if (atkGetPostVar("atkwizardpanelindex")!="")
        $this->m_currentPanelIndex = atkGetPostVar("atkwizardpanelindex");
      else
        $this->m_currentPanelIndex = 0;

      $this->m_self = &$this;
    }

    function setMode($mode = WIZARD_MODE_ADD)
    {
      $this->m_mode = $mode;
    }

    function getMode()
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
    function addPanel(&$atkWizardPanel)
    {
      $this->m_panelList[] = &$atkWizardPanel;
      $this->m_panelIndex[$atkWizardPanel->getPanelName()] = count($this->m_panelList)-1;
    }

    /**
     * Main execution function (start of the wizard). Every page load will go through
     * this function
     *
     * @return String The html output
     */
    function execute()
    {
      atkdebug("atkwizard::execute()");
      $atkwizardaction = AtkWizardActionLoader::getWizardAction(atkGetPostVar("atkwizardaction"));

      // get our node
      $currentWizardPanel = &$this->getCurrentPanel();
      $node = $currentWizardPanel->getPanelNode();

      // check if user has filled in something
      if ((atkGetPostVar("atkaction") == 'save' || atkGetPostVar("atkaction") == "update" || atkGetPostVar("atkaction") == "delete") && $node->filledInForm())
      {
        //save form
        return $this->save($atkwizardaction);
      }
      elseif ($atkwizardaction == 'finish')
      {
        return $this->finish();
      }
      else
      {
        if(!((atkGetPostVar("atkaction") == 'save' || atkGetPostVar("atkaction") == "update" || atkGetPostVar("atkaction") == "delete") && $node->filledInForm()))
        {
          global $ATK_VARS;
          $ATK_VARS['atkaction'] = "";
        }
        return $this->wizardDispatch($atkwizardaction);
      }
    }

    /**
     * Determine the wizardPanel to load and render the form
     * for this wizardPanel.
     *
     * @param String $atkWizardaction
     * @return String The html output
     */
    function wizardDispatch($atkWizardaction)
    {
      atkdebug("wizardDispatch");

      if($atkWizardaction == 'next' || $atkWizardaction == 'saveandnext')
      {
        atkdebug("wizardDispatch next");
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
      return $wizardPanel->dispatchForm($this->m_hiddenParams);
    }

    //Rename to more generic name
    function save($atkwizardaction)
    {
      atkdebug("atkwizard::save()");

      $this->setRedirectParam("atkwizardpanelindex", $this->m_currentPanelIndex);
      $this->setRedirectParam("atkwizardaction", $atkwizardaction);

      $currentWizardPanel = $this->getCurrentPanel();
      return $currentWizardPanel->save();
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
    function finish()
    {
      return "The wizard has finished.";
    }

    function setHiddenParam($key, $value)
    {
      $this->m_hiddenParams[$key] = $value;
    }

    function getHiddenParam()
    {
      return $this->m_hiddenParams;
    }


    function setRedirectParam($key, $value)
    {
      $this->m_redirectParams[$key] = $value;
    }

    function &getCurrentPanel()
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
     * @param unknown_type $name
     * @return unknown
     */

    function isFinishPanel($name)
    {
      $count = count($this->m_panelList);
      atkdebug("count: ".$count."panelname: ".$name." panelnameindex: ".$this->m_panelIndex[$name]);
      if (!isset($this->m_panelIndex[$name])) return false;
      return (($count-1) == $this->m_panelIndex[$name]);
    }


    function setPageFlags($flag)
    {
      $this->m_page_flags = $flag;
    }
  }
?>