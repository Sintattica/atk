<?php

  class atkAddHandler extends atkActionHandler 
  {
    function action_add()
    {
      atkdebug("Action add");
      $page = &$this->getPage();
      $page->addContent($this->m_node->renderActionPage("add", $this->invoke("addPage")));
    }    
    
    /**
     * Creates an add page
     */
    function addPage($record=NULL)
    {            
      $node = &$this->m_node;
      $page = &$this->getPage();
            
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/formnavigation.js");
      $page->register_loadscript("init_nav();");
      $node->addStyle("style.css");
      
      $params = $node->getDefaultActionParams();

      $formstart ='<form name="entryform" enctype="multipart/form-data" action="'.$_SERVER["PHP_SELF"].'?'.SID.'"'.
                     ' method="post" onsubmit="return globalSubmit(this)">';

      /* if we are on an admin page we would like to stay on the admin page after adding a new record */
      if ($node->m_action == "admin") $formstart.=session_form(SESSION_NESTED);
      else $formstart.=session_form();
      
      $params["formstart"] = $formstart;
      
      $ui = &$node->getUi();
      if (is_object($ui))
      {        
        $forceList = decodeKeyValueSet($node->m_postvars['atkfilter']);
        $form = $node->editForm("add",$record,$forceList);                      
        
        $params["content"] = $node->tabulate("add", $form);                
                    
        $params["buttons"][] = '<input type="submit" class="btn_save" value="'.text('save').'&nbsp;">';
        if ($node->m_action=="add")
        {
          // if action is admin, we don't show the cancelbutton..
          $params["buttons"][] ='<input type="submit" class="btn_cancel" name="atkcancel" value="'.text('cancel').'">';
        }
        $params["formend"] = '</form><SCRIPT LANGUAGE="JavaScript">placeFocus()</SCRIPT>';
        
        $output = $ui->renderAction("add", $params);
              
        $total = $ui->renderBox(array("title"=>$node->actionTitle('add'),
                                      "content"=>$output));
        
        return $total;               
      }      
      else
      {
        atkerror("ui object failure");
      }
    }
  }

?>