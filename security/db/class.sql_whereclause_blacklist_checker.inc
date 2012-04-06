<?php

/**
 * Filter a request variable, containing a WHERE clause, from the globals
 * if it is blacklisted.
 *
 * @param string $variable
 * @example filter_request_where_clause('atkselector')
 */
function filter_request_where_clause($variable)
{
  if (isset($_REQUEST[$variable]))
  {
    $values = (array)$_REQUEST[$variable];
    foreach ($values as $value)
    {
      $checker = new sql_whereclause_blacklist_checker($value);
      if (!$checker->isSafe())
      {
        atkhalt("Unsafe WHERE clause in REQUEST variable: ".$variable, 'critical');
      }
    }
  }
}

/**
 * A blacklist checker that blacklists certain SQL parts,
 * given that it always operates on an SQL where clause
 * as given by atkselector.
 *
 * Note that, like all blacklists, this is not a permanent solution.
 * Blacklists are losing battles, ATK should simply not pass SQL in the URL.
 *
 * However, as we improve ATK, for backwardscompatibility,
 * we blacklist what SQL we know to be evil in where clauses.
 */
class sql_whereclause_blacklist_checker
{
  /**
   * The WHERE clause to filter.
   *
   * @var string
   */
  private $_whereclause;

  /**
   * Blacklisted parts of SQL for where clause
   *
   * @var array
   */
  private $_disallowed=array('/*', ' --', '#', // Comment syntax
                             'ALTER ', 'DELETE FROM', 'SHOW ', 'DROP ', // DDL statements
                             'UNION ', 'UNION(', ';', // other
                             '0x3a', 'information_schema', 'row(1,1)', 'floor(rand(', // common patterns for Blind SQL injection
                            );

  /**
   * Create a new checker object for a given WHERE clause
   *
   * @param string $whereclause
   */
  public function __construct($whereclause)
  {
    $this->_whereclause = $whereclause;
  }

  /**
   * Is the given WHERE clause 'safe' (no blacklisted SQL in it)?
   *
   * Parse the WHERE clause character by character and look behind to find
   * blacklisted SQL.
   * Exception for when we're in 'quote' mode (entering a string).
   *
   * @return unknown
   */
  public function isSafe()
  {
    $single_quote_mode = false;
    $double_quote_mode = false;
    $clause_length = strlen($this->_whereclause);

    for ($i=0;$i<$clause_length;$i++)
    {
      /**
       * Check for quotes (single and double) and set a 'mode' flag
       * accordingly.
       */

      if ($this->_whereclause[$i]==="'" && $this->_whereclause[$i-1]!=="\\")
      {
        if (!$single_quote_mode) $single_quote_mode = true;
        else                     $single_quote_mode = false;
      }

      if ($this->_whereclause[$i]==="'" && $this->_whereclause[$i-1]!=="\\")
      {
        if (!$double_quote_mode) $double_quote_mode = true;
        else                     $double_quote_mode = false;
      }

      // No need to check for blacklisted SQL when we're
      // in 'string' mode
      if ($single_quote_mode || $double_quote_mode) continue;

      /**
       * Look back at the string we have and check for disallowed SQL.
       */
      foreach ($this->_disallowed as $disallowed)
      {
        $disallowed_length = strlen($disallowed);
        $test = substr($this->_whereclause, $i+1-$disallowed_length, $disallowed_length);
        if (strtolower($test)===strtolower($disallowed))
        {
          return false;
        }
      }
    }

    return true;
  }
}
