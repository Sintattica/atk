<?php

  class atkTotalizer 
  {
    var $m_node = NULL;
    var $m_columnConfig = NULL;
    
    function atkTotalizer(&$node, &$columnConfig)
    {
      $this->m_node = &$node;
      $this->m_columnConfig = &$columnConfig;
    }
    
    function totalize($rowset)
    {
      $result = array();
      $lastvalues = array();
      
      $totalizers = $this->m_columnConfig->totalizableColumns();
      $subtotalfields = $this->m_columnConfig->subtotalColumns();
      
      $totals = array();
      
      for($i=0, $_i=count($rowset); $i<$_i; $i++)
      {
        //atk_var_dump($rowset[$i], 'ditissem');        
        $record = $rowset[$i]["record"];
        for ($j=count($subtotalfields)-1; $j>=0; $j--) // reverse loop, to ensure right-to-left subtotalling
        {
          $fieldname = $subtotalfields[$j];
          $value = $record[$fieldname];
          //atk_var_dump($value);
          $p_subtotalling_attrib = &$this->m_node->m_attribList[$fieldname];
          
          if (isset($lastvalues[$fieldname]) && !$p_subtotalling_attrib->equal($record, $lastvalues))
          {
            // changed -> add subtotalrow
           //atkdebug("Changedetected");            
           //atk_var_dump($value, 'see what changed');
                        
            $result[] = $this->_subTotalRow($rowset[$i], $totals, $fieldname, $totalizers);            
          }
          
          // calculate walking subtotal.
//          atkdebug("Calculating walking subtotal");
          foreach($totalizers as $totalfield)
          {
            $p_attrib = &$this->m_node->m_attribList[$totalfield];
            $totals[$totalfield][$fieldname] = $p_attrib->sum($totals[$totalfield][$fieldname], $record);
          }          
          $lastvalues[$fieldname] = $value;                   
        }
                                
        $result[] = $rowset[$i];
                
      }
      
      // leftovers, subtotals of last rows            
      if (count($rowset))
      {
        $result[] = $this->_subTotalRow($rowset[count($rowset)-1], $totals, $fieldname, $totalizers);       
      }
      // end of leftovers
      
      return $result;
    }
    
    function _subTotalRow($row, &$totals, $fieldforsubtotal, $totalizers)
    {
      $subtotalcols = array();
      foreach($totalizers as $totalfield)
      {
        $p_attrib = &$this->m_node->m_attribList[$totalfield];
        $subtotalcols[$totalfield] = $p_attrib->display($totals[$totalfield][$fieldforsubtotal]);
        
        // reset walking total
        $totals[$totalfield][$fieldforsubtotal] = "";
      }           

      
      return $this->_createSubTotalRowFromRow($row, $fieldforsubtotal, $subtotalcols);           
    }
    
    function _createSubTotalRowFromRow($row, $fieldname, $subtotalcolumns)
    {      
      // fix type
      $row["type"] = "subtotal";
      
      // replace columns
      foreach($row["data"] as $col=>$value)
      {
        if ($col==$fieldname)
        {
          $row["data"][$col] = text("subtotal");
        }
        else if (isset($subtotalcolumns[$col]))
        {
          $row["data"][$col] = $subtotalcolumns[$col];
        }               
        else 
        {
          $row["data"][$col]="";
        }          
      }
      return $row;
    }
  }

?>