<?php

  class atkSelectHandler extends atkActionHandler
  {
    function action_select()
    {      
      $output = $this->invoke("selectPage");
      if ($output!="")
      {
        $page =  &$this->getPage();
        $page->addContent($this->m_node->renderActionPage("select", $output));
      }
    }
    
    /**
     * Select page displays records and gives the user the ability to select a record
     */
    function selectPage()
    {
      $node = &$this->m_node;
      // When there's a lot of data, records will be spread across multiple pages.
      if ($node->m_postvars['atklimit']=="") $node->m_postvars['atklimit']=atkconfig("recordsperpage");
      if ($node->m_postvars['atkstartat']=="") $node->m_postvars['atkstartat']=0;

      $filter = $node->validateFilter($node->m_postvars["atkfilter"]);          

      $recordset = $node->selectDb($filter,
                                   $node->m_postvars['atkorderby'],
                                   array("offset" => $node->m_postvars['atkstartat'], "limit" => $node->m_postvars['atklimit']),
                                   $node->m_listExcludes,
                                   "",
                                   "select");

      if (count($recordset)==1 && $node->hasFlag(NF_AUTOSELECT))
      {
        // There's only one record and the autoselect flag is set, so we
        // automatically go to the target.
        $target = stringparse(rawurldecode(atkurldecode($node->m_postvars['atktarget'])),$recordset[0], true);
        $node->redirect(session_url($target, SESSION_NESTED));
        return "";
      }
      else
      {        
        $ui = &$this->getUi();
        
        $params["header"] = text("title_select", $node->m_type);

        if ($node->m_index != "") 
        {
          $params["index"] = $node->buildIndex($recordset[0][$node->m_index]);
        }

        // create navigation bar
        $params["navbar"] = $node->buildNavigation("select");        

        $actions["select"]=atkurldecode($node->m_postvars['atktarget']);

        $params["list"] = $node->normalRecordList($recordset, $actions);
        
        $output = $ui->renderList("select", $params);
        
        return $ui->renderBox(array("title"=>$node->actionTitle('select'),
                                    "content"=>$output));         
      }

    }

  }

?>