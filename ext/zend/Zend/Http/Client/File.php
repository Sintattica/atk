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


/** Zend_Http_Client_Abstract */
require_once 'Zend/Http/Client/Abstract.php';


/**
 * HTTP client implementation that reads from files and fakes HTTP responses.
 * This may be useful for testing.
 *
 * @category   Zend
 * @package    Zend_Http
 * @subpackage Client
 * @copyright  Copyright (c) 2006 Zend Technologies USA Inc. (http://www.zend.com)
 * @license    http://framework.zend.com/license/new-bsd     New BSD License
 */
class Zend_Http_Client_File extends Zend_Http_Client_Abstract
{
    protected $_filename = '';

    /**
     * Class Constructor
     *
     * ZHttp_ClientFile file ignores URIs.  The setUri() method is simply ignored.
     * The filename to read may be set by setFilename().
     *
     * @param  null|string|Zend_Uri $filename
     * @param  array $headers
     * @return void
     */
    public function __construct($uri = null, $headers = array())
    {
        if ($uri !== null) {
            $this->setUri($uri);
        }

    	if ($headers !== array()) {
    		$this->setHeaders($headers);
    	}
    }


    /**
     * Sets the filename to read for get() response.
     *
     * @param string $filename
     * @return void
     */
    public function setFilename($filename)
    {
        if (!is_string($filename)) {
            throw new Zend_Http_Client_Exception('Filename must be a string');
        }

        $this->_filename = $filename;
    }


   /**
     * Send a GET HTTP Request
     *
     * @return Zend_Http_Response
     */
    public function get()
    {
        // if the filename was never set or set to '', fake a code 400
        if (empty($this->_filename)) {
            return new Zend_Http_Response(400, array(), '');
        }

        $file = @file_get_contents($this->_filename);
        if ($file === false) {
            throw new Zend_Http_Client_Exception("Failed reading file \"{$this->_filename}\"");
        }

        return new Zend_Http_Response(200, array(), $file);
    }


    /**
     * Send a POST HTTP Request
     *
     * @param string $data Data to send in the request
     * @return Zend_Http_Response
     */
    public function post($data)
    {
        $request = array('POST <uri> HTTP/1.0',
                         'Host: <uri>',
                         'Content-length: ' . strlen($data),
                         'Accept: */*');

        echo( get_class($this)
              . " does not support PUT. Would issue the following request:\n\n"
              . implode("\n", $request) . "\n\n"
              . $data . "\n" );

        return new Zend_Http_Response(201, array(), '');
    }


    /**
     * Send a PUT HTTP Request
     *
     * @param string $data Data to send in the request
     * @return Zend_Http_Response
     */
    public function put($data)
    {
        $request = array('PUT ' . $this->_uri . ' HTTP/1.0',
                         'Host: ' . $this->_uri,
                         'Content-length: ' . strlen($data),
                         'Accept: */*');

        echo( get_class($this)
              . " does not support PUT. Would issue the following request:\n\n"
              . implode("\n", $request) . "\n\n"
              . $data . "\n" );

        return new Zend_Http_Response(200, array(), '');
    }


    /**
     * Send a DELETE HTTP Request
     *
     * @return Zend_Http_Response
     */
    public function delete()
    {
        $request = array('DELETE ' . $this->_uri . ' HTTP/1.0',
                         'Host: ' . $this->_uri);

        echo( get_class($this)
              . " does not support DELETE. Would issue the following request:\n\n"
              . implode("\n", $request) . "\n" );

        return new Zend_Http_Response(204, array(), '');
    }

}


