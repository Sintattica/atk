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
      $this->m_tbsooo->SetZipBinary(atkconfig("ziplocation", "zip"));
      $this->m_tbsooo->SetUnzipBinary(atkconfig("unziplocation", "unzip"));
      $this->m_tbsooo->SetProcessDir(atkconfig("atktempdir"));
    }

    /**
     * Parse the given template file
     *
     * @access private
     * @param string $tpl_file Template file to parse
     * @param mixed $tpl_vars Array of template variables to merge into the template or null if you want to use the template vars set by calling Assign (which is default behaviour).
     */
    function _parse($tpl_file, $tpl_vars = null)
    {
      // Determine which template vars to use (use supplied vars if
      // available, else use m_tpl_vars (can be set using assign())
      $use_vars = is_null($tpl_vars) ? $this->m_tpl_vars : $tpl_vars;

      // Create a new copy of the opendocument template
      $this->m_tbsooo->NewDocFromTpl($tpl_file);

      // Extract content.xml from the template document
      if (!$this->m_tbsooo->LoadXmlFromDoc('content.xml'))
        return false;

      // Merge template vars with the content.xml file
      foreach($use_vars as $key => $value)
      {
        if (is_array($value)) {
          $this->m_tbsooo->MergeBlock($key, $value);
        }
        else
        {
          $this->m_tbsooo->MergeField($key, $value);
        }
      }

      // add own style to xml file (just before </office:automatic-styles>)
      // and use it for text that applies
      $this->m_tbsooo->Source = $this->addAndApplyStyles($this->m_tbsooo->Source);


      // Save the content.xml back to the copied document
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

      // Fix for internet explorer misbehaviour when using attachment as content disposition
      $useragent = getenv("HTTP_USER_AGENT");
      $attachmentword = ((strpos($useragent, "MSIE") !== false) || (strpos($useragent, "Internet Explorer") !== false)) ? "Atachment" : "Attachment";

      // Add additional http response headers to set proper content type, filename and filesize
      $file = $this->m_tbsooo->GetPathnameDoc();
      header('Content-Type: ' . ($forcedownload ? 'application/octet-stream' : $this->m_tbsooo->GetMimetypeDoc()));
      header('Content-Length: ' . filesize($file));
      header('Content-Disposition: ' . $attachmentword . '; Filename="' . (is_null($outputfilename) ? substr($file, strrpos($file, '/') + 1) : $outputfilename) . '"');

      // Send the document to the user agent
      $this->m_tbsooo->FlushDoc();

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
     */
    function store($tpl_file, $outputfilename, $tpl_vars = null)
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
        atkerror("atkOpenDocumentWriter->store: Output location '".dirname($outputfilename)."' could not be found. The outputfile '$outputfilename' can not be stored.");
        return false;
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
     */
    function addAndApplyStyles($inputString)
    {
      $inputString = $this->addPredefinedStyles($inputString);

      $inputString = $this->convertFirefoxHtmlToXML($inputString);

      $inputString = $this->convertIEHtmlToXML($inputString);
      
      $inputString = $this->convertGeneralHtmlToXML($inputString);

      return $inputString;
    }

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

      // *BOLD*
      $inputString = str_replace('&lt;span style="font-weight: bold;"&gt;', '<text:span text:style-name="atkbold">', $inputString);

      // /italic/
      $inputString = str_replace('&lt;span style="font-style: italic;"&gt;', '<text:span text:style-name="atkitalic">', $inputString);

      // substitute html- by xml-'span' tags
      $inputString = str_replace('&lt;/span&gt;' , '</text:span>', $inputString);
      
      // replace end-of-line/paragraph chars by xml-newline
      $inputString = str_replace('&lt;br /&gt;' , '<text:line-break/>', $inputString);
      $inputString = str_replace('<br />' , '<text:line-break/>', $inputString);

      return $inputString;
    }

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

      // replace end-of-line/paragraph chars by xml-newline
      $inputString = str_replace('&lt;p&gt;', '', $inputString);
      $inputString = str_replace('&lt;/p&gt;', '<text:line-break/>', $inputString);

      return $inputString;
    }
    
    function convertGeneralHtmlToXML($inputString)
    {
      /**************************************************************
       * Convert general-html tags to xml tags (same for IE/Firefox *
       **************************************************************/
      
      // Convert all html bullets to an XML-tab + *
      // Only 1 level deep supported (deeper levels will be converted to level 1 in the generated document)
      $inputString = str_replace("\r" , '', $inputString);
      $inputString = str_replace("<text:line-break/><text:line-break/>&lt;ul&gt;<text:line-break/><text:line-break/>" , '', $inputString); // first <ul>
      $inputString = str_replace("&lt;ul&gt;<text:line-break/><text:line-break/>"  , '',               $inputString);                      //  sub <ul>'s
      $inputString = str_replace('&lt;li&gt;'                                      , '<text:tab/> - ', $inputString);                      // use '-' as bullet
      $inputString = str_replace('&lt;/li&gt;<text:line-break/>'                   , '',               $inputString);                      // end bullet
      $inputString = str_replace('<text:line-break/>&lt;/ul&gt;<text:line-break/>' , '',               $inputString);                      // main closing </ul>
      $inputString = str_replace('&lt;/ul&gt;<text:line-break/><text:line-break/>' , '',               $inputString);                      //  sub closing </ul>'s 
      
      return $inputString;
    }
  }
?>