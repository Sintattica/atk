<?php

/**
 * This file is part of the ATK Framework distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage listeners
 *
 * @copyright (c)2010 Ibuildings
 * @license http://www.atk-framework.com/licensing ATK Open Source License
 * 
 * @version $Revision: 6263 $
 * $Id: class.atkdeletehandler.inc 6564 2009-11-12 10:32:52Z martijn $
 */

  /**
   * The atkRecycleBin is a generic recycle bin for records. You can add it 
   * to any node and if a record from that node will get deleted,
   * atkRecycleBin will kick in and transfer the record to the recyclebin.
   * 
   * There are 2 modes of operation. You can build your own recyclebin node,
   * and atkRecycleBin will use that to store the deleted record.
   * 
   * Alternatively, you can skip creating a node, and just create a table
   * that is identical to the one you're deleting records from. 
   * If you don't specify this table, atkRecycleBin will assume that
   * the table is called tablename_bin, where tablename is the tablename
   * from the node you're deleting records from. 
   * 
   * Usage: $node->addListener(new atkRecycleBin());
   * 
   * @todo a third mode of operation might be one serialized recyclebin
   * for all the tables in the application.
   *
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @package atk
   * @subpackage listeners
   *
   */
class atkRecycleBin extends atkTriggerListener 
{
    /**
     * The options for the recycle bin.
     */
    protected $_options = array();
    
    /**
     * Construct a new atkRecycleBin
     *
     * @param array $options Supports the following keys:
     *                       "node"  - Use a specific node as the recyclebin
     *                       "table" - Use a specfic table as the recyclebin (table needs to be
     *                                 identical to the table the records are deleted from.
     *                       If both table and node are ommitted, a default table with
     *                       appendix _bin is assumed.
     */
    public function __construct($options=array())
    {
        parent::__construct(array("delete"));
        
        $this->_options = $options;
    }
    
    /**
     * This is the actual trigger that moves the record to the recycle bin table.
     *
     * @param array $record The record that is being deleted
     * @return false if there was an error, true if everything is ok
     */
    public function preDelete($record)
    {
        atkdebug("delete performed, storing record in recyclebin");
        
        if (isset($this->_options["node"])) {
            
            $node = atkGetNode($this->_options["node"]);
            $node->addDb($record);
            
        } else {
            
            $node = clone($this->m_node);
            
            $pkFields = $node->m_primaryKey;
            foreach($pkFields as $fieldName) {
                
                // We need to make sure the record in the bin has the same primary key as the original
                // record, so we remove AF_AUTOINCREMENT and setForceInsert.
                $node->getAttribute($fieldName)->setForceInsert(true)->removeFlag(AF_AUTO_INCREMENT);
            }
            
            if (isset($this->_options["table"])) {
                $node->setTable($this->_options["table"]);
            } else { // default behaviour: assume table with _bin appendix
                $node->setTable($node->getTable()."_bin");
            }
            atkdebug("adding record to recyclebin");
            $node->addDb($record);
        }  
        return true; 
    }
    
}