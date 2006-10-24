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


/**
 * Zend_Http_Client_Abstract
 */
require_once 'Zend/Http/Client/Abstract.php';


/**
 * @category   Zend
 * @package    Zend_Http
 * @subpackage Client
 * @copyright  Copyright (c) 2006 Zend Technologies USA Inc. (http://www.zend.com)
 * @license    http://framework.zend.com/license/new-bsd     New BSD License
 */
class Zend_Http_Client extends Zend_Http_Client_Abstract
{
    /**
     * Class Constructor, create and validate Zend_Uri object
     *
     * @param  string|Zend_Uri|null $uri
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
     * Send a GET HTTP Request
     *
     * @param  int $redirectMax Maximum number of HTTP redirections followed
     * @return Zend_Http_Response
     */
    public function get($redirectMax = 5)
    {
        /**
         * @todo Implement ability to send Query Strings
         */

        // Follow HTTP redirections, up to $redirectMax of them
        for ($redirect = 0; $redirect <= $redirectMax; $redirect++) {

            // Build the HTTP request
            $hostHeader = $this->_uri->getHost() . ($this->_uri->getPort() == 80 ? '' : ':' . $this->_uri->getPort());
            $request = array_merge(array('GET ' . $this->_uri->getPath() . '?' . $this->_uri->getQuery() . ' HTTP/1.0',
                                         'Host: ' . $hostHeader,
                                         'Connection: close'),
                                   $this->_headers);

            // Open a TCP connection
            $socket = $this->_openConnection();

            // Make the HTTP request
            fwrite($socket, implode("\r\n", $request) . "\r\n\r\n");

            // Fetch the HTTP response
            $response = $this->_read($socket);

            // If the HTTP response was a redirect, and we are allowed to follow additional redirects
            if ($response->isRedirect() && $redirect < $redirectMax) {

                // Fetch the HTTP response headers
                $headers = $response->getHeaders();

                // Attempt to find the Location header
                foreach ($headers as $headerName => $headerValue) {
                    // If we have a Location header
                    if (strtolower($headerName) == "location") {
                        // Set the URI to the new value
                        if (Zend_Uri_Http::check($headerValue)) {
                        	// If we got a well formed absolute URI, set it
                        	$this->setUri($headerValue);
                        } else {
                        	// Split into path and query and set the query
                    	    list($headerValue, $query) = explode('?', $headerValue, 2);
                    	    $this->_uri->setQueryString($query);
                    	    
                        	if (strpos($headerValue, '/') === 0) {
                        		// If we got just an absolute path, set it
                          	    $this->_uri->setPath($headerValue);
                          	    
                        	} else {
                        	    // Else, assume we have a relative path
                        	    $path = dirname($this->_uri->getPath());
                        	    $path .= ($path == '/' ? $headerValue : "/{$headerValue}" );
                        	    $this->_uri->setPath($path);
                        	}
                        }
                        
                        // Continue with the new redirected request
                        continue 2;
                    }
                }
            }

            // No more looping for HTTP redirects
            break;
        }

        // Return the HTTP response
        return $response;
    }


    /**
     * Send a POST HTTP Request
     *
     * @param string $data Data to send in the request
     * @return Zend_Http_Response
     */
    public function post($data)
    {
        $socket = $this->_openConnection();

        $hostHeader = $this->_uri->getHost() . ($this->_uri->getPort() == 80 ? '' : ':' . $this->_uri->getPort());
        $request = array_merge(array('POST ' . $this->_uri->getPath() . ' HTTP/1.0',
                                     'Host: ' . $hostHeader,
                                     'Connection: close',
                                     'Content-length: ' . strlen($data)),
                               $this->_headers);

        fwrite($socket, implode("\r\n", $request) . "\r\n\r\n" . $data . "\r\n");

        return $this->_read($socket);
    }


    /**
     * Send a PUT HTTP Request
     *
     * @param string $data Data to send in the request
     * @return Zend_Http_Response
     */
    public function put($data)
    {
        $socket = $this->_openConnection();

        $hostHeader = $this->_uri->getHost() . ($this->_uri->getPort() == 80 ? '' : ':' . $this->_uri->getPort());
        $request = array_merge(array('PUT ' . $this->_uri->getPath() . ' HTTP/1.0',
                                     'Host: ' . $hostHeader,
                                     'Connection: close',
                                     'Content-length: ' . strlen($data)),
                               $this->_headers);

        fwrite($socket, implode("\r\n", $request) . "\r\n\r\n" . $data . "\r\n");

        return $this->_read($socket);
    }


    /**
     * Send a DELETE HTTP Request
     *
     * @return Zend_Http_Response
     */
    public function delete()
    {
        $socket = $this->_openConnection();

        $hostHeader = $this->_uri->getHost() . ($this->_uri->getPort() == 80 ? '' : ':' . $this->_uri->getPort());
        $request = array_merge(array('DELETE ' . $this->_uri->getPath() . ' HTTP/1.0',
                                     'Host: ' . $hostHeader,
                                     'Connection: close'),
                               $this->_headers);

        fwrite($socket, implode("\r\n", $request) . "\r\n\r\n");

        return $this->_read($socket);
    }


    /**
     * Open a TCP connection for our HTTP/SSL request.
     *
     * @throws Zend_Http_Client_Exception
     * @return resource Socket Resource
     */
    protected function _openConnection()
    {
    	if (!$this->_uri instanceof Zend_Uri) {
    		throw new Zend_Http_Client_Exception('URI must be set before performing remote operations');
    	}

        // If the URI should be accessed via SSL, prepend the Hostname with ssl://
        $host = ($this->_uri->getScheme() == 'https') ? 'ssl://' . $this->_uri->getHost() : $this->_uri->getHost();
        $socket = @fsockopen($host, $this->_uri->getPort(), $errno, $errstr, $this->_timeout);
        if (!$socket) {
            // Added more to the exception message, $errstr is not always populated and the message means nothing then.
            throw new Zend_Http_Client_Exception('Unable to Connect to ' . $this->_uri->getHost() . ': ' . $errstr .
                                                ' (Error Number: ' . $errno . ')');
        }
        return $socket;
    }


    /**
     * Read Data from the Socket
     *
     * @param Resource $socket Socket returned by {@see Zend_Http_Client::_openConnection()}
     * @return Zend_Http_Response
     */
    protected function _read($socket)
    {
    	$responseCode    = null;
    	$responseHeaders = array();
    	$responseBody    = null;

		$hdr = null;
        while (strlen($header = rtrim(fgets($socket, 8192)))) {
            if (preg_match('|HTTP/\d.\d (\d+) (\w+)|', $header, $matches)) {
                $responseCode = (int) $matches[1];
            } else if (preg_match('|^\s|', $header)) {
                if ($hdr !== null) {
	                $responseHeaders[$hdr] .= ' ' . trim($header);
                }
            } else {
                $pieces = explode(': ', $header, 2);
                $responseHeaders[$pieces[0]] = isset($pieces[1]) ? $pieces[1] : null;
            }
        }

        while (!feof($socket)) {
            $responseBody .= fgets($socket, 8192);
        }

        fclose($socket);

        return new Zend_Http_Response($responseCode, $responseHeaders, $responseBody);
    }
}


