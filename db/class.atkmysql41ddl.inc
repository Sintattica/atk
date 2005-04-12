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
  * @copyright (c)2005 Ibuildings.nl BV
  * @license http://www.achievo.org/atk/licensing ATK Open Source License
  *
  * @version $Revision$
  * $Id$
  *
  */


 /**
  * @internal Include baseclass
  */
 require_once(atkconfig("atkroot")."atk/db/class.atkmysqlddl.inc");

 /**
  * MySQL 4.1+ ddl driver.
  *
  * Implements mysql specific ddl statements.
  *
  * @author Eldad Ran <eldad@tele-concept.com>
  * @package atk
  * @subpackage db
  *
  */
 class atkMysql41DDL extends atkMysqlDDL
 {

 }
?>
