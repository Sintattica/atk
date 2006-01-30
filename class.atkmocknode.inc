<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   *
   * @copyright (c)2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
  
  /** @internal include */
  atkimport("atk.atknode");
  
  /**
   * The atkMockNode class is an atkNode mock object for testing purposes
   * 
   * The most important feature of the atkMockNode is the ability to
   * influence the result of each function call.
   * If a functioncall has no fixed result, the standard atkNode
   * function is called.
   * 
   * @todo mock every function call. This can't be done nicely until
   * we feature PHP5. For now, we add mock methods on a per-need basis
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   */
  class atkMockNode extends atkNode 
  {
    /**
     * The list of results per function call.
     */
    var $m_results;
    
    /**
     * Set the result of a function call to a specific result.
     * @todo Support parameter filtering
     * @param String $function Name of the function for the result
     * @param mixed $result The result the function should return
     */
    function setResult($function, $result)
    {
      $this->m_results[$function] = $result;
    }

    function call($function, $params)
    {            
      if (isset($this->m_results[$function]))
      {
        return $this->m_results[$function];
      }
      else 
      {
        // this doesn't work, we can't call the parent
        //return call_user_func_array(array($this, $function), $params);
      }
    }
       
    
    // ===============================  MOCKED METHODS ===================================/
    
    function getTabs($action) 
    { 
      if (isset($this->m_results["getTabs"])) 
        return $this->m_results["getTabs"]; 
      return parent::getTabs($action);
    }
          
  }


?>