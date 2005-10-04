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
      print "<i>".date('Y-m-d H:i:s')."</i>";
      print "<span class=\"pass\">[PASS]</span> ";
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);
      print implode("-&gt;", $breadcrumb);
      print "-&gt;$message<br />\n";
    }

     function paintFail($message)
    {
      SimpleReporter::paintFail($message);
      print "<i>".date('Y-m-d H:i:s')."</i>";
      print "<b><span class=\"fail\">[FAIL]</span> ";
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);
      print implode("-&gt;", $breadcrumb);
      print "-&gt;$message</b><br />\n";
    }


    function _getCss()
    {
      return '.fail {color: red; font-family: courier; } .pass { color: green; font-family: courier;}';
    }
  }

?>
