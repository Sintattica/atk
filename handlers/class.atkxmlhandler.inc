<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage handlers
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Handler class for the exporting a record to an XML file.
   *
   * @author Ivo Jansch <ivo@achievo.org>   
   * @package atk
   * @subpackage handlers
   *
   */
  class atkXmlHandler extends atkActionHandler
  {
    /**
     * The action handler method.
     */
    function action_xml()
    {      
      $recordset = $this->m_node->selectDb($this->m_postvars['atkselector'],"","","","","xml");
             
      for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
      {
        $this->invoke("xml", $recordset[$i]);
      }
    }
    
    /**
     * Convert a record to an XML document.
     * @param array $record The record to convert to xml.
     * @return String XML document.
     * @todo This handler can only handle 'simple' key/value attributes
     *       like atkAttribute. Relation support should be added.
     *
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