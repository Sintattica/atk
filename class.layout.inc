<?php

  /**
  * This class generates all output from atk.
  *
  * @author Ivo Jansch  (ivo@achievo.com)
  *
  * @version 1.2
  *
  * <b>Changes:</b>
  *
  * 1.3 22-02-2001 - removed some unused variables
  *                             - Created a new template engine.
  *
  * 1.2  04-01-2001  Sandy Pleyte (sandy@ibuildings.nl)
  *         - new variables ($m_layout, $m_path, $m_url)
  *
  *
  * 1.1  - register_script function that attributes can use if they need to 
  *        load javascripts.(Peter Verhage <peter@ibuildings.nl>)
  *
  * $Id$
  * $Log$
  * Revision 4.1  2001/04/24 12:40:50  ivo
  * skel cleanup, typo fix in moduletools.
  *
  * Revision 4.0  2001/04/23 10:19:38  ivo
  * Revision number update
  *
  * Revision 1.1.1.1  2001/04/23 10:17:06  ivo
  * Added atk4 to repository.
  *
  * Revision 1.15  2001/04/05 13:58:31  ivo
  * Small navigation fix.
  * Layout changes: head can now be rendered at the end.
  *
  * Revision 1.14  2001/04/02 09:24:33  ivo
  * Smaller fonts for tableheader of all themes, and fixed a typo
  * in class.layout.inc.
  *
  * Revision 1.13  2001/03/27 07:05:55  ivo
  * data_top and data_bottom are now themeable.
  *
  * Revision 1.12  2001/03/23 15:28:55  ivo
  * Fixed minor bugs in skel files.
  * Updated Themes-HOWTO to reflext new theme structure.
  * Fixed a bug in the atk delete action.
  * Fixed a bug in the theme support.
  *
  * Revision 1.11  2001/03/23 14:06:51  ivo
  * Theme directory restructure. Optimized theme support in layout class.
  *
  * Revision 1.10  2001/03/16 08:35:58  sandy
  * Removed some debug code
  *
  * Revision 1.9  2001/03/02 12:31:25  sandy
  * - changed the xlay files
  * - atk now uses a dynamic stylesheet
  * - updated Themes-HOWTO
  * - removed some unused stuff
  *
  * Revision 1.8  2001/02/28 08:32:13  sandy
  * - Bugfix in the class.atknode for theme support
  * - Enabled the haltfunctions again
  *
  * Revision 1.7  2001/02/26 15:33:23  martin
  * alle halt functions uitgezet. function is nog niet defined????
  *
  * Revision 1.6  2001/02/23 13:34:03  sandy
  * Changed default layout path in atk, and converted the old themes
  *
  * Revision 1.5  2001/02/23 11:17:30  sandy
  * - Updated the layout class with new template engine
  * - New Dummy attribute
  * - New skel directory
  * - and some small new config vars
  *
  * Revision 1.3  2001/02/15 19:27:46  martin
  * fixed php warning
  *
  * Revision 1.2  2001/02/15 16:20:24  ivo
  * Major new feature: security.
  *
  * Revision 1.1.1.1  2001/01/10 13:57:56  sandy
  * Achievo Tool Kit
  *
  */

// Multiple inclusion protector
if (!defined("CLASS_LAYOUT_INC"))
{
  define("CLASS_LAYOUT_INC",1);

  class layout
  {       
    /* 
     * Content
     */
    var $m_content = "";

    /**
     * Debug message
     */
    var $m_debugmsg = "";

    /**
     * Raw
     */
    var $m_raw = "";
   

    var $m_scripts = array();   
    
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
      global $g_theme;
      if($g_theme["Name"]=="") $g_theme["Name"] = $GLOBALS["config_defaulttheme"];
      if($g_theme["Name"]=="") $g_theme["Name"]="default";
      $this->initTheme();
      $this->import_layout();
    }
	
    /*
     * Import_layout
     * Imports the layout file with all the items.
     */
    function import_layout()
    {        
      global $g_theme;
      include($this->m_tpldir."layout.inc");
    }

    /**
     * Output text to page
     * @param $txt Text 
     */
    function output($txt)
    {
      $this->m_content.= $txt."\n";
    }

    /**
     * Output rawtext to page
     * @param $txt Text 
     */
    function rawoutput($txt)
    {
      $this->m_raw.= $txt."\n";
    }

    /**
     * Output 
     * @param $txt Text 
     */
    function outputFlush()
    {
      global $g_debug_msg;
      if (strlen($this->m_raw)>0)
      {
        echo $this->m_raw;
      }
      else
      {
        echo $this->m_content;
        if ($GLOBALS['config_debug'])
        {
          echo '<br>'.$g_debug_msg;
        }
      }
    }
    
    /** 
     * Generate a complete page.. 
     */
    function page($title="")
    {
      $m_page = '<html>';
      $m_page.= $this->ret_head($title);
      $m_page.= $this->ret_body();
      $m_page.= $this->m_content;
      $m_page.= '</body></html>';
      
      // hele pagina terug in de output schrijven..
      $this->m_content = $m_page;
    }

    /**
     * Generate Debug messages 
     */
    function debug($txt)
    {
      global $g_debug_msg;
      if ($GLOBALS['config_debug'])
      {
        $g_debug_msg.= "dbg: ".$txt."<br>\n";
      }
    }


    /**
     * Error report 
     * @param $title Title of the window
     * @param $txt Error message
     */
    function error($title,$txt)
    {
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
      return 60;
    }

    /**
     * Returns the searchSize 
     * @return 15
     */
    function searchSize()
    {
      return 15;
    }

    /**
     * Register a javascript for use withing the page
     * @param $file the filename of the external javascript file
     */
    function register_script($file)
    {
      if (!in_array($file, $this->m_scripts)) $this->m_scripts[] = $file;
    }

    /**
     * Output a HTML header
     * @param $title Titel van het window
     */
    function head($title)
    {
      if (!$this->m_wrotehead)
      {
        $this->output($this->ret_head($title));
        $this->m_wrotehead = true;
      }
    }
    
    /**
     * Return a HTML header
     * @param $title Titel van het window
     */
    function ret_head($title)
    {     
      $res = '<head><title>'.$title.'</title>';
      for ($i = 0; $i < sizeof($this->m_scripts); $i++) 
      {
        $res.='<script language="javascript" src="'.$this->m_scripts[$i].'"></script>';
      }
      $res.='<link href="atk/style.php" rel="stylesheet" type="text/css">';
      $res.="</head>\n";
      return $res;
    }
   
    /**
     * Output a HTML body
     * @param $extraops Not used
     */
    function body($extraprops="")
    {
      if (!$this->m_wrotebody)
      {
        $this->output($this->ret_body($extraprops));
        $this->m_wrotebody = true;
      }
    }
    
    /**
     * Return a HTML body
     * @param $extraops Not used
     */
    function ret_body($extraprops="")
    {
      return "<body ".$extraprops.">\n";
    }
    

    /**
     * Create the UI top
     * @param $title Title of the UI top
     */
    function ui_top($heading)
    {    
      $template = $this->getTemplate("block_top");      
      
      $template=str_replace('[HEADER]',$heading,$template);
      $template=str_replace('[IMAGEDIR]',$this->m_tpldir."/images",$template);      
      $this->output($template);
    }

    /**
     * Create UI Bottom
     */
    function ui_bottom()
    {     
      $template=$this->getTemplate("block_bottom");      
      $template=str_replace('[IMAGEDIR]',$this->m_tpldir."/images",$template);
      $this->output($template);
    }
    
    function data_top()
    {
      $template=$this->getTemplate("data_top");      
      $template=str_replace('[IMAGEDIR]',$this->m_tpldir."/images",$template);      
      if ($template=="") $template='<table border="1" cellspacing="1" cellpadding="3">';
      return $template;
    }
       
    function data_bottom()
    {      
      $template=$this->getTemplate("data_bottom");      
      $template=str_replace('[IMAGEDIR]',$this->m_tpldir."/images",$template);      
      if ($template=="") $template='</table>';
      return $template;
    }
 
    /**
     * Create simple table
     * @param $border border of the table
     * @param $fillout Fillout (Boolean)
     */
    function table_simple($border=0,$fillout=false)
    {
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
      $this->output('<td class="table" '.$extraprops.'>');
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
     * Create TD whitch can be returned
     * @param $contents contents for the TD
     * @param $extraops Extra options for the TD (Width, Height)
     * @return HTML Code with TD + Content
     */
    function ret_td($contents="", $extraprops="")
    {
      $output='<td class="table" '.$extraprops.'>';
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
     * Create TD header 
     * @param $contents contents for the TD header
     * @param $extraops Extra options for the TD header (Width, Height)
     */
    function td_datatitle($contents="", $extraprops="")
    {
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
       
    function getTemplate($templatefile)
    {      
      if (file_exists($this->m_tpldir.$templatefile.".tmpl"))
      {
        return implode(file($this->m_tpldir.$templatefile.".tmpl"), "");
      }
      return "";
    }
    
    function initTheme()
    {
      global $g_theme;
                  
      // First check local theme directory
      if(file_exists("themes/".$g_theme["Name"]."/layout.inc") &&
         file_exists("themes/".$g_theme["Name"]."/block_top.tmpl") &&
         file_exists("themes/".$g_theme["Name"]."/block_bottom.tmpl"))
      {        
        $this->m_tpldir = "themes/".$g_theme["Name"]."/";
      }
      // else check atk themes directory
      else if (file_exists("atk/themes/".$g_theme["Name"]."/layout.inc") &&
               file_exists("atk/themes/".$g_theme["Name"]."/block_top.tmpl") &&
               file_exists("atk/themes/".$g_theme["Name"]."/block_bottom.tmpl"))
      {
        $this->m_tpldir = "atk/themes/".$g_theme["Name"]."/";
      }
      // else fallback to default.
      else      
      {
        halt("Theme ".$g_theme["Name"]."cannot be found, swithing to default.","warning");
        $this->m_tpldir = "atk/themes/default/";
      }    
    }
  }
  
   // The one and only layout object   
  if (empty($g_layout))
  {    
    $g_layout = new layout();
  }  
}
?>
