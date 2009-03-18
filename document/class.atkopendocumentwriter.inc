<?php
  /**
   * atkOpenDocumentWriter class file
   *
   * @package atk
   * @subpackage document
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal  Include the TinyButStrong OpenOffice.org library:
   */
  include_once('tbsooo/tbs_class.php');
  include_once('tbsooo/tbsooo_class.php');

  /**
   * OpenDocumentWriter wrapper derived from atkDocumentWriter for TinyButStrong OpenOffice.org template engine
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk
   * @subpackage document
   */
  class atkOpenDocumentWriter extends atkDocumentWriter
  {

    /**
     * TinyButStrong OpenOffice.org template parser instance
     *
     * @access protected
     * @var clsTinyButStrongOOo
     */
    var $m_tbsooo = null;

    /**
     * atkOpenDocumentWriter Constructor
     */
    function atkOpenDocumentWriter()
    {
      $this->m_tbsooo = new clsTinyButStrongOOo;
      $this->m_tbsooo->VarPrefix = "documentvar_";
      $processdir = atkconfig("atktempdir") . "documents";
      if (!is_dir($processdir)) mkdir($processdir);
      $this->m_tbsooo->SetProcessDir($processdir);
			$this->m_tbsooo->SetDataCharset("UTF8");
    }

    /**
     * Parse the given template file
     *
     * @access private
     * @param string $tpl_file Template file to parse
     * @param mixed $tpl_vars Array of template variables to merge into the template or null if you want to use the template vars set by calling Assign (which is default behaviour).
     * @param string $content_file default content.xml (open office), or word/document.xml for .docx
     * @return bool Indication if parsing was succesfull
     */
    function _parse($tpl_file, $tpl_vars = null, $content_file="content.xml")
    {
      // Determine which template vars to use (use supplied vars if
      // available, else use m_tpl_vars (can be set using assign())
      $use_vars = is_null($tpl_vars) ? $this->m_tpl_vars : $tpl_vars;

      // Create a new copy of the opendocument template
      $this->m_tbsooo->NewDocFromTpl($tpl_file);

      // Extract content-file from the template document
      if (!$this->m_tbsooo->LoadXmlFromDoc($content_file))
        return false;

      // Merge template vars with the content.xml file
      foreach($use_vars as $key => $value)
      {
        if (is_array($value)) {
        	foreach ($value as $key => &$txtval) 
					  $txtval = atk_iconv(atkGetCharset(),"UTF-8",$txtval);
          $this->m_tbsooo->MergeBlock($key, $value);
        }
        else
        {
        	$value = atk_iconv(atkGetCharset(),"UTF-8",$value);
          $this->m_tbsooo->MergeField($key, $value);
        }
      }

      // add own style to xml file (just before </office:automatic-styles>)
      // and use it for text that applies. This is only done on odt files
      $filenameparts = explode(".", $tpl_file);
      if ($filenameparts[count($filenameparts)-1] == "odt")
        $this->m_tbsooo->Source = $this->addAndApplyStyles($this->m_tbsooo->Source);


      // Save the content-file back to the copied document
      if (!$this->m_tbsooo->SaveXmlToDoc())
        return false;

      return true;
    }

    /**
     * Parse and send the given template file
     *
     * @param string $tpl_file Template file to parse
     * @param string $outputfilename Name of file as it is send to the user agent
     * @param mixed $tpl_vars Array of template variables to merge into the template or null if you want to use the template vars set by calling Assign (which is default behaviour).
     * @param boolean $forcedownload If set to true the content-type is set to application/octet-stream in order to make the browser think it is receiving unknown binary data so it offers you to save the file. If set to false, the browser may perform any action related to the document type (like loading a plugin to show the document inline).
     */
    function display($tpl_file, $outputfilename, $tpl_vars = null, $forcedownload = false)
    {
      // Parse template
      if (!$this->_parse($tpl_file, $tpl_vars))
        return false;

      exportFile($this->m_tbsooo->GetPathnameDoc(), $outputfilename, ($forcedownload?"application/octet-stream":$this->m_tbsooo->GetMimetypeDoc()));

      // Remove the document from disk
      $this->m_tbsooo->RemoveDoc();

      // Remove old unremoved documents
      $this->m_tbsooo->ClearProcessDir();

      return true;
    }

    /**
     * Parse and save the given template file
     *
     * @param string $tpl_file Template file to parse
     * @param string $outputfilename Filename used to save the file
     * @param mixed $tpl_vars Array of template variables to merge into the template or null if you want to use the template vars set by calling Assign (which is default behaviour)
     * @param boolean $create_non_existing_dir try to create directory if it does not exist
     */
    function store($tpl_file, $outputfilename, $tpl_vars = null, $create_non_existing_dir=false)
    {
      // Parse template
      if (!$this->_parse($tpl_file, $tpl_vars))
        return false;

      // Get the temporary (source) filename
      $tempfilename = $this->m_tbsooo->GetPathnameDoc();

      // Throw an error and abort if the source file does not exist
      if (!file_exists($tempfilename))
      {
        atkerror("atkOpenDocumentWriter->store: Temporary file '$tempfilename' could not be found. The document most likely failed to parse.");
        return false;
      }

      // Throw an error and abort if the location of the destination file does not exist
      if (!file_exists(dirname($outputfilename)))
      {
        if ($create_non_existing_dir)
        {
          atkdebug("atkOpenDocumentWriter->store: Trying to create output location '".dirname($outputfilename)."'");
          
          if (!mkdir(dirname($outputfilename), 0755, true))
          {
            atkerror("atkOpenDocumentWriter->store: Output location '".dirname($outputfilename)."' could not be created. The outputfile '$outputfilename' can not be stored.");
            return false;
          }          
        }
        else
        {
          atkerror("atkOpenDocumentWriter->store: Output location '".dirname($outputfilename)."' could not be found. The outputfile '$outputfilename' can not be stored.");
          return false;
        }      
      }

      // Move the document to it's destination
      rename($tempfilename, $outputfilename);

      // Remove old unremoved documents
      $this->m_tbsooo->ClearProcessDir();

      return true;
    }

    /**
     * Add predefined text-styles to xml file
     * And replace html style tags by xml style-tags
     * 
     * @param string $inputString
     */
    function addAndApplyStyles($inputString)
    {
      $inputString = $this->addPredefinedStyles($inputString);

      $inputString = $this->convertFirefoxHtmlToXML($inputString);

      $inputString = $this->convertIEHtmlToXML($inputString);
      
      $inputString = $this->convertGeneralHtmlToXML($inputString);

      return $inputString;
    }

    /**
     * Add predefined styles
     *
     * @param string $inputString
     * @return string The inputstring including the predefined styles
     */
    function addPredefinedStyles($inputString)
    {
      // Add predefined styles to xml-file
      $xmlAutomaticStyleTag = '</office:automatic-styles>';
      $xmlBold       = '<style:style style:name="atkbold" style:family="text"><style:text-properties fo:font-weight="bold" style:font-weight-asian="bold" style:font-weight-complex="bold"/></style:style>';
      $xmlItalic     = '<style:style style:name="atkitalic" style:family="text"><style:text-properties fo:font-style="italic" style:font-style-asian="italic" style:font-style-complex="italic"/></style:style>';
      $xmlBoldItalic = '<style:style style:name="atkbolditalic" style:family="text"><style:text-properties fo:font-style="italic" fo:font-weight="bold" style:font-style-asian="italic" style:font-weight-asian="bold" style:font-style-complex="italic" style:font-weight-complex="bold"/></style:style>';
      $xmlUnderline  = '<style:style style:name="atkunderline" style:family="text"><style:text-properties style:text-underline-style="solid" style:text-underline-width="auto" style:text-underline-color="font-color"/></style:style>';

      $inputString = str_replace($xmlAutomaticStyleTag , $xmlBold.$xmlItalic.$xmlBoldItalic.$xmlUnderline.$xmlAutomaticStyleTag, $inputString);

      return $inputString;
    }

    /**
     * Convert firefox-html tags to xml tags
     *
     * @param string $inputString
     * @return string The converted inputstring
     */
    function convertFirefoxHtmlToXML($inputString)
    {
      /*****************************************
       * Convert firefox-html tags to xml tags *
       *****************************************/

      // for some reason, html code varies a little bit; changes will be applied to make uniform style conversions below

      // convert style param-names to lower case
      $inputString = str_replace('FONT-WEIGHT',     'font-weight',     $inputString);
      $inputString = str_replace('FONT-STYLE',      'font-style',      $inputString);
      $inputString = str_replace('TEXT-DECORATION', 'text-decoration', $inputString);

      // the ; is missing sometimes at the end of a style-line
      $inputString = str_replace('font-style: italic"&gt;', 'font-style: italic;"&gt;', $inputString);
      $inputString = str_replace('font-weight: bold"&gt;', 'font-weight: bold;"&gt;', $inputString);
      $inputString = str_replace('text-decoration: underline"&gt;', 'text-decoration: underline;"&gt;', $inputString);


      // _underline_
      $inputString = str_replace('&lt;span style="text-decoration: underline;"&gt;', '<text:span text:style-name="atkunderline">', $inputString);

      // _underline_ - no combinations with other styles supported; remove from combinations with other styles to prevent xml-errors
      $inputString = str_replace('text-decoration: underline; ', '', $inputString);
      $inputString = str_replace(' text-decoration: underline;', '', $inputString);

      // Combination of *BOLD* and /italic/
      $inputString = str_replace('&lt;span style="font-weight: bold; font-style: italic;"&gt;', '<text:span text:style-name="atkbolditalic">', $inputString);
      $inputString = str_replace('&lt;span style="font-style: italic; font-weight: bold;"&gt;', '<text:span text:style-name="atkbolditalic">', $inputString);

      // *BOLD* inside <li> tag: <li style="font-weight: bold;">bold bullet</li>
      $inputString = preg_replace("/&lt;li style=\"font-weight\: bold;\"&gt;(.*)&lt;\/li&gt;/iU", "&lt;li&gt;<text:span text:style-name=\"atkbold\">$1</text:span>", $inputString);

      // italic inside <li> tag: <li style="font-style: italic;">italic bullet</li>
      $inputString = preg_replace("/&lt;li style=\"font-style\: italic;\"&gt;(.*)&lt;\/li&gt;/iU", "&lt;li&gt;<text:span text:style-name=\"atkitalic\">$1</text:span>", $inputString);
      
      // *BOLD*
      $inputString = str_replace('&lt;span style="font-weight: bold;"&gt;', '<text:span text:style-name="atkbold">', $inputString);

      // /italic/
      $inputString = str_replace('&lt;span style="font-style: italic;"&gt;', '<text:span text:style-name="atkitalic">', $inputString);


      // substitute html- by xml-'span' tags
      $inputString = str_replace('&lt;/span&gt;' , '</text:span>', $inputString);
      
      // replace <br .*/> chars by xml-newline (also take font-styles in account)
      $inputString = preg_replace("/(<|&lt;)br.*\/(>|&gt;)/iU","<text:line-break/>", $inputString);

      return $inputString;
    }

    /**
     * Convert internet explorer-html tags to xml tags
     *
     * @param string $inputString
     * @return string The converted inputstring
     */
    function convertIEHtmlToXML($inputString)
    {
      /***************************************************
       * Convert internet explorer-html tags to xml tags *
       ***************************************************/

      // Searching combinations it not needed, since xml-tags work the same as in html
      //   HTML: <b><i>BLA</b></i> becomes
      //   XML:  <text:span text:style-name="atkbold"><text:span text:style-name="atkitalic">BLA</text:span></text:span>

      // *BOLD*
      $inputString = str_replace('&lt;strong&gt;', '<text:span text:style-name="atkbold">', $inputString);
      $inputString = str_replace('&lt;/strong&gt;', '</text:span>', $inputString);

      // /italic/
      $inputString = str_replace('&lt;em&gt;', '<text:span text:style-name="atkitalic">', $inputString);
      $inputString = str_replace('&lt;/em&gt;', '</text:span>', $inputString);

      // _underline_
      $inputString = str_replace('&lt;u&gt;', '<text:span text:style-name="atkunderline">', $inputString);
      $inputString = str_replace('&lt;/u&gt;', '</text:span>', $inputString);

      // remove paragraph chars, since they create extra newlines
      $inputString = str_replace('&lt;p&gt;', '', $inputString);
      $inputString = str_replace('&lt;/p&gt;', '', $inputString);

      // replace &nbsp; by space
      $inputString = str_replace('&nbsp;', ' ', $inputString);
      $inputString = str_replace('&amp;nbsp;', ' ', $inputString);

      return $inputString;
    }
    
    /**
     * Convert general-html tags to xml tags (same for IE and Firefox)
     *
     * @param string $inputString
     * @return string The converted inputstring
     */
    function convertGeneralHtmlToXML($inputString)
    {
      /**************************************************************
       * Convert general-html tags to xml tags (same for IE/Firefox *
       **************************************************************/
      // Convert all html bullets to an XML-tab + -
      // Only 1 level deep supported (deeper levels will be converted to level 1 in the generated document)
      $inputString = str_replace("\r" , '', $inputString);
      $inputString = str_replace("<text:line-break/><text:line-break/>&lt;ul&gt;<text:line-break/>", '', $inputString);                    // first <ul>
      $inputString = str_replace("&lt;ul&gt;<text:line-break/><text:line-break/>"  , '',               $inputString);                      //  sub <ul>'s
      $inputString = str_replace('&lt;li&gt;'                                      , '- ', $inputString);                      // use '-' as bullet
      $inputString = str_replace('&lt;/li&gt;<text:line-break/>'                   , '',               $inputString);                      // end bullet
      $inputString = str_replace('<text:line-break/>&lt;/ul&gt;<text:line-break/>' , '',               $inputString);                      // main closing </ul>
      $inputString = str_replace('&lt;/ul&gt;<text:line-break/><text:line-break/>' , '',               $inputString);                      //  sub closing </ul>'s 
      $inputString = str_replace('&lt;/ul&gt;'                                     , '',               $inputString);                      // remove remaining </ul>'s
      
      // tbsooo_class.php (LoadXmlFromDoc) converted apostrophe (&apos; -> \') from XML file for TBS functions
      // causes corrupt document: convert it back to &apos;
      $inputString = str_replace('\'', '&apos;', $inputString);
      return $inputString;
    }
  }
?>