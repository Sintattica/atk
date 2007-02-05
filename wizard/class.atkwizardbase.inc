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
 
atkimport("atk.atkcontroller");
 
/**
 * atkWizard class which is capable of using atknodes
 *
 * This class makes the distinction between update/save and 
 * navigation actions and respondis correspondingly.
 *
 * @author maurice <maurice@ibuildings.nl>
 * @package atk
 *
 */
  class atkWizardBase extends atkController
  {
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
    function dispatch($postvars, $fullpage=true)
    {
      // Use invoke to be backwards compatible with overrides 
      // of loadDispatchPage in atknode.
      atkdebug("atkWizardBase::dispatch");
      $this->invoke("loadDispatchPage", $postvars);
      $panel = &$this->getCurrentPanel();
      $node = &$panel->getPanelNode();
      $ui = &$this->getUi();
      $page = &$this->getPage();
      atkdebug("atkWizardBase::dispatch page->render flags: " . $this->m_page_flags);
      $screen = $page->render(atktext('app_shorttitle')." - ".$ui->title($node->m_module, $node->m_type, $node->m_postvars['atkaction'], true), $this->m_page_flags);
      //atkdebug("screen $screen return output ". ($this->m_return_output ? "true":"false"));
      if(!$this->m_return_output)
      {
        $output = &atkOutput::getInstance();
        $output->output($screen);
      }

      // This is the end of all things for this page..
      // so we clean up some resources..
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
     * Returns the form buttons for a certain page.
     *
     * Can be overridden by derived classes to define custom buttons.
     * @param String $action The action for which the buttons are retrieved.
     * @param array $record The record currently displayed/edited in the form.
     *                      This param can be used to define record specific
     *                      buttons.
     */
    function getFormButtons($action, $record)
    {
      $result = array();
      
      if ($this->m_mode == WIZARD_MODE_ADD)
      {
        $currentPanel = $this->getCurrentPanel();
        // We post the action as key value in de atkwizardaction var. Therefor
        // we have to convert the atkwizardaction value in atkWizard::start().                
        $node = &$this->getNode();
        if($node->m_action != 'admin')
        {
          //if we explicitly don't want the finish button we set a hidden var to post the atkwizardaction          
          if(($currentPanel->showFinishButton() == FINISH_BUTTON_DONT_SHOW) && $currentPanel->isFinishPanel())
            $atkwizardaction = "finish";
          else 
            $atkwizardaction = "next";
          
          if($this->showFinishButton())
            $result[] = '<input type="submit" class="btn_next" name="atkwizardaction[finish]" value="'.atktext("finish", "atk").'">';
          else 
            $result[] = '<input type="submit" class="btn_next" name="atkwizardaction['.$atkwizardaction.']" value="'.atktext("next", "atk").'">';
        }
        else 
        {
          //if we explicitly don't want the finish button we set a hidden var to post the atkwizardaction
          if($currentPanel->showFinishButton() == FINISH_BUTTON_DONT_SHOW && $currentPanel->isFinishPanel())
            $atkwizardaction = "finish";
          else 
            $atkwizardaction = "saveandnext";
          
          $result[] = '<input type="submit" class="btn_next" name="atkwizardaction[saveandaddnew]" value="'.atktext("saveandaddnew", "atk").'">';
          if($this->showFinishButton())
            $result[] = '<input type="submit" class="btn_next" name="atkwizardaction[finish]" value="'.atktext("finish", "atk").'">';
          else
            $result[] = '<input type="submit" class="btn_next" name="atkwizardaction['.$atkwizardaction.']" value="'.atktext("saveandnext", "atk").'">';
        }
        
        $result[] = '<input type="submit" class="btn_cancel" name="atkwizardcancel" value="'.atktext("cancel", "atk").'">';
      }
      elseif ($this->m_mode == WIZARD_MODE_EDIT)
      {
        // We post the action as key value in de atkwizardaction var. Therefor
        // we have to convert the atkwizardaction value in atkWizard::start().        
        $result[] = '<input type="submit" class="btn_save" name="atknoclose" value="'.atktext("save", "atk").'">';
        $result[] = '<input type="submit" class="btn_next" name="atkwizardaction[finish]" value="'.atktext("finish", "atk").'">';          
        $result[] = '<input type="submit" class="btn_cancel" name="atkcancel" value="'.atktext("cancel", "atk").'">';
      }
      else
      {
        $result = parent::getFormButtons($action, $record);
      }

      return $result;
    }    

    /**
     * Determine if this panel should show a finish button
     */
    function showFinishButton()
    {
      $currentPanel = $this->getCurrentPanel();
      return (($currentPanel->showFinishButton() == FINISH_BUTTON_SHOW) ||
               ($currentPanel->showFinishButton() == FINISH_BUTTON_DEFAULT && $currentPanel->isFinishPanel()));
    }    
  }
?>
