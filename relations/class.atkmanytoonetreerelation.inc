<?php
 /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage relations
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /** 
   * @internal include base class
   */
  userelation("atkManyToOneRelation");  
   
  /**
   * Extension of the atkManyToOneRelation, that is aware of the treestructure
   * (parent/child relation) in the destination node, and renders items in the
   * dropdown accordingly.
   *
   * @author Sandy Pleyte <sandy@ibuildings.nl>
   * @package atk
   * @subpackage relations
   *
   */    
  class atkManyToOneTreeRelation extends atkManyToOneRelation
  {
  
  
    var $m_current = ""; 
    var $m_level = "";

    /**
     * Constructor
     * @param $name ?
     * @param $destination ?
     * @param $flags Flags for the relation
     */
    function atkTreeManyToOneRelation($name, $destination, $flags=0)
    {
      $this->atkManyToOneRelation($name, $destination, $flags);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Record
     * @return Piece of html code that can  be used in a form to edit this
     */
    function edit($record="", $fieldprefix="")
    {
      global $m_current;
      $this->createDestination();
      $tmp1=atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey);
      $tmp2=atk_array_merge($tmp1,array($this->m_destInstance->m_parent));
      if ($this->m_destinationFilter!="")
      {
        $this->m_destInstance->addFilter(stringparse($this->m_destinationFilter,$record));
      }
      $recordset = $this->m_destInstance->selectDb("",$this->m_destInstance->m_primaryKey[0],"","",$tmp2);
            
      $m_current = $this->m_destInstance->primaryKey($record[$this->fieldName()]);
      $result = '<select name="'.$fieldprefix.$this->formName().'">';

      if ($this->hasFlag(AF_OBLIGATORY)==false)
      {
        // Relation may be empty, so we must provide an empty selectable..
        $result.= '<option value="0">'.text('select_none');
      }
      $result.=$this->createdd($recordset);
      $result.='</select>';
      return $result;
    }
    
    /**
     * Returns a piece of html code that can be used in a form to search
     * @param $record Record
     * @param $size Size of the editbox
     * @param $maxsize Maxsize of the editbox
     * @return Piece of html code that can  be used in a form to edit this
     */
    function search($record="")
    {
      $this->createDestination();
      if ($this->m_destinationFilter!="")
      {
        $this->m_destInstance->addFilter(stringparse($this->m_destinationFilter,$record));
      }
      $recordset = $this->m_destInstance->selectDb("","","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));

      $result = '<select name="atksearch['.$this->fieldName().']">';

      $pkfield = $this->m_destInstance->primaryKeyField();

      $result.= '<option value="">'.text('search_all');
      $result.=$this->createdd($recordset);
      $result.='</select>';
      return $result;
    }

    function createdd($recordset)
    {
      $t = new tree;
      for($i=0;$i<count($recordset);$i++)
      {
        $group=$recordset[$i];
        $t->addNode($recordset[$i][$this->m_destInstance->m_primaryKey[0]],
                    $this->m_destInstance->descriptor($group),
                    $recordset[$i][$this->m_destInstance->m_parent][$this->m_destInstance->m_primaryKey[0]]);
      }
      $tmp = $this->render($t->m_tree);
      return $tmp;
    }
    
    function render($tree="",$level=0)
    {
      global $m_current,$m_level;
      $res="";
      while (list($id,$objarr) = each($tree))
      {
       if($m_current == $this->m_destInstance->m_table.".".$this->m_destInstance->m_primaryKey[0]."='".$objarr->m_id."'") { $sel="SELECTED";$m_level=$level; } else { $sel=""; }
        $res.= '<option value="'.$this->m_destInstance->m_table.".".$this->m_destInstance->m_primaryKey[0]."='".$objarr->m_id."'".'" '.$sel.'>'.str_repeat("-",(2*$level))." ".$objarr->m_label;
        if (count($objarr->m_sub)>0 && $sel=="")
        {          
          $res.=$this->render($objarr->m_sub, $level+1);
        }              
      }
      $m_level=0;
      return $res;    
    }
  }
?>