<?php

  // generic action listener. subclass to actually do something useful with it.
  class atkActionListener
  {
    var $m_actionfilter = 0;
    var $m_node = NULL;
    
    function atkActionListener($actionfilter=array())
    {
      $this->m_actionfilter = $actionfilter;
    }
    
    function setNode(&$node)
    {
      $this->m_node = $node;
    }
    
    function notify($action, $record)
    {
      if (count($this->m_actionfilter)==0 || atk_in_array($action, $this->m_actionfilter))
      {
        atkdebug("Action $action performed on ".$this->m_node->atkNodeType()." (".$this->m_node->primaryKey($record));
      }
      else 
      {
        atkdebug("Ignored action $action");
      }
    }
  }

?>