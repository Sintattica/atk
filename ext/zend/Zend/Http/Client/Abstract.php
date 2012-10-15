<?php
/**
 * Zend Framework
 *
 * LICENSE
 *
 * This source file is subject to the new BSD license that is bundled
 * with this package in the file LICENSE.txt.
 * It is also available through the world-wide-web at this URL:
 * http://framework.zend.com/license/new-bsd
 * If you did not receive a copy of the license and are unable to
 * obtain it through the world-wide-web, please send an email
 * to license@zend.com so we can send you a copy immediately.
 *
 * @category   Zend
 * @package    Zend_Http
 * @subpackage Client
 * @copyright  Copyright (c) 2006 Zend Technologies USA Inc. (http://www.zend.com)
 * @license    http://framework.zend.com/license/new-bsd     New BSD License
 */


/** Zend_Http_Client_Exception */
require_once 'Zend/Http/Client/Exception.php';

/** Zend_Http_Response */
require_once 'Zend/Http/Response.php';

/** Zend_Uri */
require_once 'Zend/Uri.php';


/**
 * @category   Zend
 * @package    Zend_Http
 * @subpackage Client
 * @copyright  Copyright (c) 2006 Zend Technologies USA Inc. (http://www.zend.com)
 * @license    http://framework.zend.com/license/new-bsd     New BSD License
 */
abstract class Zend_Http_Client_Abstract
{
    /**
     * Socket Connection Timeout
     *
     * @var int Time in Seconds
     */
    protected $_timeout = 10;

    /**
     * The Zend_Uri for the URI we are accessing.
     *
     * @var Zend_Uri
     */
    protected $_uri = null;

    /**
     * Additional HTTP headers to send.
     *
     * @var array
     */
    protected $_headers = array();


    /**
     * Validates that $headers is an array of strings, where each string
     * is formed like "Field: value".  An exception is thrown on failure.
     * An empty $headers array is valid and will not throw an exception.
     *
     * @param array $headers
     * @throws Zend_Http_Client_Exception
     * @return void
     */
    final static public function validateHeaders($headers = array()) {
        // Validate headers
        if (!is_array($headers)) {
            throw new Zend_Http_Client_Exception('Headers must be supplied as an array');
        } else {
            foreach ($headers as $header) {
                if (!is_string($header)) {
                    throw new Zend_Http_Client_Exception('Illegal header supplied; header must be a string');
                } else if (!strpos($header, ': ')) {
                	/**
                	 * @todo should protect against injections by making sure one and only one header is here
                	 */
                    throw new Zend_Http_Client_Exception('Bad header.  Headers must be formatted as "Field: value"');
                }
            }
        }
    }


    /**
     * Class Constructor, create and validate Zend_Uri object
     *
     * @param  string|Zend_Uri|null $uri
     * @param  array $headers
     * @return void
     */
    abstract public function __construct($uri = null, $headers = array());


    /**
     * Sets the URI of the remote site.  Setting a new URI will automatically
     * clear the response properties.
     *
     * @param string|Zend_Uri $uri
     * @return void
     */
    final public function setUri($uri) {
        // Accept a Zend_Uri object or decompose a URI string into a Zend_Uri.
        if ($uri instanceof Zend_Uri) {
            $this->_uri = $uri;
        } else {
            // $uri string will be validated automatically by Zend_Uri.
            $this->_uri = Zend_Uri::factory($uri);
        }

        // Explicitly set the port if it's not already.
        if (!$this->_uri->getPort() && $this->_uri->getScheme() == 'https') {
            $this->_uri->setPort(443);
        } else if (!$this->_uri->getPort()) {
            $this->_uri->setPort(80);
        }
    }


    /**
     * Get the Zend_Uri for this URI.
     *
     * @throws Zend_Http_Client_Exception
     * @return Zend_Uri
     */
    final public function getUri() {
        if (!$this->_uri instanceof Zend_Uri) {
            throw new Zend_Http_Client_Exception('URI was never set with setUri()');
        }
        return $this->_uri;
    }


    /**
     * Set the $headers to send.  Headers are supplied as an array of strings,
     * where each string is a header formatted like "Field: value".
     *
     * @param array $headers
     * @return void
     */
    final public function setHeaders($headers=array()) {
        self::validateHeaders($headers);
        $this->_headers = $headers;
    }


    /**
     * Set Connection Timeout
     *
     * @param int $seconds Timeout in seconds
     * @return void
     */
    final public function setTimeout($seconds)
    {
        if (ctype_digit((string) $seconds)) {
            $this->_timeout = $seconds;
        } else {
            throw new Zend_Http_Client_Exception("Invalid Timeout. The timeout should be a numerical value in seconds");
        }
    }


    /**
     * Send a GET HTTP Request
     *
     * @return Zend_Http_Response
     */
    abstract public function get();


    /**
     * Send a POST HTTP Request
     *
     * @param string $data Data to send in the request
     * @return Zend_Http_Response
     */
    abstract public function post($data);


    /**
     * Send a PUT HTTP Request
     *
     * @param string $data Data to send in the request
     * @return Zend_Http_Response
     */
    abstract public function put($data);


    /**
     * Send a DELETE HTTP Request
     *
     * @return Zend_Http_Response
     */
    abstract public function delete();
}

