<?php

namespace Sintattica\Atk\RecordList;

use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Attributes\Attribute;

/**
 * The ColumnConfig class is used to add extended sorting and grouping
 * options to a recordlist.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ColumnConfig
{
    public $m_colcfg = [];

    /* @var Node $m_node */
    public $m_node;

    public $m_orderbyindex = 0;

    public $m_custom_atkorderby;

    /**
     * Constructor.
     *
     * @return ColumnConfig
     */
    public function __construct()
    {
    }

    /**
     * Set the node.
     *
     * @param Node $node
     */
    public function setNode($node)
    {
        $this->m_node = $node;
    }

    /**
     * Get the node.
     *
     * @return Node The node
     */
    public function getNode()
    {
        return $this->m_node;
    }

    /**
     * Get an instance of the columnconfig class.
     *
     * @param Node $node
     * @param string $id
     * @param bool $forceNew force new instance?
     *
     * @return ColumnConfig An instance of the columnconfig class
     */
    public static function getConfig($node, $id = null, $forceNew = false)
    {
        static $s_instances = [];

        $sm = SessionManager::getInstance();

        if ($id == null) {
            $id = $node->atkNodeUri();
        }

        if (!isset($s_instances[$id]) || $forceNew) {
            $cc = new self();
            $s_instances[$id] = $cc;
            $cc->setNode($node);

            $colcfg = $sm != null ? $sm->pageVar('atkcolcfg_'.$id) : null;

            if (!is_array($colcfg) || $forceNew) {
                // create new
                Tools::atkdebug('New colconfig initialising');
                $cc->init();
            } else {
                // inherit old config from session.
                Tools::atkdebug('Resuming colconfig from session');
                $cc->m_colcfg = &$colcfg;
            }

            // See if there are any url params which influence this colcfg.
            $cc->doUrlCommands();
        }

        if ($sm != null) {
            $sm->pageVar('atkcolcfg_'.$id, $s_instances[$id]->m_colcfg);
        }

        return $s_instances[$id];
    }

    /**
     * Is this attribute last?
     *
     * @return bool False
     */
    public function isLast()
    {
        return false;
    }

    /**
     * Is this attribute first?
     *
     * @return bool False
     */
    public function isFirst()
    {
        return false;
    }

    /**
     * Move left.
     *
     * @param string $attribute
     */
    public function moveLeft($attribute)
    {
        // ??
    }

    /**
     * Move right.
     *
     * @param string $attribute
     */
    public function moveRight($attribute)
    {
        // ??
    }

    /**
     * Initialize.
     */
    public function init()
    {
        foreach (array_keys($this->m_node->m_attribIndexList) as $i) {
            if (isset($this->m_node->m_attribIndexList[$i]['name']) && ($this->m_node->m_attribIndexList[$i]['name'] != '')) {
                $this->m_colcfg[$this->m_node->m_attribIndexList[$i]['name']] = [];
            }
        }

        if ($this->m_node->getOrder() != '') {
            $this->_addOrderByStatement($this->m_node->getOrder());
        }
    }

    /**
     * Hide a column.
     *
     * @param string $attribute
     */
    public function hideCol($attribute)
    {
        $this->m_colcfg[$attribute]['show'] = 0;
    }

    /**
     * Show a column.
     *
     * @param string $attribute
     */
    public function showCol($attribute)
    {
        $this->m_colcfg[$attribute]['show'] = 1;
    }

    /**
     * Set sort direction.
     *
     * @param string $attribute
     * @param string $direction
     */
    public function setSortDirection($attribute, $direction)
    {
        $this->m_colcfg[$attribute]['direction'] = $direction;
    }

    /**
     * Set sort order.
     *
     * @param string $attribute
     * @param string $value
     */
    public function setSortOrder($attribute, $value)
    {
        if ($value > 0) {
            $this->m_colcfg[$attribute]['sortorder'] = $value;
        } else {
            unset($this->m_colcfg[$attribute]);
        }
    }

    /**
     * Add orderby field.
     *
     * @param string $field
     * @param string $direction
     * @param string $extra
     * @param string $sortorder
     */
    public function addOrderByField($field, $direction, $extra = null)
    {
        if (is_null($sortorder) && $this->getMinSort() <= 1) {
            foreach ($this->m_colcfg as $fld => $config) {
                if (Tools::atkArrayNvl($config, 'sortorder') > 0) {
                    $this->m_colcfg[$fld]['sortorder'] = (int)($this->m_colcfg[$fld]['sortorder']) + 1;
                }
            }
        }

        $this->m_colcfg[$field]['sortorder'] = $sortorder === null ? 1 : $sortorder;
        $this->m_colcfg[$field]['direction'] = strtolower($direction);
        $this->m_colcfg[$field]['extra'] = $extra;
    }

    /**
     * Flatten.
     */
    public function flatten()
    {
        uasort($this->m_colcfg, array(__CLASS__, '_compareSortAttrs'));

        $i = 1;
        foreach ($this->m_colcfg as $field => $config) {
            if (array_key_exists('sortorder', $this->m_colcfg[$field]) && ($this->m_colcfg[$field]['sortorder'] > 0)) {
                $this->m_colcfg[$field]['sortorder'] = $i;
                ++$i;
            }
        }
    }

    /**
     * Get min sort.
     *
     * @return int
     */
    public function getMinSort()
    {
        $min = 999;
        foreach ($this->m_colcfg as $field => $config) {
            if (Tools::atkArrayNvl($config, 'sortorder') > 0) {
                $min = min($min, $config['sortorder']);
            }
        }

        return $min;
    }

    /**
     * Get orderby statement.
     *
     * @return string Orderby statement
     */
    public function getOrderByStatement()
    {
        $result = [];

        foreach ($this->m_colcfg as $field => $config) {
            if (Tools::atkArrayNvl($config, 'sortorder', 0) > 0 && is_object($this->m_node->m_attribList[$field])) {
                $direction = $config['direction'] == 'desc' ? 'DESC' : 'ASC';
                $res = $this->m_node->m_attribList[$field]->getOrderByStatement($config['extra'], '', $direction);
                if ($res) {
                    $result[] = $res;
                }
            }
        }

        return implode(', ', $result);
    }

    /**
     * Get order fields.
     *
     * @return array
     */
    public function getOrderFields()
    {
        $result = [];
        foreach ($this->m_colcfg as $field => $config) {
            if (is_object($this->m_node->m_attribList[$field])) {
                $result[] = $field;
            }
        }

        return $result;
    }

    /**
     * Get sort direction.
     *
     * @param string $attribute
     *
     * @return string The sort direction
     */
    public function getSortDirection($attribute)
    {
        return $this->m_colcfg[$attribute]['direction'];
    }

    /**
     * Get url command.
     *
     * @param string $attribute
     * @param string $command
     *
     * @return string
     */
    public function getUrlCommand($attribute, $command)
    {
        return "atkcolcmd[][$command]=".$attribute;
    }

    /**
     * Get url command params.
     *
     * @param string $attribute
     * @param string $command
     *
     * @return string
     */
    public function getUrlCommandParams($attribute, $command)
    {
        return array("atkcolcmd[][$command]" => $attribute);
    }

    /**
     * Do url command.
     *
     * @param array $cmd
     */
    public function doUrlCommand($cmd)
    {
        if (is_array($cmd)) {
            foreach ($cmd as $command => $param) {
                switch ($command) {
                    case 'asc':
                        $this->setSortDirection($param, 'asc');
                        break;
                    case 'desc':
                        $this->setSortDirection($param, 'desc');
                        break;
                    case 'setorder':
                        list($attrib, $value) = each($param);
                        $this->setSortOrder($attrib, $value);
                        break;
                    case 'subtotal':
                        $this->setSubTotal($param, true);
                        break;
                    case 'unsubtotal':
                        $this->setSubTotal($param, false);
                        break;
                }
            }
        }
    }

    /**
     * Do url commands.
     */
    public function doUrlCommands()
    {
        if (isset($this->m_node->m_postvars['atkcolcmd']) && is_array($this->m_node->m_postvars['atkcolcmd'])) {
            foreach ($this->m_node->m_postvars['atkcolcmd'] as $command) {
                $this->doUrlCommand($command);
            }
        } else {
            if (isset($this->m_node->m_postvars['atkorderby']) && ($this->m_node->m_postvars['atkorderby'] != '')) {
                $this->clearOrder(); // clear existing order
                // oldfashioned order by.
                $this->m_custom_atkorderby = $this->m_node->m_postvars['atkorderby'];

                // try to parse..
                $this->_addOrderByStatement($this->m_node->m_postvars['atkorderby']);
            }
        }

        // Cleanup structure
        $this->flatten();
    }

    /**
     * Get order.
     *
     * @param string $attribute
     *
     * @return string
     */
    public function getOrder($attribute)
    {
        return isset($this->m_colcfg[$attribute]['sortorder']) ? $this->m_colcfg[$attribute]['sortorder'] : 0;
    }

    /**
     * Get direction.
     *
     * @param string $attribute
     *
     * @return string
     */
    public function getDirection($attribute)
    {
        return array_key_exists('direction', $this->m_colcfg[$attribute]) ? $this->m_colcfg[$attribute]['direction'] : 'desc';
    }

    /**
     * Get attribute by order.
     *
     * @param int $order
     *
     * @return Attribute
     */
    public function getAttributeByOrder($order)
    {
        foreach ($this->m_colcfg as $attrib => $info) {
            if (Tools::atkArrayNvl($info, 'sortorder', 0) == $order) {
                return $attrib;
            }
        }

        return '';
    }

    /**
     * Count sort attributes.
     *
     * @return int
     */
    public function countSortAttribs()
    {
        $total = 0;
        foreach ($this->m_colcfg as $attrib => $info) {
            if (Tools::atkArrayNvl($info, 'sortorder', 0) > 0) {
                ++$total;
            }
        }

        return $total;
    }

    /**
     * Get direction by order.
     *
     * @param int $order
     *
     * @return string
     */
    public function getDirectionByOrder($order)
    {
        foreach ($this->m_colcfg as $attrib => $info) {
            if (Tools::atkArrayNvl($info, 'sortorder', 0) == $order) {
                return $this->getDirection($attrib);
            }
        }

        return 'asc';
    }

    /**
     * Clear order.
     */
    public function clearOrder()
    {
        $this->m_colcfg = [];
    }

    /**
     * Has subtotals?
     *
     * @return bool True or false
     */
    public function hasSubTotals()
    {
        foreach (array_keys($this->m_colcfg) as $attribute) {
            if ($this->hasSubTotal($attribute)) {
                return true;
            }
        }

        return false;
    }

    /**
     * Has subtotal?
     *
     * @param string $attribute
     *
     * @return bool True or false
     */
    public function hasSubTotal($attribute)
    {
        return (isset($this->m_colcfg[$attribute]['subtotal']) ? $this->m_colcfg[$attribute]['subtotal'] : 0) == 1;
    }

    /**
     * Has subtotal by order?
     *
     * @param int $order
     *
     * @return bool True or false
     */
    public function hasSubTotalByOrder($order)
    {
        foreach ($this->m_colcfg as $attrib => $info) {
            if (Tools::atkArrayNvl($info, 'sortorder', 0) == $order) {
                return $this->hasSubTotal($attrib);
            }
        }

        return false;
    }

    /**
     * Set subtotal.
     *
     * @param string $attribute
     * @param bool $active
     */
    public function setSubTotal($attribute, $active)
    {
        $this->m_colcfg[$attribute]['subtotal'] = ($active ? 1 : 0);
    }

    /**
     * Compare sortorder of two attributes.
     *
     * @param array $a
     * @param array $b
     *
     * @return int
     */
    public function _compareSortAttrs($a, $b)
    {
        return Tools::atkArrayNvl($a, 'sortorder', 0) <= Tools::atkArrayNvl($b, 'sortorder', 0) ? -1 : 1;
    }

    /**
     * Totalizable?
     *
     * @return bool True or false
     */
    public function totalizable()
    {
        foreach (array_keys($this->m_node->m_attribList) as $attribname) {
            $p_attrib = $this->m_node->m_attribList[$attribname];
            if ($p_attrib->hasFlag($p_attrib::AF_TOTAL)) {
                return true;
            }
        }

        return false;
    }

    /**
     * Get the totalizable columns.
     *
     * @return array
     */
    public function totalizableColumns()
    {
        $result = [];
        foreach (array_keys($this->m_node->m_attribList) as $attribname) {
            $p_attrib = $this->m_node->m_attribList[$attribname];
            if ($p_attrib->hasFlag($p_attrib::AF_TOTAL)) {
                $result[] = $attribname;
            }
        }

        return $result;
    }

    /**
     * Get the subtotal columns.
     *
     * @return array
     */
    public function subtotalColumns()
    {
        $result = [];
        foreach (array_keys($this->m_colcfg) as $attribute) {
            if ($this->hasSubTotal($attribute)) {
                $result[] = $attribute;
            }
        }

        return $result;
    }

    /**
     * Add orderby statement.
     *
     * @param string $orderby
     */
    public function _addOrderByStatement($orderby)
    {
        if (strpos($orderby, '(') !== false) {
            return; // can't do anything with complex order by's
        }

        $expression = trim($orderby);
        $expressionParts = preg_split('/\\s+/', $expression);

        if (in_array(strtoupper($expressionParts[count($expressionParts)-1]), ['DESC', 'ASC'])) {
            $direction = array_pop($expressionParts);
            $column = implode(' ', $expressionParts);
        } else {
            $column = $expression;
            $direction = 'ASC';
        }

        $direction = strtoupper($direction) == 'DESC' ? 'DESC' : 'ASC';

        $part1 = $column;
        $part2 = null;

        if (strpos($column, '.') !== false) {
            list($part1, $part2) = explode('.', $column);
        }

        if ($this->getNode()->getAttribute($part1) != null) {
            $this->addOrderByField($part1, $direction, $part2);
        } else {
            if ($part1 == $this->getNode()->getTable() && $this->getNode()->getAttribute($part2) != null) {
                $this->addOrderByField($part2, $direction);
            } else {
                // custom order by
                $this->addOrderByField($column, $direction);
            }
        }
    }
}
