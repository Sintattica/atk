<?php

  /**
  * @author Martin Roest <martin@ibuildings.nl>
  *
  * This class is responsable for managing multilanguage nodes wich have
  * multiple records per occurance.
  * It updates/saves or merges multiple records
  */

  class atkmlsplitter
  {
    function getLanguages(&$node)
    {
      if(method_exists($node,"getLanguages"))
        return $node->getLanguages();
      else
        return strtoupper(atkconfig("language"));
    }
    
    function updateLngField(&$node,&$record)
    {
      // blegh
      global $g_db;
      $sql = "UPDATE ".$node->m_table." SET ".$node->m_lngfield."='".$node->m_defaultlanguage."'
               WHERE ".$node->m_lngfield."='' AND ".$record["atkprimkey"];
      return $g_db->query($sql);
    }
    
    /**
    * add/update multiple language records
    */
    function updateMlRecords(&$node, $record, $mode="add")
    {
      atkdebug("atkmlsplitter::updateMlRecords() for mode $mode");
      $excludelist = array();
      foreach($node->m_attribList as $attribname=>$attrib)
      {
        if(is_subclass_of($node->m_attribList[$attribname],"atkRelation"))
        {
          // manytoone relations are stored only when adding
          // assume all onetomanyrelations are stored in the PRESTORE so we MUST NOT use addDb()
          // on this node if its contains relations to others
          // but for 1:n and n:1 we need to save the refkey
          if(is_a($node->m_attribList[$attribname],"atkManyToOneRelation")||
             is_a($node->m_attribList[$attribname],"atkOneToOneRelation"))
          {
            $p_attrib = &$node->m_attribList[$attribname];
            $p_attrib->createDestination();
            $attribvalue=$p_attrib->m_destInstance->m_attribList[$p_attrib->m_destInstance->primaryKeyField()]->value2db($record[$attribname]);
            $record[$p_attrib->fieldName()]=$attribvalue;
            $p_attrib=new atkAttribute($attribname);
          }
          else  $excludelist[] = $attribname;
        }
      }
      
      $languages = $this->getLanguages($node);
      $atklngrecordmodes = sessionLoad("atklng_".$node->m_type);
            
      foreach($node->m_primaryKey as $primkey)
      {
        // Make sure we don't increment the primkey
        if($node->m_attribList[$primkey]->hasFlag(AF_AUTOINCREMENT))
        $node->m_attribList[$primkey]->removeFlag(AF_AUTO_INCREMENT);
      }
      
      foreach($languages as $language)
      {
        if($atklngrecordmodes[$language]["mode"]=="updatelngfield")
        {
          $this->updateLngField($node,$record);
        }
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
        if($atklngrecordmodes[$language]["mode"]=="add") $mode="add"; // override the mode in case of missing lngrecords
        switch($mode)
        {
          case "update":
            $node->updateDb($record);
            break;
          default:
            $node->addDb($record,false,"add", $excludelist);
        }
        $record["atkprimkey"]=$oldprimkey;
      }
      sessionStore("atklng_".$node->m_type,NULL); // deleting modes
    }
    
    /**
    * Adds language condition
    */
    function addMlCondition(&$query,&$node,$mode)
    {
      if($node->hasFlag(NF_ML) && $mode!="edit" && $mode!="copy")
      {
        $query->addCondition("UPPER(".$node->m_table.".".$node->m_lngfield.")='".$node->m_defaultlanguage."'");
      }
      
      $relations=&$node->m_relations["atkmanytoonerelation"];
      if(is_array($relations) && count($relations)>0)
      {
        foreach ($relations as $attribname=>$attribute)
        {
          $p_attrib = &$relations[$attribname];
          if($p_attrib->createDestination() && $p_attrib->m_destInstance->hasFlag(NF_ML))
          {
            $dest = $p_attrib->m_destInstance;
            $query->addCondition("UPPER(".$attribname.".".$dest->m_lngfield.")='".$node->m_defaultlanguage."'");
          }
        }
      }
    }
    
    /**
    * merges multiple multilanguage records to one record with fields containing arrays needed by mlattributes
    */
    function combineMlRecordSet(&$node, &$recordset,$query)
    {
      $hasrelationwithmlnode = $this->getMlNodes($node);
      $languages = $this->getLanguages($node);
      if(count($languages)!=count($recordset))
      {
        $recordset = $this->addLngRecords($node,$recordset);
      }
      // TODO FIXME (martin): why different situations?
      // if the select uses mlconditions there are always three records and thus no need to have different situations?
//      if($node->hasFlag(NF_ML) && count($hasrelationwithmlnode)==0) // just this node has mlsupport
//      {
        $this->mergeMlRecords(&$node,&$recordset);
/*
      }
      else if(!$node->hasFlag(NF_ML) && count($hasrelationwithmlnode)>0)
      {
        // TODO:
        // i think this case is no longer valid therefor the code will probably won't work :)
        // one2one's use the lazy flag and their own load method to load data.
        atkdebug("WHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA PANIC");
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
*/
      sessionStore("atklng_".$node->m_type,$recordset[0]["atklngrecordmodes"]);
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
         $p_attrib = &$node->m_attribList[$attribname];
         if($p_attrib->createDestination() && $p_attrib->m_destInstance->hasFlag(NF_ML))
         {
           $hasrelationwithmlnode[$attribname]=&$node->m_attriblist[$attribname];
         }
       }
     }
     return $hasrelationwithmlnode;
   }     

    
   function hasLngRecord(&$node,&$recordset,$lng,&$index)
   {
     $index=0;
     foreach($recordset as $record)
     {
       if($record[$node->m_lngfield]==$lng)
         return true;
       $index++;
     }
     return false;
   }
   
   function addLngRecords(&$node,&$recordset)
   {
     $newrecordset=Array();
     $languages=$this->getLanguages($node);
     atkdebug("atkmlsplitter adding missings lngrecord for ".$node->m_type."!");
     for($i=0, $max=count($languages);$i<$max;$i++)
     {
       $index=NULL;
       if(!$this->hasLngRecord($node,$recordset,$languages[$i],$index))
       {
         $recordcount=count($newrecordset);
         $newrecordset[$recordcount]=$recordset[0]; // assume that the first record is OK.
         $newrecordset[$recordcount][$node->m_lngfield]=$languages[$i];
         
         if($languages[$i]!=$node->m_defaultlanguage) // saving atkaction
           $newrecordset[$recordcount]["atklngrecordmodes"][$languages[$i]]["mode"]="add";
         else
           $newrecordset[$recordcount]["atklngrecordmodes"][$languages[$i]]["mode"]="updatelngfield";
       }
       else $newrecordset[] = $recordset[$index];
     }
     return $newrecordset;
    }
   
    function mergeMlRecords(&$node, &$recordset)
    {
      foreach($node->m_attribList as $attribname=>$attrib)
      {
        if($node->m_attribList[$attribname]->m_mlattribute) $lngattribs[$attribname] = &$node->m_attribList[$attribname];
      }
      $ml_record[0]=$recordset[0]; // assume this is the record with the default language
      $ml_record[0]["atklngrecordmodes"]=Array();
      for($i=0, $max=count($recordset);$i<$max;$i++)
      {
        if(is_array($recordset[$i]["atklngrecordmodes"]))  // keep track off atkactions
          $ml_record[0]["atklngrecordmodes"] = array_merge($ml_record[0]["atklngrecordmodes"],$recordset[$i]["atklngrecordmodes"]);
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