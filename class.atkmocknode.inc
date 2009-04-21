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

    /**
     * Call the function
     *
     * @param string $function
     * @param array $params
     * @return mixed The result
     */
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
    
    /**
     * Get a list of tabs for a certain action.
     * @param String $action The action for which you want to retrieve the
     *                       list of tabs.
     * @return array The list of tabnames.
     *
     */
    function getTabs($action) 
    { 
      if (isset($this->m_results["getTabs"])) 
        return $this->m_results["getTabs"]; 
      return parent::getTabs($action);
    }

    /**
     * Check if the user has the rights to access existing tabs and
     * removes tabs from the list that may not be accessed
     *
     * @param array $tablist Array containing the current tablist
     * @return array with disable tabs
     */
    function checkTabRights(&$tablist)
    { 
      if (isset($this->m_results["checkTabRights"]))
      {
        $tablist = $this->m_results["checkTabRights"];
      }
      else
      {
        parent::checkTabRights($tablist);
      }
    }
    
    /**
     * Retrieve records from the database.
     *
     * Note that if 'atksearch' is set in the request vars, the search
     * expressions are automatically added as extra where-clauses.
     *
     * @todo Handling atksearch in this method is dirty, this should be
     *       controlable with parameters.
     *
     * @param String $selector Sql expression used as a where-condition, to
     *                         retrieve only records that match the
     *                         expression.
     * @param String $order The order in which to retrieve the records.
     * @param array $limit Array containing an "offset" and a "limit" to
     *                     retrieve only part of the resultset. (Pass NULL or
     *                     an empty string to retrieve all records.)
     * @param array $excludeList List of attributes to be excluded from the
     *                           query. By default, the attributes that should
     *                           be loaded are determined by the $mode
     *                           parameter. Using $excludeList, you can
     *                           explicitly exclude attributes.
     * @param array $includeList List of attributes to include in the query,
     *                           regardless of the $mode parameter.
     * @param String $mode The action for which the selectDb is called.
     *                     This param is used to determine which attributes
     *                     to include in the query and which not to include.
     * @return array Array containing the retrieved records
     */
    function selectDb($selector="", $order="", $limit="", $excludeList="",$includeList="", $mode="")
    {
      if(isset($this->m_results["selectDb"]))
        return $this->m_results["selectDb"];
      return parent::selectDb($selector,$order,$limit,$excludelist,$includelist,$mode="");
    }
  }
?>