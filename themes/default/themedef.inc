<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage themes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /** Theme definition */

  $theme['customtpls'] = array('action.tpl','action_view.tpl','actionpage.tpl','box.tpl','editform.tpl','list.tpl','login.tpl','recordlist.tpl','searchform.tpl','statusbar.tpl','tabs.tpl','viewform.tpl');  
  $theme['customimgs'] = array('help.gif', 'required_field.gif', 'arrowup.gif','arrowdown.gif','empty.gif');
  $theme['customstyles'] = array('atkdateattribute.css','dhtml_dropdown.css','recordlist.css', 'style.css', 'tabs.css');
  
  $theme['recordlist_icons'] = array('copy.gif','default.gif','delete.gif','done.gif','edit.gif','export.gif','preview.gif','select.gif','view.gif');
  $theme['toolbar_icons'] = array('bold.gif','delimiter.gif','italic.gif','link.gif','underline.gif');
  
  $theme['highlight'] = '#eeeeee';
  $theme['select'] = '#cccccc';
  
  // Outlook style menu
  $theme['ol_menucolor']='#ffffff';
  $theme['ol_wincolor']='#eeeee0';
  $theme['ol_background']='#677EB1';
  
  $theme['doctype'] = '<?xml version="1.0" encoding="UTF-8"?>'.
                      '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"'.
                      ' "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">';
                      
  $theme['menu_delimiter'] = "<br>";
  $theme['menu_pos'] = "left";
  $theme['menu_layout'] = "plain";
  $theme['menu_align'] = "center";
  $theme['top_frame'] = 0;
  $theme['recordlist_orientation']  = "left";
  $theme['recordlist_vorientation'] = "middle";
  $theme['recordlist_icons'] = "true";
  
?>
