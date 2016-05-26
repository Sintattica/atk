<?php

namespace Sintattica\Atk\Errors;

/**
 * The atkErrorHandlerObject handles the creation of the error handlers and
 * serves as a base class for them as well.
 *
 * @author Mark Wittens
 */
abstract class ErrorHandlerBase
{
    protected $params = [];

    /**
     * Constructor. Params are used to pass handler specific data to the handlers.
     *
     * @param array $params
     */
    public function __construct($params)
    {
        if (!is_array($params)) {
            $params = array($params);
        }
        $this->params = $params;
    }

    /**
     * Returns an error handler by name, params are passed to the handler.
     *
     * @param string $handlerName
     * @param array $params
     *
     * @return mixed
     */
    public static function get($handlerName, $params)
    {
        $class = __NAMESPACE__.'\\'."$handlerName".'ErrorHandler';

        return new $class($params);
    }

    /**
     * Implement the handle() function in a derived class to add customized error handling.
     *
     * @param string $errorMessage
     * @param string $debugMessage
     */
    abstract public function handle($errorMessage, $debugMessage);
}
