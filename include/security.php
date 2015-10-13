<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage include
 *
 * @copyright (c)2005 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 5473 $
 * $Id$
 */
/**
 * Filter the atkselector REQUEST variable for blacklisted SQL (like UNIONs)
 */
require_once $GLOBALS['config_atkroot'] . 'atk/security/db/class.sql_whereclause_blacklist_checker.php';
filter_request_where_clause('atkselector');
filter_request_where_clause('atkfilter');

// initialise g_ array.
$GLOBALS['g_user'] = array();
