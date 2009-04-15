<?php

 /**
  * This file is part of the Achievo ATK distribution.
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
  include_once(atkconfig("atkroot")."atk/interface/interface.atkserverinterface.inc");


  /**
   * The atkServer class is the base implementation of the ATK webservices
   * layer. It can be used to instantiate an atkServer object using any
   * of the supported protocols.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage interface
   */
  class atkServer
  {
  	private $m_protocol = "";
  	
  	/**
  	 * Get an instance of this class
  	 *
  	 * @return atkServer
  	 */
  	public function getInstance()
  	{
  	  static $s_instance = NULL;
  	  if ($s_instance==NULL)
  	  {
  	  	$s_instance = new atkServer();
  	  }
  	  return $s_instance;
  	}
  	
  	/**
  	 * Constructor
  	 *
  	 */
  	private function __construct()
  	{
  	  atkdebug("Created a new atkServer instance.");
  	}
  	
  	/**
  	 * Run the server
  	 *
  	 */
  	public function run()
  	{
  	  $output = atkinstance("atk.ui.atkoutput");
  	  $protocol = $this->getProtocol();
  	  if (!$this->isValidProtocol($protocol))
  	  {
  	  	$output->output("Server not active or invalid protocol");
  	  }
  	  else 
  	  {
  	  	$server = atknew("atk.interface.${protocol}.atk${protocol}server");
  	  	$output->output($server->handleRequest($_REQUEST));
  	  }
  	  $output->outputFlush();
  	}
  	
  	/**
  	 * Get protocol
  	 *
  	 * @return string
  	 */
  	public function getProtocol()
  	{
  	  return isset($this->m_protocol)&&$this->m_protocol!=""?$this->m_protocol:$this->getDefaultProtocol();
  	}
  	
  	/**
  	 * Set protocol
  	 *
  	 * @param string $protocol
  	 */
  	public function setProtocol($protocol)
  	{
  	  $this->m_protocol = $protocol;
  	}
  	
  	/**
  	 * Get default protocol
  	 *
  	 * @return string
  	 */
  	public function getDefaultProtocol()
  	{
  	  return (isset($_REQUEST["protocol"])?$_REQUEST["protocol"]:atkconfig("server_default_protocol"));
  	}
  	
  	/**
  	 * Is valid protocol?
  	 *
  	 * @param string $protocol
  	 * @return bool
  	 */
  	public function isValidProtocol($protocol)
  	{
  	  return in_array($protocol, array("soap"));
  	}
  }
