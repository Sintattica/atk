<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage ui
   *
   * @copyright (c)2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Implements the {atkstyle} plugin for use in templates.
   *
   * The atkstyle plugin registers a stylesheet in the current page.
   * Useful for templates that have an associated stylesheet that should
   * be loaded each time the template is included.
   *
   * Params:
   * file   The path of the stylesheet, relative to the running scripts
   *        directory.
   *
   * Example:
   * {atkstyle file="styles/default.css"}
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   */
  function smarty_function_atkstyle($params, &$smarty)
  {
    $page = &atkPage::getInstance();    
    $page->register_style($params["file"]);        
    return "";
  }
  
?>