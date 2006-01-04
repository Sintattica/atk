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
      // Determin which template vars to use (use supplied vars if
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

      // Add additional headers to set proper content type, filename and filesize
      rename($this->m_tbsooo->GetPathnameDoc(), $outputfilename);

      // Remove old unremoved documents
      $this->m_tbsooo->ClearProcessDir();

      return true;
    }
  }

?>
