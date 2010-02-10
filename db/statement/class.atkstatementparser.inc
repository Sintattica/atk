<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage db.statement
 *
 * @copyright (c) 2009 Peter C. Verhage
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * Statement parser, used for supporting named bind parameters and supporting
 * bind parameters for database drivers with no native support.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 *
 * @package atk
 * @subpackage db.statement
 */
class atkStatementParser
{
  /**
   * Characters we consider as quotes.
   *
   * @var string
   */
  const QUOTE_CHARS = '"\'`';

  /**
   * (Original) SQL query.
   *
   * @var string
   */
  private $m_query;

  /**
   * Parsed SQL query.
   *
   * @var string
   */
  private $m_parsedQuery;

  /**
   * Bind parameter positions.
   *
   * @var array
   */
  private $m_bindPositions;


  /**
   * Constructor.
   *
   * @param string $query SQL query
   */
  public function __construct($query)
  {
    $this->m_query = $query;
    $this->_parse();
  }

  /**
   * Parses the SQL query. Replaced any named bind parameter by anonymous bind
   * parameters and saves the positions of the bind parameters in an array.
   */
  private function _parse()
  {
    $query = $this->m_query;
    $bindPositions = array();
    $anonBindParams = 0;

    $quoteChars = array_flip(str_split(self::QUOTE_CHARS));
    $quoteChar = null;

    for ($i = 0, $length = strlen($query); $i < $length; $i++)
    {
      $char = $query{$i};

      if (isset($quoteChars[$char]))
      {
        if ($quoteChar == null)
        {
          $quoteChar = $char;
        }
        else if ($quoteChar == $char)
        {
          $quoteChar = null;
        }
      }
      else if ($quoteChar == null && $char == '?')
      {
        $bindPositions[$i] = $anonBindParams++;
      }
      else if ($quoteChar == null && $char == ':')
      {
        if (preg_match('/^:(\w+)/', substr($query, $i), $matches))
        {
          $name = $matches[1];
          $bindPositions[$i] = $name;
          $query = substr($query, 0, $i).'?'.substr($query, $i + strlen($name) + 1);
          $length = strlen($query);
        }
      }
    }

    $this->m_parsedQuery = $query;
    $this->m_bindPositions = $bindPositions;
  }

  /**
   * Returns the original SQL query.
   *
   * @return string query
   */
  public function getQuery()
  {
    return $this->m_query;
  }

  /**
   * Returns the parsed SQL query, e.g. named bind parameters are replaced
   * by anonymous bind parameters.
   *
   * @return string parsed query
   */
  public function getParsedQuery()
  {
    return $this->m_parsedQuery;
  }

  /**
   * Returns the positions for the bind parameters in the query.
   *
   * The key of the array contains the character position, the value
   * contains the bind parameter name or offset.
   *
   * @return array
   */
  public function getBindPositions()
  {
    return $this->m_bindPositions;
  }
}