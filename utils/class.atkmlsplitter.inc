<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage utils
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * This class is responsible for managing multilanguage nodes which have
   * multiple records per occurance.
   * It updates/saves or merges multiple records.
   * This is used by multilingual atkNodes. It should generally not be
   * necessary to use this class directly.
   *
   * @author Martin Roest <martin@ibuildings.nl>
   * @package atk
   * @subpackage utils
   *
   */
  class atkmlsplitter
  {
    /**
     * Constructor
     *
     */
    function atkMlSplitter()
    {
      // constructor
    }

    /**
     * Retrieve the mlsplitter instance
     * @return atkOutput The instance.
     */
    function &getInstance()
    {
      static $s_mlsplitter = NULL;
      if (!is_object($s_mlsplitter))
      {
        $s_mlsplitter = new atkmlsplitter();
        atkdebug("Created a new atkmlsplitter instance");
      }
      return $s_mlsplitter;
    }

    /**
     * Get supported languages
     *
     * @param atkNode $node
     * @return array Array with supported languages
     */
    function getLanguages(&$node)
    {
      if(method_exists($node,"getLanguages"))
      {
        return $node->getLanguages();
      }
      $lngs = atkconfig("supported_languages");
      for($i=0, $_i=count($lngs); $i<$_i; $i++) $lngs[$i]=strtoupper($lngs[$i]);
      return $lngs;
    }

    /**
     * Update the language field
     *
     * @param atkNode $node
     * @param array $record
     * @return bool
     */
    function updateLngField(&$node,&$record)
    {
      // blegh
      $db = &atkGetDb();
      $sql = "UPDATE ".$node->m_table." SET ".$node->m_lngfield."='".$node->m_defaultlanguage."'
               WHERE ".$node->m_lngfield."='' AND ".$record["atkprimkey"];
      return $db->query($sql);
    }

    /**
     * add/update multiple language records
     * because nodes are cached and we have to make some attribute modifications you
     * can't pass the node as a reference!!
     *
     * @param atkNode $node
     * @param array $record
     * @param string $mode 
     */
    function updateMlRecords($node, $record, $mode="add")
    {
      atkdebug("atkmlsplitter::updateMlRecords() for mode $mode");
      $excludelist = array();
      $relations = array();

      foreach($node->m_attribList as $attribname=>$attrib)
      {
        if(is_subclass_of($node->m_attribList[$attribname],"atkRelation"))
        {
          // manytoone relations are stored only when adding
          // assume all onetomanyrelations are stored in the PRESTORE so we MUST NOT use addDb()
          // on this node if its contains relations to others
          // but for 1:n and n:1 we need to save the refkey
          if((is_a($node->m_attribList[$attribname],"atkManyToOneRelation")
	               || is_a($node->m_attribList[$attribname],"atkOneToOneRelation")) && hasFlag($node->m_attribList[$attribname]->storageType(), ADDTOQUERY))
          {
          	$relations[$attribname] = $node->m_attribList[$attribname];

          	$p_attrib = &$node->m_attribList[$attribname];
            $p_attrib->createDestination();
            $attribvalue=$p_attrib->m_destInstance->m_attribList[$p_attrib->m_destInstance->primaryKeyField()]->value2db($record[$attribname]);
            $record[$p_attrib->fieldName()]=$attribvalue;
            $p_attrib=new atkAttribute($attribname);
            $p_attrib->m_ownerInstance = &$node;
            $p_attrib->init();
          }
          else  $excludelist[] = $attribname;
        }
      }

      $languages = $this->getLanguages($node);
      $atklngrecordmodes = sessionLoad("atklng_".$node->m_type);

      $autoincrementflags=Array();
      foreach($node->m_primaryKey as $primkey)
      {
        // Make sure we don't increment the primkey
        if($node->m_attribList[$primkey]->hasFlag(AF_AUTOINCREMENT))
        $node->m_attribList[$primkey]->removeFlag(AF_AUTO_INCREMENT);
        $autoincrementflags[]=$primkey;
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

        $editMode = $mode;
        if($atklngrecordmodes[$language]["mode"]=="add")
        {
          $editMode="add"; // override the mode in case of missing lngrecords
        }

        switch($editMode)
        {
          case "update":
            $node->updateDb($record);
            break;
          default:
            $node->addDb($record,false,$mode, $excludelist);
        }
        $record["atkprimkey"]=$oldprimkey;
      }

      foreach($node->m_attribList as $attribname=>$attrib)
      {
        // restore the default language
        if($node->m_attribList[$attribname]->m_mlattribute)
        {
          $node->m_attribList[$attribname]->m_language=$node->m_defaultlanguage;
        }
      }

      foreach($autoincrementflags as $primkey)
      {
        // restore the attrib flags
        $node->m_attribList[$primkey]->addFlag(AF_AUTO_INCREMENT);
      }

      foreach($relations as $attribname=>$relation)
      {
      	// restore the relations
      	$node->m_attribList[$attribname]=$relation;
      }

      sessionStore("atklng_".$node->m_type,NULL); // deleting modes
    }

    /**
     * Adds language condition
     * 
     * @param atkQuery $query
     * @param atkNode $node
     * @param string $mode
     * @param string $joinalias
     */
    function addMlCondition(&$query,&$node,$mode,$joinalias)
    {
      global $ATK_VARS;
      $lng = (isset($ATK_VARS["atklng"])?$ATK_VARS["atklng"]:"");

      if (!$lng) $lng = $node->m_defaultlanguage;

      if($node->hasFlag(NF_ML) && $mode!="edit" && $mode!="copy")
      {
        $fieldname = $joinalias.".".$node->m_lngfield;
        $query->addCondition("({$fieldname} = '' OR {$fieldname} IS NULL OR UPPER({$fieldname})='".strtoupper($lng)."')");
      }
    }

    /**
    * merges multiple multilanguage records to one record with fields containing arrays needed by mlattributes
    * 
    * @param atkNode $node
    * @param array $recordset
    * @param atkQuery $query
    */
    function combineMlRecordSet(&$node, &$recordset,$query)
    {
      $hasrelationwithmlnode = $this->getMlNodes($node);
      $languages = $this->getLanguages($node);
      if(count($languages)!=count($recordset))
      {
        $recordset = $this->addLngRecords($node,$recordset);
      }
      $this->mergeMlRecords($node,$recordset);
      sessionStore("atklng_".$node->m_type,$recordset[0]["atklngrecordmodes"]);
    }

   /**
    * this is used to find 1:1 relations with multilanguage support
    * we need these relation because the recordlist will have them included
    * when editting a record we have to combine these records
    * 
    * @param atkNode $node
    * @return array Array with relationnames
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

   /**
    * Has language record?
    *
    * @param atkNode $node
    * @param array $recordset
    * @param string $lng
    * @param int $index
    * @return bool
    */
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

   /**
    * Add language records
    *
    * @param atkNode $node
    * @param array $recordset
    * @return array Array with records to add
    */
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

    /**
     * Merge multilanguage records
     *
     * @param atkNode $node
     * @param array $recordset
     */
    function mergeMlRecords(&$node, &$recordset)
    {
      $lngattribs=array();
      $lngattribvalues=array();
      foreach($node->m_attribList as $attribname=>$attrib)
      {
        if($node->m_attribList[$attribname]->m_mlattribute) $lngattribs[$attribname] = &$node->m_attribList[$attribname];
      }
      $i = $this->searchRecordDefaultLanguage($recordset, $node->m_defaultlanguage);
      $ml_record[0]=$recordset[$i]; // assume this is the record with the default language
      $ml_record[0]["atklngrecordmodes"]=Array();
      for($i=0, $max=count($recordset);$i<$max;$i++)
      {
        if(is_array($recordset[$i]["atklngrecordmodes"]))  // keep track off atkactions
          $ml_record[0]["atklngrecordmodes"] = array_merge($ml_record[0]["atklngrecordmodes"],$recordset[$i]["atklngrecordmodes"]);
        foreach($lngattribs as $lngattribname=>$lngattrib)
        {
          $lngattribvalues[$lngattribname][strtoupper($recordset[$i][$node->m_lngfield])]=$recordset[$i][$lngattribname];
        }
      }
      foreach($lngattribvalues as $lngattribname=>$value)
      {
        $ml_record[0][$lngattribname]=$value;
      }
      $recordset=$ml_record;
    }

    /**
     * Search the recordset for the default language
     *
     * @param array $recordset
     * @param string $defaultlanguage
     * @return int The position in the recordset array where the defaultlanguage is found
     */
    function searchRecordDefaultLanguage($recordset, $defaultlanguage)
    {
      for ($i=0;$i<count($recordset);$i++)
      {
        if ($recordset[$i]["lng"] == $defaultlanguage) return $i;
      }
      return 0;
    }
  }
?>
