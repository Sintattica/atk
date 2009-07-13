<?php

function filter_request_where_clause($variable)
{
  if (isset($_REQUEST[$variable]))
  {
    $checker = new sql_whereclause_blacklist_checker($_REQUEST[$variable]);
    if (!$checker->isSafe())
    {
      atkerror("Unsafe WHERE clause in REQUEST variable: ".$variable);
      $_REQUEST[$variable] = $_GET[$variable] = $_POST[$variable] = '';
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
 * we blacklist what SQL we know to be evil.
 */
class sql_whereclause_blacklist_checker
{
  private $_whereclause;
  private $_disallowed=array('/*', ' --', '#', // Comment syntax
                             'ALTER ', 'DELETE FROM', 'SHOW ', 'DROP ', // DDL statements
                             'UNION ', 'UNION(', ';'); // other

  public function __construct($whereclause)
  {
    $this->_whereclause = $whereclause;
  }

  public function isSafe()
  {
    $single_quote_mode = false;
    $double_quote_mode = false;
    $clause_length = strlen($this->_whereclause);
    for ($i=0;$i<$clause_length;$i++)
    {
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

      // We don't check when we're
      if ($single_quote_mode || $double_quote_mode) continue;

      foreach ($this->_disallowed as $disallowed)
      {
        $disallowed_length = strlen($disallowed);
        $test = substr($this->_whereclause, $i+1-$disallowed_length, $disallowed_length);
        if ($test===$disallowed)
        {
          return false;
        }
      }
    }
    return true;
  }
}