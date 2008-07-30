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
   * @version $Revision$
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
    function _parse($tpl_file, $tpl_vars = null)
    {
      return parent::_parse($tpl_file, $tpl_vars, $content_file="word/document.xml");
    }
  }
?>
