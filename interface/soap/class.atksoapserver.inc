<?php

  class atkSoapServer implements atkServerInterface
  {
  	private $m_server = NULL;
  	
  	public function __construct()
  	{
  	  $this->m_server = new SoapServer(null,array("uri"=>"http://".$_SERVER['HTTP_HOST']."/atkdemo/"));
  	  $this->m_server->setObject($this);
  	}
  	
  	public function handleRequest($request)
  	{
  	   return "Hello Soap World";
  	   
  	}
  	
  	public function __call($method, $args)
  	{
  	  atkdebug("Function $method called with args: ".var_export($args, true));
  	}
  }