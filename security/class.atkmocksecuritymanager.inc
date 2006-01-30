<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage security
   *
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
  
  /** @internal include */
  atkimport("atk.security.atksecuritymanager");
  
  /**
   * The atkMockSecurityManager class is an atkSecurityManager mock 
   * object for testing purposes
   * 
   * The most important feature of the atkMockSecurityManager is the 
   * ability to influence the result of each function call.
   * 
   * @todo mock every function call. This can't be done nicely until
   * we feature PHP5. For now, we add mock methods on a per-need basis
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage security
   */
  class atkMockSecurityManager extends atkSecurityManager 
  {
    var $m_resultallowed = array();  
    
    function setAllowed($result, $nodeprivilege="all")
    {
      $this->m_resultallowed[$nodeprivilege] = $result;
    }
    
    function allowed($node, $privilege)
    {
      if (isset($this->m_resultallowed["all"])) return $this->m_resultallowed["all"];
      if (isset($this->m_resultallowed[$node.".".$privilege])) return $this->m_resultallowed[$node.".".$privilege];
      return parent::allowed($node, $privilege);
    }
  }
  
?>