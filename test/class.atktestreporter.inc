<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage test
   *
   * @copyright (c)2005 Ivo Jansch
   * @copyright (c)2005 Marcus Baker
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * This class is an extension to the default HtmlReporter of the SimpleTest
   * framework. The difference is that this reporter also displays succesful
   * passes. (Thanks to Marcus of http://www.lastcraft.com for the example
   * code).
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @author Marcus Baker <marcus@lastcraft.com>
   * @package atk
   * @subpackage utils
   *
   */
  class atkTestReporter extends HtmlReporter
  {
    function paintPass($message)
    {
      parent::paintPass($message);
      $passline = "<i>".date('Y-m-d H:i:s')."</i>";
      $passline.= "<span class=\"pass\">[PASS]</span> ";
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);
      $passline.= implode("-&gt;", $breadcrumb);
      $passline.= "-&gt;$message<br />\n";
      echo str_pad($passline, 4096);
      flush();
    }

    function paintFail($message)
    {
      SimpleReporter::paintFail($message);
      $failline = "<i>".date('Y-m-d H:i:s')."</i>";
      $failline.= "<b><span class=\"fail\">[FAIL]</span> ";
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);
      $failline.= implode("-&gt;", $breadcrumb);
      $failline.= "-&gt;$message</b><br />\n";
      echo str_pad($failline, 4096);
      flush();
    }

    function _getCss()
    {
      return '.fail {color: red; font-family: courier; } .pass { color: green; font-family: courier;}';
    }
  }

?>