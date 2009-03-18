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
   */
  atkimport("atk.security.encryption.atkencryption");
  userelation("atkonetoonerelation");
  
  /**
   * Relationship that can link 2 tables based on a secure link
   * that can not be decrypted when not logged in through an atk
   * application. 
   * This effectively secures the database so that data in two
   * tables can not be correlated by mischievous access to the database.
   *
   * @author Mark Baaijens <mark@ibuildings.nl>
   *
   * @package atk
   * @subpackage relations
   *
   */
  class atkSecureRelation extends atkOneToOneRelation
  {
    var $m_crypt = NULL;
    var $m_linktable;
    var $m_linkfield;
    var $m_linkuserfield = "username";
    var $m_keylength;
    var $m_searching = false;
    var $m_keylookup = array();
    var $m_records = array();
    var $m_searcharray=array();
    var $m_linkpass;
    var $m_linkbackfield;
    var $m_ownersearch;
    var $m_destsearch;
    var $m_cachefield;

    /**
     * Creates an atkSecureRelation, 
     * similar to an atkOneToOne relation only encrypted
     *
     * @param string $name        The unique name of the attribute. In slave 
     *                            mode, this corresponds to the foreign key 
     *                            field in the database table. 
     * @param string $destination The destination node (in module.nodename
     *                            notation)
     * @param string $linktable   The table we link to
     * @param string $linkfield   The field we link to
     * @param string $linkbackfield  
     * @param int $keylength      The length of the encryption key
     * @param string $refKey=""   In master mode, this specifies the foreign 
     *                            key field from the destination node that 
     *                            points to the master record. In slave mode, 
     *                            this parameter should be empty.
     * @param string $encryption  The encryption to use
     * @param int $flags          Attribute flags that influence this 
     *                            attributes' behavior.     
     */
    function atkSecureRelation($name, $destination, $linktable, $linkfield, $linkbackfield, $keylength, $refKey="", $encryption, $flags=0)
    {
      $this->atkOneToOneRelation($name, $destination, $refKey, $flags|AF_ONETOONE_ERROR);
      $this->createDestination();
      $this->m_crypt = &atkEncryption::getEncryption($encryption);
            
      $this->m_linktable = $linktable;
      $this->m_linkfield = $linkfield;
      $this->m_keylength = $keylength;
      $this->m_linkbackfield = $linkbackfield;
    }
    
    /**
     * Set the name of the cache field
     *
     * @param string $fieldname The cache fieldname
     */
    function setCacheField($fieldname="cache")
    {
      $this->m_cachefield = $fieldname;
    }
  
    /**
     * Adds the attribute / field to the list header. This includes the column name and search field.
     *
     * Framework method. It should not be necessary to call this method directly.
     *
     * @param String $action the action that is being performed on the node
     * @param array  $arr reference to the the recordlist array
     * @param String $fieldprefix the fieldprefix
     * @param int    $flags the recordlist flags
     * @param array  $atksearch the current ATK search list (if not empty)
     * @param String $atkorderby the current ATK orderby string (if not empty)
     */
    function addToListArrayHeader($action, &$arr, $fieldprefix, $flags, $atksearch, $atkorderby)
    {
      if ($this->hasFlag(AF_ONETOONE_INTEGRATE))
      {
        // integrated version, don't add ourselves, but add all columns from the destination.
        if ($this->createDestination())
        {
          foreach (array_keys($this->m_destInstance->m_attribList) as $attribname)
          {
            $p_attrib = &$this->m_destInstance->getAttribute($attribname);
            $p_attrib->addFlag(AF_NO_SORT);
          }
        }
      }
      parent::addToListArrayHeader($action, $arr, $fieldprefix, $flags, $atksearch, $atkorderby);
    }
    
    /**
     * Gets the password for the link
     * for more security the administrator gets a random password. You have to capture in your application that
     * the administrator is only able to insert the first record in this relation and make also a useracount with it.
     * @param string $linktable the table where we find the linkpass
     * @param string $linkfield the field where we find the encrypted linkpass
     * @param string $encryption The encryption to use
     * @return string           the password for the link
     */
    function getLinkPassword($linktable,$linkfield,$encryption="")
    {      
      if ($this->m_linkpass) return $this->m_linkpass;
      if (!$linktable) $linktable = $this->m_linktable;
      if (!$linkfield) $linkfield = $this->m_linkfield;
      
      $user = getUser();
      $username = $user['name'];
      $password = $user['PASS'];

      if($encryption)
        $crypt = atkEncryption::getEncryption($encryption);
      else
        $crypt = $this->m_crypt;
  
      if($username == "administrator")
      {
        //if the administrator asks for a  password we generate one
        //because the administrator only makes the first person
        global $linkpass;
        if(!$linkpass)
          $linkpass = $crypt->getRandomKey($password);
      }
      else
      {
        $query = "SELECT ".$linkfield." as pass FROM ".$linktable." WHERE ".atkconfig("auth_userfield")." = '".$username."'";

        $db = &atkGetDb();
        $rec = $db->getrows($query);
        if(count($rec) < 1)
          return $linkpass;
          
        $encryptedpass = array_pop($rec);

        $linkpass = $encryptedpass['pass'];
      }
      $this->m_linkpass = $crypt->decryptKey($linkpass,$password);
      return $this->m_linkpass;
    }
  
    /**
     * This function in the atkOneToOneRelation store the record of the parentnode in the DB
     * with the reference key of the other table. 
     * So we encrypt the reference key before we call the method.
     * For more documentation see the atkOneToOneRelation
     * 
     * @param atkQuery $query The SQL query object
     * @param String $tablename The name of the table of this attribute
     * @param String $fieldaliasprefix Prefix to use in front of the alias
     *                                 in the query.
     * @param Array $rec The record that contains the value of this attribute.
     * @param int $level Recursion level if relations point to eachother, an
     *                   endless loop could occur if they keep loading
     *                   eachothers data. The $level is used to detect this
     *                   loop. If overriden in a derived class, any subcall to
     *                   an addToQuery method should pass the $level+1.
     * @param String $mode Indicates what kind of query is being processing:
     *                     This can be any action performed on a node (edit,
     *                     add, etc) Mind you that "add" and "update" are the
     *                     actions that store something in the database,
     *                     whereas the rest are probably select queries.
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
    {
      $records = $this->m_records;
      
      if (count($records)==0 && !$this->m_searching)
      {
        if(is_array($rec))
        {
          $link = $rec[$this->fieldName()][$this->m_destInstance->m_primaryKey[0]];
          $cryptedlink = $this->m_crypt->encrypt($link,$this->getLinkPassword($this->m_linktable, $this->m_linkfield));
          $rec[$this->fieldName()][$this->m_destInstance->m_primaryKey[0]] = addslashes($cryptedlink);
        }
  
        return parent::addToQuery($query, $tablename, $fieldaliasprefix, $rec, $level, $mode);
      }
      else // lookup matching
      {
        $where = array();

        foreach (array_keys($this->m_keylookup) as $decryptedlink)
        {                     
          $where[] = $decryptedlink;
        }
        
        if ($tablename) $tablename .= ".";
        $query->addSearchCondition($tablename.$this->m_ownerInstance->primaryKeyField()." IN ('".implode("','", $where)."')");
      }
    }
  
    /**
     * This function in the atkOneToOneRelation loads the record of the childnode from the DB
     * with the the id from de reference key in childnode. 
     * So we decrypt the reference key before we call the method. 
     * For more documentation see the atkOneToOneRelation
     * 
     * @param atkDb $db The database object
     * @param array $record The record
     * @param string $mode The mode we're in ("add", "edit", "copy")
     * @return array The loaded records
     */
    function load(&$db, $record, $mode)
    {                 
      if ($this->m_searching)
      {
        if($this->m_searcharray !== $this->m_ownerInstance->m_postvars["atksearch"][$this->fieldName()] 
           && is_array($this->m_searcharray))
        {
          $this->m_records = array();
          $this->m_keylookup= array();
          // perform query on destination node to retrieve all records.
          if ($this->createDestination())
          {
            $this->m_searcharray = $this->m_ownerInstance->m_postvars["atksearch"][$this->fieldName()];

            //if al search values are equal, then make it an OR search
            if(count(array_unique(array_values($this->m_searcharray))) == 1)
              $this->m_destInstance->m_postvars['atksearchmethod'] = "OR";

             $oldsearcharray = $this->m_searcharray;
            // check wether mentioned fields are actually in the node
            foreach ($this->m_searcharray as $searchfield => $searchvalue)
            {
              if (!is_object($this->m_destInstance->m_attribList[$searchfield]))
                unset($this->m_searcharray[$searchfield]);
            }            
            $this->m_destInstance->m_postvars["atksearch"] =        $this->m_searcharray;
            $this->m_destInstance->m_postvars["atksearchmode"] =    $this->m_ownerInstance->m_postvars["atksearchmode"];
            $this->m_destInstance->m_postvars["atksearchmethod"] =  $this->m_ownerInstance->m_postvars["atksearchmethod"];
            
            $records = $this->m_destInstance->selectDb();
            $this->m_searcharray = $oldsearcharray;
            $errorconfig = atkconfig("securerelation_decrypterror", null);

            // create lookup table for easy reference.            
            for ($i=0, $_i=count($records); $i<$_i; $i++)
            {              
              $decryptedlink = $this->decrypt($records[$i], $this->m_linkbackfield);
              
              if(!$decryptedlink && $errorconfig) 
              {
                atkdebug("Unable to decrypt link: ".$link."for record: ".var_export($records[$i], true)." with linkbackfield: ".$this->m_linkbackfield);
                $decrypterror=true;
              }
              else
              {
                $this->m_keylookup[$decryptedlink] = $i;
                $this->m_records[] = $records[$i];     
              }
            }
            if ($decrypterror)
            {
              if ($errorconfig==2)      atkerror("There were errors decrypting the secured links, see debuginfo");
              else if ($errorconfig==1) mailreport();
            }
            return $this->m_records;
          }
        }
        else // lookup table present, postload stage
        {
          $this->m_searching = false;
          return $this->m_records[$this->m_keylookup[$record[$this->m_ownerInstance->primaryKeyField()]]];
        }
      }
      else 
      {
        if (!$record[$this->fieldName()] || (!$record[$this->m_cachefield] && $this->m_cachefield))
        {
          $query = "SELECT ".$this->fieldName();
          $query.= ($this->m_cachefield?",{$this->m_cachefield}":"");
          $query.=" FROM ".$this->m_ownerInstance->m_table." WHERE ".$this->m_ownerInstance->m_table.".".$this->m_ownerInstance->primaryKeyField()."='".$record[$this->m_ownerInstance->primaryKeyField()]."'";
          $result = $db->getrows($query);
        }
        else 
        {
          $result[0] = $record;
        }
        $cryptedlink = $this->decrypt($result[0],$this->fieldName());
        $records[0][$this->fieldName()] = $cryptedlink;

        if ($cryptedlink)
        {

          $record[$this->fieldName()] = $cryptedlink;

          //for the use of encrypted id's we don't want to use the refkey,
          //because in that case we have to encrypt the id of the employee
          //and than atk CAN get the destination data, but not the owner data.
          //so we backup de refkey, make in empty and put it back after loading the record.
          $backup_refkey = $this->m_refKey;
          $this->m_refKey = "";
          $load = parent::load($db, $record, $mode);
          $this->m_refKey = $backup_refkey;
          return $load;
        }
        else
        {
          atkdebug("Could not decrypt the link: $link for ".$this->m_ownerInstance->primaryKeyField()."='".$record[$this->m_ownerInstance->primaryKeyField()]);
        }
      }
    }
    
    /**
     * Decrypt the field
     *
     * @param array $record
     * @param array $field
     * @return string The decrypted value
     */
    function decrypt($record, $field)
    { 
      global $g_encryption;
      
      if (!$this->m_linkpass) $this->getLinkPassword($this->m_linktable, $this->m_linkfield, $g_encryption);
      
      if (!$this->m_cachefield || !$record[$this->m_cachefield])
      {
        $cryptedlink = $this->m_crypt->decrypt($record[$field],$this->m_linkpass);
        if ($this->m_ownerInstance)
        {
          if ($this->m_cachefield && is_numeric($cryptedlink) && $cryptedlink && $record[$this->m_ownerInstance->primaryKeyField()] && $this->createDestination())
          {
            if ($this->m_ownerInstance->m_attribList[$field]) $cachetable = $this->m_ownerInstance->m_table;
            else if ($this->m_destInstance->m_attribList[$field]) $cachetable = $this->m_destInstance->m_table;
            
            $db = &atkGetDb();
            $db->query("UPDATE $cachetable
                              SET {$this->m_cachefield}='$cryptedlink'
                              WHERE ".$this->m_ownerInstance->primaryKeyField()." = '".
            $record[$this->m_ownerInstance->primaryKeyField()]."'");
          }
          else if (!$cryptedlink || !is_numeric($cryptedlink))
          {
            atkdebug("decrypt($record, $field) failed! and yielded: $cryptedlink");
            return NULL;
          }
        }
        else
        {
          halt("no ownerinstance found for the secure relation");
        }
      }
      else
      {
        $cryptedlink = $record[$this->m_cachefield];
      }
      return $cryptedlink;
    }

    /**
     * For creating a new user put the linkpassword in the db
     * @param string $id the id of the user to create
     * @param string $pass the password for the user
     */
    function newUser($id,$pass)
    {
      $db = &atkGetDb();
      $linkpass = $this->m_crypt->encryptKey($this->getLinkPassword($this->m_linktable,$this->m_linkfield),$pass);
      $query = "UPDATE $this->m_linktable SET $this->m_linkfield = '".$linkpass."' WHERE id = '$id'";
      $db->query($query);
    }
        
    /**
     * Returns the condition which can be used when calling atkQuery's addJoin() method
     * Joins the relation's owner with the destination
     */
    function _getJoinCondition()
    {
      $db = &atkGetDb();
      
      // decrypt the encrypted keys to get the tables joined
      $temp_query = "SELECT " . $this->fieldName() . " FROM " . $this->m_ownerInstance->m_table;
      $result = $db->getRows($temp_query);
      
      $condition = "";
      foreach($result as $recordArray)
      {
        $record = $recordArray[$this->fieldName()];
        $decrypted_record = $this->decrypt($recordArray,$this->fieldName());
        if ($condition == "")
          $whereOrAnd = "(";
        else
          $whereOrAnd = "OR";
          
        $condition .= $whereOrAnd . " (" . $this->m_destInstance->m_table . "." . $this->m_destInstance->primaryKeyField() . "='" . $decrypted_record . "' ";
        $condition .= "AND " . $this->m_ownerInstance->m_table . "." . $this->fieldName() . "=\"".addslashes($record)."\") ";
      }
      $condition .= ") ";

      return $condition;
    }
    
    /**
     * Determine the load type of this attribute.
     *
     * @param String $mode The type of load (view,admin,edit etc)
     * @param bool $searching Is this a search?
     *
     * @return int Bitmask containing information about load requirements.
     *             POSTLOAD|ADDTOQUERY when AF_ONETOONE_LAZY is set.
     *             ADDTOQUERY when AF_ONETOONE_LAZY is not set.
     */
    function loadType($mode, $searching=false)
    {
      if ($searching) 
      {        
        $this->m_searching = true;
        return PRELOAD|ADDTOQUERY|POSTLOAD;
      }
      else 
      {
        return parent::loadType($mode, $searching);
      }
    }    
    
    /**
     * Creates a search condition for a given search value, and adds it to the
     * query that will be used for performing the actual search.
     *
     * @param atkQuery $query The query to which the condition will be added.
     * @param String $table The name of the table in which this attribute
     *                      is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param String $searchmode The searchmode to use. This can be any one
     *                           of the supported modes, as returned by this
     *                           attribute's getSearchModes() method.
     * @param string $fieldaliasprefix optional prefix for the fieldalias in the table
     */
    function searchCondition(&$query, $table, $value, $searchmode, $fieldaliasprefix='')
    {
      //dummy implementation, we handle our own search in the destination node.
    }
    
    /**
     * Creates a searchcondition for the field,
     * was once part of searchCondition, however,
     * searchcondition() also immediately adds the search condition.
     *
     * @param atkQuery $query     The query object where the search condition should be placed on
     * @param String $table       The name of the table in which this attribute
     *                              is stored
     * @param mixed $value        The value the user has entered in the searchbox
     * @param String $searchmode  The searchmode to use. This can be any one
     *                              of the supported modes, as returned by this
     *                              attribute's getSearchModes() method.
     * @return String The searchcondition to use.
     */
    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      // Off course, the secure relation has to have a special search condition
      // because searching on a secure relation has to be broken up in 2 pieces
      // first the destination, then the owner, filtered by the results from
      // the search on the destination

      $searchConditions = array();
      $descfields = $this->m_destInstance->descriptorFields();
      $prevdescfield = "";
      foreach($descfields as $descField)
      {
        if ($descField !== $prevdescfield && $descField !== $this->m_owner)
        {
          $p_attrib = &$this->m_destInstance->getAttribute($descField);
          if (is_object($p_attrib))
          {
            if ($p_attrib->m_destInstance)
            {
              $itsTable = $p_attrib->m_destInstance->m_table;
            }
            else
            {
              $itsTable = $p_attrib->m_ownerInstance->m_table;
            }
            
            if (is_array($searchmode)) $searchmode = $searchmode[$this->fieldName()];
            if (!$searchmode) $searchmode = atkconfig("search_defaultmode");
            // checking for the getSearchCondition
            // for backwards compatibility
            if (method_exists($p_attrib,"getSearchCondition"))
            {
              $searchcondition = $p_attrib->getSearchCondition($query,$itsTable,$value,$searchmode);
              if ($searchcondition)
              {
                $searchConditions[] = $searchcondition;
              }
            }
            else
            {
              $p_attrib->searchCondition($query,$itsTable,$value,$searchmode);
            }
          }
          $prevdescfield = $descField;
        }
      }
      
      if (!$this->m_destsearch[$value] && $this->createDestination()) 
      {
        $this->m_destsearch[$value] = $this->m_destInstance->selectDb(implode(" OR ",$searchConditions));
      }
      
      foreach ($this->m_destsearch[$value] as $result)
      {
        $destresult = $this->decrypt($result,$this->m_linkbackfield);
        if ($destresult) $destresults[] = $destresult;
      }
      
      if ($query->m_joinaliases[$this->m_ownerInstance->m_table."*".$this->m_ownerInstance->primaryKeyField()]) $table = $query->m_joinaliases[$this->m_ownerInstance->m_table."*".$this->m_ownerInstance->primaryKeyField()];
      else if (in_array($this->m_ownerInstance->m_table, $query->m_tables)) $table = $this->m_ownerInstance->m_table;
      else $table=null;
      
      if (!empty($destresults) && $table)
        return $table.".".$this->m_ownerInstance->primaryKeyField()." IN (".implode(",",$destresults).")";
    }
  }
?>
