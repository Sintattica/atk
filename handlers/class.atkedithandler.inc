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
      $node->addStyle("style.css");
      
      $params = $node->getDefaultActionParams($locked);

      $params["formstart"] = '<form name="entryform" enctype="multipart/form-data" action="'.$_SERVER["PHP_SELF"].'?'.SID.'"'.
                                   ' method="post" onsubmit="return globalSubmit(this)">'.
                                   session_form();
      
      $ui = &$node->getUi();
      
      if (is_object($ui))
      {
      
        $forceList = decodeKeyValueSet($node->m_postvars['atkfilter']);
        $form =$this->editForm("edit",$record,$forceList,$node->m_postvars['atksuppress']);

        $params["content"] = $node->tabulate("edit", $form);
        
        $params["buttons"] = $node->getFormButtons("edit", $record);
        
        $output = $ui->renderAction("edit", $params);
              
        return $ui->renderBox(array("title"=>$node->actionTitle('edit', $record),
                                      "content"=>$output));        
      }
      else
      {
        atkerror("Failed to create ui object");
      }      
    }   
    
    /**
     * Function outputs a form in which a record can be edited.
     *
     * @param $mode         the edit mode ("add" or "edit")
     * @param $forcelist    a key-value array used to preset certain fields to a certain value
     * @param $suppresslist an array of fields that you want to hide
     * @param $fieldprefix  if set, each form element is prefixed with the specified
     *                      prefix (used in embedded form fields)
     *
     * @return edit form HTML string
     */
    function editForm($mode="add", $record = NULL, $forceList="", $suppressList="", $fieldprefix="")
    {      
      $node = &$this->m_node;      
      
      /* get data, transform into form, return */            
      $data = $node->editArray($mode, $record, $forceList, $suppressList, $fieldprefix);                 
      // Format some things for use in tpl.
      /* check for errors and display them */
      $tab = $node->getActiveTab();
      $error_title = "";
      
      // Handle errors
      $errors = array();
      if (count($data['error']) > 0)
      {      
        $error_title = '<b>'.text('error_formdataerror').'</b>';        

        foreach ($data["error"] as $error)
        {
          if ($error['err'] == "error_primarykey_exists")
          {
            $pk_err_attrib[] = $error['attrib_name'];
          }
          else
          {
            $type = (empty($error["node"]) ? $node->m_type : $error["node"]);
            
            if(is_array($error['attrib_name']))
              $attrib_name = $error['attrib_name'][0];
            else
              $attrib_name = $error['attrib_name'];
              
            $p_attrib = &$node->m_attribList[$attrib_name];

            $index = $p_attrib->m_index;
            $tabs  = $node->m_attribIndexList[$index]["tabs"];
            
            if (count($node->getTabs($node->m_action)) > 1
                 && $p_attrib->showOnTab($tab))
              $error_tab = ' ('.text("error_tab").' '.href($_SERVER["PHP_SELF"]."?atktab=".$tabs[0],text("tab_".$tabs[0], $node->m_type),SESSION_DEFAULT,true).')';
            else $error_tab = "";
            
            if(!is_array($error['attrib_name']))
            {	
              $label = text($error['attrib_name'], $type);
            }
            else
            {
              $label = array();
              foreach($error['attrib_name'] as $attrib)
                $label[] = text($attrib, $type);

                $label= implode(", ", $label);
            }
                
            $errors[] = array("msg"=>$error['msg'].$error_tab, "label"=>$label);
          }
        }
        if (count($pk_err_attrib)>0) // Make primary key error message
        {                  
          for($i=0;$i<count($pk_err_attrib); $i++)
          {
            $pk_err_msg .= text($pk_err_attrib[$i]);
            if (($i+1) < count($pk_err_attrib)) $pk_err_msg .= ", ";
          }
          $errors[] = array("label"=>text("error_primarykey_exists"),
                            "msg"=>$pk_err_msg);
        }        
        
      }
      
      // Handle fields
      // load images
      atkimport("atk.ui.atktheme");
      $theme = &atkTheme::getInstance();
      $tipimg = $theme->imgPath("help.gif");
      $reqimg = '<img align="top" onMouseOver="javascript:window.status=\''.
                   addslashes(text("field_obligatory")).'\';" src="'.$theme->imgPath("required_field.gif").'" border="0" alt="'.
                   text("field_obligatory").'">';            

      /* display the edit fields */
      $fields = array();
      for ($i = 0, $_i= count($data["fields"]); $i<$_i; $i++)
      {
        $field = &$data["fields"][$i];
				
				$field_name = $field['name'];
				
				// elementen die bij een selectie van tabs horen
				$tabnumber = count($field["attribute"]->m_ownerInstance->m_attribList[$field_name]->m_tabs);
				if($tabnumber > 1)
				{
					$tab_name = "tab_";
					for($j = 0; $j < $tabnumber; $j++)
					{
						$tab_name .= $field["attribute"]->m_ownerInstance->m_attribList[$field_name]->m_tabs[$j];
						$tab_name .= ".";
					}
					// staat het element op de initieel geselecteerde tab?
					if (strstr($tab_name,$tab)){ $initial_on_tab = "yes"; } else { $initial_on_tab = "no"; }
				}
				
				// elementen die bij 1 of alle tabs horen
				else
				{
					$tab_name = $field["attribute"]->m_ownerInstance->m_attribList[$field_name]->m_tabs[0];
					
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
				
        $tplfield = array();
				
				$tplfield["tab"] = $tab_name;
				$tplfield["initial_on_tab"] = $initial_on_tab;

				/* check for seperator */
        if (!is_array($field) && $field == "-" && $i > 0 && $data["fields"][$i-1] != "-")
        {
          $tplfield["line"] = "<hr>"; 
        }
        /* double seperator, ignore */
        elseif (!is_array($field) && $field == "-")
        {
        }        
        /* only HTML */
        elseif (!is_array($field))
        {
          $tplfield["line"] = $field;
        }
        /* edit field */
        else
        {                  
          /* does the field have a label? */
          if ($field["label"] !== NULL)
          {
            if ($field["label"] == "") 
            {
              $tplfield["label"] = "";
            }
            else
            {
              $tplfield["label"] =  $field["label"];
              if ($field["error"]) // TODO KEES
              {
                $tplfield["error"] = $field["error"];
              }
            }            
          }
          
          /* html source */
          $tplfield["widget"] = $field["html"];
          $editsrc = $field["html"];

          /* obligatory indicator */
          if ($field["obligatory"]) 
          {
            $editsrc.= $reqimg."&nbsp;";
            $tplfield["obligatory"] = $reqimg;
          }

          /* tooltip */
          $ttip = "txt_".$node->m_type."_".$field["name"]."_tooltip";
          if ($GLOBALS[$ttip] != "")
          {
            $tooltip = '<img align="top" src="'.$tipimg.'" border="0" alt="'.$GLOBALS[$ttip].'" onClick="javascript:alert(\''.addslashes($GLOBALS[$ttip]).'\')" onMouseOver="javascript:window.status=\''.addslashes($GLOBALS[$ttip]).'\';">';
            $tplfield["tooltip"] = $tooltip;
            $editsrc.=$tooltip."&nbsp;";
          }

          $tplfield["full"] = $editsrc;
        }
        $fields[] = $tplfield; // make field available in numeric array
        $params[$field["name"]] = $tplfield; // make field available in associative array
      }
      
      $ui = &$this->getUi();
      $page = &$this->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/formsubmit.js");
      foreach ($data["hide"] as $hidden)
      {
        $result.= $hidden;
      }
			
      $params["activeTab"] = $tab;
      $params["fields"] = $fields; // add all fields as an numeric array.
      $params["errortitle"] = $error_title;
      $params["errors"] = $errors; // Add the list of errors.
      
      $result.= $ui->render($node->getTemplate($mode, $record, $tab), $params);
			return $result;
            
    }
  }

?>