<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * @internal include base class
   */
  include_once($config_atkroot."atk/attributes/class.atkdummyattribute.inc");
   
  /**
   * The atkToolbar displays a set of buttons that can be used
   * to manipulate text in textboxes (bold, italic, underline).
   *
   * This attribute only works in Internet Explorer 4 and up.
   *
   * The attribute has no database interaction and does not correspond to a 
   * database field.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   *   
   */   
  class atkToolbar extends atkDummyAttribute
  {
    /** 
     * Default constructor.
     *
     * @param String $name Name of the attribute (unique within a node)
     * @param int $flags Flags for the attribute.
     */
    function atkToolbar($name, $flags=0)  
    {      
      $this->atkAttribute($name,$flags|AF_HIDE_LIST|AF_BLANKLABEL);      
    }
    
    /**
     * Returns a piece of html code that can be used to represent this
     * attribute in an HTML form.
     *
     * @param array $record The record that is currently being edited.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record="", $fieldprefix="")
    {
      global $config_atkroot, $g_theme;
      
      $theme = &atkTheme::getInstance();

      $page = &atkPage::getInstance();
      $page->register_script($config_atkroot."atk/javascript/newwindow.js");
      $page->register_script($config_atkroot."atk/javascript/class.atktoolbar.js");      
      $res  = '<a href="javascript:modifySelection(\'<b>\',\'</b>\');"><img src="'.$theme->iconPath("bold","toolbar").'" border="0" alt="Vet"></a> ';
      $res .= '<a href="javascript:modifySelection(\'<i>\',\'</i>\');"><img src="'.$theme->iconPath("italic","toolbar").'" border="0" alt="Schuin"></a> ';
      $res .= '<a href="javascript:modifySelection(\'<u>\',\'</u>\');"><img src="'.$theme->iconPath("underline","toolbar").'" border="0" alt="Onderstreept"></a>';      

      // TODO/FIXME:This is platform specific code and should not be here
      // I think is still needed for older platform version (M1, M2) 
      $res .= '&nbsp;<img src="'.$theme->iconPath("delimiter","toolbar").'" border="0">&nbsp;';
      $res .= '<a href="javascript:popupSelection(\'pagesel.php\',\'pagesel\');" onmouseover="selectie=document.selection.createRange();"><img src="'.$iconpath.'link.gif" border="0" alt="Link"></a>';    

      return $res;
    }
  }
  
?>