<?php

  class atkUI
  {
    var $m_smarty = NULL;  
    var $m_theme = NULL;
    var $m_node = NULL;
    
    function atkUI()
    {
      atkimport("atk.ui.atktheme");      
      $this->m_theme = &atkTheme::getInstance();
      
      atkimport("atk.ui.atksmarty");
      $this->m_smarty = &atkSmarty::getInstance();          
            
      atkdebug("Created a new atkUI");            
    }        
    
    function setNode(&$node)
    {
      $this->m_node = &$node;
    }
    
    function renderAction($action, $vars)
    {
      // todo.. action specific templates
      switch($action)
      {
        case "view":
          $tpl = "action_view.tpl";
          break;
        default:
          $tpl = "action.tpl";
      }
      return $this->render($tpl, $vars);
    }      
    
    function renderList($action, $vars)
    {
      return $this->render("list.tpl", $vars);
    }
    
    function renderBox($vars)
    {
      return $this->render("box.tpl", $vars);
    }
    
    function renderTabs($vars)
    {      
      if ($this->m_theme->getAttribute("tabtype")=="dhtml")
      {
        $page = &atkPage::getInstance();
        $page->register_script(atkconfig("atkroot")."atk/javascript/dhtml_tabs.js.php");
        
        $tabscript = "var tabs = new Array();\n";
        foreach($vars["tabs"] as $tab)
        {
          $tabscript.="tabs[tabs.length] = '".$tab['tab']."';\n";
        
        }
        $page->register_scriptcode($tabscript);        
                
      }
			
      return $this->render("tabs.tpl", $vars);
    }
    
    function render($tplname, $vars=array())
    { 
      // Backwardscompatibility hack.. g_layout must be initGui'd
      global $g_layout;
      $g_layout->initGui();
      
      // First clear any existing smarty var.
      $this->m_smarty->clear_all_assign();
      
      // Then set some defaults that we need in all templates.
      $this->m_smarty->assign("themedir", $this->m_theme->themeDir());

      $this->m_smarty->assign("atkroot", atkconfig("atkroot"));

      $this->m_smarty->assign($vars);              
      
      // if $tplname contains a full pathname, the template
      // is rendered as is. 
      // If it's just a filename, we lookup the template in
      // the theme directory.
      if (strpos($tplname, "/")===false)
      {
        // lookup template in theme.
        $tplname = $this->m_theme->tplPath($tplname);    
      }
      
      // Smarty fetches templates relative from the template_dir setting.
      // Since that is an application directory, and themes reside in
      // a different directory, we have to hack the template_dir 
      // setting.
      $old = $this->m_smarty->template_dir;
      $this->m_smarty->template_dir = "./"; // current dir, becaue tplname already contains full relative path.
      $res = &$this->m_smarty->fetch($tplname);
      $this->m_smarty->template_dir = $old;
                       
      return $res;
      
    }        
    
    /**
      * This function returns a complete themed path for a given stylesheet.
      * This is a convenience method, which calls the stylePath method on
      * the theme instance. 
      *
      * @param String $style The filename (without path) of the stylesheet for
      *                      which you want to complete the path.
      */
    function stylePath($style)
    {
      return $this->m_theme->stylePath($style);
    }
    
    /**
      * This function returns a suitable title text for an action.
      * Example: echo $g_layout->title("employee", "edit"); might return:
      *          'Edit an existing employee'
      */
    function title($module, $nodetype, $action)
    {
      // first look for specific text.
      $key = 'title_'.$module.'_'.$nodetype.'_'.$action;
      if ($GLOBALS["txt_".$key]!="")
      {
        return text($key);
      }
      else
      {
        $key = $module."_".$nodetype;
        if ($GLOBALS["txt_".$key]!="")
        {
          return text($key)." - ".text($action, $nodetype);
        }
        else
        {
          $key = 'title_'.$nodetype.'_'.$action;
          if ($GLOBALS["txt_".$key]!="")
          {
            return text($key);
          }
          else
          {
            // if no specific text available for this node,
            // generate a generic title
            return text($nodetype, $module)." - ".text($action, $nodetype);
          }
        }
      }
    }
      
  }

?>
