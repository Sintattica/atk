<?php
  /**
   * atkDocumentAttribute class file
   *
   * @package atk
   * @subpackage attributes
   *
   * @author ivo <ivo@ibuildings.nl>
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   *
   * @version $Revision$
   * $Id$
   */

  // Use the atkdummyattribute to extend from
  useattrib("atkdummyattribute");

  /**
   * DocumentAttribute class for adding document generation functionality to a node
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   */
  class atkDocumentAttribute extends atkDummyAttribute
  {

    /**
     * atkDocumentAttribute constructor
     *
     * @param string $name description
     * @param integer $flags description
     */
    function atkDocumentAttribute($name, $flags=0)
    {
      // Call parent constructor with addition of the af_hide_add flag
      // because this attribute should not be used in add operations where
      // the recorddata is not yet present in the database.
      $this->atkDummyAttribute($name, "", $flags|AF_HIDE_ADD|AF_DUMMY_SHOW_LABEL);
    }

    /**
     * Gets the display code for the document selector in the specified mode
     *
     * @param array $record The record that holds the value for this attribute.
     * @return string HTML code for selection control
     */
    function edit($record, $mode)
    {
      return $this->documentSelector($record, $mode, false);
    }

    /**
     * Gets the display code for the document selector in the specified mode
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $mode Name of the mode in which the attribute should be displayed
     * @return string description
     */
    function display($record, $mode)
    {
      if (($mode!="csv") && ($mode!="plain"))
        return $this->documentSelector($record, $mode);
      else
        return "";
    }

    /**
     * Gets document files from given path for a specific record
     *
     * @param string $path Path to search for documents
     * @param array $record Array of attributes for the record on which the documentfiles to be offered must be enumerated
     * @return array List of files to offer
     */
    function getDocumentFiles($path, $record)
    {
      // Read the directory contents using the directorytraverser
      $dirtrav = &atknew("atk.utils.atkdirectorytraverser");
      return $dirtrav->getDirContents($path);
    }

    /**
     * Gets the display code for the document selector in the specified mode
     *
     * @param string $mode Name of the mode in which the document selector will be displayed
     * @return string Display code for this attribute in the specified mode
     */
    function documentSelector($record, $mode = "", $addForm=true)
    {
      // Compose the path to use when searching for docuemnt templates
      $basepath = atkconfig("doctemplatedir", "doctemplates/");
      $module = $this->m_ownerInstance->m_module;
      $node = $this->m_owner;
      $path = $basepath . $module . "/" . $node;

      // Only continue if the path is valid and exists
      if (is_dir($path))
      {
        // Get the list of document files
        $contents = $this->getDocumentFiles($path, $record);

        // Add the select box to the html
        $html = '<select name="atkdoctpl">';
        foreach ($contents as $entry)
        {
          if (is_file($path . "/" . $entry)) // todo: && check of tie een open office extensie heeft
          {
            $html .= '<option value="' . urlencode($entry) . '">' . $entry;
          }
        }
        $html .= '</select>';

        // Add the button to the html
        $selector = $this->m_ownerInstance->primaryKey($record);        
        $onclickscript = 'window.location="'.session_url(dispatch_url($module . "." . $node, "document", array("atkselector"=>$selector)), SESSION_DEFAULT).'atkdoctpl="+this.form.atkdoctpl.value;';

        $html .= '<input type="button" class="btn_doc_open" name="atkdocument" value="' . atktext("open") . '" onclick=\''.$onclickscript.'\'>';

        // Wrap the input elements in a unique session form when no form is present yet (in list and view mode)
        if ((($mode=="list")||($mode=="view")) && $addForm) {
          static $documentSelectorFormCounter = 0;
          $html = '<form name="documentSelectorForm' . ++$documentSelectorFormCounter . '">' . session_form(). $html . '</form>';
        }

        // Return the generated html
        return $html;
      }
      return "";
    }
  }

?>