<?php

  class atkFeedbackHandler extends atkActionHandler
  {
    function action_feedback()
    {
      $page = &$this->getPage();
      $output = $this->invoke("feedbackPage", $this->m_postvars["atkfbaction"], $this->m_postvars["atkactionstatus"], $this->m_postvars["atkfbmessage"]);
      $page->addContent($this->m_node->renderActionPage("feedback", $output));
    }
    
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