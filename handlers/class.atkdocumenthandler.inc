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
        return;
      }

      // Compose the filename to be used (doctemplatedir/module/node/<docmentfilename)
      $basepath = atkconfig("doctemplatedir", "doctemplates/");
      $module = $this->m_node->m_module;
      $node = $this->m_node->m_type;
      $tpl_file = $basepath  . $module . "/" . $node . "/" . $this->m_postvars["atkdoctpl"];

      // Check for invalid characters in filename, modulename and nodename in order to prevent hacking
      if (ereg("[<>\\/|]", $module . $node . $this->m_postvars["atkdoctpl"]) !== false)
      {
        atkerror("Invalid filename given.");
        return;
      }

      // Check if the file exists
      if (!is_file($tpl_file))
      {
        atkerror("Given file does not exist.");
        return;
      }

      // Assign the record variables to the OpenOffice.org DocumentWriter
      $this->assignRecordVars($openDocumentWriter);

      // Send the document to the browser
      $openDocumentWriter->display($tpl_file, $this->m_postvars["atkdoctpl"]);

      // Halt further execution to prevent atk rendering it's interface causing to corrupt the opendocument file
      exit;
    }

    function assignRecordVars(&$documentWriter)
    {
      // Get the node and select the recordattributes from the database
      $node = $this->m_node;
      $record = $node->selectDb($this->m_postvars['atkselector'], "", "", $this->m_viewExcludes, "", "document");

      // Loop through the attributes in order to assign them all to the documentwriter
      foreach (array_keys($node->m_attribIndexList) as $i)
      {
        $p_attrib = &$node->m_attribList[$node->m_attribIndexList[$i]["name"]];

        // Get the internal attribute name
        $key = $node->m_attribIndexList[$i]["name"];

        // Get the Label of the attribute (can be suppressed with AF_NOLABEL or AF_BLANKLABEL)
        if ($p_attrib->hasFlag(AF_NOLABEL) || $p_attrib->hasFlag(AF_BLANKLABEL))
          $label = "";
        else
          $label = $p_attrib->label($record);

        // Get the display value by calling <attribute>_display().
        // An <attributename>_display function may be provided in a derived
        // class to display an attribute. If it exists we will use that method
        // else we will just use the attribute's display method.
        $funcname = $p_attrib->m_name."_display";
        if (method_exists($node, $funcname))
          $value = $node->$funcname($record[0], "plain");
        else
          $value = $p_attrib->display($record[0], "plain");

        // Supply the attribute label as [key_label] and value as [key]
        // where key is the name of the attribute
        $documentWriter->Assign($key . "_label", $label);
        $documentWriter->Assign($key, $value);
      }
    }

  }
?>