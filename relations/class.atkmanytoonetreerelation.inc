<?php
  /**
   * A N:1 relation between two classes.
   * For example, projects all have one coordinator, but one
   * coordinator can have multiple projects. So in the project
   * class, there's a ManyToOneRelation to a coordinator.
   *
   * @author Ivo Jansch <ivo@achievo.com>
   * @version 0.8
   *
   * <b>Changed</b>
   *
   * 0.8 :   - Fixed the 0.7 fix, it didn't work for PHP3. (Ivo)
   * 0.7 :   - Fix a bug when atkManyToOneRelation was used as a primary key. (Ivo)
   *
   * $Id$
   * $Log$
   * Revision 4.1  2001/05/01 12:32:54  sandy
   * removed the debug functions and created a new Tree relations
   *
   * Revision 4.1  2001/04/23 13:21:22  peter
   * Introduction of module support. An ATK application can now have zero
   * or more modules which can, but don't have to, contain ATK nodes.
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:13  ivo
   * Added atk4 to repository.
   *
   * Revision 1.3  2001/04/04 09:01:49  ivo
   * AF_READONLY now also affects add forms.
   * Implemented hide() method for manytoonerelation.
   *
   * Revision 1.2  2001/02/22 22:47:16  peter
   * changed search/edit API, some clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:58:00  sandy
   * Achievo Tool Kit
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
      $this->atkRelation($name, $destination, $flags);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Record
     * @return Piece of html code that can  be used in a form to edit this
     */
    function edit($record="")
    {
      global $m_current;
      $this->createDestination();
      $tmp1=atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey);
      $tmp2=atk_array_merge($tmp1,array($this->m_destInstance->m_parent));
      $this->m_destInstance->selectDb("","cat_id","","",$tmp2);
      $current = createNode($this->m_destination); 
      $current->m_records[0] = $record[$this->fieldName()];
      
      $m_current = $current->primaryKey();
      $result = '<select name="'.$this->formName().'">';

      if ($this->hasFlag(AF_OBLIGATORY)==false)
      {
        // Relation may be empty, so we must provide an empty selectable..
        $result.= '<option value="0">'.text('select_none');
      }
      $result.=$this->createdd();
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
      $this->m_destInstance->selectDb("","","","",atk_array_merge($this->m_destInstance->descriptorFields(),$this->m_destInstance->m_primaryKey));

      $result = '<select name="atksearch['.$this->fieldName().']">';

      $pkfield = $this->m_destInstance->primaryKeyField();

      $result.= '<option value="">'.text('search_all');
      $result.=$this->createdd();
      $result.='</select>';
      return $result;
    }

    function createdd()
    {
      $t = new tree;
      for($i=0;$i<count($this->m_destInstance->m_records);$i++)
      {
        $group=$this->m_destInstance->m_records[$i];
        $t->addNode($this->m_destInstance->m_records[$i][$this->m_destInstance->m_primaryKey[0]],$this->m_destInstance->descriptor($group),$this->m_destInstance->m_records[$i][$this->m_destInstance->m_parent][$this->m_destInstance->m_primaryKey[0]]);
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
        //echo "$res<br>\n";
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