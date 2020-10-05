<?php
namespace Sintattica\Atk\Db;

use Sintattica\Atk\Core\Config;

/**
 * PostgreSQL driver for ATK
 *
 * @author Samuel Bf
 */
class PgsqlDb extends Db
{
    /**
     * Database type
     *
     * @var string
     */
    public $m_type = 'postgresql';

    /**
     * Intialize some default options and start the first transaction
     *
     * {@inheritDoc}
     */
    protected function init($connectionName, $driver, $connectionOptions = [])
    {
        parent::init($connectionName, $driver, $connectionOptions);
        $this->query('SET NAMES ' . $this->quote($connectionOptions['charset'] ?? 'UTF8'));
    }
    /**
     * Flags metadata fetched from information_schema.table_constraints for PostgreSQL
     *
     * @doc inherit
     */
    public function metadata($table, $extraCols = [])
    {
        $meta = parent::metadata($table, array_merge(['column_default'], $extraCols));

        // We can approximate serial fields (a.k.a autoincrement) with one where 'column_default' starts with
        // nextval(
        foreach ($meta as $column => $values) {
            if (substr($values['column_default'], 0, 8) == 'nextval(') {
                $meta[$column]['flags'] |= self::MF_AUTO_INCREMENT;
            }
        }

        // UNIQUE/PRIMARY KEY information resides in constraint_column_usage and table_constraints tables :
        $stmt = $this->prepare('SELECT column_name, ccu.constraint_name, constraint_type FROM '.
            'information_schema.constraint_column_usage AS ccu '.
            'LEFT JOIN information_schema.table_constraints AS tc ON ccu.constraint_name = tc.constraint_name '.
            'WHERE tc.table_name = :table');
        $stmt->execute([':table' => $table]);

        if (!$stmt) {
            Tools::atkdebug("Metadata query failed for supplementary flags for table {$table}.");
            return $meta;
        }

        while ($row = $stmt->fetch()) {
            switch ($row['constraint_type']) {
                case 'UNIQUE':
                    $meta[$row['column_name']]['flags'] |= self::MF_UNIQUE;
                    break;
                case 'PRIMARY KEY':
                    $meta[$row['column_name']]['flags'] |= self::MF_UNIQUE;
                    $meta[$row['column_name']]['flags'] |= self::MF_PRIMARY;
                    break;
            }
        }
        return $meta;
    }

    /**
     * Get the next sequence number using PostgreSQL sequences
     *
     * @param string $sequence the sequence name
     *
     * @return int the next sequence id
     */
    public function nextid($sequence)
    {
        $sequence = Config::getGlobal('database_sequenceprefix').$sequence.Config::getGlobal('database_sequencesuffix');

        /* get sequence number and increment */
        $stmt = $this->prepare('SELECT nextval(:sequence)');
        $stmt->execute([':sequence' => $sequence]);

        if (!$stmt and $this->errorCode() == '42P01') {
            // Creating the sequence when the error is 'this sequence does not exists'
            $this->query('CREATE SEQUENCE '.self::quoteIdentifier($sequence));
            $stmt = $this->prepare('SELECT nextval(:sequence)');
            $stmt->execute([':sequence' => $sequence]);
        }
        // Other errors : failing gracefully
        if (!$stmt){
            $this->halt("Failed to get nextid of sequence {$sequence} :".$this->getErrorMsg());
        }

        /* return id */
        return $stmt->fetch(\PDO::FETCH_NUM)[0];
    }

    /**
     * Regexp search condition for Postgresql
     *
     * @param string $fieldname The field which will be matched
     * @param string $value The regexp it will be matched against
     *
     * @return string Piece of SQL query
     */
    public function func_regexp($fieldname, $value)
    {
        $placeholder = QueryPart::placeholder($fieldname);
        $negate = ($value[0] == '!') ? '!':'';
        $suffix = $this->getForceCaseInsensitive() ? '*' : '';
        if ($value[0] == '!') {
            $value = substr($value, 1);
        }
        $parameter = [$placeholder => $value];
        $field = Db::quoteIdentifier($field);

        return new QueryPart("{$fieldname} {$negate}~{$suffix} {$placeholder}", $parameter);
    }

    /**
     * Postgresql has a concat_ws function that simplifies concat_coalesce.
     */
    public function func_concat_coalesce(array $fields) : string
    {
        if (empty($fields)) {
            return '';
        }
        return "CONCAT_WS('', ".implode(',', $fields).')';
    }
}
