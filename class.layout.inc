<?php

  /**
  * This class generates all output from atk.
  *
  * @author Ivo Jansch  (ivo@achievo.org)
  *
  * @version $Revision$
  * $Id$
  */

  // Global debug and error msg
  $g_debug_msg = array();
  $g_error_msg = array();

  class layout
  {    
    
    var $m_saved = "";

    /**
     * Raw
     */    

    var $m_submitscripts = array();        
    
    var $m_atkui = NULL;
    var $m_atkoutput = NULL;
    var $m_atkpage = NULL;

    /**
     * The directory where the current template files are located..
     */
    var $m_tpldir = "";

    var $m_wrotehead = false;
    var $m_wrotebody = false;

    /**
     * Constructor
     */
    function layout()
    {          
      $this->m_atkoutput = &atkOutput::getInstance();      
      $this->m_atkui = &atknew("atk.ui.atkui");
    }


    /**
          * Clear content
          */
    function clear()
    {
      $this->m_content='';
    }

    /**
      * Gui initialisation.
      * Call this before using gui functions (themes)..
      */
    function initGui()
    {
      atkdebug("Obsolete use of \$g_layout->initGui()!");
      static $initialized = false;
      global $g_theme;

      if (!$initialized)
      {
        global $g_theme;
        if($g_theme["Name"]=="") $g_theme["Name"] = $GLOBALS["config_defaulttheme"];
        if($g_theme["Name"]=="") $g_theme["Name"]="default";
        $this->initTheme();
        $this->import_layout();             
        
        // Also load stylesheet, for people still using initGui will not have an addStyle
        // call in their code.
        $page = &$this->getPage();
        $page->register_style($this->m_atkui->stylePath("style.css"));

        atkdebug("Initialized GUI");
        $initialized = true;
      }
    }

    /*
     * Import_layout
     * Imports the layout file with all the items.
     */
    function import_layout()
    {
      atkdebug("Obsolete use of \$g_layout->import_layout()!");
      
      global $g_theme;      
      
      // stylesheets
      $page = &$this->getPage();
      if (is_object($page))
      {
        if ($g_theme["css_default"]!="")
        {
          $page->register_style($this->m_tpldir.$g_theme["css_default"]);
        }
        else
        {
          $page->register_style((file_exists(atkconfig("atkroot")."./style.css") ? "." : atkconfig("atkroot")."atk").'/style.css');
        }
      }
    }

    /**
     * Output text to page
     * @param $txt Text
     */
    function output($txt)
    {
      atkdebug("Obsolete use of \$g_layout->output()!");
      $this->m_atkoutput->output($txt);
    }

    /**
     * Output rawtext to page
     * @param $txt Text
     */
    function rawoutput($txt)
    {
      atkdebug("Obsolete use of \$g_layout->rawoutput()!");
      $this->m_atkoutput->rawoutput($txt);
    }

    /**
     * Output all data to the browser.
     * @param $nocache. If set to true, the function sends no-cache headers
     *                                (the default, for backwardcompatibility)
     *                  If set to false, browsers can cache the result.
     * @param $lastmodificationstamp. If set, this timestamp is used to send
     *                                the http Last-Modified header.
     */
    function outputFlush($nocache=true, $lastmodificationstamp="",$gzip=false)
    {
      atkdebug("Obsolete use of \$g_layout->outputFlush()!");
      $this->m_atkoutput->outputFlush($nocache, $lastmodificationstamp);
    }
  
    /**
     * Generate a complete page..
     */
    function page($title="")
    {
      atkdebug("Obsolete use of \$g_layout->page()!");
      $page = &$this->getPage();
      $this->output($page->render($title, $this->m_atkoutput->m_content));      
    }

    /**
     * Error report
     * @param $title Title of the window
     * @param $txt Error message
     */
    function error($title,$txt)
    {
      atkdebug("Obsolete use of \$g_layout->error()!");
      $this->ui_top($title);
      $this->output("<br><br><br>");
      $this->output($txt);
      $this->output("<br><br><br>");
      $this->ui_bottom();
    }


    /**
     * Returns the maxInputSize
     * @return 60
     */
    function maxInputSize()
    {
      atkdebug("Obsolete use of \$g_layout->maxInputSize()!");
      $size = atkconfig("max_input_size");
      if ($size!="") return $size;
      return 70;
    }

    /**
     * Returns the searchSize
     * @return 20
     */
    function searchSize()
    {
      atkdebug("Obsolete use of \$g_layout->searchSize()!");
      return 20;
    }

    /**
     * Register a javascript for use withing the page
     * @param $file the filename of the external javascript file
     */
    function register_script($file)
    {
      atkdebug("Obsolete use of \$g_layout->register_script()!");
      $page = &$this->getPage();
      $page->register_script($file);
    }

    function register_submitscript($code)
    {
      atkdebug("Obsolete use of \$g_layout->register_submitscript()!");
      if (!in_array($code, $this->m_submitscripts)) $this->m_submitscripts[] = $code;
    }

    /**
     * Register a Cascading StyleSheet for use withing the page
     * @param $file the filename of the external stylesheet
     */
    function register_style($file)
    {
      atkdebug("Obsolete use of \$g_layout->register_style()!");
      $page = &$this->getPage();
      $page->register_style($file);
    }

    /**
     * This function returns a suitable title text for an action.
     * Example: echo $g_layout->title("employee", "edit"); might return:
     *          'Edit an existing employee'
     */
    function title($module, $nodetype, $action)
    {
      atkdebug("Obsolete use of \$g_layout->title()!");
      return $this->m_atkui->title($module, $nodetype, $action);
    }

    /**
     * Output a HTML header
     * @param $title Titel van het window
     */
    function head($title)
    {
      atkdebug("Obsolete use of \$g_layout->head()!");
      if (!$this->m_wrotehead)
      {
        $page = &$this->getPage();
        $this->output($page->head($title));
        $this->m_wrotehead = true;
      }
    }
    
    // Dummy, creates an atkPage if one does not exist (for backward compatibility).
    function &getPage()
    {
      if (!is_object($this->m_atkpage))
      {
        $this->m_atkpage = &atknew("atk.ui.atkpage");
      }
      return $this->m_atkpage;
    }  

    /**
     * Return a HTML header
     * @param $title Titel van het window
     */
    function ret_head($title)
    {
      atkdebug("Obsolete use of \$g_layout->ret_head()!");
      $page = &$this->getPage();
      return $page->head($title);
    }

    /**
     * Output a HTML body
     * @param $extraops Not used
     */
    function body($extraprops="")
    {
      atkdebug("Obsolete use of \$g_layout->body()!");
      if (!$this->m_wrotebody)
      {
        $page = &$this->getPage();
        $this->output($page->body($extraprops));
        $this->m_wrotebody = true;
      }
    }

    /**
     * Return a HTML body
     * @param $extraops Not used
     */
    function ret_body($extraprops="")
    {
      atkdebug("Obsolete use of \$g_layout->ret_body()!");
      $page = &$this->getPage();
      return $page->body($extraprops);
    }


    /**
     * Create the UI top
     * @param $title Title of the UI top
     */
    function ui_top($title)
    {
      atkdebug("Obsolete use of \$g_layout->ui_top()!");
      $this->m_saved = $this->m_atkoutput->m_content;
      $this->m_atkoutput->m_content = "";
      //atkimport("atk.ui.atksmarty");
      //$smarty = &atkSmarty::getInstance();
      //$smarty->assign("title", $title);      
      $this->m_lasttitle = $title;
      
    }

    function ret_tpl($tplname, $vars="", $fallbacktpl="")
    {
      atkdebug("Obsolete use of \$g_layout->ret_tpl()!");
      $template = $this->getTemplate($tplname, $fallbacktpl);
      $template=str_replace('[IMAGEDIR]',$this->m_tpldir."images",$template);

      if (is_array($vars))
      {
        foreach($vars as $key=>$value)
        {
          $template=str_replace('['.strtoupper($key).']',$value,$template);
        }
      }
      return $template;
    }

    function tpl($tplname, $vars="", $fallbacktpl="")
    {
      atkdebug("Obsolete use of \$g_layout->tpl()!");
      $this->output($this->ret_tpl($tplname, $vars, $fallbacktpl));
    }

    /**
     * Create UI Bottom
     */
    function ui_bottom()
    {
      atkdebug("Obsolete use of \$g_layout->ui_bottom()!");
      $rendered = $this->m_atkui->render("box.tpl", array("content"=>$this->m_atkoutput->m_content,
                                                          "title"=>$this->m_lasttitle));
      $this->m_atkoutput->m_content = $this->m_saved;
      $this->m_saved = "";
      $this->output($rendered);
    }

    /**
     * Create the tab panel top
     * @param $tabs the tab panel tabs
     */
    function tabs_top($tabs)
    {
      atkdebug("Obsolete use of \$g_layout->tabs_top()!");
      if (file_exists($this->m_tpldir."tabs_top.tmpl"))
      {
        $this->tpl("tabs_top", array("tabs"=>$tabs));
      }
      else
      {
        $this->output('<table border="0" cellpadding="0" cellspacing="0" width="98%" align="center" valign="top">
                        <tr>
                          <td width="100%">
                            <table border="1" cellpadding="2" cellspacing="0">
                              <tr>');
        $this->output($tabs);
        $this->output('</tr></table><table border="1" cellspacing="0" cellpadding="5" width="100%"><tr><td>');
      }
    }

    /**
     * Create the tab panel bottom
     */
    function tabs_bottom()
    {
      atkdebug("Obsolete use of \$g_layout->tabs_bottom()!");
      if (file_exists($this->m_tpldir."tabs_bottom.tmpl"))
      {
        $this->tpl("tabs_bottom");
      }
      else
      {
        $this->output('</td></tr></table></td></tr></table>');
      }
    }

    /**
     * Creates a tab HTML string
     * @param $title the tab title
     * @param $link the tab link
     * @param $selected is this tab selected?
     */
    function ret_tab($title, $link, $selected=false)
    {
      atkdebug("Obsolete use of \$g_layout->ret_tab()!");
      if (file_exists($this->m_tpldir."tab_".($selected ? "selected" : "normal").".tmpl"))
      {
        return $this->ret_tpl("tab_".($selected ? "selected" : "normal"),
                            array("title"=>$title,
                                  "link"=>$link));
      }
      else
      {
        if ($selected)
        {
          return $this->ret_td('<b>'.$title.'&nbsp;</b>', 'valign="middle" align="left"');
        }
        else
        {
          return $this->ret_td('<a href="'.$link.'" style="color:black;text-decoration:none">'.$title.'</a>&nbsp;', 'valign="middle" align="left" nowrap');
        }
      }
    }

    function data_top()
    {
      atkdebug("Obsolete use of \$g_layout->data_top()!");
      global $g_theme;
      $template=$this->ret_tpl("data_top");

      if ($template=="")
      {
        if (!isset($g_theme["border"]))
        {
          $template='<table border="1" cellspacing="0" cellpadding="3">';
        }
        else
        {
          $template='<table border="0" cellspacing="0" cellpadding="0" class="backtable"><tr><td><table border="0" cellspacing="'.$g_theme["border"].'" cellpadding="3">';
        }
      }
      return $template;
    }

    function data_bottom()
    {
      atkdebug("Obsolete use of \$g_layout->data_bottom()!");
      global $g_theme;
      $template=$this->ret_tpl("data_bottom");

      if ($template=="")
      {
        if (!isset($g_theme["border"]))
        {
          $template='</table>';
        }
        else
        {
          $template='</table></td></tr></table>';
        }
      }

      return $template;
    }

    /**
     * Create simple table
     * @param $border border of the table
     * @param $fillout Fillout (Boolean)
     */
    function table_simple($border=0,$fillout=false)
    {
      atkdebug("Obsolete use of \$g_layout->tableSimple()!");
      $this->output('<table border="'.$border.'" cellspacing="0" cellpadding="2"');
      if ($fillout) $this->output(' width="100%"');
      $this->output(">\n");
    }

    /**
     * Create simple table whitch can be returned
     * @param $border border of the table
     * @param $fillout Fillout (Boolean)
     * @return HTML Code for a table
     */
    function ret_table_simple($border=0,$fillout=false)
    {
      atkdebug("Obsolete use of \$g_layout->ret_table_simple()!");
      $output = '<table border="'.$border.'" cellspacing="0" cellpadding="2"';
      if ($fillout) $output.=' width="100%"';
      $output.=">\n";
      return $output;
    }

    /**
     * Create TD
     * @param $contents contents for the TD
     * @param $extraops Extra options for the TD (Width, Height)
     */
    function td($contents="", $extraprops="")
    {
      atkdebug("Obsolete use of \$g_layout->td()!");
      $this->output($this->ret_td($contents, $extraprops));
    }

    /**
     * Create TD whitch can be returned
     * @param $contents contents for the TD
     * @param $extraops Extra options for the TD (Width, Height)
     * @return HTML Code with TD + Content
     */
    function ret_td($contents="", $extraprops="")
    {
      atkdebug("Obsolete use of \$g_layout->ret_td()!");
      $output='<td class="table" '.$extraprops.'>';
      if (strlen($contents) == 0)
      {
        $output.="&nbsp;";
      }
      else
      {
        $output.= $contents;
      }
      $output.="</td>\n";
      return $output;
    }

    /**
     * Create TR
     * @param $contents contents for the TD
     * @param $extraops Extra options for the TD (Width, Height)
     * @return HTML Code with TD + Content
     */
    function tr_top($alt=false, $extraprops="")
    {
      atkdebug("Obsolete use of \$g_layout->tr_top()!");
      return '<tr class="row'.($alt?"2":"1").'" '.$extraprops.'>';
    }

    function tr_bottom()
    {
      atkdebug("Obsolete use of \$g_layout->tr_bottom()!");
      return '</tr>';
    }

    /**
     * Create TD header
     * @param $contents contents for the TD header
     * @param $extraops Extra options for the TD header (Width, Height)
     */
    function td_datatitle($contents="", $extraprops="")
    {
      atkdebug("Obsolete use of \$g_layout->td_datatitle()!");  
      $this->output('<td class="tableheader" '.$extraprops.'>');
      if ($contents=="")
      {
        $this->output("&nbsp;");
      }
      else
      {
        $this->output($contents);
      }
      $this->output("</td>\n");
    }

    /**
     * Create TD header whitch can be returned
     * @param $contents contents for the TD header
     * @param $extraops Extra options for the TD header (Width, Height)
     * @return HTML Code with TD + Content
     */
    function ret_td_datatitle($contents="", $extraprops="")
    {
      atkdebug("Obsolete use of \$g_layout->ret_td_datatitle()!");
      global $g_colorset;
      $output='<td class="tableheader" '.$extraprops.'>';
      if ($contents=="")
      {
        $output.="&nbsp;";
      }
      else
      {
        $output.=$contents;
      }
      $output.="</td>\n";
      return $output;
    }

    /**
     * Returns the HTML code for an edit form based on the supplied data.
     * @param  $data pointer to the data array
     * @param  $node pointer to the originating node
     * @return HTML code (string) for edit form
     */
    function ret_editForm(&$data, &$node)
    {
      atkdebug("Obsolete use of \$g_layout->ret_editForm()!");
      global $config_atkroot;

      /* javascript */
      $this->register_script($config_atkroot."atk/javascript/formsubmit.js");

      $tab = $node->getActiveTab();

      $form .= $this->ret_table_simple();
      foreach ($data["hide"] as $hide) $form .= $hide;

      /* check for errors and display them */
      if (count($data['error']) > 0)
      {
        $form .= '<tr>';
        $error_title = '<b>'.text('error_formdataerror').'</b>';
        $errormsg = "";

        $pk_err = array();
        foreach ($data["error"] as $error)
        {
          if ($error['err'] == "error_primarykey_exists")
          {
            $pk_err_attrib[] = $error['attrib_name'];
          }
          else
          {
            $type = (empty($error["node"]) ? $node->m_type : $error["node"]);
            $index = $node->m_attribList[$error['attrib_name']]->m_index;
            $tabs  = $node->m_attribIndexList[$index]["tabs"];
            if (count($node->m_tabList[$node->m_action]) > 1 &&
                 $tabs != "*" && !atk_in_array($tab, $tabs))
            {
              $error_tab = ' ('.text("error_tab").' '.href($_SERVER["PHP_SELF"]."?atktab=".$tabs[0],text("tab_".$tabs[0], $node->m_type),SESSION_DEFAULT,true).')';
            }
            else 
            {
              $error_tab = "";
            }
            $errormsg.="<br>".text($error['attrib_name'], $type).': '.$error['msg'].$error_tab;
          }
        }
        if (count($pk_err_attrib)>0) // Make primary key error message
        {
          $pk_err_msg = "<br>".text(error_primarykey_exists).": ";
          for($i=0;$i<count($pk_err_attrib); $i++)
          {
            $pk_err_msg .= text($pk_err_attrib[$i]);
            if (($i+1) < count($pk_err_attrib)) $pk_err_msg .= ", ";
          }
          $form.=$this->ret_td($error_title.$pk_err_msg.$errormsg.'<br><br>','colspan="2"');
        }
        else
        {
          $form.=$this->ret_td($error_title.$errormsg.'<br><br>','colspan="2"');
        }

        $form.='</tr>';
      }

      // load images
      $reqimg = '<img align="top" onMouseOver="javascript:window.status=\''.addslashes(text("field_obligatory")).'\';" src="'.$this->getImgSrc("required_field.gif").'" border="0" alt="'.text("field_obligatory").'">&nbsp;';
      $tipimg = $this->getImgSrc("help.gif");

      /* display the edit fields */
      for ($i = 0; $i < count($data["fields"]); $i++)
      {
        $field = &$data["fields"][$i];

        /* check for seperator */
        if (!is_array($field) && $field == "-" && $i > 0 && $data["fields"][$i-1] != "-")
          $form .= '<tr>'.$this->ret_td('<hr>', 'colspan="2" valign="middle"').'</tr>';

        /* double seperator, ignore */
        elseif (!is_array($field) && $field == "-");

        /* only HTML */
        elseif (!is_array($field))
          $form .= '<tr>'.$this->ret_td($field, 'colspan="2" valign="middle"').'</tr>';

        /* edit field */
        else
        {
          $form.='<tr>';

          /* Keep track of the number of td's we have to fill with the edit thingee..
           * This depends on AF_NOLABEL for example.
           */
          $tdcount = 1;

          /* does the field have a label? */
          if ($field["label"] !== NULL)
          {
            if ($field["label"] == "") 
              $form .= $this->ret_td('&nbsp;');
            elseif ($field["error"]) 
              $form .= $this->ret_td('<span class="error">'.$field["label"].': </span>', 'valign="top"');
            else 
              $form .= $this->ret_td($field["label"].': ','valign="top"');
          }

          /* If there's no label, the other td's have to be filled up. */
          else $tdcount++;

          /* html source */
          $editsrc = $field["html"];

          /* obligatory indicator */
          if ($field["obligatory"]) $editsrc.= $reqimg;

          /* tooltip */
          $ttip = "txt_".$node->m_type."_".$field["name"]."_tooltip";
          if ($GLOBALS[$ttip] != "")
          {
            $editsrc.= '<img align="top" src="'.$tipimg.'" border="0" alt="'.$GLOBALS[$ttip].'" onClick="javascript:alert(\''.addslashes($GLOBALS[$ttip]).'\')" onMouseOver="javascript:window.status=\''.addslashes($GLOBALS[$ttip]).'\';">&nbsp;';
          }

          $form.=$this->ret_td($editsrc,'colspan="'.$tdcount.'" valign="top"');
          $form.='</tr>';
        }
      }

      $form.='</table>';
      return $form;
    }

    function getTemplate($templatefile, $fallback="")
    {
      atkdebug("Obsolete use of \$g_layout->getTemplate()!");
      if (file_exists($this->m_tpldir.$templatefile.".tmpl"))
      {
        return implode(file($this->m_tpldir.$templatefile.".tmpl"), "");
      }
      else
      {
        // Template not found, let's try fallback template
        if ($fallback!="") return $this->getTemplate($fallback);
      }
      return "";
    }

    function theme_style($stylesheetname)
    {        
       atkdebug("Obsolete use of \$g_layout->theme_style()!");
      if (file_exists($this->m_tpldir."styles/$stylesheetname"))
      {
        $this->register_style($this->m_tpldir."styles/$stylesheetname");
      }
      else
      {
        // Not found in current theme, fallback to default
        $this->register_style("atk/themes/default/styles/$stylesheetname");
      }
    }

    function initTheme()
    {      
      atkdebug("Obsolete use of \$g_layout->initTheme()!");
      atkimport("atk.ui.atktheme");
      $theme = &atkTheme::getInstance();
      $this->m_tpldir = $theme->themeDir();
    }

    function getImgSrc($img)
    {
      atkdebug("Obsolete use of \$g_layout->getImgSrc()!");
      global $config_atkroot, $g_theme;
      $themeDir   = $config_atkroot."atk/themes/".$g_theme["Name"]."/images/";
      $defaultDir = $config_atkroot."atk/themes/default/images/";
      if (file_exists($themeDir.$img))
      {
        return $themeDir.$img;
      }
      else
      {
        return $defaultDir.$img;
      }
    }

  }

   // The one and only layout object
  if (empty($g_layout))
  {
    atkdebug("Obsolete creation of g_layout");
    $g_layout = new layout();
  }

?>
