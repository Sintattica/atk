<?php

  class atkSearchHandler extends atkActionHandler
  {
    function action_search()
    {
      $page = &$this->getPage();
      $page->addcontent($this->m_node->renderActionPage("search", $this->invoke("searchPage", $this->m_postvars["atksearch"])));
    }
    
    function searchPage($record=NULL)
    {         
      $node = &$this->m_node;
      
      $node->addStyle("style.css");      
      
      $page = &$this->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");      
      $ui = &$this->getUi();
      if (is_object($ui))
      {        
        $params = array();
        $params["formstart"] = '<form name="entryform" action="'.$_SERVER["PHP_SELF"].'?'.SID.'" method="post">';

        // usually, extended search is called from a page, and we go back to that page
        // when the user presses search.
        if (atkLevel()>0)
        {
          $params["formstart"].=session_form(SESSION_BACK);
        }
        // but sometimes we directly call the searchpage. if that happens,
        // we reload the adminpage with the searched items.
        else
        {
          $params["formstart"].=session_form(SESSION_DEFAULT);
          $params["formstart"].='<input type="hidden" name="atkaction" value="admin">';
        }
        $params["formstart"].='<input type="hidden" name="atknodetype" value="'.$node->atknodetype().'">';        
        $params["formstart"].='<input type="hidden" name="atkstartat" value="0">'; // start at first page after new search

        $params["content"] = $this->invoke("searchForm", $record);
        $params["buttons"][] = '<input type="submit" class="btn_search" value="'.text('search').'">';
        $params["formend"] = '</form><SCRIPT LANGUAGE="JavaScript">placeFocus()</SCRIPT>';      
      
        $output = $ui->renderAction("search", $params);
              
        $total = $ui->renderBox(array("title"=>$node->actionTitle('search'),
                                      "content"=>$output));
        
        return $total;               
      }      
      else
      {
        atkerror("ui object failure");
      }
    }
    
    /**
     * Function outputs a form that the user can use to search records.
     */
    function searchForm($record = NULL)
    {
      $node = &$this->m_node;
      
      $ui = &$this->getUi();
      
      if (is_object($ui))
      {
        $node->setAttribSizes();
  
        $defaults = $record;
  
        $dbSearchModes = $node->m_db->getSearchModes();
        $params = array();
        $params['searchmode_title'] = text("search_mode");
        $params['searchmode_and'] = '<input type="radio" name="atksearchmethod" value="AND" checked>'.text("search_and");
        $params['searchmode_or'] = '<input type="radio" name="atksearchmethod" value="OR">'.text("search_or");      
        $params["fields"] = array();                
  
        foreach (array_keys($node->m_attribList) as $attribname)
        {
          $p_attrib = &$node->m_attribList[$attribname];
          $attribSearchModes = $p_attrib->getSearchModes();        
  
          if (!$p_attrib->hasFlag(AF_HIDE_SEARCH))
          {
            $field = array();
            // We must perform an array_values over array_intersect, to maintain a
            // correct numeric array index (array_intersect mangles those)            
            $searchModes = array_values(array_intersect($attribSearchModes, $dbSearchModes));
  
            if (count($searchModes)>0) // Don't display this attribute if it doesn't support searching.
            {
            
              $funcname = $p_attrib->m_name."_search";
  
              if (method_exists($node,$funcname))
              {
                $field["widget"] = $node->$funcname($defaults, $fieldprefix);
              }
              else
              {
                $field["widget"] = $p_attrib->search($defaults, true); // second param indicates extended search.
              }
              
              $field["full"] = $field["widget"]; // lateron, we might add more to full
              
              if(count($searchModes)==1)
              {
                $field["searchmode"] = '<input type="hidden" name="atksearchmode['.$p_attrib->m_name.']" value="'.$searchModes[0].'">'.text("search_".$searchModes[0]);
              }
              else
              {
                $sel = '<select name="atksearchmode['.$p_attrib->m_name.']">';
                foreach (array_values($searchModes) as $searchMode)
                {
                  // if supported, we select the 'like' searchmode by default
                  $selected="";
                  if ($node->m_postvars["atksearchmode"][$p_attrib->m_name]==$searchMode||
                      ((!is_array($node->m_postvars["atksearchmode"]) || $node->m_postvars["atksearchmode"][$p_attrib->m_name]=="")&&
                       $searchMode=="substring")) $selected="selected";

                  $sel.= '<option value="'.$searchMode.'" '.$selected.'>'.text("search_".$searchMode);
                }
                $sel.= '</select>';
                $field["searchmode"] = $sel;
              }                        
            
              // The Label of the attribute (can be suppressed with AF_NOLABEL or AF_BLANKLABEL)
              // For each attribute, a txt_<attributename> must be provided in the language files.
              if ($p_attrib->hasFlag(AF_NOLABEL)==false)
              {
                if ($p_attrib->hasFlag(AF_BLANKLABEL))
                {
                  $field['label'] = "";
                }
                else
                {
                  $field['label'] = $p_attrib->label($record);
                }
              }
              else
              {
                $field['line'] = $field['widget'];
              }
              $params["fields"][] = $field;
              
            }            
          }                
        }
                
        return $ui->render($node->getTemplate("search", $record), $params);

      }    
      else
      {
        atkerror("ui object error");
      }
      
    }

  }

?>