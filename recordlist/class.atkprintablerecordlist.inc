<?php

  atkimport("atk.recordlist.atkrecordlist");
  
  class atkPrintableRecordList extends atkRecordlist 
  {
    
    /**
     * Creates printableRecordlist
     * @param $suppresslist
     * obsolete by specialRecordList
     */
    function render(&$node, $recordset, $suppressList="", $flags=0)
    {
      $this->setNode($node);
      $this->m_flags = $flags;
      
      $output='<table border="0" cellspacing="0" cellpadding="4">';

      $output.="<tr>";

      // stuff for the totals row..
      $totalisable = false;
      $totals = array();

      // display a headerrow with titles.
      // Since we are looping the attriblist anyway, we also check if there
      // are totalisable collumns.
      foreach (array_keys($this->m_node->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_node->m_attribList[$attribname];
        $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
        if (
            ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
            &&
            (
              ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
              ||($this->m_node->m_action!="select")
            )
            &&$musthide==false
           )
        {
          $output.='<td><b>'.text($p_attrib->fieldName(),$this->m_node->m_type).'</b></td>';

          // the totalisable check..
          if ($p_attrib->hasFlag(AF_TOTAL))
          {
            $totalisable = true;
          }

        }
      }

      $output.="</tr>";

      for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
      {
        $output.='<tr>';
        foreach (array_keys($this->m_node->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_node->m_attribList[$attribname];
          $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
          if (
              ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
              &&
              (
                ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
                ||($this->m_node->m_action!="select")
              )
              &&$musthide==false
             )
          {
            // An <attributename>_display function may be provided in a derived
            // class to display an attribute.
            $funcname = $p_attrib->m_name."_display";

            if (method_exists($this->m_node,$funcname))
            {
              $value=$this->m_node->$funcname($recordset[$i], "list");
            }
            else
            {
              // otherwise, the display function of the particular attribute
              // is called.
              $value=$p_attrib->display($recordset[$i], "list");
            }
            $output.='<td>'.($value==""?"&nbsp;":$value).'</td>';

            // Calculate totals..
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $totals[$attribname] = $p_attrib->sum($totals[$attribname], $recordset[$i]);
            }
          }
        }

        $output.='</tr>';
      }

      // totalrow..
      if ($totalisable)
      {
        $totalRow = '<tr>';

        // Third loop.. this time for the totals row.
        foreach (array_keys($this->m_node->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_node->m_attribList[$attribname];
         $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
          if (
              ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
              &&
              (
                ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
                ||($this->m_node->m_action!="select")
              )
              &&$musthide==false
             )
          {
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $totalRow.='<td><b>'.$p_attrib->display($totals[$attribname], "list").'</b></td>';
            }
            else
            {
              $totalRow.='<td>&nbsp;</td>';
            }
          }
        }

        $totalRow.="</tr>";

        $output.=$totalRow;
      }

      $output.='</table>';

      return $output;

    }
  }

?>