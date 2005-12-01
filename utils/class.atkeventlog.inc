<?php

  atkimport("atk.utils.atkactionlistener");
  
  class atkEventLog extends atkActionListener
  {
    function notify($action, $record)
    {
      $user = &getUser();
      $userid = $user[atkconfig("auth_userpk")];
      if ($userid=="") $userid=0; // probably administrator
      $node = $this->m_node->atkNodeType();
      $db = &$this->m_node->getDb();
      $primarykey = $db->escapeSQL($this->m_node->primaryKey($record));
      
      $db->query("INSERT INTO atkeventlog (id, userid, stamp, node, action, primarykey)
                    VALUES(".$db->nextid("atkeventlog").", $userid, ".$db->func_now().", '$node', '$action', '$primarykey')");
      $db->commit();
    }
  }

?>
