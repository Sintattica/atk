<?php
/*
*Tree tools
*
* $Id$
* $Log$
* Revision 1.1  2001/04/23 10:17:07  ivo
* Initial revision
*
*/



class node 
  {
    var $m_id;
    var $m_label;
    var $m_sub = array();
    
    function node($id, $label)
    {
      $this->m_id = $id;
      $this->m_label = $label;
    }
  }
  
  class tree
  {                                    
    var $m_tree = array();                 
    var $m_allnodes = array();
    
    function addNode($id, $naam, $parent=0)
    {
      $n = new node($id, $naam);
      $this->m_allnodes[$id] = &$n;
      
      if ($parent==0)
      {
        $this->m_tree[] = &$n;
      }
      else
      {
        $tmp = &$this->m_allnodes[$parent];
        $tmp->m_sub[] = &$n;
      }      
    }
    
   
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
    
  }
  
?>