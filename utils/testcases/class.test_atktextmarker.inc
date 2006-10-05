<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage utils
   * @access private
   *
   * @copyright (c)2006 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Include the class we are about to test.
   */
  atkimport("atk.utils.atktextmarker");

   /**
   * Testcases for class atktextmarker.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   * @access private
   */
  class test_atkTextMarker extends atkTestCase
  {
    function testMarker()
    {
      $marker = new atkTextMarker();
      $marker->setText("Hello World");
      $position = $marker->getOriginalPosition(10);
      $this->assertEqual($position, 10, "with no manipulations, position should be unaffected");

      $marker->hide(2,2); // Hide 'll'
      $position = $marker->getOriginalPosition(8);
      $this->assertEqual($position, 10, "Position 8 is position 10 in the original text");
      $this->assertEqual($marker->getText(), "Heo World", "'ll' chars should be hidden");

      $marker->hide(2,2); // Hide 'o '
      $position = $marker->getOriginalPosition(6);
      $this->assertEqual($position, 10, "Position 6 is position 10 in the original text");
      $this->assertEqual($marker->getText(), "HeWorld", "'o ' chars should be hidden");
    }
  }

?>