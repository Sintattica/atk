<?php

namespace Sintattica\Atk\Relations;

class PolymorphicOneToOneRelation extends OneToOneRelation
{
    /*
     * The name of the foreign key field in the master node to the type table.
     * @access private
     * @var String
     */
    public $m_typefk = '';

    /*
     * The name of the foreign key field in the master node to the type table.
     * @access private
     * @var String
     */
    public $m_discriminatorfield = '';

    /*
     * $modulename The module name
     * @access private
     * @var String
     */
    public $m_modulename = '';

    /**
     * Default Constructor.
     *
     * The atkPolymorphicOneToOneRelation extends OneToOneRelation:
     * <b>Example:</b>
     * <code>
     *  $this->add(new atkPolymorphicOneToOneRelation("details","fruittype_id","table","poly.orange",
     *               "poly","fruit_id",self::AF_CASCADE_DELETE ));
     * </code>
     *
     * @param string $name The unique name of the attribute.
     * @param string $typefk The name of the foreign key field in the master node to the type table .
     * @param string $discriminatorfield The name of the field in the type table wich stores the type tablename
     *                                   (a node with the same name must be created).
     * @param string $defaultdest The default destination node (in module.nodename
     *                                   notation)
     * @param string $modulename The module name
     * @param string $refKey Specifies the foreign key
     *                                   field from the destination node that points to
     *                                   the master record.
     * @param int $flags Attribute flags that influence this attributes'
     *                                   behavior.
     */
    public function __construct($name, $typefk, $discriminatorfield, $defaultdest, $modulename, $refKey, $flags = 0)
    {
        parent::__construct($name, '', $refKey, $flags | self::AF_HIDE_LIST);
        $this->m_typefk = $typefk;
        $this->m_discriminatorfield = $discriminatorfield;
        $this->m_destination = $defaultdest;
        $this->m_modulename = $modulename;
    }

    public function loadType()
    {
        return self::POSTLOAD;
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
     * @return array Sets the destination from the record and
     *               return the atkonetoone load function
     */
    public function load(&$db, $record, $mode)
    {
        $this->m_destination = $this->m_modulename.'.'.$record[$this->m_typefk][$this->m_discriminatorfield];
        $this->m_destInstance = $this->m_modulename.'.'.$record[$this->m_typefk][$this->m_discriminatorfield];

        return parent::load($db, $record, $mode);
    }
}
