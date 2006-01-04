<?php
  /**
   * atkDocumentHandler class file
   *
   * @package atk
   * @subpackage handlers
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
   * Handler class for the document action
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk
   * @subpackage handlers
   */
  class atkDocumentHandler extends atkActionHandler
  {

    /**
     * The action handler.
     */
    function action_document()
    {
      // Add "Action document" to debug log to indicate this function is entered
      atkdebug("Action document");

      // Load and instantiate the documentwriter
      atkimport("atk.document.atkdocumentwriter");
      $openDocumentWriter = &atkDocumentWriter::getInstance("opendocument");

      // ATKSelector must be available to perform this action
      if ($this->m_postvars["atkselector"] == "")
      {
        atkerror("Selector parameter not available.");
        return false;
      }

      // Compose the filename to be used (doctemplatedir/module/node/<docmentfilename)
      $basepath = atkconfig("doctemplatedir", "doctemplates/");
      $module = $this->m_node->m_module;
      $node = $this->m_node->m_type;
      $tpl_file = $basepath  . $module . "/" . $node . "/" . $this->m_postvars["atkdoctpl"];

      // Check for invalid characters in filename, modulename and nodename in order to prevent hacking
      if (ereg("[<>\\/|;]", $module . $node . $this->m_postvars["atkdoctpl"]) !== false)
      {
        atkerror("Invalid filename given.");
        return false;
      }

      // Check if the file exists
      if (!is_file($tpl_file))
      {
        atkerror("Given file does not exist.");
        return false;
      }

      // Assign the record variables to the OpenOffice.org DocumentWriter
      if (method_exists($this->m_node, "assignDocumentVars"))
        $this->m_node->assignDocumentVars($this, $openDocumentWriter, $this->m_postvars["atkselector"]);
      else
        $this->assignDocumentVars($openDocumentWriter, $this->m_postvars["atkselector"]);

      // Send the document to the browser
      if (!$openDocumentWriter->display($tpl_file, $this->m_postvars["atkdoctpl"]))
        return false;

      // Halt further execution to prevent atk rendering it's interface causing to corrupt the opendocument file
      exit;
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
     * @param atkDocumentWriter $documentWriter DocumentWriter to which the variables should be assigned
     * @param atkNode $node Node for which the labels should be retrieved
     * @param String $prefix Prefix to be used when assigning the variables (used to avoid conflicting names)
     */
    function _assignLabels(&$documentWriter, &$node, $prefix)
    {
      // Get all labels for the given node
      $labels = $this->getRecordLabels($node);

      // Assign all labels to the documentwriter
      foreach($labels as $key => $label)
        $documentWriter->Assign($prefix . $key . "_label", $label);
    }

    /**
     * Enter description here...
     *
     * @param atkDocumentWriter $documentWriter DocumentWriter to which the variables should be assigned
     * @param atkNode $node Node to be used when displaying the records
     * @param Array $records Array of records that should be assigned to the documentwriter
     * @param String$prefix Prefix to be used when assigning the variables (used to avoid conflicting names)
     */
    function assignDocumentMultiRecord(&$documentWriter, &$node, $records, $prefix = "")
    {
      // Assign all labels to the documentwriter
      $this->_assignLabels(&$documentWriter, &$node);

      // Initialize the displayvalues array
      $displayvalues = array();

      // Loop through all records and add the displayvalues to the array
      foreach($records as $record)
        $displayvalues[] = $this->getRecordDisplayValues($node, $record);

      // Assign the displayvalues array to the documentwriter
      $documentWriter->Assign($prefix . $node->m_type, $displayvalues);
    }

    /**
     * Enter description here...
     *
     * @param atkDocumentWriter $documentWriter DocumentWriter to which the variables should be assigned
     * @param atkNode $node Node to be used when displaying the record
     * @param Array $record Record that should be assigned to the documentwriter
     * @param String $prefix Prefix to be used when assigning the variables (used to avoid conflicting names)
     */
    function assignDocumentSingleRecord(&$documentWriter, &$node, $record, $prefix = "")
    {
      // Assign all labels to the documentwriter
      $this->_assignLabels(&$documentWriter, &$node, $prefix);

      // Get all display values from the given record
      $displayvalues = $this->getRecordDisplayValues($node, $record);

      // Loop through all display values and assign them to the documentwriter
      foreach($displayvalues as $key => $displayvalue)
        $documentWriter->Assign($prefix . $key, $displayvalue);
    }

    /**
     * Assigns commonly used variables to a documentWriter
     *
     * @param atkDocumentWriter $documentWriter DocumentWriter to which the variables should be assigned
     * @param unknown_type $prefix Prefix to be used when assigning the variables (used to avoid conflicting names)
     */
    function assignDocumentGenericVars(&$documentWriter, $prefix = "")
    {
      // Get the current date and a reference to an atkDateAttribute in order to format the current date
      $date = adodb_getdate();
      $dateattribute = new atkDateAttribute("dummy");

      // Assign the date in short and long format as [shortdate] and [longdate]
      $documentWriter->Assign($prefix . "shortdate", $dateattribute->formatDate($date, "d-m-Y", 0));
      $documentWriter->Assign($prefix . "longdate", $dateattribute->formatDate($date, "d F Y", 1));
    }

    /**
     * Default document assignment function (assigns the given record and
     * the generic vars)
     *
     * @param atkDocumentWriter $documentWriter DocumentWriter to which the variables should be assigned
     * @param String $selector String containing the selector used to get the document from the database
     */
    function assignDocumentVars(&$documentWriter, $selector)
    {
      // Load the selected record from the database
      $record = $this->m_node->selectDb($selector, "", "", $this->m_viewExcludes, "", "document");

      // Assign the record to the documentWriter
      $this->assignDocumentSingleRecord($documentWriter, $this->m_node, $record[0]);

      // Also assign the generic (date) vars tot the documentWriter
      $this->assignDocumentGenericVars($documentWriter);
    }

  }
?>