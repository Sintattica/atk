<?php
 /*
  * function to get multilanguage strings
  *
  * This is actually a wrapper for ATK's atktext() method, for
  * use in templates.
  *
  * @author Ivo Jansch <ivo@achievo.org>
  *
  * Example: {atktext id="users.userinfo.description"}
  *          {atktext id="userinfo.description" module="users"}
  *          {atktext id="description" module="users" node="userinfo"}
  *
  */
 function smarty_function_atktext($params, &$smarty)
 {
   switch(substr_count($params["id"], "."))
   {
     case 1:
     {
       list($module, $id) = explode(".", $params["id"]);
       return atktext($id, $module, $params["node"]);
     }
     case 2:
     {
       list($module, $node, $id) = explode(".", $params["id"]);
       return atktext($id, $module, $node);
     }
     default: return atktext($params["id"], $params["module"], $params["node"], $params["lng"]);
   }
 }

?>