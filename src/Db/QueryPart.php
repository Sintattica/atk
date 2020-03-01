<?php
namespace Sintattica\Atk\Db;

use Sintattica\Atk\Core\Tools;

/**
 * QueryParts holds bits of SQL queries with their parameters and types.
 *
 * It may also hold a full query which can be prepared and executed with 
 * Db->queryP() method
 *
 * @author Samuel BF
 */
class QueryPart
{
    /**
     * string containing the SQL expression
     *
     * @access readonly
     * @var string
     */
    protected $sql;

    /**
     * array of ':parameter_name' => [value, type] wher type is one of
     * \PDO::PARAM_ constant value
     * In fact, only PARAM_INT and PARAM_STR are used.
     *
     * @access readonly
     * @var array of string => [mixed $value, int $pdo_type]
     */
    protected $parameters = [];

    /**
     * array of int, used to alias parameters by appending numbers when
     * parameters are added several times in the same query
     *
     * @access protected
     * @var array of string => int
     */
    protected $parameterCounter = [];

    /**
     * Initialize variables and fill types if not specified
     *
     * Values are passed as an array $placeholder => $value.
     *
     * @param string $sql query part that can contain placeholders
     * @param array [string => mixed] $values indexed by strings (placeholders)
     *              that are present in $sql.
     */
    public function __construct(string $sql, array $values = [])
    {
        $this->sql = $sql;
        $this->parameters = [];
        foreach($values as $placeholder => $value) {
            $type = is_int($value) ? \PDO::PARAM_INT:\PDO::PARAM_STR;
            $this->parameters[$placeholder] = [$value, $type];
        }
    }

    /**
     * getter for $sql and $parameters
     */
    public function __get(string $name)
    {
        switch ($name) {
            case 'sql':
                return $this->sql;
            case 'parameters':
                return $this->parameters;
            default:
                throw new \Exception("Cannot find or access property {$name} of QueryPart.");
        }
    }

    /**
     * Append another query to current query, ensuring there is no conflict with parameter names
     *
     * @param QueryPart $secondPart to append to the this one.
     *
     * @return QueryPart $this
     */
    public function append(QueryPart $secondPart)
    {
        $sql2 = $secondPart->sql;
        // Taking care of parameters with the same name present in both QueryParts
        $parameterNames1 = array_keys($this->parameters);
        foreach ($secondPart->parameters as $name => $param) {
            $newName = $name;
            if (in_array($name, $parameterNames1)) {
                if (!isset($this->parameterCounter[$name])) {
                    $this->parameterCounter[$name] = 0;
                }
                $newName = $name . '_al' . ($this->parameterCounter[$name]++);
                $sql2 = str_replace($name, $newName, $sql2);
            }
            $this->parameters[$newName] = $param;
        }
        $this->sql .= ' '.$sql2;

        return $this;
    }

    /**
     * Append SQL string to current query (without parameters)
     *
     * @param string $sql to append to the this one.
     *
     * @return QueryPart $this
     */
    public function appendSql(string $sql)
    {
        $this->sql .= ' '.$sql;
        return $this;
    }

    /**
     * Replaces [___] in SQL expression by values from $values
     *
     * [___] are replaced by placeholders and values from $values are put
     * in $this->parameters.
     * The replacement is similar to StringParser : [xx.yy.zz] is replaced
     * by $values['xx']['yy']['zz'] value.
     *
     * If a [____] does not correspond to a value in $value, it is replaced
     * by the empty string ''.
     *
     * Warning : parameters do not work inside quotes.
     *
     * @param array $values to replace values with.
     * @param boolean $asParameters put values in Parameters (true) or
     *                              directly in $this->sql (false)
     */
    public function parse($values, $asParameters = true)
    {
        // Extracting [xxx] parts from Sql expression :
        $fields = [];
        preg_match_all("/\[([^\]]*)\]+/", $this->sql, $fields);
        $fields = $fields[1];

        foreach ($fields as $field) {
            $localValues = $values;
            $localField = $field;
            // Resolving '.' sections :
            for ($dotPos = strpos($localField, '.'); $dotPos !== false; $dotPos = strpos($localField, '.')) {
                $beforeDot = substr($localField, 0, $dotPos);
                if (!isset($localValues[$beforeDot])) {
                    break;
                }
                $localValues = $localValues[$beforeDot];
                $localField = substr($localField, $dotPos + 1);
            }

            $value = $localValues[$localField] ?? '';
            if (!$asParameters) {
                $this->sql = str_replace("[{$field}]", $value, $this->sql);
            } else {
                $placeholder = self::placeholder($field);
                if (in_array($placeholder, array_keys($this->parameters))) {
                    // In case we already have a parameter with the same name...
                    $placeholder .= '_al'.($this->parameterCounter[$placeholder]++);
                }

                $this->sql = str_replace("[{$field}]", $placeholder, $this->sql);
                $this->parameters[$placeholder] = [$value, \PDO::PARAM_STR];
            }
        }
    }

    /**
     * Like implode on strings, but for QueryParts
     *
     * @param string $glue to put between sql parts (no parameters in it, just plain string)
     * @param array $pieces of QueryParts
     * @param bool $wrap resulting SQL query into parenthesis if more than one part is present
     *                   (default false)
     *
     * @return QueryPart with all parameters and glue, null if no pieces given
     */
    public static function implode(string $glue, array $pieces, $wrap = false)
    {
        // Removing null pieces :
        $pieces = array_filter($pieces, function($x) { return !is_null($x); });
        if (empty($pieces)) {
            return null;
        }
        if (count($pieces) == 1) {
            return $pieces[0];
        }
        $query = new QueryPart('');
        if ($wrap) {
            $query->appendSql('(');
        }
        $query->append(array_shift($pieces));
        while ($nextQuery = array_shift($pieces)) {
            $query->appendSql($glue);
            $query->append($nextQuery);
        }
        if ($wrap) {
            $query->appendSql(')');
        }
        return $query;
    }

    /**
     * Returns a valid placeholder/parameter name
     *
     * It can only contain A-Za-z0-9_ characters and may start with a ':'.
     * Source: https://github.com/php/php-src/blob/master/ext/pdo/pdo_sql_parser.re#L47
     *
     * @param string $initial name of the placeholder
     *
     * @return string $placeholder with only accepted chars
     */
    public static function placeholder(string $name) : string
    {
        $newName = preg_replace('/[^A-Za-z0-9_]/', '_', $name);
        $cksum = '';
        // If we replaced some characters, then we append a checksum part to it
        // to avoid that placeholder('首页') and placeholder('典范') return the same name.
        if ($newName != $name) {
            $cksum = '_'.substr(md5($name), 0, 8);
        }
        return ':'.$newName.$cksum;
    }
}
