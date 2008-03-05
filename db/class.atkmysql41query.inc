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
  * @copyright (c)2000-2004 Ibuildings.nl BV
  * @license http://www.achievo.org/atk/licensing ATK Open Source License
  *
  *
  */

 /**
  * @internal Include baseclass
  */
 require_once(atkconfig("atkroot")."atk/db/class.atkmysqliquery.inc");

 /**
  * This class was moved to mysqli, because the functions were not mysql 4.1
  * specific but mysqli specific.
  *
  * @deprecated
  * @package atk
  * @subpackage db
  */
  class atkMysql41Query extends atkMysqliQuery
  {
  }
?>