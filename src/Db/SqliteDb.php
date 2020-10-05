<?php
namespace Sintattica\Atk\Db;

use Sintattica\Atk\Core\Tools;

/**
 * PostgreSQL driver for ATK
 *
 * @author Samuel Bf
 */
class SqliteDb extends Db
{
    /**
     * Database type
     *
     * @var string
     */
    public $m_type = 'sqlite';

    /**
     * Retrieve the meta data of a certain table from the database
     *
     * see Db->metadata for documentation.
     *
     * @param string $table     the table name
     * @param array  $extraCols not used for Sqlite driver.
     *
     * @return array with meta data
     */
    protected function metadata($table, $extraCols = [])
    {
        Tools::atkdebug("Retrieving metadata for $table");

        $stmt = $this->query('PRAGMA table_info('.Db::quoteIdentifier($table).')');

        $result = [];
        while ($field = $stmt->fetch()) {
            $key = $field['name'];
            $result[$key] = [
                'table' => $table,
                'name' => $key,
                'flags' => 0,
                ];
            // Type, len and precision computation :
            // varchar(m) -> type=>varchar, len=>m
            // decimal(10,2) -> type=>decimal, len=>10, precision=>2
            $typeParts = explode('(', $field['type']);
            $result[$key]['type'] = $typeParts[0];
            $result[$key]['gentype'] = $this->getGenericType($typeParts[0]);
            if (!isset($typeParts[1])) {
                $result[$key]['len'] = null;
                $result[$key]['precision'] = null;
            } else {
                $comma = strpos(',', $typeParts[1]);
                if ($comma === false) {
                    $result[$key]['len'] = substr($typeParts[1], 0, strlen($typeParts[1])-1);
                    $result[$key]['precision'] = null;
                } else {
                    $detailsParts = explode(',', $typeParts[1]);
                    $result[$key]['len'] = $detailsParts[0];
                    $result[$key]['precision'] = substr($detailsParts[1], 0, strlen($detailsParts[1])-1);
                }
            }
            // Flags computation
            if ($field['notnull'] == 1) {
                $result[$key]['flags'] |= self::MF_NOT_NULL;
            }
            if ($field['pk'] == 1) {
                $result[$key]['flags'] |= self::MF_PRIMARY | self::MF_PRIMARY;
                if ($result[$key]['gentype'] == Db::FT_NUMBER) {
                    // « a column with type INTEGER PRIMARY KEY is an alias for the ROWID »
                    // (source : https://sqlite.org/autoinc.html)
                    $result[$key]['flags'] |= self::MF_AUTO_INCREMENT;
                }
            }
        }

        Tools::atkdebug("Metadata for $table complete");
        return $result;
    }

    /**
     * This function checks if the table exists in the database
     *
     * @param string $tableName the table to find
     *
     * @return bool true if found, false if not found
     */
    public function tableExists($tableName)
    {
        return $this->getValue('SELECT COUNT(*) FROM sqlite_master WHERE type = \'table\' AND name = :tableName', [':tableName' => $tableName]);
    }

    /**
     * Sqlite does not support regex searches natively.
     *
     * @return array with search modes
     */
    public function getSearchModes()
    {
        return array(
            'exact',
            'substring',
            'wildcard',
            'greaterthan',
            'greaterthanequal',
            'lessthan',
            'lessthanequal',
            'between',
        );
    }
}
