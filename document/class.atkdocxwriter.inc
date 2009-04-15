<?php
  /**
   * atkDocxWriter class file
   *
   * @package atk
   * @subpackage document
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   *
   * @version $Revision: 6320 $
   */

  /**
   * DocumentWriter framework class for writing .docx (MS Office) files.
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk
   * @subpackage document
   */
  class atkDocxWriter extends atkOpenDocumentWriter 
  {
    /**
     * Parse the given template file
     *
     * @param string $tpl_file Template file to parse
     * @param mixed $tpl_vars Array of template variables to merge into the 
     * 												template or null if you want to use the template 
     * 												vars set by calling Assign (which is default 
     * 												behaviour).
     * @return bool Indication if parsing was succesfull
     */
    function _parse($tpl_file, $tpl_vars = null)
    {
      return parent::_parse($tpl_file, $tpl_vars, $content_file="word/document.xml");
    }
  }
?>
