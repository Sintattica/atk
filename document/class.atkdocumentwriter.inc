<?php
  /**
   * atkDocumentWriter class file
   *
   * @package atk
   * @subpackage document
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * General DocumentWriter framework class. Should be extended to support specific file formats.
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk
   * @subpackage document
   */
  class atkDocumentWriter
  {

    /**
     * Template vars array
     *
     * @access protected
     * @var array
     */
    var $m_tpl_vars = array();

    /**
     * atkDocumentWriter Constructor.
     *
     * Dont use this, use &atkDocumentWriter::getInstance($format) instead to get a singleton instance for any format used
     */
    function atkDocumentWriter()
    {
    }

    /**
     * Assigns values to template variables
     *
     * @param string|array $tpl_var Template variable name or array of variable name/value pairs
     * @param mixed $value Value to assign (only used if $tpl_var is a string)
     */
    function assign($tpl_var, $value = null)
    {
      if (is_array($tpl_var))
      {
        foreach ($tpl_var as $key => $val)
        {
          if ($key != '')
          {
            $this->m_tpl_vars[$key] = $val;
          }
        }
      }
      else
      {
        if ($tpl_var != '')
          $this->m_tpl_vars[$tpl_var] = $value;
      }
    }

    /**
     * Returns labels for all attributes of a node
     *
     * @param atkNode $node Node for which the labels should be retrieved
     * @return Array Associative array containing attributename=>label pairs
     */
    function getRecordLabels(&$node)
    {
      // Initialize the result array
      $result = array();

      // Loop through the attributes in order to assign them all to the documentwriter
      foreach(array_keys($node->m_attribList) as $key)
      {

        // Get a reference to the attribute
        $p_attrib = &$node->m_attribList[$key];

        // Get the Label of the attribute (can be suppressed with AF_NOLABEL or AF_BLANKLABEL)
        if ($p_attrib->hasFlag(AF_NOLABEL) || $p_attrib->hasFlag(AF_BLANKLABEL))
          $result[$key] = "";
        else
          $result[$key] = $p_attrib->label(array());
      }

      // Return the array containing attributename=>label pairs
      return $result;
    }

    /**
     * Returns labels for all attributes of a node
     *
     * @param atkNode $node Node for which the displayvalues should be retrieved
     * @param Array $record Record for which the display values should be determined
     * @return Array Associative array containing attributename=>displayvalue pairs
     */
    function getRecordDisplayValues(&$node, $record)
    {
      // Initialize the result array
      $result = array();

      // Loop through the attributes in order to assign them all to the documentwriter
      foreach(array_keys($node->m_attribList) as $key)
      {

        // Get a reference to the attribute
        $p_attrib = &$node->m_attribList[$key];

        // Get the display value by calling <attribute>_display().
        // An <attributename>_display function may be provided in a derived
        // class to display an attribute. If it exists we will use that method
        // else we will just use the attribute's display method.
        $funcname = $p_attrib->m_name."_display";
        if (method_exists($node, $funcname))
          $result[$key] = $node->$funcname($record, "plain");
        else
          $result[$key] = $p_attrib->display($record, "plain");
      }

      // Return the array containing attributename=>displayvalue pairs
      return $result;
    }

    /**
     * Assigns the labels for all attributes of a node to the documentWriter
     *
     * @param atkNode $node Node for which the labels should be retrieved
     * @param String $prefix Prefix to be used when assigning the variables (used to avoid conflicting names)
     */
    function _assignLabels(&$node, $prefix)
    {
      // Get all labels for the given node
      $labels = $this->getRecordLabels($node);

      // Assign all labels to the documentwriter
      foreach($labels as $key => $label)
        $this->Assign($prefix . $key . "_label", $label);
    }

    /**
     * Enter description here...
     *
     * @param atkNode $node Node to be used when displaying the records
     * @param Array $records Array of records that should be assigned to the documentwriter
     * @param String$prefix Prefix to be used when assigning the variables (used to avoid conflicting names)
     */
    function assignDocumentMultiRecord(&$node, $records, $prefix = "")
    {
      // Assign all labels to the documentwriter
      $this->_assignLabels($node, $prefix);

      // Initialize the displayvalues array
      $displayvalues = array();

      // Loop through all records and add the displayvalues to the array
      foreach($records as $record)
        $displayvalues[] = $this->getRecordDisplayValues($node, $record);

      // Assign the displayvalues array to the documentwriter
      $this->Assign($prefix . $node->m_type, $displayvalues);
    }

    /**
     * Enter description here...
     *
     * @param atkNode $node Node to be used when displaying the record
     * @param Array $record Record that should be assigned to the documentwriter
     * @param String $prefix Prefix to be used when assigning the variables (used to avoid conflicting names)
     */
    function assignDocumentSingleRecord(&$node, $record, $prefix = "")
    {
      // Assign all labels to the documentwriter
      $this->_assignLabels($node, $prefix);

      // Get all display values from the given record
      $displayvalues = $this->getRecordDisplayValues($node, $record);

      // Loop through all display values and assign them to the documentwriter
      foreach($displayvalues as $key => $displayvalue)
        $this->Assign($prefix . $key, $displayvalue);
    }

    /**
     * Assigns data to the document based on only a nodename and a selector
     *
     * @param string $nodename Name (module.node) for the node
     * @param string $selector Selector containing a SQL expression
     * @param string $prefix Prefix to be used when assigning variables to the document
     * @return boolean True if a record is found and assigned, false if not
     */
    function assignRecordByNodeAndSelector($nodename, $selector, $prefix = "")
    {
      // Do not continue and return false if no selector given
      if ($selector == "")
        return false;

      // Assign the quotation owner to the document
      $node = &atkGetNode($nodename);

      // Get the record from the database
      $records = $node->selectDb($selector, "", "", "", "", "view");

      // Do not continue and return false if no records were found
      if (count($records) == 0)
        return false;

      // Assign the record to the document.
      $this->assignDocumentSingleRecord($node, $records[0], $prefix);

      // Return succesfully
      return true;
    }

    /**
     * Assigns commonly used variables to a documentWriter
     *
     * @param unknown_type $prefix Prefix to be used when assigning the variables (used to avoid conflicting names)
     */
    function assignDocumentGenericVars($prefix = "")
    {
      // Get the current date and a reference to an atkDateAttribute in order to format the current date
      $date = adodb_getdate();
      $dateattribute = new atkDateAttribute("dummy");

      // Assign the date in short and long format as [shortdate] and [longdate]
      $this->Assign($prefix . "shortdate", $dateattribute->formatDate($date, "d-m-Y", 0));
      $this->Assign($prefix . "longdate", $dateattribute->formatDate($date, "j F Y", 0));
    }

    /**
     * Get a singleton instance of the atkDocumentWriter class for any format used
     *
     * @param string $format Document format to be used (defaults to opendocument).
     * @return atkDocumentWriter Returns singleton instance of atkDocumentWriter descendant (depends on given format)
     */
    function &getInstance($format = "opendocument")
    {
      static $s_instance = NULL;

      if ($s_instance == NULL)
      {
        if ($format == "opendocument") {
          atkdebug("Creating a new atkOpenDocumentWriter instance");
          atkimport("atk.document.atkopendocumentwriter");
          $s_instance = new atkOpenDocumentWriter();
        }
        else {
          atkdebug(sprintf("Failed to create atkDocumentWriter instance (unknown format: %s)", $format));
        }
      }
      return $s_instance;
    }

  }

?>