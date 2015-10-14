<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage interface
 *
 * @copyright (c)2007-2008 Ivo Jansch
 * @copyright (c)2007-2008 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 6320 $
 */
/**
 * @todo Replace this with Marks's interface importer.
 */
include_once(Atk_Config::getGlobal("atkroot") . "atk/interface/interface.atkserverinterface.php");

/**
 * The SOAP implementation for the ATK webservices layer.
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage interface
 */
class Atk_SoapServer implements Atk_ServerInterface
{
    private $m_server = NULL;

    /**
     * Constructor
     *
     */
    public function __construct()
    {
        $this->m_server = new SoapServer(null, array("uri" => "http://" . $_SERVER['HTTP_HOST'] . "/atkdemo/"));
        $this->m_server->setObject($this);
    }

    /**
     * Handle request
     *
     * @param string $request
     * @return String
     */
    public function handleRequest($request)
    {
        return "Hello Soap World";
    }

    /**
     * Call the soap function
     *
     * @param string $method
     * @param array $args
     */
    public function __call($method, $args)
    {
        Atk_Tools::atkdebug("Function $method called with args: " . var_export($args, true));
    }

}
