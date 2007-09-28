<?php

  /**
   * @todo Replace this with Marks's interface importer.
   */
  include_once(atkconfig("atkroot")."atk/interface/interface.atkserverinterface.inc");

  class atkServer
  {
  	private $m_protocol = "";
  	
  	public function getInstance()
  	{
  	  static $s_instance = NULL;
  	  if ($s_instance==NULL)
  	  {
  	  	$s_instance = new atkServer();
  	  }
  	  return $s_instance;
  	}
  	
  	private function __construct()
  	{
  	  atkdebug("Created a new atkServer instance.");
  	}
  	
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
  	
  	public function getProtocol()
  	{
  	  return isset($this->m_protocol)&&$this->m_protocol!=""?$this->m_protocol:$this->getDefaultProtocol();
  	}
  	
  	public function setProtocol($protocol)
  	{
  	  $this->m_protocol = $protocol;
  	}
  	
  	public function getDefaultProtocol()
  	{
  	  return (isset($_REQUEST["protocol"])?$_REQUEST["protocol"]:atkconfig("server_default_protocol"));
  	}
  	
  	public function isValidProtocol($protocol)
  	{
  	  return in_array($protocol, array("soap"));
  	}
  }