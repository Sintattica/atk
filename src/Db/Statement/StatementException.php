<?php namespace Sintattica\Atk\Db\Statement;

/**
 * Statement exception.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 *
 * @package atk
 * @subpackage db.statement
 */
class StatementException extends \Exception
{
    const MISSING_BIND_PARAMETER = 1;
    const NO_DATABASE_CONNECTION = 2;
    const PREPARE_STATEMENT_ERROR = 3;
    const STATEMENT_NOT_EXECUTED = 4;
    const STATEMENT_ERROR = 5;
    const OTHER_ERROR = 6;

    /**
     * @param string $message
     * @param int $code
     */
    public function __construct($message, $code)
    {
        parent::__construct($message, $code);
    }

}
