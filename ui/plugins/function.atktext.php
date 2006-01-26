<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage ui
   *
   * @copyright (c)2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
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
   if (!isset($params["id"])) $params["id"] = $params[0];
   switch(substr_count($params["id"], "."))
   {
     case 1:
     {
       list($module, $id) = explode(".", $params["id"]);
       $str = atktext($id, $module, $params["node"]);
       break;
     }
     case 2:
     {
       list($module, $node, $id) = explode(".", $params["id"]);
       $str = atktext($id, $module, $node);
       break;
     }
     default: $str = atktext($params["id"], $params["module"], $params["node"], $params["lng"]);
   }

   // parse the rest of the params in the string
   atkimport("atk.utils.atkstringparser");
   $parser = &new atkStringParser($str);
   return $parser->parse($params);
  }

?>