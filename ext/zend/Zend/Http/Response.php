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
 * @category   Zend
 * @package    Zend_Http
 * @subpackage Client
 * @copyright  Copyright (c) 2006 Zend Technologies USA Inc. (http://www.zend.com)
 * @license    http://framework.zend.com/license/new-bsd     New BSD License
 */
class Zend_Http_Response
{
	/**
     * HTTP Response Code (i.e. 404 or 200)
     *
     * @var null|int
     */
    protected $_code;

    /**
     * Response Headers
     *
     * @var array
     */
    protected $_headers;

    /**
     * Response body
     *
     * @var null|string
     */
    protected $_body;


	/**
	 * Class Constructor, read and parse HTTP response
	 *
	 * @param null
	 */
	public function __construct($code=null, $headers=array(), $body=null)
	{
		$this->_code    = $code;
		$this->_headers = $headers;
		$this->_body    = $body;
	}


	/**
	 * Check if Response is an error
	 *
	 * @return boolean
	 */
	public function isError()
	{
		// Check if response is one of the 400 or 500 error codes
		return substr($this->_code, 0, 1) == 4 || substr($this->_code, 0, 1) == 5;
	}


	/**
	 * Check if Response is successful
	 *
	 * @return bool
	 */
	public function isSuccessful()
	{
		return substr($this->_code, 0, 1) == 2;
	}


	/**
	 * Check if Response is a redirect
	 *
	 * @return boolean
	 */
	public function isRedirect()
	{
		return substr($this->_code, 0, 1) == 3;
	}


	/**
	 * Get Response Body
	 *
	 * @return string
	 */
	public function getBody()
	{
		return $this->_body;
	}


	/**
	 * Return Response Status
	 *
	 * @return int
	 */
	public function getStatus()
	{
		return $this->_code;
	}


	/**
	 * Return Response Headers
	 *
	 * @return array
	 */
	public function getHeaders()
	{
		return $this->_headers;
	}
}