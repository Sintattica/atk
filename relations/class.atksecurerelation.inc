<?php

atkimport("atk.utils.atkencryption");
userelation("atkonetoonerelation");

class atkSecureRelation extends atkOneToOneRelation
{
  var $m_crypt = NULL;
  var $m_linktable;
  var $m_linkfield;
  var $m_linkuserfield = "username";
  var $m_keylength;

  function atkSecureRelation($name, $destination, $linktable, $linkfield, $keylength, $encryption, $refKey="", $flags=0)
  {
    $this->m_crypt = atkEncryption::getEncryption($encryption);
    
    $this->m_linktable = $linktable;
    $this->m_linkfield = $linkfield;
    $this->m_keylength = $keylength;

    $this->atkOneToOneRelation($name, $destination, $refKey, $flags);
  }
  
  function getLinkPassword($linktable,$linkfield)
  {
    $user = &getUser();
    $username = $user['name'];
    return atkconfig("administratorpassword");
  
    if($username == "administrator")
    {
      $linkpass = atkconfig("administratorpassword");
    }
    else
    {
      global $g_db;
      
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
  
  function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
  {
    if($mode=="add")
    {
      //this function in atkonotoonerelation store the record of the parentnode in the db
      //with the refkey of the other table. so we encrypt the refkey before we get in the function.
      $link = $rec[$this->fieldName()][$this->m_destInstance->m_primaryKey[0]];
      $cryptedlink = $this->m_crypt->encrypt($link,$this->getLinkPassword($this->m_linkfield,$this->m_linktable));
      $rec[$this->fieldName()][$this->m_destInstance->m_primaryKey[0]] = $cryptedlink;
    }

    return parent::addToQuery($query, $tablename, $fieldaliasprefix, $rec, $level, $mode);
  }
  
  function load($db, $record, $mode)
  {
    //this function in atkonetoonerelation loads the record of the childnode form the db
    //with the the id from de refkey in childnode. so we decrypt the refkey before we get in the function
    $link = $record[$this->fieldName()];
    $cryptedlink = $this->m_crypt->decrypt($link,$this->getLinkPassword($this->m_linkfield,$this->m_linktable));
    $record[$this->fieldName()] = $cryptedlink;

    return parent::load($db, $record, $mode);
  }
  
  function newUser($user,$pass)
  {
    global $g_db;
    
    $linktable = $this->m_linktable;
    $linkfield = $this->m_linkfield;
    $linkuserfield = $this->m_linkuserfield;
    $linkpass = $this->m_crypt->encrypt($this->getLinkPassword($linktable,$linkfield),$pass);
    
    $query = "UPDATE $linktable SET $linkfield = '$linkpass' WHERE $linkuserfield = '$user' LIMIT 1";
    $g_db->query($query);
  }
}
?>
