<?PHP

  /**
  * validate the atknode records
  * this new class is made to decrease the size of the atknode class.
  *
  * @author Kees van Dieren <kees@ibuildings.nl>
  * @version $Revision$
  * $Id$
  * @copyright Ibuildings.nl BV
  * @package atk
  */
  class atkNodeValidator extends atkNode
  {

    /**
    * The node which needs to get validated
    *
    * @access private
    * @var atkNode nodeObj
    */
    var $m_nodeObj = null;

    /**
    * the record of the node which will get validated
    *
    * @access private
    * @var KeyVal record
    */
    var $m_record = array();

    /**
    * the mode in which the validate will get runned
    *
    * @access private
    * @var string mode
    */
    var $m_mode = "add";

    /**
    * constructor
    *
    */
    function atkNodeValidator() 
    {
    }

    /**
    * set the list of fields which will get ignored 
    * @param array fieldArr
    *
    * @return void
    */
    function setIgnoreList($fieldArr)
    {	
      $this->m_ignoreList = $fieldArr;
    }

    /**
    * set the mode in which to validate
    * @param string mode
    *
    * @return void
    */
    function setMode($mode)
    {	
      $this->m_mode = &$mode;
    }

    /**
    * set the Node which should be validated
    *
    * @param Object nodeObj
    * @return void
    */
    function setNode(&$nodeObj)
    {	
      $this->m_nodeObj = $nodeObj;
    }

    /**
    * set the record which should get validated
    *
    * @return void
    */
    function setRecord(&$record)
    {	
      $this->m_record = &$record;
    }

    /**
    * validate an atknode
    *
    * @param string mode (optional) : override the mode
    * @param array ignoreList (optional) : override the ignoreList
    *
    * @return void
    */
    function validate($mode="", $ignoreList=array())
    {	
      atkdebug("validate() with mode $mode for node ".$this->m_nodeObj->atkNodeType());

      global $g_db;

      // check overrides
      if(count($ignoreList))
        $this->m_ignoreList = $ignoreList;

      if($mode != "")
        $this->m_mode = $mode;
      
      // set the record
      $record = &$this->m_record;

      // First we check if the values for the attributes are correct
      foreach (array_keys($this->m_nodeObj->m_attribList) as $attribname)
      {
        if (!atk_in_array($attribname, $this->m_ignoreList))
        {
          $p_attrib = &$this->m_nodeObj->m_attribList[$attribname];

          if (!( $p_attrib->isEmpty($record)))
          {
            $funcname = $p_attrib->m_name."_validate";
            if (method_exists($this,$funcname))
            {
              $this->m_nodeObj->$funcname($record, $this->m_mode);
            }
            else
            {
              $p_attrib->validate($record, $this->m_mode);
            }
          }
        }
      }
      if(count($record['atkerror']) > 0) return false;

      // After the values check, we check the Flags.
      foreach (array_keys($this->m_nodeObj->m_attribList) as $attribname)
      {
        if (!atk_in_array($attribname, $this->m_ignoreList))
        {
          $p_attrib = &$this->m_nodeObj->m_attribList[$attribname];

          if(count($record['atkerror']) > 0) return false;
          //Validate if ne
          if ($p_attrib->hasFlag(AF_PRIMARY) && !$p_attrib->hasFlag(AF_AUTO_INCREMENT))
          {
          //  $atkorgkey = $this->m_nodeObj->orgKey();
            $atkorgkey = $record["atkprimkey"];
            if(($atkorgkey == '' // no orgkey, so adding this record
                || $atkorgkey != $this->m_nodeObj->primaryKey($record)) // key has changed, so check is necessary
               &&  count($g_db->getrows("SELECT ".$p_attrib->fieldName()." FROM ".$this->m_nodeObj->m_table." WHERE ".$this->m_nodeObj->primaryKey($record)))>0
              )
            {
              triggerError($record, $attribname, 'error_primarykey_exists', '', $this->m_nodeObj->m_type);
            }
          }

          // if no root elements may be added to the tree, then every record needs to have a parent!
          if ($p_attrib->hasFlag(AF_PARENT) && $this->m_nodeObj->hasFlag(NF_TREE_NO_ROOT_ADD) && $this->m_nodeObj->m_action == "save")
            $p_attrib->m_flags |= AF_OBLIGATORY;

          // validate obligatory fields (but not the auto_increment ones, because they don't have a value yet)
          if ($p_attrib->hasFlag(AF_OBLIGATORY) && !$p_attrib->hasFlag(AF_AUTO_INCREMENT) && $p_attrib->isEmpty($record))
          {
            triggerError($record, $attribname, 'error_obligatoryfield', '', $this->m_nodeObj->m_type);
          }
          // if flag is primary
          else if ($p_attrib->hasFlag(AF_UNIQUE) && !$p_attrib->hasFlag(AF_PRIMARY) &&
                   count($g_db->getrows("SELECT ".$p_attrib->fieldName()." FROM ".$this->m_nodeObj->m_table." WHERE $attribname='".escapeSQL($this->m_nodeObj->m_postvars[$this->m_nodeObj->m_attribList[$attribname]->fieldName()])."' AND NOT (".$this->m_nodeObj->primaryKey($record).")"))>0
                  )
          {
            triggerError($record, $attribname, 'error_uniquefield', '', $this->m_nodeObj->m_type);
          }
          else
          {
            //don't validate empty fields
            if (!( $p_attrib->isEmpty($record)))
            {
              $funcname = $p_attrib->m_name."_validate";

              if (method_exists($this,$funcname))
              {
                $this->m_nodeObj->$funcname($record, $this->m_mode);
              }
              else
              {
                $p_attrib->validate($record, $this->m_mode);
              }
            }
          }
        }
      }
      
      $this->validateUniqueFieldSets($record);

      for ($i = 0, $_i = count($record["atkerror"]); $i < $_i; $i++)
        $record["atkerror"][$i]["node"] = $this->m_nodeObj->m_type;
    }

    /**
    * validate the field sets
    *
    * @param KeyVal $record
    * @return void
    */
    function validateUniqueFieldSets(&$record)
    {	
      global $g_db;
      foreach($this->m_nodeObj->m_uniqueFieldSets as $uniqueFieldSet)
      {
        $whereStr = " WHERE 1 ";
        foreach($uniqueFieldSet as $field)
        {

          // manyToOneyRelations could contain multiple values. When calling fetchValue
          // from this type of array, it will return an key-val array
          $whereValueMixed = $this->m_nodeObj->m_attribList[$field]->fetchValue($this->m_nodeObj->m_postvars);

          if(is_array($whereValueMixed))
          {	

            // add the current attribute itself to the string and remove it 
            // from the array. 
            $whereValueStr = "'".escapeSQL($whereValueMixed[$field])."'";
            unset($whereValueMixed[$field]);

            foreach($whereValueMixed as $name => $value)
              $whereValueStr .= " and ".$name." = '".escapeSQL($value)."'";

          }
          else          
            $whereValueStr = "'".escapeSQL($whereValueMixed)."'";

          $whereStr .= "AND ".$field." = ".$whereValueStr." " ;
        }
        if(count($g_db->getRows("
          SELECT 
            1            
          FROM
            ".$this->m_nodeObj->m_table.
            $whereStr."
          AND NOT
            (".$this->m_nodeObj->primaryKey($record).")")
                )> 0
          )        
        {	
          triggerError($record, $uniqueFieldSet, 'error_uniquefieldset', '', $this->m_nodeObj->m_type);
          atk_var_dump($record[$field], "jdsklfjdsflkdsjf lsdkfjsldk fds");
        }// end if count(rows)
      } // end foreach uniqueFieldSet
    } // end function validate_uniqueFieldSets
  } // end class validator
?>