<?php

  class atkEditHandler extends atkActionHandler
  {
    function action_edit()
    {
      $page = &$this->getPage();    
      
      atkdebug("actionedit calling selectDb()");
      $recordset = $this->m_node->selectDb($this->m_postvars['atkselector'],"","","","","edit");
      if ($this->m_node->hasFlag(NF_LOCK))
      {
        if ($this->m_node->m_lock->lock($this->m_node->primaryKey($recordset[0]), $this->m_node->m_table)) 
        {
          $res = $this->invoke("editPage", $recordset[0], true);
        }
        else
        {
          $res = $this->m_node->lockPage();
        }
      }  
      else
      {                
        $res = $this->invoke("editPage", $recordset[0], false);       
        echo $this->m_node->test;
      }
            
      $page->addContent($this->m_node->renderActionPage("edit", $res));
    }
    
    /**
     * Creates an edit page
     */
    function editPage($record, $locked=FALSE)
    {        
      $node = &$this->m_node;      
      $page = &$this->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");      
      $page->register_script(atkconfig("atkroot")."atk/javascript/formnavigation.js");
      $page->register_loadscript("init_nav();");
      $node->addStyle("style.css");
      
      $params = $node->getDefaultActionParams($locked);

      $params["formstart"] = '<form name="entryform" enctype="multipart/form-data" action="'.$_SERVER["PHP_SELF"].'?'.SID.'"'.
                                   ' method="post" onsubmit="return globalSubmit(this)">'.
                                   session_form();
      
      $ui = &$node->getUi();
      
      if (is_object($ui))
      {
      
        $forceList = decodeKeyValueSet($node->m_postvars['atkfilter']);
        $form =$node->editForm("edit",$record,$forceList,$node->m_postvars['atksuppress']);

        $params["content"] = $node->tabulate("edit", $form);
        
        $params["buttons"] = $node->getFormButtons("edit", $record);
        
        $output = $ui->renderAction("edit", $params);
              
        return $ui->renderBox(array("title"=>$ui->title($node->m_module, $node->m_type,'edit'),
                                      "content"=>$output));        
      }
      else
      {
        atkerror("Failed to create ui object");
      }      
    }   
  }

?>