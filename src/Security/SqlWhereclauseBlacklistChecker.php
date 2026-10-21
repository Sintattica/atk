<?php

namespace Sintattica\Atk\Security;

use Exception;
use Sintattica\Atk\Core\Tools;

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
class SqlWhereclauseBlacklistChecker
{
    /**
     * The WHERE clause to filter.
     *
     * @var string
     */
    private $_whereclause;

    /**
     * Blacklisted parts of SQL for where clause.
     *
     * @var array
     */
    private $_disallowed = array(
        '/*',
        ' --',
        '#', // Comment syntax
        'ALTER ',
        'DELETE FROM',
        'SHOW ',
        'DROP ', // DDL statements
        'UNION ',
        'UNION(',
        ';', // other
        '0x3a',
        'information_schema',
        'row(1,1)',
        'floor(rand(', // common patterns for Blind SQL injection
    );

    /**
     * Create a new checker object for a given WHERE clause.
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
     * @return bool
     */
    public function isSafe()
    {
        // Build the stream of characters that are OUTSIDE string literals, so a
        // legitimate quoted value cannot trigger a false positive and, conversely,
        // an attacker cannot hide structure inside quotes (they would have to close
        // the quote first, which lands back in this stream). Whitespace runs are
        // collapsed to a single space so tricks like UNION<TAB>SELECT or UNION/**/SELECT
        // cannot evade keyword matching.
        $code = $this->stripStringLiterals($this->_whereclause);
        $code = strtolower(preg_replace('/\s+/', ' ', $code));

        // A legitimate atkselector / atkfilter is only "field = value" comparisons
        // joined by AND/OR with grouping parentheses. None of the following ever
        // appears in that form, so their presence outside a string literal is an
        // injection attempt.
        $patterns = [
            '/;/',                                   // stacked queries
            '#(--|\#|/\*|\*/)#',                      // SQL comments
            '/\bunion\b/',                            // UNION [ALL] SELECT
            '/\bselect\b/',                           // sub-selects
            '/\binto\b/',                             // INTO OUTFILE/DUMPFILE
            '/\b(drop|alter|create|delete|insert|update|truncate|grant|revoke|rename|replace|load_file|outfile|dumpfile)\b/',
            // function call: an identifier (not a logical operator) directly before "(".
            // Grouping parentheses such as "OR (" are allowed because the word before
            // the "(" is a logical keyword.
            '/\b(?!(?:and|or|not|in|like|is|between|xor|div|mod|regexp|rlike)\b)[a-z_][a-z0-9_]*\s*\(/',
            '/0x[0-9a-f]/',                           // hex literals
            '/@@/',                                   // server system variables
            '/information_schema/',
        ];

        foreach ($patterns as $pattern) {
            if (preg_match($pattern, $code)) {
                return false;
            }
        }

        return true;
    }

    /**
     * Return the clause with the content of single/double quoted string literals
     * removed, so only the "code" outside strings remains for inspection. Handles
     * backslash escapes and doubled-quote ('') escapes.
     *
     * @param string $clause
     *
     * @return string
     */
    private function stripStringLiterals(string $clause): string
    {
        $out = '';
        $len = strlen($clause);
        $quote = null; // current string delimiter, or null when outside a string

        for ($i = 0; $i < $len; ++$i) {
            $ch = $clause[$i];

            if ($quote === null) {
                if ($ch === "'" || $ch === '"') {
                    $quote = $ch;
                    $out .= ' '; // placeholder so adjacent tokens stay separated
                } else {
                    $out .= $ch;
                }
                continue;
            }

            // inside a string literal
            if ($ch === '\\') {
                ++$i; // skip the escaped character
                continue;
            }
            if ($ch === $quote) {
                if ($i + 1 < $len && $clause[$i + 1] === $quote) {
                    ++$i; // doubled quote -> escaped, stay in string
                    continue;
                }
                $quote = null; // closing quote
            }
        }

        return $out;
    }

    /**
     * Filter a request variable, containing a WHERE clause, from the globals
     * if it is blacklisted.
     *
     * @param string $variable
     *
     * @throws Exception
     * @example filter_request_where_clause('atkselector')
     */
    public static function filter_request_where_clause($variable)
    {
        if (isset($_REQUEST[$variable])) {
            $values = (array)$_REQUEST[$variable];
            foreach ($values as $value) {
                $checker = new self($value);
                if (!$checker->isSafe()) {
                    Tools::atkhalt('Unsafe WHERE clause in REQUEST variable: '.$variable, 'critical');
                }
            }
        }
    }
}
