<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage ui
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Rendering flags.
   */
  define("HTML_BODY",1);                        // Add body tags to page
  define("HTML_HEADER",2);                      // Add header to page
  define("HTML_DOCTYPE",4);                     // Add doctype to page
  define("HTML_ALL",HTML_BODY|HTML_HEADER);     // Shortcut
  define("HTML_STRICT",HTML_ALL|HTML_DOCTYPE);  // Shortcut

   
  /**
   * Page renderer. 
   *
   * This class is used to render output as an html page. It takes care of
   * creating a header, loading javascripts and loading stylesheets.
   * Since any script will output exactly one page to the browser, this is 
   * a singleton. Use getInstance() to retrieve the one-and-only instance.
   *
   * @todo This should actually not be a singleton. HTML file generation
   *       scripts may need an instance per page generated.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage ui
   *
   */
  class atkPage 
  {
    /**
     * The list of javascript files to load.
     * @access private
     * @var array
     */
    var $m_scriptfiles = array();
    
    /**
     * List of javascript code statements to include in the header.
     * @access private
     * @var array
     */
    var $m_scriptcode = array();
    
    /**
     * List of javascript code statements to execute when a form on
     * the page is submitted.
     * @access private
     * @var array
     */
    var $m_submitscripts = array();
    
    /**
     * List of javascript code statements to execute when the page
     * is loaded.
     * @access private
     * @var array
     */
    var $m_loadscripts = array();
    
    /**
     * List of stylesheet files to load.
     * @access private
     * @var array
     */
    var $m_stylesheets = array();
    
    /** 
     * List of style statements to include in the header.
     * @access private
     * @var array
     */
    var $m_stylecode = array();
		
    /**
     * The content to put on the page.
     * @access private
     * @var String
     */
    var $m_content = "";
    
    /**
     * The hidden variables for the page
     * @access private
     * @var Array
     */
    var $m_hiddenvars = array();
    
    /**
     * Retrieve the one-and-only atkPage instance.
     * @return atkPage
     */
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
     * Register a javascript file to be included.
     *
     * If called twice for the same filename, the file is loaded only once.
     * @param String $file The (relative path and) filename of the javascript
     *                     file.
     */
    function register_script($file)
    {
      if (!in_array($file, $this->m_scriptfiles)) $this->m_scriptfiles[] = $file;
    }
    
    /**
     * Register a javascript code statement which will be rendered in the
     * header.
     *
     * The method has a duplicate check. Registering the exact same statement
     * twice, will result in the statement only being rendered and executed
     * once.
     * @param String $code The javascript code to place in the header.
     */
    function register_scriptcode($code)
    {
      if (!in_array($code, $this->m_scriptcode)) $this->m_scriptcode[] = $code;
    }
    
    /**
     * Register a javascript code statement that is executed when a form on
     * the page is submitted.
     * @todo This is inconsequent, if multiple forms are present, each should
     *       have its own submitscripts. Should be moved to an atkForm class.
     * @param String $code The javascript code fragment to execute on submit.
     */
    function register_submitscript($code)
    {      
      if (!in_array($code, $this->m_submitscripts)) $this->m_submitscripts[] = $code;
    }
    
    /** 
     * Register a javascript code statement that is executed on pageload.
     * @param String $code The javascript code fragment to execute on load.
     */
   function register_loadscript($code)
   {
     if (!in_array($code, $this->m_loadscripts)) $this->m_loadscripts[] = $code;
   }
    
    /**
     * Register a Cascading Style Sheet.
     * 
     * This method has a duplicate check. Calling it with the same stylesheet
     * more than once, will still result in only one single include of the
     * stylesheet.
     * @param String $file The (relative path and) filename of the stylesheet.
     */
    function register_style($file)
    {    
      if (!in_array($file, $this->m_stylesheets)) $this->m_stylesheets[] = $file;
    }
		
    /**
     * Register Cascading Style Sheet fragment that will be included in the
     * page header.
     * @param String $code The Cascading Style Sheet code fragment to place in
     *                     the header.
     */
     function register_stylecode($code)
     {
       if (!in_array($code, $this->m_stylecode)) $this->m_stylecode[] = $code;
     }
     
     /**
      * Register hidden variables. These will be accessible to javascript and DHTML functions/scripts
      * but will not be shown to the user unless he/she has a very, very old browser 
      * that is not capable of rendering CSS
      * @param array $hiddenvars the hiddenvariables we want to register
      */
     function register_hiddenvars($hiddenvars)
     {
     	 foreach ($hiddenvars as $hiddenvarname => $hiddenvarvalue)
     	 {
     	 	 $this->m_hiddenvars[$hiddenvarname] = $hiddenvarvalue;
     	 }
     }
    
    /**
     * Generate the HTML header (<head></head>) statement for the page,
     * including all scripts and styles.
     * @param String $title Title of the html page.
     * @return String The HTML pageheader, including <head> and </head> tags.
     */
    function head($title)
    {
      $res = "<head>\n  <title>$title</title>\n";
      
      // $Name$ is the CVS tag.
      $version = atkversion();
      if ("\$Name$"!="\$"."Name:  $") $version.=" ($Name$)";
      $res.= '  <META NAME="atkversion" CONTENT="'.$version.'">'."\n";
      $res.= '  <META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset='.text("charset","","atk").'">'."\n";         
      $this->addScripts($res);

      for ($i = 0; $i < count($this->m_stylesheets); $i++)
        $res.='  <link href="'.$this->m_stylesheets[$i].'" rel="stylesheet" type="text/css">'."\n";
				
      for ($i = 0; $i < count($this->m_stylecode); $i++)
	$res.= '<style> '.$this->m_stylecode[$i].' </style>'."\n";

      $res.="</head>\n";
      return $res;
    }
    
    /**
     * Adds javascripts from the member variables to HTML output
     * @param String $res The HTML output
     */
    function addScripts(&$res)
    {
      for ($i = 0; $i < count($this->m_scriptfiles); $i++)
      {
        $res.='  <script type="text/javascript" language="JavaScript" src="'.$this->m_scriptfiles[$i].'"></script>'."\n";
      }

      $res.="  <script type=\"text/javascript\">
  function globalSubmit(form)
  {
     var retval = true;\n";
      
      for ($i=0, $_i = count($this->m_submitscripts); $i<$_i; $i++)
      {
        $res.="    retval = ".$this->m_submitscripts[$i]."\n";
        $res.="    if (retval!=true) return false;\n";
      }
      
      $res.="    return retval;
   }\n";
      
      if (count($this->m_loadscripts))
      {
        $res.="  function globalLoad()
  {\n";
        for ($i=0, $_i=count($this->m_loadscripts); $i<$_i; $i++)
        {
          $res.= $this->m_loadscripts[$i]."\n";
        }
        $res.="  }\n";
      }
      
      // javascript code.   
      for ($i=0, $_i=count($this->m_scriptcode); $i<$_i; $i++)
      {
        $res.= $this->m_scriptcode[$i]."\n";
      }
      
      $res.= "  </script>\n\n";
    }
    
    
    /**
     * Add content to the page.
     * @param String $content The content to add to the page.
     */
    function addContent($content)
    {      
      $this->m_content.=$content;
    }
    
    /**
     * Generate the HTML body (<body></body>) statement for the page.
     * @param String $extraprops Extra attributes to add to the <body> tag.
     * @return String The HTML body, including <body> and </body> tags.
     */
    function body($extraprops="")
    {
      global $ATK_VARS;

      $res = '<body ';
      if (count($this->m_loadscripts)) 
      {
        $res.='onLoad="globalLoad(); if ( window.showTab ) {showTab(\'';
        if ($ATK_VARS['atktab']) $res.=$ATK_VARS['atktab'];
        $res.='\');}" ';
      }
      $res.= $extraprops.">\n";
      return $res;
    }
    
    /**
     * Render the complete page, including head and body.
     * @param String $title Title of the HTML page.
     * @param boolean $flags Set to true to generate <body> tags. It is useful
     *                      to set this to false only when rendering content
     *                      that either already had its own <body></body>
     *                      statement, or content that needs no body
     *                      statements, like a frameset. (DEPRICATED !!)
     * @param int $flags Flags for the render function
     * @param string $extrabodyprops  Extra attributes to add to the <body> tag.
     * @return String The HTML page, including <html> and </html> tags.
     */
    function render($title, $flags=7,$extrabodyprops="")
    { 
      if(is_bool($flags) && $flags==true) 
      { 
        $flags=HTML_STRICT; 
      }
      elseif(is_bool($flags) && $flags==false)
      {
        $flags=HTML_HEADER|HTML_DOCTYPE;
      }
      $page = '';
      if(hasFlag($flags,HTML_DOCTYPE)) $page.=atkconfig("doctype");
      $page.= "\n<html>\n";
      if(hasFlag($flags,HTML_HEADER)) $page.= $this->head($title);
      if (hasFlag($flags,HTML_BODY)) $page.= $this->body($extrabodyprops);
      $page.= $this->m_content."\n";
      $page.= $this->renderHiddenVars();
      if (hasFlag($flags,HTML_BODY)) $page.= "</body>\n";
      $page.= "</html>\n";            
     
      return $page;
    }
    
    /**
     * Here we render a hidden div in the page with hidden variables
     * that we want to make accessible to client side scripts
     * @return string a hidden div with the selected ATK variabels
     */
    function renderHiddenVars()
    {  	
    	if ($this->m_hiddenvars)
    	{
    	  $page.= "\n".'<div id="hiddenvars" style="display: none">';
    		foreach ($this->m_hiddenvars as $hiddenvarname => $hiddenvarvalue)
    		{
        	$page.="\n <span id='$hiddenvarname'>$hiddenvarvalue</span>";
    		}
    		$page.="\n</div>";
    	}
    	return $page;
    }
    
    /**
     * Default constructor.
     * @access private
     */
    function atkPage()
    {
      // Load default app-level stylesheet.
      $this->register_style((file_exists(atkconfig("atkroot")."./style.css") ? "." : atkconfig("atkroot")."atk").'/style.css');  
    }
        
  }

?>
