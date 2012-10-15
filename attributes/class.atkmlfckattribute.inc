<?php
/**
 * The atkMlFckAttribute class represents an multilanguage FCK-attribute of an
 * atkNode, and will show different FCK editors for the various languages
 * configured.
 *
 * @author Harrie Verveer <harrie@ibuildings.nl>
 * @package atk
 * @subpackage attributes
 *
 */
class atkMlFckAttribute extends atkMlTextAttribute
{
    var $m_editorname = "";
    var $m_urlpath = "";
    var $fck_opt=array( 'ToolbarSet'    => 'Default',
                        'Width'         => '100%',
                        'Height'        => '300px');

    /**
     * Create a new instance of the atkMlFckAttribute
     *
     * @param string $name  the name of our attribute
     * @param int    $size  the size for this attribute
     * @param int    $flags the flags for this attribute
     * @param array  $opt   the options to pass to the FCK editor
     */
    public function __construct($name, $flags=0, $size=0, Array $opt=array())
    {
        $config_fck = atkconfig('fck');

        if(is_array($config_fck))
        {
            $this->fck_opt = array_merge($this->fck_opt,$config_fck);
        }

        if(is_array($opt))
        {
            $this->fck_opt = array_merge($this->fck_opt,$opt);
        }

        $this->fck_opt["Language"] = atkconfig("language");
        $this->atkMlTextAttribute($name, $flags, $size);
    }

    /**
     * Returns the piece of HTML that can be used to edit the field's value. In
     * this particular occassion we will include the FCK library and setup the
     * right parameters for this particular set of editors.
     *
     * @param array     $record     array with fields
     * @param string    $prefix     The fieldprefix to put in front of the name
     *                              of any html form element for this attribute.
     * @param string    $mode       The mode we're in ('add' or 'edit')
     * @return string piece of html code with a textarea
     */
    public function edit($record="", $prefix="", $mode="")
    {
        include_once(atkconfig('atkroot') . "atk/attributes/fck/fckeditor.php");

        $languages = $this->getLanguages();
        $resultHtml = '';
        $fieldName = $this->formName();
        
        $fieldNameLng = $fieldName . '[' . $languages[0] . ']';
        $resultHtml .= $this->editFck($record, $fieldprefix, $fieldNameLng);

        return $resultHtml;
    }

    /**
     * Adds the attribute's edit / hide HTML code to the edit array.
     *
     * This method is called by the node if it wants the data needed to create
     * an edit form.
     *
     * @param String $mode     the edit mode ("add" or "edit")
     * @param array  $arr      pointer to the edit array
     * @param array  $defaults pointer to the default values array
     * @param array  $error    pointer to the error array
     * @param String $fieldprefix   the fieldprefix
     */
    public function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
        $fckIsEdited = $this->m_edited;
        parent::addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);
        $this->m_edited = $fckIsEdited;

        $languages = $this->getLanguages();
        $langCount = count($languages);

        for($i = 1; $i < $langCount; $i++)
        {
            $curlng = $languages[$i];
            $entry = array( "name" => $this->m_name . "_ml",
                            "obligatory" => $this->hasFlag(AF_OBLIGATORY),
                            "attribute" => $this);

            $entry["label"] = $this->label($defaults) .
                ' (<label id="' . $fieldprefix . $this->formName().'_label_ ' .
                strtolower($curlng) . '">' .
                atktext("language_".strtolower($curlng)) .
                '</label>)';

            $entry["id"] = $this->getHtmlId($fieldprefix);
            $entry["tabs"] = $this->m_tabs;
            $entry["sections"] = $this->m_sections;

            $fieldNameLng = $this->formName() . '[' . $curlng . ']';
            $entry["html"] = $this->editFck($defaults, $fieldprefix, $fieldNameLng);

            $arr["fields"][] = $entry;
            $this->m_edited = false;
        }
    }

    /**
     * Returns the html code to edit the value of this attribute
     *
     * @param array $record Array with fields
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $fieldname the fieldname for this field. If left empty the
     *                          default will be used.
     * @return string the html that displays the edit field
     */
    public function editFck($record="", $fieldprefix="", $fieldname="")
    {
        if ('' == $fieldname)
        {
            $fieldname = $this->fieldName();
        }

        if (strstr($fieldname, '['))
        {
            preg_match('/^(.+)\[(.+?)\]$/', $fieldname, $matches);
            if (count($matches) == 3)
            {
                $value = $record[$matches[1]][$matches[2]];
            } 
            else
            {
                $value = isset($record[$fieldname]) ? $record[$fieldname] : "";
            }
        }

        $id = $fieldprefix . $fieldname;
        $this->registerKeyListener($id, KB_CTRLCURSOR);

        $oFCKeditor = new FCKeditor($fieldprefix . $fieldname);
        $oFCKeditor->BasePath = atkconfig("atkroot") . "atk/attributes/fck/";

        $oFCKeditor->Value = $value;
        $oFCKeditor->ToolbarSet = $this->fck_opt['ToolbarSet'];
        $oFCKeditor->Width = $this->fck_opt['Width'];
        $oFCKeditor->Height = $this->fck_opt['Height'];
        $oFCKeditor->Config["AutoDetectLanguage"] = false;
        $oFCKeditor->Config["DefaultLanguage"] = $this->fck_opt["Language"];

        if (!empty($this->fck_opt['CustomConfigurationsPath']))
        {
            $oFCKeditor->Config["CustomConfigurationsPath"] = "../../../../" .
                    $this->fck_opt["CustomConfigurationsPath"];
        }

        return $oFCKeditor->CreateHtml();
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * @param array $rec The record that holds this attribute's value.
     * @return String The database compatible value
     */
    public function value2db($rec)
    {
        if (is_array($rec)&&isset($rec[$this->fieldName()][$this->m_language]))
        {
            $dbval = $this->escapeSQL(preg_replace("/\&quot;/Ui","\"",$rec[$this->fieldName()][$this->m_language]));
            return $dbval;
        }

        return NULL;
    }
}