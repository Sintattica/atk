<?php
  /**
   * The atkToolbar displays a set of buttons that can be used
   * to manipulate text in textboxes (bold, italic, underline).
   *
   * This attribute only works in Internet Explorer 4 and up.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
   */
  
  // atkToolbar is based on dummyattribute.
  include_once($config_atkroot."atk/attributes/class.atkdummyattribute.inc");

  class atkToolbar extends atkDummyAttribute
  {
    function atkToolbar($name, $flags=0)  
    {      
      $this->atkAttribute($name,$flags|AF_HIDE_LIST|AF_BLANKLABEL);      
    }
    
    function edit($rec="", $fieldprefix="")
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
