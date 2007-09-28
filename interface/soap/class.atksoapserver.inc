<?php

  class atkSoapServer implements atkServerInterface
  {
  	private $m_server = NULL;
  	
  	public function __construct()
  	{
  	  $this->m_server = new SoapServer();
  	  $this->m_server->setObject($this);
  	}
  	
  	public function handleRequest($request)
  	{
  	   echo "Hello Soap World";
  	   
  	}
  	
  	public function __call($method, $args)
  	{
  	  atkdebug("Function $method called with args: ".var_export($args, true));
  	}
  }