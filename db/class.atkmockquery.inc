<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage db
   *
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 4173 $
   * $Id$
   */

  /**
   * Base include
   */
  atkimport("atk.db.atkquery");

  /**
   * A mock query abstraction.
   *
   * This is a dummy query abstraction that can be used in testcases.
   *
   * @author Boy Baukema <boy@ibuildings.nl>
   * @package atk
   * @subpackage db
   */
  class atkMockQuery extends atkQuery
  {
  }
?>