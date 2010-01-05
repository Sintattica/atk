<?php
  userelation("atkonetoonerelation");
  class atkPolymorphicOneToOneRelation extends atkOneToOneRelation
  {
    /**
     * The name of the foreign key field in the master node to the type table.
     * @access private
     * @var String
     */
    var $m_typefk="";

    /**
     * The name of the foreign key field in the master node to the type table.
     * @access private
     * @var String
     */
    var $m_discriminatorfield="";

    /**
     * $modulename The module name
     * @access private
     * @var String
     */
    var $m_modulename="";

    /**
     * Default Constructor
     *
     * The atkPolymorphicOneToOneRelation extends atkOneToOneRelation:
     * <b>Example:</b>
     * <code>
     *  $this->add(new atkPolymorphicOneToOneRelation("details","fruittype_id","table","poly.orange",
     *               "poly","fruit_id",AF_CASCADE_DELETE ));
     * </code>
     *
     * @param String $name The unique name of the attribute.
     * @param String $typefk The name of the foreign key field in the master node to the type table .
     * @param String $discriminatorfield The name of the field in the type table wich stores the type tablename
     * (a node with the same name must be created).
     * @param String $defaultdest The default destination node (in module.nodename
     *                            notation)
     * @param String $modulename The module name
     * @param String $refKey Specifies the foreign key
     *                       field from the destination node that points to
     *                       the master record.
     * @param int $flags Attribute flags that influence this attributes'
     *                   behavior.
     */

    function atkPolymorphicOneToOneRelation($name,$typefk,$discriminatorfield,
                                            $defaultdest,$modulename,$refKey, $flags=0)
    {
      $this->atkOneToOneRelation($name,"",$refKey, $flags|AF_HIDE_LIST);
      $this->m_typefk=$typefk;
      $this->m_discriminatorfield=$discriminatorfield;
      $this->m_destination =$defaultdest;
      $this->m_modulename =$modulename;
    }

    function loadType()
    {
      return POSTLOAD;
    }

    /**
     * Retrieve detail records from the database.
     *
     * Called by the framework to load the detail records.
     *
     * @param atkDb $db The database used by the node.
     * @param array $record The master record
     * @param String $mode The mode for loading (admin, select, copy, etc)
     *
     * @return array Sets the destination from the record and
     *                       return the atkonetoone load function
     */
    function load(&$db, $record, $mode)
    {
      $this->m_destination = $this->m_modulename.".".$record[$this->m_typefk][$this->m_discriminatorfield];
      $this->m_destInstance = $this->m_modulename.".".$record[$this->m_typefk][$this->m_discriminatorfield];
      return parent::load($db, $record, $mode);
    }
  }
?>