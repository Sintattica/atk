<?php

atkimport('atk.wizard.atkwizard');
atkimport('atk.wizard.atkwizardpanel');
  
  /**
   * test_atkwizard class file
   *
   * @package atk
   * @subpackage testcases
   *
   * @author maurice <maurice@ibuildings.nl>
   *
   * @copyright (c) 2008 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   *
   * @version $Revision: $
   * $Id:  $
   */
  
  /**
   * Testcases for atkWizard
   *
   * @author maurice <maurice@ibuildings.nl>
   * @package atk
   * @subpackage testcases
   */
  class test_atkwizard extends atkTestCase 
  {
    /**
     * Test if construction of wizard goes well and if we can add
     * wizardpanels.
     */
    public function test_wizard_construction()
    {
      $wizard         = &new atkWizard();
      
      $this->assertTrue(is_object($wizard->m_self)); // The m_self variable should be set.
      $this->assertEqual($wizard->m_isWizardInitiated, true, "Initiation status is set to true.");
      $this->assertEqual($wizard->m_currentPanelIndex, 0, "Wizard panel index should start at 0.");
      
      //Add a wizardpanel
      $firstPanel = new atkWizardPanel($wizard, "FirstPanel", "firstTestNode");
      $wizard->addPanel($firstPanel);
      
      $secondPanel = new atkWizardPanel($wizard, "SecondPanel", "secondTestNode");
      $wizard->addPanel($secondPanel);
      
      $currentPanel = $wizard->getCurrentPanel();
      /* @var $currentPanel atkwizardpanel */
      $this->assertTrue(is_object($currentPanel)); 
      $this->assertEqual($currentPanel->getPanelName(), "FirstPanel", "Currentpanel should have the name 'FirstPanel'");
    }
  }