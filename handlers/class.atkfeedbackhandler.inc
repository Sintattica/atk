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
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Handler class for the feedback action of a node. The handler draws a
   * screen with a message, giving the user feedback on some action that
   * occurred.
   *
   * @author Ivo Jansch <ivo@achievo.org>   
   * @package atk
   * @subpackage handlers
   *
   */
  class atkFeedbackHandler extends atkActionHandler
  {
    /**
     * The action handler method.
     */
    function action_feedback()
    {
      $page = &$this->getPage();
      $output = $this->invoke("feedbackPage", $this->m_postvars["atkfbaction"], $this->m_postvars["atkactionstatus"], $this->m_postvars["atkfbmessage"]);
      $page->addContent($this->m_node->renderActionPage("feedback", $output));
    }
    
    /**
     * The method returns a complete html page containing the feedback info.
     * @param String $action The action for which feedback is provided
     * @param int $actionstatus The status of the action for which feedback is
     *                          provided
     * @param String $message An optional message to display in addition to the
     *                        default feedback information message.
     * 
     * @return String The feedback page as an html String.
     */
    function feedbackPage($action, $actionstatus, $message="")
    {
      $node = &$this->m_node;
      $ui = &$this->getUi();            
      
      $node->addStyle("style.css");

      $params["content"] = text('feedback_'.$action.'_'.atkActionStatus($actionstatus), $node->m_type).
                           ' <br>'.$message;

      if (atkLevel()>0)
      {      
        $params["formstart"] = '<form method="get">'.session_form(SESSION_BACK);
        $params["buttons"][] = '<input type="submit" class="btn_cancel" value="&lt;&lt; '.text('back').'">';
        $params["formend"] = '</form>';
      }
      
      $output = $ui->renderAction($action, $params);
              
      return $ui->renderBox(array("title"=>$node->actionTitle($action),
                                  "content"=>$output)); 
    
    }
  }

?>