<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk   
   * @todo The atktreetools should be moved to the utils subpackage.
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Node class, represents a node in a tree.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk   
   */
  class node 
  {
    var $m_id;
    var $m_label; // DEPRECATED, use $m_object instead.
    var $m_object; 
    var $m_img;
    var $m_sub = array();
    
    function node($id, $object, $img="")
    {
      $this->m_id = $id;
      $this->m_object = $object;
      $this->m_label = &$this->m_object; // DEPRECATED, but available for backwardcompatibility.
      $this->m_img = $img;      
    }
  }
  
  /**
   * Tree class, used to build trees of nodes.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk   
   */
  class tree
  {                                    
    var $m_tree = array();                 
    var $m_allnodes = array();
    var $m_parentless = array(); // Array to keep stuff that can not yet be inserted into the array. 
    
    function addNode($id, $naam, $parent=0, $img="")
    {
      $n = new node($id, $naam, $img);
      $this->m_allnodes[$id] = &$n;
            
      if (array_key_exists($id,$this->m_parentless) && is_array($this->m_parentless[$id]))
      {
        // In the parentless array, there are children that belong to this new record.
        $n->m_sub = &$this->m_parentless[$id];
        unset($this->m_parentless[$id]);        
      }
      
      if (empty($parent))
      {
        $this->m_tree[] = &$n;
        }
      else
      {
        $tmp = &$this->m_allnodes[$parent];
        if (is_object($tmp))
        {
          $tmp->m_sub[] = &$n;
        }
        else
        {
          // Dangling thingee.
          $this->m_parentless[$parent][] = &$n;
        }
      }      
    }  
   
    /** 
     * Example render function. Implement your own.
     */
    function render($tree="", $level=0)
    {
      // First time: root tree..
      if ($tree == "") $tree = $this->m_tree;
      $res="";
      while (list($id,$objarr) = each($tree))
      {
        $res.= '<tr><td>'.str_repeat("-",(2*$level))." ".$objarr->m_label.'</td></tr>';
        if (count($objarr->m_sub)>0)
        {          
          $res.=$this->render($objarr->m_sub, $level+1);
        }              
      }
      return $res;  
    }
    
    /**
     * Pops tree's on the session
     */
    function sessionTree()
    {
      global $ATK_VARS;
      $postTree = $ATK_VARS["atktree"];
      $sessionTree = sessionLoad("atktree");
      if ($postTree !="" && $sessionTree != $postTree) 
      {
       sessionStore("atktree",$postTree); // new in the session
       $realTree = $postTree;
      }
      else
      {
       $realTree = $sessionTree; // use the last known tree
      }
      $ATK_VARS["atktree"] == $realTree; // postvars now should contain the last Knowtree
    }
  }
?>
