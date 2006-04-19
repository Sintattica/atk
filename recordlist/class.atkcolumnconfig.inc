<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage recordlist
   *
   * @copyright (c)2003-2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Sort ascending
   */
  define("RL_SORT_ASC" , 1);

  /**
   * Sort descending
   */
  define("RL_SORT_DESC", 2);

  /**
   * The atkColumnConfig class is used to add extended sorting and grouping
   * options to a recordlist.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage recordlist
   */
  class atkColumnConfig
  {
    var $m_colcfg = array();
    var $m_node = NULL;
    var $m_orderbyindex = 0;
    var $m_custom_atkorderby;

    function atkColumnConfig()
    {
    }

    function setNode(&$node)
    {
      $this->m_node = $node;
    }

    function &getConfig(&$node)
    {
      global $g_sessionManager;
      static $s_instances = array();

      $id = $node->atkNodeType();

      if (!isset($s_instances[$id]))
      {
        $s_instances[$id] = &new atkColumnConfig();
        $s_instances[$id]->setNode($node);

        $colcfg = $g_sessionManager->pageVar("atkcolcfg_".$id);

        if (!is_array($colcfg))
        {
          // create new
          atkdebug("New colconfig initialising");
          $s_instances[$id]->init();
        }
        else
        {
          // inherit old config from session.
          atkdebug("Resuming colconfig from session");
          $s_instances[$id]->m_colcfg = &$colcfg;
        }

        // See if there are any url params which influence this colcfg.
        $s_instances[$id]->doUrlCommands();

      }

      $g_sessionManager->pageVar("atkcolcfg_".$id, $s_instances[$id]->m_colcfg);

      return $s_instances[$id];
    }

    function isLast($attribute)
    {
      return false;
    }

    function isFirst($attribute)
    {
      return false;
    }

    function moveLeft($attribute)
    {
      // ??
    }

    function moveRight($attribute)
    {
      // ??
    }

    function init()
    {
      foreach (array_keys($this->m_node->m_attribIndexList) as $i)
      {
        if (isset($this->m_node->m_attribIndexList[$i]["name"]) && ($this->m_node->m_attribIndexList[$i]["name"]!=""))
        {
          $this->m_colcfg[$this->m_node->m_attribIndexList[$i]["name"]] = array();
        }
      }
    }



    function hideCol($attribute)
    {
      $this->m_colcfg[$attribute]["show"]=0;
    }

    function showCol($attribute)
    {
      $this->m_colcfg[$attribute]["show"]=1;
    }

    function setSortDirection($attribute, $direction)
    {
      $this->m_colcfg[$attribute]["direction"] = $direction;
    }

    function setSortOrder($attribute, $value)
    {
      if ($value>0)
      {
        $this->m_colcfg[$attribute]["sortorder"] = $value;
      }
      else
      {
        unset($this->m_colcfg[$attribute]);
      }
    }

    function addOrderByField($field, $direction)
    {
      if ($this->getMinSort()<=1)
      {
        foreach($this->m_colcfg as $fld=>$config)
        {
          if ($config["sortorder"]>0)
          {
            //var_dump($this->m_colcfg[$fld]["sortorder"]);
            $this->m_colcfg[$fld]["sortorder"] = (int)($this->m_colcfg[$fld]["sortorder"])+1;
          }
        }
      }
      $this->m_colcfg[$field]["sortorder"] = 1;
      $this->m_colcfg[$field]["direction"] = $direction;

    }

    function flatten()
    {
      $result = uasort($this->m_colcfg, array("atkColumnConfig", "_compareSortAttrs"));

      $i=1;
      foreach ($this->m_colcfg as $field=>$config)
      {
        if ($this->m_colcfg[$field]["sortorder"]>0)
        {
          $this->m_colcfg[$field]["sortorder"] = $i;
          $i++;
        }
      }
    }

    function getMinSort()
    {
      $min = 999;
      foreach ($this->m_colcfg as $field=>$config)
      {
        if ($config["sortorder"]>0)
        {
          $min = min($min, $config["sortorder"]);
        }
      }
      //echo $min;
      atkdebug("min is ".$min);
      return $min;
    }

    function getOrderByStatement()
    {
      $result = array();

      foreach($this->m_colcfg as $field=>$config)
      {
        if ($config["sortorder"]>0 && is_object($this->m_node->m_attribList[$field]))
        {
          $direction = ($config["direction"]=="desc"?" DESC":"");
          $res = $this->m_node->m_attribList[$field]->getOrderByStatement().$direction;
          if ($res) $result[] = $res;
        }
      }
      return implode(", ", $result);
    }

    function getOrderFields()
    {
      $result = array();
      foreach($this->m_colcfg as $field=>$config)
      {
        if (is_object($this->m_node->m_attribList[$field]))
        {
          $result[] = $field;
        }
      }
      return $result;
    }

    function getSortDirection($attribute)
    {
      return $this->m_colcfg[$attribute]["direction"];
    }

    function getUrlCommand($attribute, $command)
    {
      return "atkcolcmd[][$command]=".$attribute;
    }

    function doUrlCommand($cmd)
    {
      if (is_array($cmd))
      {
        foreach ($cmd as $command=>$param)
        {
          switch ($command)
          {
            case "asc": $this->setSortDirection($param, "asc"); break;
            case "desc": $this->setSortDirection($param, "desc"); break;
            case "setorder":
              list($attrib, $value) = each($param);
              $this->setSortOrder($attrib, $value);
              break;
            case "subtotal": $this->setSubTotal($param, true); break;
            case "unsubtotal": $this->setSubTotal($param, false); break;
          }
        }
      }
    }

    function doUrlCommands()
    {
      global $ATK_VARS;

      if (isset($ATK_VARS["atkcolcmd"]) && is_array($ATK_VARS["atkcolcmd"]))
      {
        foreach($ATK_VARS["atkcolcmd"] as $command)
        {
          $this->doUrlCommand($command);
        }
      }
      else if (isset($ATK_VARS["atkorderby"]) && ($ATK_VARS["atkorderby"]!=""))
      {
        // oldfashioned order by.
        $this->m_custom_atkorderby = $ATK_VARS["atkorderby"];

        // try to parse..
        $elems = explode(" ", $ATK_VARS["atkorderby"]);
        $lastattr="";
        foreach ($elems as $elem)
        {
          if (is_object($this->m_node->m_attribList[$elem]))
          {
            // attribute reference
            $lastattr=$elem;
            $this->addOrderByField($elem, "asc");
          }
          else if (strtolower($elem)=="asc"||strtolower($elem)=="desc")
          {
            // direction modifier for previous statement
            if ($lastattr!="")
            {
              $this->setSortDirection($lastattr, strtolower($elem));
            }
          }
          else
          {
            // custom order by
            $lastattr="";
            list($table, $field) = explode(".", $elem);

            //echo $table."-".$field;
            if (($table == $this->m_node->m_type) && is_object($this->m_node->m_attribList[$field]))
            {
              //echo 'yes';
              $this->addOrderByField($field, "asc");
            }
          }
        }
      }

      // Cleanup structure
      $this->flatten();

    }

    function getOrder($attribute)
    {
      return $this->m_colcfg[$attribute]["sortorder"];
    }

    function getDirection($attribute)
    {
      return ($this->m_colcfg[$attribute]["direction"]!=""?$this->m_colcfg[$attribute]["direction"]:"asc");
    }

    function getAttributeByOrder($order)
    {
      foreach($this->m_colcfg as $attrib=>$info)
      {
        if ($info["sortorder"]==$order)
        {
          return $attrib;
        }
      }
      return "";
    }

    function countSortAttribs()
    {
      $total = 0;
      foreach ($this->m_colcfg as $attrib=>$info)
      {
        if ($info["sortorder"]>0) $total++;
      }
      return $total;
    }

    function getDirectionByOrder($order)
    {
      foreach($this->m_colcfg as $attrib=>$info)
      {
        if ($info["sortorder"]==$order) return $this->getDirection($attrib);
      }
      return "asc";
    }

    function clearOrder()
    {
      $this->m_colcfg = array();
    }

    function hasSubTotals()
    {
      foreach(array_keys($this->m_colcfg) as $attribute)
      {
        if ($this->hasSubTotal($attribute)) return true;
      }
      return false;
    }

    function hasSubTotal($attribute)
    {
      return ($this->m_colcfg[$attribute]["subtotal"]==1);
    }

    function hasSubTotalByOrder($order)
    {
      foreach($this->m_colcfg as $attrib=>$info)
      {
        if ($info["sortorder"]==$order) return $this->hasSubTotal($attrib);
      }
      return false;
    }

    function setSubTotal($attribute, $active)
    {
      $this->m_colcfg[$attribute]["subtotal"] = ($active?1:0);
    }

    function _compareSortAttrs($a, $b)
    {
      return ($a["sortorder"]<=$b["sortorder"]?-1:1);
    }

    function totalizable()
    {
      foreach(array_keys($this->m_node->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_node->m_attribList[$attribname];
        if ($p_attrib->hasFlag(AF_TOTAL)) return true;
      }
      return false;
    }

    function totalizableColumns()
    {
      $result = array();
      foreach(array_keys($this->m_node->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_node->m_attribList[$attribname];
        if ($p_attrib->hasFlag(AF_TOTAL)) $result[] = $attribname;
      }
      return $result;
    }

    function subtotalColumns()
    {
      $result = array();
      foreach(array_keys($this->m_colcfg) as $attribute)
      {
        if ($this->hasSubTotal($attribute)) $result[]=$attribute;
      }
      return $result;
    }

  }

?>