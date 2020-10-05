<?php

namespace Sintattica\Atk\Relations;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Utils\StringParser;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Db\QueryPart;
use Sintattica\Atk\Db\Db;

/**
 * The atkRelation class defines a relation to another node.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @abstract
 */
class Relation extends Attribute
{
    /**
     * @var String Destination node.
     */
    public $m_destination;

    /** @var Node $m_destInstance Destination instance */
    public $m_destInstance;

    /**
     * Filters for destination records.
     *
     * These filters may include [attribute] placeholders that will be replaced in
     * query by current record (from  owner's node) 'attribute' value (in a
     * parametrized form).
     *
     * @var array of QueryParts
     */
    public $m_destinationFilters = [];

    /**
     * Descriptor template for destination node.
     *
     * @var String
     */
    public $m_descTemplate = null;

    /**
     * Descriptor handler.
     * @var Object
     */
    public $m_descHandler = null;

    /**
     * Since most relations do not store anything in a field, the default
     * fieldtype for relations is FT_UNSUPPORTED. Exceptions (like the
     * many2oone relation, which stores a foreign key) can implement their
     * own dbFieldType().
     * @var int
     */
    public $m_dbfieldtype = Db::FT_UNSUPPORTED;

    /**
     * Constructor.
     *
     * @param string $name The name of the relation.
     * @param int $flags Flags for the relation
     * @param string $destination The destination node (in module.name notation)
     */
    public function __construct($name, $flags = 0, $destination)
    {
        parent::__construct($name, $flags);
        $this->m_destination = $destination;
    }

    /**
     * Returns the destination filters.
     *
     * Note : if you want the condition to add to a SQL query, use
     * parseFilter($record).
     *
     * @return array of strings The destination filters.
     */
    public function getDestinationFilter()
    {
        return $this->m_destinationFilters;
    }

    /**
     * Sets the destination filter.
     *
     * The $filter SQL expression (either as a string or as a QueryPart object) may
     * contain [attribute] placeholders.
     *
     * @param string|QueryPart $filter The destination filter.
     * @param array $params if $filter is a SQL string with placeholders for parameters,
     *                      this array contains parameters for $filter.
     */
    public function setDestinationFilter($filter, $params = [])
    {
        $this->m_destinationFilters = [];
        $this->addDestinationFilter($filter, $params);
    }

    /**
     * Adds a filter value to the destination filter.
     *
     * @param string|QueryPart $filter The destination filter.
     * @param array $params if $filter is a SQL string with placeholders for parameters,
     *                      this array contains parameters for $filter.
     *
     * @return $this
     */
    public function addDestinationFilter($filter, $params = [])
    {
        if ($filter instanceof QueryPart) {
            $this->m_destinationFilters[] = $filter;
        } elseif (is_string($filter) and !empty($filter)) {
            $this->m_destinationFilters[] = new QueryPart($filter, $params);
        }
        return $this;
    }

    /**
     * Get descriptor handler.
     *
     * @return object descriptor handler
     */
    public function getDescriptorHandler()
    {
        return $this->m_descHandler;
    }

    /**
     * Set descriptor handler.
     *
     * @param object $handler The descriptor handler.
     */
    public function setDescriptorHandler($handler)
    {
        $this->m_descHandler = $handler;
    }

    /**
     * Sets the descriptor template for the destination node.
     *
     * @param string $template The descriptor template.
     */
    public function setDescriptorTemplate($template)
    {
        $this->m_descTemplate = $template;
    }

    /**
     * Forwards description handler calls to the real description handler.
     *
     * Never call this function directly:
     * A. If $this->m_descHandler is set, then :
     *  - createDestination will set m_destInstance descHandler to $this
     *  - m_destInstance->descriptor() will call $this->descriptor (this function)
     *  - this function will call $this->m_descHandler->[name]_descriptor (if exists)
     *  - if above method does not exist, it will call $this->m_descHandler->descriptor.
     * B. If $this->m_descHandler is not set and $this->m_descTemplate is set then :
     *  - createDestination will set m_destInstance descTemplate to $this->m_descTemplate.
     *  - m_destInstance->descriptor() will parse the common descTemplate.
     * C. If neither $this->m_descHandler nor $this->m_descTemplate is set then :
     *  - createDestination will not do anything
     *  - m_destInstance->descriptor() will work as usual.
     *
     * @param array $record The record
     * @param Node $node The atknode object
     *
     * @return string with the descriptor
     */
    public function descriptor($record, $node)
    {
        $method = $this->m_name.'_descriptor';
        if (method_exists($this->m_descHandler, $method)) {
            return $this->m_descHandler->$method($record, $node);
        } else {
            return $this->m_descHandler->descriptor($record, $node);
        }
    }

    /**
     * Create the instance of the destination.
     *
     * If succesful, the instance is stored in the m_destInstance member variable.
     *
     * @return bool true if succesful, false if something went wrong.
     */
    public function createDestination()
    {
        if (!is_object($this->m_destInstance)) {
            $atk = Atk::getInstance();
            $cache_id = $this->m_owner.'.'.$this->m_name;
            $this->m_destInstance = $atk->atkGetNode($this->m_destination, true, $cache_id);

            // Validate if destination was created succesfully
            if (!is_object($this->m_destInstance)) {
                Tools::atkerror("Relation with unknown nodetype '".$this->m_destination."' (in node '".$this->m_owner."')");
                $this->m_destInstance = null;

                return false;
            }

            if ($this->hasFlag(self::AF_NO_FILTER)) {
                $this->m_destInstance->m_flags |= Node::NF_NO_FILTER;
            }

            foreach (array_keys($this->m_destInstance->m_attribList) as $key) {
                $attribute = $this->m_destInstance->m_attribList[$key];

                if (is_subclass_of($attribute, 'Relation') && is_object($this->m_ownerInstance) && $attribute->m_destination == $this->m_ownerInstance->atkNodeUri()
                ) {
                    $attribute->m_destInstance = $this->m_ownerInstance;

                    if (Tools::count($attribute->m_tabs) == 1 && $attribute->m_tabs[0] == 'default') {
                        $attribute->setTabs($this->m_tabs);
                    }
                }
            }

            if (!empty($this->m_descHandler)) {
                $this->m_destInstance->setDescriptorHandler($this);
            }

            if (!empty($this->m_descTemplate)) {
                $this->m_destInstance->setDescriptorTemplate($this->m_descTemplate);
            }
        }

        return true;
    }

    public function display($record, $mode)
    {
        return $record[$this->fieldName()];
    }

    /**
     * Validation method. Empty implementation. Derived classes may override
     * this function.
     *
     * @abstract
     *
     * @param array $record The record that holds the value for this
     *                       attribute. If an error occurs, the error will
     *                       be stored in the 'atkerror' field of the record.
     * @param string $mode The mode for which should be validated ("add" or
     *                       "update")
     */
    public function validate(&$record, $mode)
    {
    }

    /**
     * Check if the relation is empty.
     *
     * @param array $record The record to check
     *
     * @return bool true if a destination record is present. False if not.
     */
    public function isEmpty($record)
    {
        if ($this->createDestination() && isset($record[$this->fieldName()][$this->m_destInstance->primaryKeyField()])) {
            return empty($record[$this->fieldName()][$this->m_destInstance->primaryKeyField()]);
        } else {
            if ($this->createDestination() && isset($record[$this->fieldName()])) {
                return empty($record[$this->fieldName()]);
            }
        }

        return true; // always empty if error.
    }

    /**
     * Retrieve the searchmodes supported by the relation.
     *
     * @return array A list of supported searchmodes.
     */
    public function getSearchModes()
    {
        // exact match and substring search should be supported by any database.
        // (the LIKE function is ANSI standard SQL, and both substring and wildcard
        // searches can be implemented using LIKE)
        // Possible values
        //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
        return array('exact');
    }

    /**
     * Get the searchmode for nested/child attributes.
     *
     * @param string|array $searchmode searchmode
     * @param string $childname the child attribute's name
     *
     * @return string|array the child searchmode
     */
    protected function getChildSearchMode($searchmode, $childname)
    {
        if (is_array($searchmode) && isset($searchmode[$childname])) {
            return $searchmode[$childname];
        }

        return $searchmode;
    }

    /**
     * Returns the condition (SQL) that should be used when we want to join a relation's
     * owner node with the parent node.
     *
     * @param Query $query The query object
     * @param string $tablename The tablename
     * @param string $fieldalias
     *
     * @return string SQL string for joining the owner with the destination.
     *                Defaults to false.
     */
    public function getJoinCondition($tablename = '', $fieldalias = '')
    {
        return false;
    }

    /**
     * Returns an instance of the node that the relation points to.
     *
     * @return Node The node that this relation points to, or NULL if the destination is not valid.
     */
    public function getDestination()
    {
        if ($this->createDestination()) {
            return $this->m_destInstance;
        }

        return null;
    }

    /**
     * Attempts to get a translated label which can be used when composing an "add" link.
     *
     * @return string Localised "add" label
     */
    public function getAddLabel()
    {
        $key = 'link_'.$this->fieldName().'_add';
        $label = Tools::atktext($key, $this->m_ownerInstance->m_module, $this->m_ownerInstance->m_type, '', '', true);
        if ($label == '') {
            $label = Tools::atktext($key, $this->m_destInstance->m_module, '', '', '', true);
            if ($label == '') {
                $key = 'link_'.Tools::getNodeType($this->m_destination).'_add';
                $label = Tools::atktext($key, $this->m_destInstance->m_module, '', '', '', true);
                if ($label == '') {
                    $label = Tools::atktext('link_add', 'atk');
                }
            }
        }

        return $label;
    }

    /**
     * Parses the destination filter into a QueryPart condition
     *
     * @param array $record the current record
     *
     * @return QueryPart $filter condition
     */
    public function parseFilter($record)
    {
        $conditions = [];
        foreach ($this->m_destinationFilters as $filter) {
            $filter->parse($record);
            $conditions[] = $filter;
        }

        if (empty($conditions)) {
            // The 'always-true' condition
            return new QueryPart('1=1');
        } else {
            return QueryPart::implode('AND', $conditions, true);
        }
    }
}
