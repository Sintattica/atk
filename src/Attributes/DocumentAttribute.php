<?php namespace Sintattica\Atk\Attributes;


use Sintattica\Atk\Utils\DirectoryTraverser;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;

/**
 * DocumentAttribute class for adding document generation functionality to a node
 *
 * @author guido <guido@ibuildings.nl>
 * @package atk
 * @subpackage attributes
 */
class DocumentAttribute extends DummyAttribute
{

    /**
     * DocumentAttribute constructor
     *
     * @param string $name description
     * @param integer $flags description
     */
    function __construct($name, $flags = 0)
    {
        // Call parent constructor with addition of the af_hide_add flag
        // because this attribute should not be used in add operations where
        // the recorddata is not yet present in the database.
        parent::__construct($name, "", $flags | self::AF_HIDE_ADD | DummyAttribute::AF_DUMMY_SHOW_LABEL);
    }

    /**
     * Gets the display code for the document selector in the specified mode
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return string HTML code for selection control
     */
    function edit($record = '', $fieldprefix = '', $mode = '')
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
        if (($mode != "csv") && ($mode != "plain")) {
            return $this->documentSelector($record, $mode);
        } else {
            return "";
        }
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
        $dirtrav = new DirectoryTraverser();
        return $dirtrav->getDirContents($path);
    }

    /**
     * Gets the display code for the document selector in the specified mode
     *
     * @param array $record The record that contains this attribute's value
     * @param string $mode Name of the mode in which the document selector will be displayed
     * @param boolean $addForm Do we need to add the form html code?
     *
     * @return string Display code for this attribute in the specified mode
     */
    function documentSelector($record, $mode = "", $addForm = true)
    {
        // Compose the path to use when searching for docuemnt templates
        $basepath = Config::getGlobal("doctemplatedir", "doctemplates/");
        $module = $this->m_ownerInstance->m_module;
        $node = $this->m_owner;
        $path = $basepath . $module . "/" . $node;

        // Only continue if the path is valid and exists
        if (!is_dir($path)) {
            return $this->text("invalid_path");
        }

        // Get the list of document files
        $contents = $this->getDocumentFiles($path, $record);

        if (empty($contents)) {
            return $this->text("no_document_templates_found");
        }

        // Add the select box to the html
        $html = '<select class="form-control" name="atkdoctpl">';
        foreach ($contents as $entry) {
            if (is_file($path . "/" . $entry)) { // todo: && check of tie een open office extensie heeft
                $html .= '<option value="' . urlencode($entry) . '">' . $entry;
            }
        }
        $html .= '</select> ';

        // Add the button to the html
        $selector = $this->m_ownerInstance->primaryKey($record);
        $onclickscript = 'window.location="' . SessionManager::sessionUrl(Tools::dispatch_url($module . "." . $node,
                "document", array("atkselector" => $selector)),
                SessionManager::SESSION_DEFAULT) . '&atkdoctpl="+this.form.atkdoctpl.value;';

        $html .= '<input type="button" class="btn_doc_open" name="atkdocument" value="' . Tools::atktext("open") . '" onclick=\'' . $onclickscript . '\'>';

        // Wrap the input elements in a unique session form when no form is present yet (in list and view mode)
        if ((($mode == "list") || ($mode == "view")) && $addForm) {
            static $documentSelectorFormCounter = 0;
            $html = '<form name="documentSelectorForm' . ++$documentSelectorFormCounter . '">' . Tools::session_form() . $html . '</form>';
        }

        // Return the generated html
        return $html;
    }

}


