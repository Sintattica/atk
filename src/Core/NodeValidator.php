<?php

namespace Sintattica\Atk\Core;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;

/**
 * Validator for records, based on node definition.
 *
 * The class takes a node, and based on the attribute definitions,
 * validation can be performed on records.
 *
 * @author Kees van Dieren <kees@ibuildings.nl>
 */
class NodeValidator
{
    /**
     * @var Node $m_nodeObj The node which needs to get validated
     * @access private
     */
    public $m_nodeObj = null;

    /**
     * @var array $m_record the record of the node which will get validated
     * @access private
     */
    public $m_record = [];

    /**
     * @var String $m_mode the mode in which the validate will get runned
     * @access private
     */
    public $m_mode = '';

    /**
     * @var array $m_ignoreList the list of fields which will get ignored
     * @access private
     */
    public $m_ignoreList = [];

    /**
     * constructor.
     */
    public function atkNodeValidator()
    {
    }

    /**
     * set the list of fields which will get ignored.
     *
     * @param array $fieldArr List of fields to ignore during validation
     */
    public function setIgnoreList($fieldArr)
    {
        $this->m_ignoreList = $fieldArr;
    }

    /**
     * set the mode in which to validate.
     *
     * @param string $mode The mode ("edit"/"add")
     */
    public function setMode($mode)
    {
        $this->m_mode = $mode;
    }

    /**
     * Set the Node which should be validated.
     *
     * @param Node $nodeObj The node for validation
     */
    public function setNode($nodeObj)
    {
        $this->m_nodeObj = $nodeObj;
    }

    /**
     * set the record which should get validated.
     *
     * @param array $record The record to validate. The record is passed by
     *                      reference, because errors that are found are
     *                      stored in the record.
     */
    public function setRecord(&$record)
    {
        $this->m_record = &$record;
    }

    /**
     * Validate a record.
     *
     * @param string $mode Override the mode
     * @param array $ignoreList Override the ignoreList
     */
    public function validate($mode = '', $ignoreList = array())
    {
        // check overrides
        if (Tools::count($ignoreList)) {
            $this->setIgnoreList($ignoreList);
        }

        if ($mode != '') {
            $this->setMode($mode);
        }

        Tools::atkdebug('validate() with mode '.$this->m_mode.' for node '.$this->m_nodeObj->atkNodeUri());

        // set the record
        $record = &$this->m_record;

        // Check flags and values
        $db = $this->m_nodeObj->getDb();
        foreach ($this->m_nodeObj->m_attribIndexList as $attribdata) {
            $attribname = $attribdata['name'];
            if (!Tools::atk_in_array($attribname, $this->m_ignoreList)) {
                $p_attrib = $this->m_nodeObj->m_attribList[$attribname];

                $this->validateAttributeValue($p_attrib, $record);

                if ($p_attrib->hasFlag(Attribute::AF_PRIMARY) && !$p_attrib->hasFlag(Attribute::AF_AUTO_INCREMENT)) {
                    $atkorgkey = $record['atkprimkey'];
                    if ($atkorgkey == '' || !Node::primaryKeyStringEqual($atkorgkey, $this->m_nodeObj->primaryKeyString($record))) {
                        $cnt = $this->m_nodeObj->select($this->m_nodeObj->primaryKey($record))->ignoreDefaultFilters(true)->ignorePostvars(true)->getRowCount();
                        if ($cnt > 0) {
                            Tools::atkTriggerError($record, $p_attrib, 'error_primarykey_exists');
                        }
                    }
                }

                // if no root elements may be added to the tree, then every record needs to have a parent!
                if ($p_attrib->hasFlag(Attribute::AF_PARENT) && $this->m_nodeObj->hasFlag(TreeNode::NF_TREE_NO_ROOT_ADD) && $this->m_nodeObj->m_action == 'save') {
                    $p_attrib->m_flags |= Attribute::AF_OBLIGATORY;
                }

                // validate obligatory fields (but not the auto_increment ones, because they don't have a value yet)
                if ($p_attrib->hasFlag(Attribute::AF_OBLIGATORY) && !$p_attrib->hasFlag(Attribute::AF_AUTO_INCREMENT) && $p_attrib->isEmpty($record)) {
                    Tools::atkTriggerError($record, $p_attrib, 'error_obligatoryfield');
                } // if flag is primary
                else {
                    if ($p_attrib->hasFlag(Attribute::AF_UNIQUE) && !$p_attrib->hasFlag(Attribute::AF_PRIMARY) && !$p_attrib->isEmpty($record)) {
                        $condition = Query::simpleValueCondition($this->m_nodeObj->getTable(), $attribname, $p_attrib->value2db($record));
                        if ($this->m_mode != 'add') {
                            $condition->appendSql('AND ');
                            $condition->append($this->m_nodeObj->primaryKey($record, true));
                        }
                        $cnt = $this->m_nodeObj->select($condition)->ignoreDefaultFilters(true)->ignorePostvars(true)->getRowCount();
                        if ($cnt > 0) {
                            Tools::atkTriggerError($record, $p_attrib, 'error_uniquefield');
                        }
                    }
                }
            }
        }

        if (isset($record['atkerror']) && Tools::count($record['atkerror']) > 0) {
            for ($i = 0, $_i = Tools::count($record['atkerror']); $i < $_i; ++$i) {
                $record['atkerror'][$i]['node'] = $this->m_nodeObj->m_type;
            }
        }

        $this->validateUniqueFieldSets($record);

        if (isset($record['atkerror'])) {
            for ($i = 0, $_i = Tools::count($record['atkerror']); $i < $_i; ++$i) {
                $record['atkerror'][$i]['node'] = $this->m_nodeObj->m_type;
            }

            return false;
        }

        return true;
    }

    /**
     * Validate attribute value.
     *
     * @param Attribute $p_attrib pointer to the attribute
     * @param array $record record
     */
    public function validateAttributeValue($p_attrib, &$record)
    {
        if (!$p_attrib->isEmpty($record)) {
            $funcname = $p_attrib->m_name.'_validate';
            if (method_exists($this->m_nodeObj, $funcname)) {
                $this->m_nodeObj->$funcname($record, $this->m_mode);
            } else {
                $p_attrib->validate($record, $this->m_mode);
            }
        }
    }

    /**
     * Check unique field combinations.
     * The function is called by the validate() method automatically. It is
     * not necessary to call this manually in a validation process.
     * Errors that are found are stored in the $record parameter.
     *
     * @param array $record The record to validate
     */
    public function validateUniqueFieldSets(&$record)
    {
        $db = $this->m_nodeObj->getDb();
        foreach ($this->m_nodeObj->m_uniqueFieldSets as $uniqueFieldSet) {
            $query = $db->createQuery($this->m_nodeObj->m_table);

            $attribs = [];
            foreach ($uniqueFieldSet as $field) {
                $attrib = $this->m_nodeObj->m_attribList[$field];
                if ($attrib) {
                    $attribs[] = $attrib;

                    if (method_exists($attrib, 'createDestination') && isset($attrib->m_refKey) && is_array($attrib->m_refKey) && Tools::count($attrib->m_refKey) > 1) {
                        $attrib->createDestination();
                        foreach ($attrib->m_refKey as $refkey) {
                            $query->addCondition(Db::quoteIdentifier($refkey).' = :refkey', [':refkey' => $record[$attrib->fieldName()][$refkey]]);
                        }
                    } else {
                        if (!$attrib->isNotNullInDb() && $attrib->isEmpty($record)) {
                            $query->addCondition(Db::quoteIdentifier($field).' IS NULL');
                        } else {
                            $query->addCondition(Db::quoteIdentifier($field).' = :value', [':value' => $attrib->value2db($record)]);
                        }
                    }
                } else {
                    Tools::atkerror("Field $field is mentioned in uniquefieldset but does not exist in ".$this->m_nodeObj->atkNodeUri());
                }
            }

            if ($this->m_mode != 'add') {
                $query->addCondition($this->m_nodeObj->primaryKey($record, true));
            }

            if ($query->executeCount() > 0) {
                Tools::atkTriggerError($record, $attribs, 'error_uniquefieldset');
            }
        }
    }
}
