<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * Popup file used by the colorpicker attribute.
   *
   * @package atk
   * @subpackage popups
   * @access private
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal Includes etc.
   */
  include_once($config_atkroot."atk.inc");

  atksession("admin");
  atksecure();

  useattrib("atkcolorpickerattribute");

  // builds matrix

  $colHeight = "11"; // height of each color element
  $colWidth  = "11";   // width of each color element
  $formRef   = $_GET["field"];
  $matrix    = colorMatrix($colHeight,$colWidth,$formRef,0,$_GET["usercol"]);
  $prefix    = $config_atkroot."atk/images/";

  $layout  = "<form name='entryform'>";
  $layout .= "<table width='100%' border='0' cellpadding='1' cellspacing='0' style='border: 1px solid #000000;'>";
  $layout .= "<tr bgcolor='#FFFFFF'>";
  $layout .= " <td valign='top' align='left'>".$matrix[0]."</td>";
  $layout .= " <td valign='top' align='left'>".$matrix[1]."</td>";
  $layout .= " <td valign='top' align='left'>".$matrix[2]."</td>";
  $layout .= "</tr>";
  $layout .= "<tr bgcolor='#FFFFFF'>";
  $layout .= " <td valign='top' align='left'>".$matrix[3].$matrix[4]."</td>";
  $layout .= " <td valign='top' align='left'>".$matrix[5].$matrix[6]."<br></td>";
  $layout .= " <td valign='top' align='right' class='table'>";
  $layout .= "  &nbsp;".atktext("colorcode", "atk").": &nbsp;<input type='text' name='".$formRef."' size='7' maxlength='7' value='' style='font-family: verdana; font-size: 11px;'>&nbsp;";
  $layout .= " </td>";
  $layout .= "</tr>";
  $layout .= "<tr bgcolor='#FFFFFF'>";
  $layout .= " <td colspan='2' valign='top' align='left'>".$matrix[7]."</td>";
  $layout .= " <td valign='top' align='right'>";
  $layout .= " <input type='button' name='close' value='".atktext("select", "atk")."'  style='font-family: verdana; font-size: 11px;' onClick='remoteUpdate(\"".$formRef."\", \"".$prefix."\");'>&nbsp;";
  $layout .= " <input type='button' name='cancel' value='".atktext("cancel", "atk")."' style='font-family: verdana; font-size: 11px;' onClick='window.close();'>&nbsp;<br><br>";
  $layout .= " </td>";
  $layout .= "</tr>";
  $layout .= "</table>";
  $layout .= "</form>";

  //  Display's the picker in the current ATK style-template
  $page = &atknew("atk.ui.atkpage");
  $theme = &atkinstance("atk.ui.atktheme");
  $output = &atkOutput::getInstance();

  $page->register_style($theme->stylePath("style.css"));
  $page->register_script(atkconfig("atkroot")."atk/javascript/colorpicker.js");
  $page->addContent($layout);
  $output->output($page->render(atktext("colorpicker_selectcolor", "atk"), true));

  $output->outputFlush();

?>