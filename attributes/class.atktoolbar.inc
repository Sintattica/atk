<?php
  /**
   * The atkToolbar displays a set of buttons that can be used
   * to manipulate text in textboxes (bold, italic, underline).
   *
   * This attribute only works in Internet Explorer 4 and up.
   *
   * @author Ivo Jansch (ivo@achievo.com)
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
      global $g_layout, $config_atkroot, $g_theme;
      
      if($g_theme["ToolbarIcon_dir"]!=""&&is_dir($config_atkroot."themes/".$g_theme["Name"]."/".$g_theme["ToolbarIcon_dir"]))
      {
        $iconpath = $config_atkroot."themes/".$g_theme["Name"]."/".$g_theme["ToolbarIcon_dir"]."/";
      }
      else
      {
        $iconpath = $config_atkroot."atk/themes/default/toolbar_icons/";
      }

      $g_layout->register_script($config_atkroot."atk/javascript/newwindow.js");
      $g_layout->register_script($config_atkroot."atk/javascript/class.atktoolbar.js");      
      $res = '<a href="javascript:modifySelection(\'<b>\',\'</b>\');"><img src="'.$iconpath.'bold.gif" border="0"></a> ';
      $res.= '<a href="javascript:modifySelection(\'<i>\',\'</i>\');"><img src="'.$iconpath.'italic.gif" border="0"></a> ';
      $res.= '<a href="javascript:modifySelection(\'<u>\',\'</u>\');"><img src="'.$iconpath.'underline.gif" border="0"></a>';      
      return $res;
    }
  }
  
?>
