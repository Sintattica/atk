<?php

/**
* @author Martin Roest <martin@ibuildings.nl>
*/

  class atkmlsplitter
  {
    function getLanguages()
    {
      return strtoupper(atkconfig("language"));
    }
    
    /**
    * add/update multiple language records
    */
    function updateMlRecords(&$node, $record,$mode="add")
    {
      $languages = $this->getLanguages();
      
      foreach($node->m_primaryKey as $primkey)
      {
        // Make sure we don't increment the primkey
        if($node->m_attribList[$primkey]->hasFlag(AF_AUTOINCREMENT))
        $node->m_attribList[$primkey]->removeFlag(AF_AUTO_INCREMENT);
      }
      
      foreach($languages as $language)
      {
        if($language==$node->m_defaultlanguage) continue;
        foreach($node->m_attribList as $attribname=>$attrib)
        {
          if($node->m_attribList[$attribname]->hasFlag(AF_ML)) $record[$attribname]=$language;
          if($node->m_attribList[$attribname]->m_mlattribute)
          {
            // change the language of the attribute
            $node->m_attribList[$attribname]->m_language=$language;
          }
        }
        $record["atkprimkey"] = $node->primaryKey($record)." AND ".$node->m_table.".".$node->m_lngfield."='$language' ";
        switch($mode)
        {
          case "update":
          $node->updateDb($record);
          break;
          default:
          $node->addDb($record,false,"add");
        }
        $record["atkprimkey"]=$oldprimkey;
      }
    }
    
    /**
    * merges multiple multilanguage records to one record with fields containing arrays needed by mlattributes
    */
    function combineMlRecordSet(&$node, &$recordset,$query)
    {
      $ml_record=Array();
      
      // possible situations
      // 1. we are a ML node and thus we have multiple records
      // 2. we are a ML node and have 1 or more 1:1 relations with ML nodes (ai what happends then)
      // 3. we aren't a ML node but we have 1 or more 1:1 relation with a ML node
      
      $hasrelationwithmlnode = $this->getMlNodes($node);
      
      if($node->hasFlag(NF_ML) && count($hasrelationwithmlnode)==0) // just this node has mlsupport
      {
        $this->mergeMlRecords(&$recordset);
      }
      else if($node->hasFlag(NF_ML) && count($hasrelationwithmlnode)>0)
      {
        // oh oh what to do.
        atkdebug("combineMlRecordSet action not supported!!!!!");
      }
      else if(!$node->hasFlag(NF_ML) && count($hasrelationwithmlnode)>0)
      {
        $newrecord[0] = $recordset[0];
        // we have relations with mlnodes
        for($i=0,$i_=count($recordset);$i<$i_;$i++)
        {
          $query->deAlias($recordset[$i]);
          atkDataDecode($recordset[$i]);
          foreach($hasrelationwithmlnode as $attribname=>$attrib)
          {
            $tmprecord[$attribname][] = $recordset[$i][$attribname];
          }
        }
        foreach($tmprecord as $attribname=>$attribrecords)
        {
          $node->m_attribList[$attribname]->createDestination();
          $this->combineMlRecordSet($node->m_attribList[$attribname]->m_destInstance, $attribrecords,$query);
          $newrecord[0][$attribname]=$attribrecords[0];
        }
        $recordset=$newrecord;
      }
    }
    
   /**
    * this is used to find 1:1 relations with multilanguage support
    * we need these relation because the recordlist will have them included
    * when editting a record we have to combine these records
    */
   function getMlNodes(&$node)
   {
     // we only have to check the 1:1 relations!!
     $hasrelationwithmlnode=Array();
     if(is_array($node->m_relations["atkonetoonerelation"]))
     {
       foreach($node->m_relations["atkonetoonerelation"] as $attribname=>$attribute)
       {
         $p_attrib = &$this->m_attribList[$attribname];
         if($p_attrib->createDestination() && $p_attrib->m_destInstance->hasFlag(NF_ML))
         {
           $hasrelationwithmlnode[$attribname]=&$this->m_attriblist[$attribname];
         }
       }
     }
     return $hasrelationwithmlnode;
   }     

    
    function mergeMlRecords(&$node, &$recordset)
    {
      foreach($node->m_attribList as $attribname=>$attrib)
      {
        if($node->m_attribList[$attribname]->m_mlattribute) $lngattribs[$attribname] = &$node->m_attribList[$attribname];
      }
      for($i=0, $max=count($recordset);$i<$max;$i++)
      {
        if($node->m_defaultlanguage==$recordset[$i][$node->m_lngfield]) $ml_record[0]=$recordset[$i];
        foreach($lngattribs as $lngattribname=>$lngattrib)
        {
          $lngattribvalues[$lngattribname][$recordset[$i][$node->m_lngfield]]=$recordset[$i][$lngattribname];
        }
      }
      foreach($lngattribvalues as $lngattribname=>$value)
      {
        $ml_record[0][$lngattribname]=$value;
      }
      $recordset=$ml_record;
    }
    
  }

?>