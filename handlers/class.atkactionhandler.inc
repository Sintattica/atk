<?php

  class atkActionHandler
  {
    var $m_node = NULL;
    var $m_action = "";
    var $m_postvars = array();
    
    function atkActionHandler()
    {      
    }
    
    /**
     * The handle() method handles the action.
     * The default implementation invokes an action_$action method and
     * stores the postvars. Custom handlers may override this.
     */
    function handle(&$node, $action, &$postvars)
    {      
      $this->m_postvars = &$postvars;
      $this->m_node = &$node;
      $this->m_action = $action;
      $this->invoke("action_".$action);
    }
    
    function setNode(&$node)
    {
      $this->m_node = &$node;
    }
    
    function &getPage()
    {
      return $this->m_node->getPage();
    }
    
    function &getUi()
    {          
      return $this->m_node->getUi();
    }
    
    function invoke($methodname)
    {            
      $arguments = func_get_args(); // Put arguments in a variable (php won't let us pass func_get_args() to other functions directly.      
      // the first argument is $methodname, which we already defined by name.      
      array_shift($arguments);
      
      if ($this->m_node!==NULL && method_exists($this->m_node, $methodname)) 
      {
        atkdebug("Invoking '$methodname' override on node");
        // We pass the original object as first parameter to the override. 
        array_unshift($arguments, $this);
        return call_user_func_array(array($this->m_node, $methodname), $arguments);
      }
      else if (method_exists($this, $methodname)) 
      {
        atkdebug("Invoking '$methodname' on actionhandler for action ".$this->m_action);
        return call_user_func_array(array($this, $methodname), $arguments);
      }
      atkerror("Undefined method '$methodname' in atkActionHandler");
    }
    
    /** 
     * Static factory method to create a default action handler for a certain action.
     */
    function &getDefaultHandler($action)
    {      
      // The next if statement checks for 'known' actions. All unknown actions
      // are handled the backwardscompatible default way (invoking action_$action on the node)
      $filename = atkconfig("atkroot")."atk/handlers/class.atk".$action."handler.inc";
      if (file_exists($filename))
      {
        return atknew("atk.handlers.atk".$action."handler");
      }
      else
      {
        // We don't have handlers yet for other actions.
        return new atkActionHandler(); // De default handler will automatically
                                       // invoke the node methods.
      }
    }
  }

?>