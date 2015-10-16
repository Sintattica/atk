<?php


use Sintattica\Atk\Security\SqlWhereclauseBlacklistChecker;

/**
 * Filter the atkselector REQUEST variable for blacklisted SQL (like UNIONs)
 */
SqlWhereclauseBlacklistChecker::filter_request_where_clause('atkselector');
SqlWhereclauseBlacklistChecker::filter_request_where_clause('atkfilter');

// initialise g_ array.
$GLOBALS['g_user'] = array();
