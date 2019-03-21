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
     * \PDO::PARAM_ constant value (fallback to \PDO::PARAM_INT for int
     * values and \PDO::PARAM_STR for other values if unspecified)
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
     */
    public function __construct(string $sql, array $parameters = [])
    {
        $this->sql = $sql;
        $this->parameters = [];
        foreach($parameters as $name => $parameter) {
            if (isset($parameter[1])) {
                $type = $parameter[1];
            } else {
                $type = gettype($parameter[0])=='integer' ? \PDO::PARAM_INT:\PDO::PARAM_STR;
            }
            $this->parameters[$name] = [$parameter[0], $type];
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
                throw new \Exception( "Cannot find or access property {$name} of QueryPart." );
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
     * Like implode on strings, but for QueryParts
     *
     * @param string $glue to put between sql parts (no parameters in it, just plain string)
     * @param array $pieces of QueryParts
     *
     * @return QueryPart with all parameters and glue
     */
    public static function implode(string $glue, array $pieces) : QueryPart
    {
        $query = array_shift($pieces);
        if (!count($pieces)) {
            return $query;
        }
        while ($nextQuery = array_shift($pieces)) {
            $query->appendSql($glue);
            $query->append($nextQuery);
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
