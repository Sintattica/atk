<?php
/*
*Tree tools
*
* $Id$
* $Log$
* Revision 4.2  2001/06/19 11:42:35  ivo
* New feature in treeview: leaf icon can now be userdefined.
*
* Revision 4.1  2001/05/01 12:32:54  sandy
* removed the debug functions and created a new Tree relations
*
* Revision 4.0  2001/04/23 10:19:38  ivo
* Revision number update
*
* Revision 1.1.1.1  2001/04/23 10:17:07  ivo
* Added atk4 to repository.
*
*/



  class node 
  {
    var $m_id;
    var $m_label;
    var $m_img;
    var $m_sub = array();
    
    function node($id, $label, $img="")
    {
      $this->m_id = $id;
      $this->m_label = $label;
      $this->m_img = $img;      
    }
  }
  
  class tree
  {                                    
    var $m_tree = array();                 
    var $m_allnodes = array();
    
    function addNode($id, $naam, $parent=0, $img="")
    {
      $n = new node($id, $naam, $img);
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