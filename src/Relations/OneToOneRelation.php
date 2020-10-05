<?php

namespace Sintattica\Atk\Relations;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Db\QueryPart;
use Sintattica\Atk\Core\Node;

/**
 * Implementation of one-to-one relationships.
 *
 * An atkOneToOneRelation defines a relation between two tables where there
 * is one record in the first table that belongs to one record in the
 * second table.
 *
 * When editing a one-to-one relation, the form for the destination record
 * is embedded in the form of the master record. When using the flag
 * self::AF_ONETOONE_INTEGRATE, this is done transparantly so the user does not
 * even notice that the data he's editing comes from 2 separate tables.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class OneToOneRelation extends Relation
{
    /**
     * flags specific for atkOneToOneRelation.
     */
    /**
     * Override the default no add flag.
     */
    const AF_ONETOONE_ADD = 33554432;

    /**
     * Enable error notifications / triggers.
     */
    const AF_ONETOONE_ERROR = 67108864;

    /**
     * Invisibly integrate a onetoonerelation as if the fields where part of the current node.
     * If the relation is integrated, no divider is drawn, and the section heading is suppressed.
     * (Integration does not affect the way data is stored or manipulated, only how it is displayed.).
     */
    const AF_ONETOONE_INTEGRATE = 134217728;

    /**
     * Use lazy loading instead of query addition.
     */
    const AF_ONETOONE_LAZY = 268435456;

    /**
     * Respects tab/sections that have been assigned to this attribute instead of using the
     * tabs assigned for the attributes in the destination node. This flag is only useful in
     * integration mode.
     */
    const AF_ONETOONE_RESPECT_TABS = 536870912;

    /*
     * The name of the referential key attribute in the target node.
     * @access private
     * @var String
     */
    public $m_refKey = '';

    /**
     * Default Constructor.
     *
     * The atkOneToOneRelation supports two configurations:
     * - Master mode: The current node is considered the master, and the
     *                referential key pointing to the master record is in the
     *                destination node.
     * - Slave mode: The current node is considered the child, and the
     *               referential key pointing to the master record is in the
     *               current node.
     * The mode to use is detected automatically based on the value of the
     * $refKey parameter.
     *
     * <b>Example:</b>
     * <code>
     * $this->add(new atkOneToOneRelation("child", "mymod.childnode", "parent_id"));
     * </code>
     *
     * @param string $name The unique name of the attribute. In slave mode,
     *                            this corresponds to the foreign key field in the
     *                            database table.  (The name is also used as the section
     *                            heading.)
     * @param int $flags Attribute flags that influence this attributes' behavior.
     * @param string $destination the destination node (in module.nodename
     *                            notation)
     * @param string $refKey In master mode, this specifies the foreign key
     *                            field from the destination node that points to
     *                            the master record. In slave mode, this parameter
     *                            should be empty.
     */
    public function __construct($name, $flags = 0, $destination, $refKey = '')
    {
        $flags = $flags | self::AF_ONETOONE_LAZY;
        parent::__construct($name, $flags, $destination);
        $this->m_refKey = $refKey;
    }

    public function display($record, $mode)
    {
        if ($mode == 'view') {
            return;
        }

        $myrecord = $record[$this->fieldName()];

        if ($this->createDestination() && is_array($myrecord)) {
            $result = $this->m_destInstance->descriptor($myrecord);
            if (!in_array($mode, array('csv', 'plain'))) {
                $result = htmlspecialchars($result);
            }
        } else {
            $result = $this->text('none');
        }

        return $result;
    }

    public function edit($record, $fieldprefix, $mode)
    {
        // Because of the self::AF_INTEGRATE feature, the edit() method has a void implementation.
        // The actual edit code is handled by addToEditArray().
    }

    /**
     * Set the initial values of this attribute.
     *
     * @return array Array with initial values
     */
    public function initialValue()
    {
        if ($this->m_initialValue !== null) {
            return parent::initialValue();
        }

        if ($this->createDestination()) {
            return $this->m_destInstance->initial_values();
        }

        return null;
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        if (!$this->createDestination()) {
            return;
        }
        if ($mode == 'add' || $mode == 'update') {
            // When storing, we don't add to the query.. we have our own store() method..
            // With one exception. If the foreign key is in the source node, we also need to update
            // the refkey value.
            if ($this->m_refKey == '' && ($mode == 'add'||$mode == 'update')) {
                $query->addField($this->fieldName(), $record[$this->fieldName()][$this->m_destInstance->m_primaryKey[0]]);
            }
            return;
        }
        // No add or update ... we're now in select mode :
        if ($this->hasFlag(self::AF_ONETOONE_LAZY) && $this->m_refKey == '') {
            parent::addToQuery($query, $tablename, $fieldaliasprefix, $record, $level, $mode);
            return;
        }

        if ($this->m_refKey != '') {
            // Foreign key is in the destination node.
            $condition = Db::quoteIdentifier($tablename, $this->m_ownerInstance->m_primaryKey[0]).'='.
                Db::quoteIdentifier($fieldaliasprefix.$this->fieldName(), $this->m_refKey);
        } else {
            // Foreign key is in the source node
            $condition = Db::quoteIdentifier($tablename, $this->fieldName()).'='.
                Db::quoteIdentifier($fieldaliasprefix.$this->fieldName(), $this->m_destInstance->m_primaryKey[0]);
        }
        $query->addJoin($this->m_destInstance->m_table, $fieldaliasprefix.$this->fieldName(), $condition, true);

        // we pass true as the last param to addToQuery, because we need all fields..
        $this->m_destInstance->addToQuery($query, $fieldaliasprefix.$this->fieldName(), $level + 1, true, $mode);
    }

    /**
     * Retrieve detail records from the database.
     *
     * Called by the framework to load the detail records.
     *
     * @param Db $db The database used by the node.
     * @param array $record The master record
     * @param string $mode The mode for loading (admin, select, copy, etc)
     *
     * @return array Recordset containing detailrecords, or NULL if no detail
     *               records are present. Note: when $mode is edit, this
     *               method will always return NULL. This is a framework
     *               optimization because in edit pages, the records are
     *               loaded on the fly.
     */
    public function load($db, $record, $mode)
    {
        if (!$this->createDestination()) {
            return null;
        }
        if ($this->m_refKey == '') {
            // Foreign key in owner
            $condition = Query::simpleValueCondition($this->m_destInstance->m_table, $this->m_destInstance->m_primaryKey[0], $record[$this->fieldName()]);
        } else {
            // Foreign key in destination
            $condition = Query::simpleValueCondition($this->m_destInstance->m_table, $this->m_refKey,
                $this->m_ownerInstance->m_attribList[$this->m_ownerInstance->primaryKeyField()]->value2db($record));

            if (!empty($this->m_destinationFilters)) {
                $condition = QueryPart::implode('AND', [$condition, $this->parseFilter($record)]);
            }
        }

        return $this->m_destInstance->select($condition)->mode($mode)->getFirstRow();
    }

    /**
     * The delete method is called by the framework to inform the attribute
     * that the master record is deleted.
     *
     * Note that the framework only calls the method when the
     * self::AF_CASCADE_DELETE flag is set. When calling this method, the detail
     * record belonging to the master record is deleted.
     *
     * @param array $record The record that is deleted.
     *
     * @return bool true if cleanup was successful, false otherwise.
     */
    public function delete($record)
    {
        $atk = Atk::getInstance();
        $classname = $this->m_destination;
        $cache_id = $this->m_owner.'.'.$this->m_name;
        $rel = $atk->atkGetNode($classname, true, $cache_id);
        Tools::atkdebug("O2O DELETE for $classname: ".$this->m_refKey.'='.$record[$this->m_ownerInstance->primaryKeyField()]);

        if ($this->m_refKey != '') {
            // Foreign key is in the destination node
            $condition = Query::simpleValueCondition($rel->m_table, $this->m_refKey, $this->m_ownerInstance->m_attribList[$this->m_ownerInstance->primaryKeyField()]->value2db($record));
        } else {
            // Foreign key is in the source node.
            $condition = Query::simpleValueCondition($rel->m_table, $rel->m_primaryKey[0], $record[$this->fieldName()][$this->m_ownerInstance->primaryKeyField()]);
        }

        return $rel->deleteDb($condition);
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * For the regular Attribute, this means escaping things like
     * quotes and slashes. Derived attributes may reimplement this for their
     * own conversion.
     * This is the exact opposite of the db2value method.
     *
     * @param array $rec The record that holds this attribute's value.
     *
     * @return string The database compatible value
     */
    public function db2value($rec)
    {
        // we need to pass all values to the destination node, so it can
        // run it's db2value stuff over it..
        if ($this->hasFlag(self::AF_ONETOONE_LAZY) && $this->m_refKey == '') {
            return parent::db2value($rec);
        }

        if ($this->createDestination()) {
            (isset($rec[$this->fieldName()][$this->m_destInstance->primaryKeyField()])) ? $pkval = $rec[$this->fieldName()][$this->m_destInstance->primaryKeyField()] : $pkval = null;
            if ($pkval != null && $pkval != '') { // If primary key is not filled, there was no record, so we
                // should return NULL.
                foreach (array_keys($this->m_destInstance->m_attribList) as $attribname) {
                    $p_attrib = $this->m_destInstance->m_attribList[$attribname];
                    $rec[$this->fieldName()][$attribname] = $p_attrib->db2value($rec[$this->fieldName()]);
                }
                // also set the primkey..
                $rec[$this->fieldName()]['atkprimkey'] = $this->m_destInstance->primaryKey($rec[$this->fieldName()]);

                return $rec[$this->fieldName()];
            }
        }

        return;
    }

    public function fetchMeta($metadata)
    {
        // Initialize this destinations attribute sizes.
        if ($this->hasFlag(self::AF_ONETOONE_INTEGRATE)) {
            $this->createDestination();
            $this->getDestination()->setAttribSizes();
        }
    }

    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * This implementation uses the destination node to fetch any field that
     * belongs to the other side of the relation.
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     *
     * @return array The internal value
     */
    public function fetchValue($postvars)
    {
        // we need to pass all values to the destination node, so it can
        // run it's fetchValue stuff over it..
        if (!$this->createDestination() or !isset($postvars[$this->getHtmlName()])) {
            return null;
        }
        $result = $postvars[$this->getHtmlName()];
        foreach ($this->m_destInstance->m_attribList as $attribname => $p_attrib) {
            $result[$attribname] = $p_attrib->fetchValue($postvars[$this->getHtmlName()]);
        }
        return $result;
    }

    /**
     * Determine the storage type of this attribute.
     *
     * With this method, the attribute tells the framework whether it wants
     * to be stored in the main query (addToQuery) or whether the attribute
     * has its own store() implementation.
     * For the atkOneToOneRelation, the results depends on whether the
     * relation is used in master or slave mode.
     *
     * Framework method. It should not be necesary to call this method
     * directly.
     *
     * @param string $mode The type of storage ("add" or "update")
     *
     * @return int Bitmask containing information about storage requirements.
     *             self::POSTSTORE  when in master mode.
     *             self::PRESTORE|self::ADDTOQUERY when in slave mode.
     */
    public function storageType($mode = null)
    {
        // Mode specific storage type.
        if (isset($this->m_storageType[$mode]) && $this->m_storageType[$mode] !== null) {
            return $this->m_storageType[$mode];
        } // Global storage type (key null is special!)
        else {
            if (isset($this->m_storageType[null]) && $this->m_storageType[null] !== null) {
                return $this->m_storageType[null];
            } else {
                if ($this->m_refKey != '') {
                    // foreign key is in destination node, so we must store the
                    // destination AFTER we stored the master record.
                    return self::POSTSTORE;
                } else {
                    // foreign key is in source node, so we must store the
                    // relation node first, so we can store the foreign key
                    // when we store the master record. To store the latter,
                    // we must also perform an addToQuery.
                    return self::PRESTORE | self::ADDTOQUERY;
                }
            }
        }
    }

    /**
     * Determine the load type of this attribute.
     *
     * With this method, the attribute tells the framework whether it wants
     * to be loaded in the main query (addToQuery) or whether the attribute
     * has its own load() implementation.
     * For the atkOneToOneRelation, this depends on the presence of the
     * self::AF_ONETOONE_LAZY flag.
     *
     * Framework method. It should not be necesary to call this method
     * directly.
     *
     * @param string $mode The type of load (view,admin,edit etc)
     *
     * @return int Bitmask containing information about load requirements.
     *             self::POSTLOAD|self::ADDTOQUERY when self::AF_ONETOONE_LAZY is set.
     *             self::ADDTOQUERY when self::AF_ONETOONE_LAZY is not set.
     */
    public function loadType($mode)
    {
        if (isset($this->m_loadType[$mode]) && $this->m_loadType[$mode] !== null) {
            return $this->m_loadType[$mode];
        } else {
            if (isset($this->m_loadType[null]) && $this->m_loadType[null] !== null) {
                return $this->m_loadType[null];
            } else {
                if ($this->hasFlag(self::AF_ONETOONE_LAZY)) {
                    return self::POSTLOAD | self::ADDTOQUERY;
                } else {
                    return self::ADDTOQUERY;
                }
            }
        }
    }

    /**
     * Store detail record in the database.
     *
     * @param Db $db The database used by the node.
     * @param array $record The master record which has the detail records
     *                       embedded.
     * @param string $mode The mode we're in ("add", "edit", "copy")
     *
     * @return bool true if store was successful, false otherwise.
     */
    public function store($db, &$record, $mode)
    {
        if ($this->createDestination()) {
            $vars = $this->_getStoreValue($record);
            if ($vars['mode'] == 'edit') {
                Tools::atkdebug('Updating existing one2one record');
                // we put the vars in the postvars, because there is information
                // like atkorgkey in it that is vital.
                // but we restore the postvars after we're done updating
                $oldpost = $this->m_destInstance->m_postvars;
                $this->m_destInstance->m_postvars = $vars;
                $res = $this->m_destInstance->updateDb($vars);
                $this->m_destInstance->m_postvars = $oldpost;

                return $res;
            } elseif ($vars['mode'] == 'add' || $mode == 'add' || $mode == 'copy') {
                if (!empty($vars['atkprimkey']) && $mode != 'copy') {
                    // destination record already exists, and we are not copying.
                    $result = true;
                } else {
                    Tools::atkdebug("atkonetoonerelation->store(): Adding new one2one record for mode $mode");
                    $this->m_destInstance->preAdd($vars);
                    $result = $this->m_destInstance->addDb($vars, true, $mode);
                }
                if ($this->m_refKey == '') {
                    // Foreign key is in source node, so we must update the record value with
                    $record[$this->fieldName()][$this->m_destInstance->m_primaryKey[0]] = $vars[$this->m_destInstance->m_primaryKey[0]];
                }

                return $result;
            } else {
                Tools::atkdebug('atkonetoonerelation->store(): Nothing to store in one2one record');

                return true;
            }
        }
    }

    /**
     * Needs update?
     *
     * @param array $record the record
     *
     * @return bool needs update
     */
    public function needsUpdate($record)
    {
        return $this->m_forceupdate || (parent::needsUpdate($record) && $this->createDestination() && !$this->m_destInstance->hasFlag(Node::NF_READONLY));
    }

    /**
     * Gets the value to store for the onetoonerelation.
     *
     * @param array &$record The record to get the value from
     *
     * @return mixed The value to store
     */
    public function &_getStoreValue(&$record)
    {
        $vars = &$record[$this->fieldName()];
        if ($this->m_refKey != '') {
            // Foreign key is in destination node
            if ($this->destinationHasRelation()) {
                $vars[$this->m_refKey][$this->m_ownerInstance->primaryKeyField()] = $record[$this->m_ownerInstance->primaryKeyField()];
            } else {
                //if the a onetoonerelation has no relation on the other side the m_refKey is not an array
                // experimental, will the next line always work?
                $refattr = $this->m_destInstance->getAttribute($this->m_refKey);
                if (isset($refattr->m_destination) && $refattr->m_destination) {
                    /*
                     * If we have a destination, the ref key is a non-onetoone relation!
                     * So we have to treat the record as such... this is specifically geared towards
                     * the manytoone relation and may not work for others
                     * A way should be found to make this work for whatever
                     * maybe use value2db and db2value on eachother or something?
                     */
                    $vars[$this->m_refKey][$this->m_ownerInstance->primaryKeyField()] = $this->m_ownerInstance->m_attribList[$this->m_ownerInstance->primaryKeyField()]->value2db($vars[$this->m_refKey]);
                } else {
                    $vars[$this->m_refKey] = $this->m_ownerInstance->m_attribList[$this->m_ownerInstance->primaryKeyField()]->value2db($record);
                }
            }
        } else {
            // Foreign key is in source node
            // After add, we must store the key value.
        }

        return $vars;
    }

    /**
     * Determine the type of the attribute on the other side.
     *
     * On the other side of a oneToOneRelation (in the destination node),
     * there may be a regular Attribute for the referential key, or an
     * atkOneToOneRelation pointing back at the source. This method discovers
     * which of the 2 cases we are dealing with.
     *
     * @return bool True if the attribute on the other side is a
     *              relation, false if not.
     */
    public function destinationHasRelation()
    {
        if ($this->createDestination()) {
            if (isset($this->m_refKey) && !empty($this->m_refKey)) {
                // foreign key is in the destination node.
                $attrib = $this->m_destInstance->m_attribList[$this->m_refKey];
            } else {
                // foreign key is in the source node. In this case, we must check the primary key
                // of the target node.
                $attrib = $this->m_destInstance->m_attribList[$this->m_destInstance->m_primaryKey[0]];
            }
            if (is_object($attrib) && strpos(get_class($attrib), 'elation') !== false) {
                return true;
            }
        }

        return false;
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its values. (<input type="hidden">).
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     *
     * @return string html
     */
    public function hide($record, $fieldprefix, $mode)
    {
        Tools::atkdebug('hide called for '.$this->fieldName());
        if ($this->createDestination()) {
            $myrecord = null;
            if ($record[$this->fieldName()] != null) {
                $myrecord = $record[$this->fieldName()];

                if ($myrecord[$this->m_destInstance->primaryKeyField()] == null) {
                    // rec has no primkey yet, so we must add instead of update..
                    $mode = 'add';
                } else {
                    $mode = 'edit';
                    $myrecord['atkprimkey'] = $this->m_destInstance->primaryKeyString($myrecord);
                }
            } else {
                $mode = 'add';
            }

            $output = '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'[mode]" value="'.$mode.'">';
            $output .= $this->m_destInstance->hideForm($mode, $myrecord, [], $this->getHtmlName($fieldprefix).'_AE_');

            return $output;
        }

        return '';
    }

    /**
     * Adds the attribute's edit / hide HTML code to the edit array.
     *
     * This method is called by the node if it wants the data needed to create
     * an edit form. The method is an override of Attribute's method,
     * because in the atkOneToOneRelation, we need to implement the
     * self::AF_ONETOONE_INTEGRATE feature.
     *
     * This is a framework method, it should never be called directly.
     *
     * @param string $mode the edit mode ("add" or "edit")
     * @param array $arr pointer to the edit array
     * @param array $defaults pointer to the default values array
     * @param array $error pointer to the error array
     * @param string $fieldprefix the fieldprefix
     */
    public function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
        /* hide */
        if (($mode == 'edit' && $this->hasFlag(self::AF_HIDE_EDIT)) || ($mode == 'add' && $this->hasFlag(self::AF_HIDE_ADD))) {
            /* when adding, there's nothing to hide... */
            if ($mode == 'edit' || ($mode == 'add' && !$this->isEmpty($defaults))) {
                $arr['hide'][] = $this->hide($defaults, $fieldprefix, $mode);
            }
        } /* edit */ else {
            /* we first check if there is no edit override method, if there
             * is this method has the same behaviour as the Attribute's method
             */
            if (method_exists($this->m_ownerInstance, $this->m_name.'_edit') || $this->edit($defaults, $fieldprefix, $mode) !== null) {
                self::addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);
            } /* how we handle 1:1 relations normally */ else {
                if (!$this->createDestination()) {
                    return;
                }

                $myrecord = null;

                /* readonly */
                if ($this->m_destInstance->hasFlag(Node::NF_READONLY) || ($mode == 'edit' && $this->hasFlag(self::AF_READONLY_EDIT)) || ($mode == 'add' && $this->hasFlag(self::AF_READONLY_ADD))) {
                    $this->createDestination();
                    $attrNames = $this->m_destInstance->getAttributeNames();
                    foreach ($attrNames as $attrName) {
                        $attr = $this->m_destInstance->getAttribute($attrName);
                        $attr->addFlag(self::AF_READONLY);
                    }
                }

                /* we first check if the record doesn't already exist */
                if (isset($defaults[$this->fieldName()]) && !empty($defaults[$this->fieldName()])) {
                    /* record has no primarykey yet, so we must add instead of update */
                    $myrecord = $defaults[$this->fieldName()];

                    if (empty($myrecord[$this->m_destInstance->primaryKeyField()])) {
                        $mode = 'add';
                    } /* record exists! */ else {
                        $mode = 'edit';
                        $myrecord['atkprimkey'] = $this->m_destInstance->primaryKeyString($myrecord);
                    }
                } /* record does not exist */ else {
                    $mode = 'add';
                }

                /* mode */
                $arr['hide'][] = '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'[mode]" value="'.$mode.'">';

                /* add fields */
                $forceList = [];
                if ($this->m_refKey != '') {
                    if ($this->destinationHasRelation()) {
                        $forceList[$this->m_refKey][$this->m_ownerInstance->primaryKeyField()] = $defaults[$this->m_ownerInstance->primaryKeyField()];
                    } else {
                        // its possible that the destination has no relation back. In that case the refKey is just an attribute
                        $forceList[$this->m_refKey] = Tools::atkArrayNvl($defaults, $this->m_ownerInstance->primaryKeyField());
                    }
                }

                $a = $this->m_destInstance->editArray($mode, $myrecord, $forceList, [], $this->getHtmlName($fieldprefix).'_AE_', false, false);

                /* hidden fields */
                $arr['hide'] = array_merge($arr['hide'], $a['hide']);

                /* editable fields, if self::AF_NOLABEL is specified or if there is just 1 field with the
                 * same name as the relation we don't display a label
                 * TODO FIXME
                 */
                if (!is_array($arr['fields'])) {
                    $arr['fields'] = [];
                }
                if (!$this->hasFlag(self::AF_ONETOONE_INTEGRATE) && !$this->hasFlag(self::AF_NOLABEL) && !(Tools::count($a['fields']) == 1 && $a['fields'][0]['name'] == $this->m_name)) {
                    /* separator and name */
                    if ($arr['fields'][Tools::count($arr['fields']) - 1]['html'] !== '-') {
                        $arr['fields'][] = array(
                            'html' => '-',
                            'tabs' => $this->m_tabs,
                            'sections' => $this->getSections(),
                        );
                    }
                    $arr['fields'][] = array(
                        'line' => '<b>'.Tools::atktext($this->m_name, $this->m_ownerInstance->m_module, $this->m_ownerInstance->m_type).'</b>',
                        'tabs' => $this->m_tabs,
                        'sections' => $this->getSections(),
                    );
                }

                if (is_array($a['fields'])) {
                    // in non-integration mode we move all the fields to the one-to-one relations tabs/sections
                    if (!$this->hasFlag(self::AF_ONETOONE_INTEGRATE) || $this->hasFlag(self::AF_ONETOONE_RESPECT_TABS)) {
                        foreach (array_keys($a['fields']) as $key) {
                            $a['fields'][$key]['tabs'] = $this->m_tabs;
                            $a['fields'][$key]['sections'] = $this->getSections();
                        }
                    }

                    $arr['fields'] = array_merge($arr['fields'], $a['fields']);
                }

                if (!$this->hasFlag(self::AF_ONETOONE_INTEGRATE) && !$this->hasFlag(self::AF_NOLABEL) && !(Tools::count($a['fields']) == 1 && $a['fields'][0]['name'] == $this->m_name)) {
                    /* separator */
                    $arr['fields'][] = array(
                        'html' => '-',
                        'tabs' => $this->m_tabs,
                        'sections' => $this->getSections(),
                    );
                }

                $fields = $arr['fields'];
                foreach ($fields as &$field) {
                    $field['attribute'] = '';
                }
            }
        }
    }

    /**
     * Adds the attribute's view / hide HTML code to the view array.
     *
     * This method is called by the node if it wants the data needed to create
     * a view form.
     *
     * This is a framework method, it should never be called directly.
     *
     * @param string $mode the mode ("view")
     * @param array $arr pointer to the view array
     * @param array $defaults pointer to the default values array
     */
    public function addToViewArray($mode, &$arr, &$defaults)
    {
        if ($this->hasFlag(self::AF_HIDE_VIEW)) {
            return;
        }

        /* we first check if there is no display override method, if there
         * is this method has the same behaviour as the Attribute's method
         */
        if (method_exists($this->m_ownerInstance, $this->m_name.'_display') || $this->display($defaults, 'view') !== null) {
            $this->addToViewArray($mode, $arr, $defaults);
        } /* how we handle 1:1 relations normally */ else {
            if (!$this->createDestination()) {
                return;
            }

            $record = $defaults[$this->fieldName()];
            $a = $this->m_destInstance->viewArray($mode, $record, false);

            /* editable fields, if self::AF_NOLABEL is specified or if there is just 1 field with the
             * same name as the relation we don't display a label
             * TODO FIXME
             */
            if (!is_array($arr['fields'])) {
                $arr['fields'] = [];
            }
            if (!$this->hasFlag(self::AF_ONETOONE_INTEGRATE) && !$this->hasFlag(self::AF_NOLABEL) && !(Tools::count($a['fields']) == 1 && $a['fields'][0]['name'] == $this->m_name)) {
                /* separator and name */
                if ($arr['fields'][Tools::count($arr['fields']) - 1]['html'] !== '-') {
                    $arr['fields'][] = array(
                        'html' => '-',
                        'tabs' => $this->m_tabs,
                        'sections' => $this->getSections(),
                    );
                }
                $arr['fields'][] = array(
                    'line' => '<b>'.Tools::atktext($this->m_name, $this->m_ownerInstance->m_module, $this->m_ownerInstance->m_type).'</b>',
                    'tabs' => $this->m_tabs,
                    'sections' => $this->getSections(),
                );
            }

            if (is_array($a['fields'])) {
                if (!$this->hasFlag(self::AF_ONETOONE_INTEGRATE) || $this->hasFlag(self::AF_ONETOONE_RESPECT_TABS)) {
                    foreach (array_keys($a['fields']) as $key) {
                        $a['fields'][$key]['tabs'] = $this->m_tabs;
                        $a['fields'][$key]['sections'] = $this->getSections();
                    }
                }
                $arr['fields'] = array_merge($arr['fields'], $a['fields']);
            }

            if (!$this->hasFlag(self::AF_ONETOONE_INTEGRATE) && !$this->hasFlag(self::AF_NOLABEL) && !(Tools::count($a['fields']) == 1 && $a['fields'][0]['name'] == $this->m_name)) {
                /* separator */
                $arr['fields'][] = array('html' => '-', 'tabs' => $this->m_tabs, 'sections' => $this->getSections());
            }
        }
    }

    /**
     * Check if a record has an empty value for this attribute.
     *
     * @param array $record The record that holds this attribute's value.
     *
     * @todo This method is not currently implemented properly and returns
     *       false in all cases.
     *
     * @return bool
     */
    public function isEmpty($record)
    {
        return false;
    }

    /**
     * Checks if a value is valid.
     *
     * For the atkOneToOneRelation, this method delegates the actual
     * validation of values to the destination node.
     *
     * @param array $record The record that holds the value for this
     *                       attribute. If an error occurs, the error will
     *                       be stored in the 'atkerror' field of the record.
     * @param string $mode The mode for which should be validated ("add" or
     *                       "update")
     */
    public function validate(&$record, $mode)
    {
        // zitten self::AF_ONETOONE_ERROR en self::AF_OBLIGATORY elkaar soms in de weg
        if ($this->hasFlag(self::AF_ONETOONE_ERROR) && ($mode != 'add' || !$this->hasFlag(self::AF_HIDE_ADD)) && $this->createDestination()) {
            $this->m_destInstance->validate($record[$this->fieldName()], $mode, array($this->m_refKey));

            // only add 'atkerror' record when 1:1 relation contains error
            if (!isset($record[$this->fieldName()]['atkerror'])) {
                return;
            }

            foreach ($record[$this->fieldName()]['atkerror'] as $error) {
                $error['tab'] = $this->hasFlag(self::AF_ONETOONE_RESPECT_TABS) ? $this->m_tabs[0] : $error['tab'];
                $record['atkerror'][] = $error;
            }
        }
    }

    /**
     * Get list of additional tabs.
     *
     * Attributes can add new tabs to tabbed screens. This method will be
     * called to retrieve the tabs. When self::AF_ONETOONE_INTEGRATE is set, the
     * atkOneToOneRelation adds tabs from the destination node to the tab
     * screen, so the attributes are seamlessly integrated but still on their
     * own tabs.
     *
     * @param string $action The action for which additional tabs should be loaded.
     *
     * @return array The list of tabs to add to the screen.
     */
    public function getAdditionalTabs($action = null)
    {
        if ($this->hasFlag(self::AF_ONETOONE_INTEGRATE) && $this->createDestination()) {
            $detailtabs = $this->m_destInstance->getTabs($action);
            if (Tools::count($detailtabs) == 1 && $detailtabs[0] == 'default') {
                // All elements in the relation are on the default tab. That means we should
                // inherit the tab from the onetoonerelation itself.
                return parent::getAdditionalTabs($action);
            }

            return $detailtabs;
        }

        return parent::getAdditionalTabs($action);
    }

    /**
     * Check if the attribute wants to be shown on a certain tab.
     *
     * @param string $tab The name of the tab to check.
     *
     * @return bool
     */
    public function showOnTab($tab)
    {
        if ($this->hasFlag(self::AF_ONETOONE_INTEGRATE) && $this->createDestination()) {
            foreach (array_keys($this->m_destInstance->m_attribList) as $attribname) {
                $p_attrib = $this->m_destInstance->m_attribList[$attribname];
                if ($p_attrib->showOnTab($tab)) {
                    return true;
                } // If we have one match, we can return true.
            }
            // None of the destionation attributes wants to be displayed on the tab.
            // If the entire onetoone itself is on that tab however, we should put all attribs on
            // this tab.
            return parent::showOnTab($tab);
        }

        return parent::showOnTab($tab);
    }


    public function addToListArrayHeader(
        $action,
        &$arr,
        $fieldprefix,
        $flags,
        $atksearch,
        $columnConfig,
        DataGrid $grid = null,
        $column = '*'
    ) {
        if ($this->hasFlag(self::AF_HIDE_LIST) || !$this->createDestination()) {
            return;
        }

        if ((!$this->hasFlag(self::AF_ONETOONE_INTEGRATE) && $column == '*') || $column == null) {
            // regular behaviour.
            parent::addToListArrayHeader($action, $arr, $fieldprefix, $flags, $atksearch, $columnConfig, $grid, $column);

            return;
        } else {
            if (!$this->hasFlag(self::AF_ONETOONE_INTEGRATE) || ($column != '*' && $this->getDestination()->getAttribute($column) == null)) {
                throw new \Exception("Invalid list column {$column} for atkOneToOneRelation ".$this->getOwnerInstance()->atkNodeUri().'::'.$this->fieldName());
            }
        }

        // integrated version, don't add ourselves, but add all columns from the destination.
        $prefix = $this->getHtmlName($fieldprefix).'_AE_';
        foreach (array_keys($this->m_destInstance->m_attribList) as $attribname) {
            if ($column != '*' && $column != $attribname) {
                continue;
            }

            $p_attrib = $this->m_destInstance->getAttribute($attribname);
            $p_attrib->addToListArrayHeader($action, $arr, $prefix, $flags, $atksearch[$this->fieldName()], $columnConfig, $grid, null);
        }
    }


    public function addToListArrayRow(
        $action,
        &$arr,
        $nr,
        $fieldprefix,
        $flags,
        $edit = false,
        DataGrid $grid = null,
        $column = '*'
    ) {
        if ($this->hasFlag(self::AF_HIDE_LIST) || !$this->createDestination()) {
            return;
        }

        if ((!$this->hasFlag(self::AF_ONETOONE_INTEGRATE) && $column == '*') || $column == null) {
            parent::addToListArrayRow($action, $arr, $nr, $fieldprefix, $flags, $edit, $grid, $column);

            return;
        } else {
            if (!$this->hasFlag(self::AF_ONETOONE_INTEGRATE) || ($column != '*' && $this->getDestination()->getAttribute($column) == null)) {
                throw new \Exception("Invalid list column {$column} for atkOneToOneRelation ".$this->getOwnerInstance()->atkNodeUri().'::'.$this->fieldName());
            }
        }

        // integrated version, don't add ourselves, but add all columns from the destination
        // small trick, the destination record is in a subarray. The destination
        // addToListArrayRow will not expect this though, so we have to modify the
        // record a bit before passing it to the detail columns.
        $oldrecord = $arr['rows'][$nr]['record'];
        $arr['rows'][$nr]['record'] = $arr['rows'][$nr]['record'][$this->fieldName()];

        $prefix = $this->getHtmlName($fieldprefix).'_AE_';
        foreach (array_keys($this->m_destInstance->m_attribList) as $attribname) {
            if ($column != '*' && $column != $attribname) {
                continue;
            }

            $p_attrib = $this->m_destInstance->getAttribute($attribname);
            $p_attrib->addToListArrayRow($action, $arr, $nr, $prefix, $flags, $edit, $grid, null);
        }

        $arr['rows'][$nr]['record'] = $oldrecord;
    }

    /**
     * Creates a searchcondition for the field,
     * was once part of searchCondition, however,
     * searchcondition() also immediately adds the search condition.
     *
     * @param Query $query The query object where the search condition should be placed on
     * @param string $table The name of the table in which this attribute
     *                           is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param string $searchmode The searchmode to use. This can be any one
     *                           of the supported modes, as returned by this
     *                           attribute's getSearchModes() method.
     * @param string $fieldname
     *
     * @return string The searchcondition to use.
     */
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        if (!$this->createDestination()) {
            return null;
        }
        if (!is_array($value)) {
            // we were passed a value that is not an array, so appearantly the function calling us
            // does not know we are a relation, not just another attrib
            // so we assume that it is looking for something in the descriptor def of the destination
            $alias = $fieldname.$this->fieldName().'_AE_'.$this->m_destInstance->m_table;
            $query->addJoin($this->m_destInstance->m_table, $alias, $this->getJoinCondition('', $alias), false);
            return $this->m_destInstance->getTemplateSearchCondition($query, $alias, $this->m_destInstance->getDescriptorTemplate(), $value, $searchmode, $fieldname);
        }
        // we are a relation, so instead of hooking ourselves into the
        // query, hook the attributes in the destination node onto the query
        $conditions = [];
        foreach ($value as $key => $val) {
            // if we aren't searching for anything in this field, there is no need
            // to look any further:
            if ($val === '' || $val === null) {
                continue;
            }

            $p_attrib = $this->m_destInstance->m_attribList[$key];

            if (is_object($p_attrib)) {
                if ($this->m_refKey) {
                    // master mode
                    $new_table = $this->fieldName();
                } else {
                    // slave mode
                    $new_table = $this->m_destInstance->m_table;

                    // we need to left join the destination table into the query
                    // (don't worry ATK won't add it when it's already there)
                    $query->addJoin($new_table, $new_table, ($this->getJoinCondition()), false);
                }
                $conditions[] = $p_attrib->getSearchCondition($query, $new_table, $val, $this->getChildSearchMode($searchmode, $p_attrib->fieldName()));
            } else {
                // attribute not found in destination, so it should
                // be in the owner (this is the case when extra fields
                // are in the relation
                $p_attrib = $this->m_ownerInstance->m_attribList[$key];
                if (is_object($p_attrib)) {
                    $conditions[] = $p_attrib->getSearchCondition($query, $p_attrib->getOwnerInstance()->getTable(), $val, $this->getChildSearchMode($searchmode, $p_attrib->fieldName()));
                } else {
                    Tools::atkdebug("Field $key was not found in this relation (this is very weird)");
                }
            }
        }
        return QueryPart::implode('OR', $conditions, true);
    }

    /**
     * Returns the condition which can be used when calling Query's addJoin() method
     * Joins the relation's owner with the destination.
     *
     * @param string $tablename The name of the table
     * @param string $fieldalias The field alias
     *
     * @return string condition the condition that can be pasted into the query
     */
    public function getJoinCondition($tablename = '', $fieldalias = '')
    {
        if ($tablename == '') {
            $tablename = $this->m_ownerInstance->m_table;
        }
        if ($fieldalias == '') {
            $fieldalias = $this->m_destInstance->m_table;
        }
        $condition = Db::quoteIdentifier($tablename, $this->fieldName());
        $condition .= '=';
        $condition .= Db::quoteIdentifier($fieldalias, $this->m_destInstance->primaryKeyField());

        return $condition;
    }


    public function addToSearchformFields(&$fields, $node, &$record, $fieldprefix = '', $extended = true)
    {
        if ($this->hasFlag(self::AF_ONETOONE_INTEGRATE) && $this->createDestination()) {
            $prefix = $this->getHtmlName($fieldprefix).'_AE_';
            foreach (array_keys($this->m_destInstance->m_attribList) as $attribname) {
                $p_attrib = $this->m_destInstance->m_attribList[$attribname];

                if (!$p_attrib->hasFlag(self::AF_HIDE_SEARCH)) {
                    $p_attrib->addToSearchformFields($fields, $node, $record[$this->fieldName()], $prefix, $extended);
                }
            }
        } else {
            parent::addToSearchformFields($fields, $node, $record, $fieldprefix, $extended);
        }
    }

    /**
     * Convert the internal value to the database value.
     *
     * @param array $rec The record that holds the value for this attribute
     *
     * @return mixed The database value
     */
    public function value2db($rec)
    {
        if (is_array($rec) && isset($rec[$this->fieldName()])) {
            if (is_array($rec[$this->fieldName()])) {
                return $rec[$this->fieldName()][$this->m_destInstance->primaryKeyField()];
            } else {
                return $rec[$this->fieldName()];
            }
        }

        return;
    }
}
