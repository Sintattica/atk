<?php

  class atkAdminHandler extends atkActionHandler
  {
    function action_admin()
    {
      $page = &$this->getPage();
      
      $res = array();
      if ($this->m_node->hasFlag(NF_NO_ADD)==false&&$this->m_node->allowed("add"))
      {
        if (!$this->m_node->hasFlag(NF_ADD_LINK)) // otherwise, in adminPage, an add link will be added.
        {
          $res[]=$this->invoke("addPage");
        }
      }
      $res[]= $this->invoke("adminPage");
                        
      $page->addContent($this->m_node->renderActionPage("admin", $res));
            
    }
    
    function addPage()
    {
      // Reuse the atkAddHandler for the addPage.
      $handler = atknew("atk.handlers.atkaddhandler");
      $handler->setNode($this->m_node);
      return $handler->invoke("addPage");
    }
    
    /**
     * Admin page displays records and the actions that can be performed on
     * them (edit, delete)
     */
    function adminPage($actions="")
    {     
      $node = &$this->m_node;
      $node->addStyle("style.css");
      
      $ui = &$this->getUi();
      if (is_object($ui))
      {        
        $params = array();

        // When there's a lot of data, records will be spread across multiple
        // pages.
        if ($node->m_postvars['atklimit']=="") $node->m_postvars['atklimit']=atkconfig("recordsperpage");
        if ($node->m_postvars['atkstartat']=="") $node->m_postvars['atkstartat']=0;

        $recordset = $node->selectDb($node->m_postvars['atkfilter'],
                                     $node->m_postvars['atkorderby'],
                                     array("offset" => $node->m_postvars['atkstartat'], "limit" => $node->m_postvars['atklimit']),
                                     $node->m_listExcludes,
                                     "",
                                     "admin");

        // check for search-action (if search returns only one record!)
        if (is_array($node->m_postvars['atksearch']) && count($node->m_postvars['atksearch']) > 0 &&
            is_array($node->m_search_action) && count($recordset) == 1)
        {
          for ($i = 0, $_i = count($node->m_search_action); $i < $_i; $i++)
            if ($node->allowed($node->m_search_action[$i], $recordset[$i]))
            {
              $node->redirect(session_url($_SERVER["PHP_SELF"].'?atknodetype='.$node->atkNodeType().
                              '&atkaction='.$node->m_search_action[$i].
                              '&atkselector='.rawurlencode($node->primaryKey($recordset[0])),
                               SESSION_REPLACE));
              break;
            }
        }                

        $params["header"] = $this->invoke("adminHeader");      

        if ($node->hasFlag(NF_ADD_LINK) && !$node->hasFlag(NF_NO_ADD) && $node->allowed("add"))
        {
          $addurl = $_SERVER["PHP_SELF"].'?atknodetype='.$node->atknodetype().'&atkaction=add';
          if ($GLOBALS["txt_link_".getNodeType($node->m_type)."_add"]!="")
          {
            // specific text
            $label = text("link_".getNodeType($node->m_type)."_add");
          }
          else
          {
            // generic text
            $label = text(getNodeType($node->m_type))." ".text("add");
          }
          $params["header"].="<br><br>".href($addurl,$label,SESSION_NESTED);
        }

        // create index and navigation bar
        $navigator = &atknew("atk.utils.atknavigator");
        if ($node->m_index != "") 
        {
          $params["index"] = $navigator->buildIndex($node, $recordset[0][$node->m_index]);
        }        
        $params["navbar"] = $navigator->buildNavigation($node, "admin");
        
        if(!is_array($actions))
        {
          $actions = $node->defaultActions("admin");
        }

        $flags = ($node->hasFlag(NF_MRA) ? RL_MRA : 0) | ($node->hasFlag(NF_MRPA) ? RL_MRPA : 0) | ($node->hasFlag(NF_LOCK) ? RL_LOCK : 0);
        $recordlist = &atknew("atk.recordlist.atkrecordlist");
        $params["list"] = $recordlist->render($node, $recordset, $actions, $flags);
 
        $params["footer"] = $this->invoke("adminFooter");
        
        $output = $ui->renderList("admin", $params);   
      
        return $ui->renderBox(array("title"=>$node->actionTitle('admin'),
                                    "content"=>$output));
      }                 
      else
      {
        atkerror("Failed to create ui object");
      }      
    }
    
    /**
     * Function that is called when creating an adminPage. Developers can override
     * this function in their classes and return a string.
     */
    function adminHeader()
    {
      return "";
    }

   /**
     * Function that is called when creating an adminPage. Developers can override
     * this function in their classes and return a string.
     */
    function adminFooter()
    {
      return "";
    }
  }

?>