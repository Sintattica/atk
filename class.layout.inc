<?php

  /**
  * This class generates all output from atk.
  *
  * @author Ivo Jansch  (ivo@achievo.com)
  *
  * @version $Revision$
  *
  * $Id$  
  *
  */
  
  // Global debug and error msg
  $g_debug_msg = array();
  $g_error_msg = array();

  class layout
  {       
    /* 
     * Content
     */
    var $m_content = "";
  
    /**
     * Raw
     */
    var $m_raw = "";
   
    var $m_submitscripts = array();
    var $m_scripts = array();   
    var $m_stylesheets = array();
    
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
      $this->register_style((file_exists("./style.php") ? "." : "atk").'/style.php');
    }
	
    /*
     * Import_layout
     * Imports the layout file with all the items.
     */
    function import_layout()
    {        
      global $g_theme;
      include_once($this->m_tpldir."layout.inc");
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
      global $g_debug_msg, $g_error_msg;
      if (strlen($this->m_raw)>0)
      {
        echo $this->m_raw;
      }
      else
      {
        // echo some headers first..
  
        // Since atk pages are always dynamic, we have to prevent that some browsers cache
        // the pages
        header ("Expires: Mon, 26 Jul 1997 05:00:00 GMT");    // Date in the past
        header ("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT"); // always modified
        header ("Cache-Control: no-cache, must-revalidate");  // HTTP/1.1
        header ("Pragma: no-cache");                          // HTTP/1.0
  
        // Set the content type.
        header ("Content-Type: text/html; charset=".text('charset')); // The character set (defined
                                                                      // in the language files
                                                                
        echo $this->m_content;
        if (count($g_error_msg)>0)
        {
          echo '<br><font color="#FF0000">error: '.implode("<br>\nerror: ",$g_error_msg).'</font>';
          // also send a mail report with errormessages.. 
          mailreport();
        }
        if ($GLOBALS['config_debug'])
        {
          echo '<br>dbg: '.implode("<br>\ndbg: ",$g_debug_msg);
        }
      }
    }
    
    /** 
     * Generate a complete page.. 
     */
    function page($title="")
    {
      $m_page = "<html>\n";
      $m_page.= $this->ret_head($title);
      $m_page.= $this->ret_body();
      $m_page.= $this->m_content;
      $m_page.= "\n</body>\n</html>\n";
      
      // hele pagina terug in de output schrijven..
      $this->m_content = $m_page;
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
      return 20;
    }

    /**
     * Register a javascript for use withing the page
     * @param $file the filename of the external javascript file
     */
    function register_script($file)
    {
      if (!in_array($file, $this->m_scripts)) $this->m_scripts[] = $file;
    }
    
    function register_submitscript($code)
    {
      if (!in_array($code, $this->m_submitscripts)) $this->m_submitscripts[] = $code;
    }

    /**
     * Register a Cascading StyleSheet for use withing the page
     * @param $file the filename of the external stylesheet
     */
    function register_style($file)
    {
      if (!in_array($file, $this->m_stylesheets)) $this->m_stylesheets[] = $file;
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
      $res = "<head>\n  <title>$title</title>\n";
      global $config_atkroot;
      // $Name$ is the CVS tag.
      $version = atkversion();
      if ("\$Name$"!="\$"."Name:  $") $version.=" ($Name$)";
      $res.= '  <meta name="atkversion" value="'.$version.'">'."\n";
      
      for ($i = 0; $i < count($this->m_scripts); $i++) 
        $res.='  <script language="javascript" src="'.$this->m_scripts[$i].'"></script>'."\n";
              
      $res.="<script language=\"javascript\">\n
               function globalSubmit(form)
               {
                 var retval = true;\n";
      for ($i=0; $i < count($this->m_submitscripts); $i++)
      {
        $res.="retval = ".$this->m_submitscripts[$i]."\n";
        $res.="if (retval!=true) return false;\n";
      }      
      $res.="return retval;
               }
             </script>\n";
      
      for ($i = 0; $i < count($this->m_stylesheets); $i++) 
        $res.='  <link href="'.$config_atkroot.$this->m_stylesheets[$i].'" rel="stylesheet" type="text/css">'."\n";
      
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
      global $g_theme;
      $template=$this->getTemplate("data_top");      
      $template=str_replace('[IMAGEDIR]',$this->m_tpldir."/images",$template);
      if (!isset($g_theme["border"]))
      {  
        if ($template=="") $template='<table border="1" cellspacing="0" cellpadding="3">';
      }
      else
      {
        if ($template=="") $template='<table border="0" cellspacing="0" cellpadding="0" class="backtable"><tr><td><table border="0" cellspacing="'.$g_theme["border"].'" cellpadding="3">';
      }
      return $template;
    }
       
    function data_bottom()
    {      
      global $g_theme;
      $template=$this->getTemplate("data_bottom");      
      $template=str_replace('[IMAGEDIR]',$this->m_tpldir."/images",$template);      
      if (!isset($g_theme["border"]))
      {
        if ($template=="") $template='</table>';
      }
      else
      {
        if ($template=="") $template='</table></td></tr></table>';
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
      return '<tr class="row'.($alt?"2":"1").'" '.$extraprops.'>';            
    }
    
    function tr_bottom()
    {
      return '</tr>';
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
      if(file_exists(atkconfig("atkroot")."themes/".$g_theme["Name"]."/layout.inc") &&
         file_exists(atkconfig("atkroot")."themes/".$g_theme["Name"]."/block_top.tmpl") &&
         file_exists(atkconfig("atkroot")."themes/".$g_theme["Name"]."/block_bottom.tmpl"))
      {        
        $this->m_tpldir = atkconfig("atkroot")."themes/".$g_theme["Name"]."/";
      }
      // else check atk themes directory
      else if (file_exists(atkconfig("atkroot")."atk/themes/".$g_theme["Name"]."/layout.inc") &&
               file_exists(atkconfig("atkroot")."atk/themes/".$g_theme["Name"]."/block_top.tmpl") &&
               file_exists(atkconfig("atkroot")."atk/themes/".$g_theme["Name"]."/block_bottom.tmpl"))
      {
        $this->m_tpldir = atkconfig("atkroot")."atk/themes/".$g_theme["Name"]."/";
      }
      // else fallback to default.
      else      
      {
        halt("Theme ".$g_theme["Name"]."cannot be found, swithing to default.","warning");
        $this->m_tpldir = atkconfig("atkroot")."atk/themes/default/";
      }    
    }
    
    function getImgSrc($img)
    {
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
    $g_layout = new layout();
  }  
?>
