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
   */
  atkimport("atk.utils.atkencryption");
  userelation("atkonetoonerelation");
  
  /**
   * Base class for all ATK encryption methods
   *
   * @author Mark Baaijens <mark@ibuildings.nl>
   *
   * @package atk
   * @subpackage utils
   *
   */
  class atkSecureRelation extends atkOneToOneRelation
  {
    var $m_crypt = NULL;
    var $m_linktable;
    var $m_linkfield;
    var $m_linkuserfield = "username";
    var $m_keylength;

    /**
     * Creates an atkSecureRelation, 
     * similar to an atkOneToOne relation only encrypted
     *
     * @param string $name        The unique name of the attribute. In slave mode, 
     *                            this corresponds to the foreign key field in the
     *                            database table. 
     * @param string $destination The destination node (in module.nodename
     *                            notation)
     * @param string $linktable   The table we link to
     * @param string $linkfield   The field we link to
     * @param int $keylength      The length of the encryption key
     * @param string $encryption  The type of encryption to use
     * @param string $refKey=""   In master mode, this specifies the foreign key
     *                            field from the destination node that points to
     *                            the master record. In slave mode, this parameter
     *                            should be empty.
     * @param int $flags          Attribute flags that influence this attributes'
     *                            behavior.     
     */
    function atkSecureRelation($name, $destination, $linktable, $linkfield, $keylength, $encryption, $refKey="", $flags=0)
    {
      $this->m_crypt = atkEncryption::getEncryption($encryption);
    
      $this->m_linktable = $linktable;
      $this->m_linkfield = $linkfield;
      $this->m_keylength = $keylength;

      $this->atkOneToOneRelation($name, $destination, $refKey, $flags);
    }
  
    /**
     * Gets the password for the link
     * @param string $linktable the table where we link to (?)
     * @param string $linkfield the field where we link to (?)
     * @return string           the password for the link
     */
    function getLinkPassword($linktable,$linkfield)
    {
      global $g_db;
      
      $user = &getUser();
      $username = $user['name'];
  
      if($username == "administrator")
      {
        $linkpass = atkconfig("administratorpassword");
      }
      else
      {
        $password = $user['password'];
      
        $query = "SELECT ".$linkfield." as pass FROM ".$linktable." WHERE ".atkconfig("auth_userfield")." = '".$username."'";

        $rec = $g_db->getrows($query);
        if(count($rec) < 1)
          return $linkpass;

        $encryptedpass = array_pop($rec);
        //atkerror($encryptedpass['pass']." ");
        $crypt = atkEncryption::getEncryption("base64");
        $linkpass = $crypt->decrypt($encryptedpass['pass'],$password);
      }
    
      //atkerror("linkpass: ".$linkpass);
      return $linkpass;
    }
  
    /**
     * This function in the atkOneToOneRelation store the record of the parentnode in the DB
     * with the reference key of the other table. 
     * So we encrypt the reference key before we call the method.
     * For more documentation see the atkOneToOneRelation
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
    {
      if($mode=="add")
      {
        $link = $rec[$this->fieldName()][$this->m_destInstance->m_primaryKey[0]];
        $cryptedlink = $this->m_crypt->encrypt($link,$this->getLinkPassword($this->m_linkfield,$this->m_linktable));
        $rec[$this->fieldName()][$this->m_destInstance->m_primaryKey[0]] = $cryptedlink;
      }

      return parent::addToQuery($query, $tablename, $fieldaliasprefix, $rec, $level, $mode);
    }
  
    /**
     * This function in the atkOneToOneRelation loads the record of the childnode from the DB
     * with the the id from de reference key in childnode. 
     * So we decrypt the reference key before we call the method. 
     * For more documentation see the atkOneToOneRelation
     */
    function load($db, $record, $mode)
    {
      $link = $record[$this->fieldName()];
      $cryptedlink = $this->m_crypt->decrypt($link,$this->getLinkPassword($this->m_linkfield,$this->m_linktable));
      $record[$this->fieldName()] = $cryptedlink;

      return parent::load($db, $record, $mode);
    }
  
    /**
     * Creates a new user (?)
     * @param string $user the name of the user to create
     * @param string $pass the password for the user
     */
    function newUser($user,$pass)
    {
      global $g_db;
      $linkpass = $this->m_crypt->encrypt($this->getLinkPassword($linktable,$linkfield),$pass);
    
      $query = "UPDATE $this->m_linktable SET $this->m_linkfield = '$linkpass' WHERE $this->m_linkuserfield = '$user' LIMIT 1";
      $g_db->query($query);
    }
  }
?>