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
     * array of ':parameter_name' => value
     * 
     * @access readonly
     * @var array of string => mixed
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
        $this->parameters = $parameters;
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
     * Concatenate 2 query parts ensuring there is no conflict without parameter names
     *
     * @param QueryPart $secondPart to append to the this one.
     *
     * @return QueryPart $this
     */
    public function concat(QueryPart $secondPart)
    {
        $sql2 = $secondPart->sql;
        // Taking care of parameters with the same name present in both QueryParts
        $parameterNames1 = array_keys($this->parameters);
        foreach ($secondPart->parameters as $name => $value) {
            $newName = $name;
            if (in_array($name, $parameterNames1)) {
                if (!isset($this->parameterCounter[$name])) {
                    $this->parameterCounter[$name] = 0;
                }
                $newName = $name . '_al' . ($this->parameterCounter[$name]++);
                $sql2 = str_replace($name, $newName, $sql2);
            }
            $this->parameters[$newName] = $value;
        }
        $this->sql .= $sql2;
        
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
            $query->concat(new QueryPart($glue, []));
            $query->concat($nextQuery);            
        }
        return $query;
    }
}
