<?php

  class atkXmlHandler extends atkActionHandler
  {
    function action_xml()
    {      
      $recordset = $this->m_node->selectDb($this->m_postvars['atkselector'],"","","","","xml");
             
      for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
      {
        $this->invoke("xml", $recordset[$i]);
      }
    }
    
    /**
     * Parse xml tags
     */
    function xml($record)
    {
      $node = &$this->m_node;
      $xml = "<".$node->m_type." ";

      foreach (array_keys($node->m_attribList) as $attribname)
      {
        $p_attrib = &$node->m_attribList[$attribname];
        if ($record[$p_attrib->fieldName()]!="")
        {
          $xml.=$attribname.'="'.$record[$p_attrib->fieldName()].'" ';
        }
      }
      $xml.='/>';

      if ($node->m_postvars['tohtml']==1)
      {
        return htmlspecialchars($xml).'<br>';
      }
      else
      {
        return $xml;
      }
    }
  }

?>