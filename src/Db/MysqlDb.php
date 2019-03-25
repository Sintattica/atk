<?php
namespace Sintattica\Atk\Db;

/**
 * MySQL/PDO driver for ATK
 *
 * @author Samuel Bf
 */
class MysqlDb extends Db
{
    public $m_type = 'mysql';    


    /**
     * Returns some custom error strings on error and sets ANSI mode for MySQL
     *
     * ANSI mode replaces backticks with double-quotes for identifier escaping and allows us to use ||
     * to concatenate strings. For more information, see :
     * https://dev.mysql.com/doc/refman/8.0/en/sql-mode.html#sql-mode-important
     *
     * See http://php.net/manual/fr/pdo.construct.php for documentation on parameters
     *
     * @param string $dsn       Data source name, starting with "mysql:" (or unexpected things can happen ...)
     * @param string $username  username
     * @param string $passwd    password
     * @param string $options   some specific options
     */
    public function __construct($dsn, $username = '', $passwd = '', $options = [])
    {
        $options[\PDO::MYSQL_ATTR_INIT_COMMAND] = $options[\PDO::MYSQL_ATTR_INIT_COMMAND] ?? 'SET sql_mode="ANSI"';
        try {
            parent::__construct($dsn, $username, $passwd, $options);
        } catch(\PDOException $e) {
            switch($e->errorInfo[2]) {
                case 1044:
                    $this->halt(sprintf(Tools::atktext('db_access_denied_database', 'atk'), $dbconfig[$conn]['user'], $dsn));
                    break;
                case 1045:
                    $this->halt(sprintf(Tools::atktext('db_access_denied_user', 'atk'), $dbconfig[$conn]['user'], $dsn));
                    break;
                case 1049:
                    $this->halt($errMsg = sprintf(Tools::atktext('db_unknown_database', 'atk'), $dsn));
                    break;
                case 2004:
                case 2005:
                    $this->halt(sprintf(Tools::atktext('db_unknown_host', 'atk'), $dsn));
                    break;
                default:
                    throw($e);
                    break;
            }
        }
    }

    /**
     * MySQL-specific metadata fetched from information_schema.columns
     *
     * @doc inherit
     */
    public function metadata($table, $extraCols = [])
    {
        $meta = parent::metadata($table, ['column_key', 'extra']);
        foreach ($meta as $name => $informations) {
            if ($informations['column_key'] == 'PRI') {
                $meta[$name]['flags'] |= self::MF_PRIMARY | self::MF_UNIQUE;
            } elseif ($informations['column_key'] == 'UNI') {
                $meta[$name]['flags'] |= self::MF_UNIQUE;
            }
            if (strpos($informations['extra'], 'auto_increment') !== false) {
                $meta[$name]['flags'] |= self::MF_AUTO_INCREMENT;
            }
        }
        return $meta;
    }

    /**
     * MySQL has a concat_ws function :
     */
    public function func_concat_ws(array $fields, string $separator, bool $remove_all_spaces = false)
    {
        if (count($fields) == 0 or !is_array($fields)) {
            return '';
        } elseif (count($fields) == 1) {
            return $fields[0];
        }

        if ($remove_all_spaces) {
            return "REPLACE(CONCAT_WS('$separator', ".implode(',', $fields)."), ' ', '')";
        } else {
            return "CONCAT_WS('$separator', ".implode(',', $fields).')';
        }
    }
}
