<?php

  class atkPage 
  {
    var $m_scriptfiles = array();
    var $m_scriptcode = array();
    var $m_submitscripts = array();
    var $m_loadscripts = array();
    var $m_stylesheets = array();
    var $m_content = "";
    
    function &getInstance()
    {
      static $s_page = NULL;
      if ($s_page == NULL)
      {
        $s_page = new atkPage();
        atkdebug("Created a new atkPage instance");        
      }
      return $s_page;
    }
            
    /**
     * Register a javascript file for use withing the page
     * @param $file the filename of the external javascript file
     */
    function register_script($file)
    {
      if (!in_array($file, $this->m_scriptfiles)) $this->m_scriptfiles[] = $file;
    }
    
    /**
     * Register a piece of javascript code for use in the header.
     * @param String $code The javascript code to place in the header.
     */
    function register_scriptcode($code)
    {
      if (!in_array($code, $this->m_scriptcode)) $this->m_scriptcode[] = $code;
    }
    
    /**
     * Register a javascript for validation on submit of a form.
     * @param $code the code to execute on submit.
     */
    function register_submitscript($code)
    {      
      if (!in_array($code, $this->m_submitscripts)) $this->m_submitscripts[] = $code;
    }
    
    /** 
     * Register a piece of javascript for execution on load of the document.
     * @param $code the code to execute on load.
     */
   function register_loadscript($code)
   {
     if (!in_array($code, $this->m_loadscripts)) $this->m_loadscripts[] = $code;
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
     * Return a HTML header
     * @param $title Titel van het window
     */
    function head($title)
    {
      $res = "<head>\n  <title>$title</title>\n";
      
      // $Name$ is the CVS tag.
      $version = atkversion();
      if ("\$Name$"!="\$"."Name:  $") $version.=" ($Name$)";
      $res.= '  <meta name="atkversion" value="'.$version.'">'."\n";

      for ($i = 0; $i < count($this->m_scriptfiles); $i++)
        $res.='  <script language="javascript" src="'.$this->m_scriptfiles[$i].'"></script>'."\n";

      $res.="<script language=\"javascript\">\n
               function globalSubmit(form)
               {
                 var retval = true;\n";
      for ($i=0, $_i = count($this->m_submitscripts); $i<$_i; $i++)
      {
        $res.="retval = ".$this->m_submitscripts[$i]."\n";
        $res.="if (retval!=true) return false;\n";
      }
      $res.="return retval;
               }\n";
               
      if (count($this->m_loadscripts))
      {
        $res.="function globalLoad()
               {\n";
        for ($i=0, $_i=count($this->m_loadscripts); $i<$_i; $i++)
        {
          $res.= $this->m_loadscripts[$i]."\n";
        }
        $res.= " }\n";
      }
      
      // javascript code.   
      for ($i=0, $_i=count($this->m_scriptcode); $i<$_i; $i++)
      {
        $res.= $this->m_scriptcode[$i]."\n";
      }
      
      $res.= "</script>\n";

      for ($i = 0; $i < count($this->m_stylesheets); $i++)
        $res.='  <link href="'.$this->m_stylesheets[$i].'" rel="stylesheet" type="text/css">'."\n";

      $res.="</head>\n";
      return $res;
    }
    
    function addContent($content)
    {      
      $this->m_content.=$content;
    }
    
    /**
     * Return a HTML body
     * @param $extraops Not used
     */
    function body($extraprops="")
    {
      $res = '<body ';
      if (count($this->m_loadscripts)) $res.='onLoad="globalLoad()" ';
      $res.= $extraprops.">\n";
      return $res;
    }
    
    function render($title, $body=true)
    {      
      $page = "<html>\n";
      $page.= $this->head($title);
      if ($body) $page.= $this->body();
      $page.= $this->m_content."\n";
      if ($body) $page.= "</body>\n";
      $page.= "</html>\n";            
     
      return $page;
    }
    
    function atkPage()
    {
      // Load default app-level stylesheet.
      $this->register_style((file_exists(atkconfig("atkroot")."./style.css") ? "." : atkconfig("atkroot")."atk").'/style.css');  
    }
        
  }

?>
