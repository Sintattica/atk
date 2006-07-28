<?php

  require_once(atkconfig("atkroot")."atk/security/class.auth_db.inc");

  class auth_ijmis extends auth_db
  { 
    function validateUser($user, $passwd)
    {
      // ijmis plugin negeert user/password, we kijken naar de sessie van ijmis om te zien of we ingelogd zijn.
      if (isset($_SESSION["mis_toegang"]) && $_SESSION["mis_toegang"]=="OK")
      {
        return AUTH_SUCCESS;
      }
      return parent::validateUser($user, $passwd);
    }
    
    function getUser($user)
    {
      $db = &atkGetDb('ijmis');
      $recs = $db->getrows("SELECT gebruikersnaam, voorletters, tussenvoegsel, naam FROM medewerkers WHERE id=".$_SESSION["medewerker_id"]);
      $user["medewerker_id"] = $_SESSION["medewerker_id"];
      $user["login"] = $recs[0]["gebruikersnaam"];
      $user["name"] = $recs[0]["voorletters"]." ".$recs[0]["tussenvoegsel"]." ".$recs[0]["naam"];
      
      return $user;
    }
  }

?>