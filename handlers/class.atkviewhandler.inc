<?php

  class atkViewHandler extends atkActionHandler
  {
    function action_view()
    {
      atkdebug("Action view");
      $recordset = $this->m_node->selectDb($this->m_postvars['atkselector'],"","",$this->m_viewExcludes,"","view");
      
      $page = &$this->getPage();
            
      $page->addContent($this->m_node->renderActionPage("admin", $this->invoke("viewPage", $recordset[0], $this->m_node)));
    }
    
    /**
     * Creates an view (=readonly) page
     */
    function viewPage($record, $node)
    {            
      $ui = &$this->getUi();
      $node->addStyle("style.css");
      
      if (is_object($ui))
      {
        $params = $node->getDefaultActionParams();        
        
        $fields = array();          
  
        // get active tab
        $tab = $node->getActiveTab();
				
        // For all attributes we use the display() function to display the
        // attributes current value. This may be overridden by supplying
        // an <attributename>_display function in the derived classes.
        foreach (array_keys($node->m_attribIndexList) as $i)
        {				
          $tplfield = array();
          $p_attrib = &$node->m_attribList[$node->m_attribIndexList[$i]["name"]];
					
					$field_name = $node->m_attribIndexList[$i]["name"];
					
					$tabnumber = count($p_attrib->m_ownerInstance->m_attribList[$field_name]->m_tabs);
					
					if($tabnumber > 1)
					{
						$tab_name = "tab_";
						for($j = 0; $j < $tabnumber; $j++)
						{
							$tab_name .= $p_attrib->m_ownerInstance->m_attribList[$field_name]->m_tabs[$j];
							$tab_name .= ".";
						}
						// staat het element op de initieel geselecteerde tab?
						if (strstr($tab_name,$tab)){ $initial_on_tab = "yes"; } else { $initial_on_tab = "no"; }
					}
					
					// elementen die bij 1 of alle tabs horen
					else
					{
						$tab_name = $p_attrib->m_ownerInstance->m_attribList[$field_name]->m_tabs[0];

						// Van deze elementen kijken welke bij de tab horen die moet worden weergegeven
						if (strstr($tab_name,$tab)){ $initial_on_tab = "yes"; } else { $initial_on_tab = "no"; }	
						
						// elementen die bij alle tabs horen
						if ($tab_name == "*")
						{
							$tab_name = "alltabs";
							$initial_on_tab = "yes";
						}
						elseif (empty($tab_name))
						{
							$tab_name = "tab_default";
						}
						else
						{				
							$tab_name = "tab_" . $tab_name;
						}
					}
					
					$tplfield["tab"] = $tab_name;
					$tplfield["initial_on_tab"] = $initial_on_tab;
				 
          // An <attributename>_display function may be provided in a derived
          // class to display an attribute. If it exists we will use that method
          // else we will just use the attribute's display method.
          $funcname = $p_attrib->m_name."_display";
          if (method_exists($node, $funcname)) $editsrc = $node->$funcname($record, "view");
          else $editsrc=$p_attrib->display($record, "view");
					
          $tplfield["full"] = $editsrc;
          $tplfield["widget"] = $editsrc; // in view mode, widget and full are equal
             
          // The Label of the attribute (can be suppressed with AF_NOLABEL or AF_BLANKLABEL)
          // For each attribute, a txt_<attributename> must be provided in the language files.
          if ($p_attrib->hasFlag(AF_NOLABEL)==false)
          {
            if ($p_attrib->hasFlag(AF_BLANKLABEL))
            {
              $tplfield["label"] = "";
            }
            else
            {
              $tplfield["label"] = $p_attrib->label($record);
            }
          }
					else
          {
            // Make the rest fill up the entire line
            $tplfield["label"] = "";
            $tplfield["line"] = $tplfield["full"];
          }
          
          $fields[] = $tplfield;
            
        }  
        
        $innerform = $ui->render($node->getTemplate("view", $record, $tab), array("fields"=>$fields));
				
        $params["activeTab"] = $tab;
				
        $params["content"] = $node->tabulate("view", $innerform);        
  
        $params["formstart"] = '<form method="get">'.session_form(SESSION_NESTED);
        $params["buttons"][] = '<input type="submit" class="btn_cancel" name="atkback" value="&lt;&lt; '.text('back').'">';
                          
        // if appropriate, display an edit button.
        if (!$node->hasFlag(NF_NO_EDIT)&&$node->allowed("edit", $record))
        {        
          $params["buttons"][] = '<input type="hidden" name="atkaction" value="edit">'.
                     '<input type="hidden" name="atknodetype" value="'.$node->atkNodeType().'">'.
                     '<input type="submit" class="btn" value="'.text('edit').'">';
        }      
      
        $params["formend"] = '</form>';                
        
        $output = $ui->renderAction("view", $params);
        
        $total = $ui->renderBox(array("title"=>$ui->title($node->m_module, $node->m_type,'view'),
                                      "content"=>$output));
        return $total;
      }
      else
      {
        atkerror("ui object error");
      }
    }
  }

?>