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
     * @param int $keylength      The length of the encryption key
     * @param string $refKey=""   In master mode, this specifies the foreign 
     *                            key field from the destination node that 
     *                            points to the master record. In slave mode, 
     *                            this parameter should be empty.
     * @param int $flags          Attribute flags that influence this 
     *                            attributes' behavior.     
     */
    function atkSecureRelation($name, $destination, $linktable, $linkfield, $keylength, $refKey="", $encryption, $flags=0)
    {
      $this->atkOneToOneRelation($name, $destination, $refKey, $flags);
      $this->createDestination();
      $this->m_crypt = &atkEncryption::getEncryption($encryption);
            
      $this->m_linktable = $linktable;
      $this->m_linkfield = $linkfield;
      $this->m_keylength = $keylength;
    }
  
    /**
     * Gets the password for the link
     * for more security the administrator gets a random password. You have to capture in your application that
     * the administrator is only able to insert the first record in this relation and make also a useracount with it.
     * @param string $linktable the table where we find the linkpass
     * @param string $linkfield the field where we find the encrypted linkpass
     * @return string           the password for the link
     */
    function getLinkPassword($linktable,$linkfield,$encryption="")
    {
      global $g_db;
      
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

        $rec = $g_db->getrows($query);
        if(count($rec) < 1)
          return $linkpass;
          
        $encryptedpass = array_pop($rec);

        $linkpass = $encryptedpass['pass'];
      }

      return $crypt->decryptKey($linkpass,$password);
    }
  
    /**
     * This function in the atkOneToOneRelation store the record of the parentnode in the DB
     * with the reference key of the other table. 
     * So we encrypt the reference key before we call the method.
     * For more documentation see the atkOneToOneRelation
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
        $query->addCondition($this->m_ownerInstance->m_table.".".$this->m_ownerInstance->primaryKeyField()." IN ('".implode("','", $where)."')");
      }
    }
  
    /**
     * This function in the atkOneToOneRelation loads the record of the childnode from the DB
     * with the the id from de reference key in childnode. 
     * So we decrypt the reference key before we call the method. 
     * For more documentation see the atkOneToOneRelation
     */
    function load($db, $record, $mode)
    {          
      if ($this->m_searching)
      {
        if (count($this->m_records)==0) // lookup not yet created, first loading stage (preload)
        {
          // perform query on destination node to retrieve all records.
          if ($this->createDestination())
          {
            $this->m_destInstance->m_postvars["atksearch"] = $this->m_ownerInstance->m_postvars["atksearch"]["data"];
            $records = $this->m_destInstance->selectDb();
            
            // create lookup table for easy reference.            
            $linkpass = $this->getLinkPassword($this->m_linktable,$this->m_linkfield);           
            for ($i=0, $_i=count($records); $i<$_i; $i++)
            {
              $link = $records[$i][$this->m_refKey];                              
              $decryptedlink = $this->m_crypt->decrypt($link,$linkpass);
              
              if (!$decryptedlink) atkdebug("Unable to decrypt: $link");
              else
              {
                $this->m_keylookup[$decryptedlink] = $i;
                $this->m_records[] = $records[$i];     
              }
            }
            return $this->m_records;
          }
        }
        else // lookup table present, postload stage
        {
          return $this->m_records[$this->m_keylookup[$record[$this->m_ownerInstance->primaryKeyField()]]];
        }
      }
      else 
      {
        global $g_db;
  
        $query = "SELECT ".$this->fieldName()." FROM ".$this->m_ownerInstance->m_table." WHERE $this->m_ownerInstance->primaryKeyField()=".$record[$this->m_ownerInstance->primaryKeyField()];
        $result = $g_db->getrows($query);
        $link = $result[0][$this->fieldName()];
  
        $cryptedlink = $this->m_crypt->decrypt($link,$this->getLinkPassword($this->m_linktable,$this->m_linkfield));
        $record[$this->fieldName()] = $cryptedlink;
  
        return parent::load($db, $record, $mode); 
      }
    }
  
    /**
     * For creating a new user put the linkpassword in the db
     * @param string $id the id of the user to create
     * @param string $pass the password for the user
     */
    function newUser($id,$pass)
    {
      global $g_db;
      $linkpass = $this->m_crypt->encryptKey($this->getLinkPassword($this->m_linktable,$this->m_linkfield),$pass);
      $query = "UPDATE $this->m_linktable SET $this->m_linkfield = '".$linkpass."' WHERE id = '$id' LIMIT 1";
      $g_db->query($query);
    }
    
    function searchCondition(&$query, $table, $value, $searchmode)
    {
      //dummy implementation, we handle our own search in the destination node.
    }
    
    /**
     * Returns the condition which can be used when calling atkQuery's addJoin() method
     * Joins the relation's owner with the destination
     */
    function _getJoinCondition()
    {
      global $g_db;
      
      // decrypt the encrypted keys to get the tables joined
      $temp_query = "SELECT " . $this->fieldName() . " FROM " . $this->m_ownerInstance->m_table;
      $result = $g_db->getRows($temp_query);
      
      $condition = "";
      $linkpass = $this->getLinkPassword($this->m_linktable,$this->m_linkfield);
      foreach($result as $recordArray)
      {
        $record = $recordArray[$this->fieldName()];
        $decrypted_record = $this->m_crypt->decrypt($record,$linkpass);
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
  }
?>
