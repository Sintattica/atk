<?php namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Language;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;

/**
 * Attribute wrapper for CKEditor (the successor of FCK Editor)
 * See http://ckeditor.com
 *
 */
class CkAttribute extends TextAttribute
{
    /**
     * @var array CKEditor configuration (default)
     */
    protected $ckOptions = [
        // the toolbar groups arrangement
        'toolbarGroups' => [
            ['name' => 'clipboard', 'groups' => ['clipboard', 'undo', 'document']],
            ['name' => 'editing', 'groups' => ['find', 'selection', 'spellchecker']],
            ['name' => 'links'],
            ['name' => 'insert'],
            '/',
            ['name' => 'basicstyles', 'groups' => ['basicstyles', 'cleanup']],
            ['name' => 'paragraph', 'groups' => ['list', 'indent', 'align']],
            ['name' => 'styles'],
            ['name' => 'colors'],
        ],
        // remove some buttons
        'removeButtons' => 'Save,NewPage,Preview,Anchor,Flash,Smiley,PageBreak,Iframe,Subscript,Superscript,Font,Styles',
        // remove display of html tags on bottom bar
        'removePlugins' => 'elementspath',
        // simplify the windows
        'removeDialogTabs' => 'image:advanced;link:advanced',
        // set the size
        'height' => 250,
    ];

    /**
     * Constructor
     * @param string $name Name of the attribute
     * @param int $flags Flags for the attribute
     * @param array $options CKEditor configuration options (overrides default)
     */
    public function __construct($name, $flags = 0, $options = null)
    {
        /** update CKEditor configuration options */
        $this->ckOptions['language'] = Language::getLanguage();
        $this->ckOptions['wsc_lang'] = $this->ckOptions['scayt_sLang'] = Tools::atktext('locale');
        // global config override
        if (is_array(Config::getGlobal('ck_options'))) {
            $this->ckOptions = array_merge($this->ckOptions, Config::getGlobal('ck_options'));
        }
        // instance override
        if (is_array($options)) {
            $this->ckOptions = array_merge($this->ckOptions, $options);
        }

        parent::__construct($name, 0, $flags);
    }

    public function edit($record, $fieldprefix, $mode)
    {
        $page = $this->getOwnerInstance()->getPage();

        $id = $this->getHtmlId($fieldprefix);

        // register CKEditor main script
        $page->register_script(Config::getGlobal('assets_url').'lib/ckeditor/ckeditor.js');

        // activate CKEditor
        $options = json_encode($this->ckOptions);
        $page->register_loadscript("CKEDITOR.replace('$id', $options);");

        return parent::edit($record, $fieldprefix, $mode);
    }

    public function display($record, $mode)
    {
        return Tools::atkArrayNvl($record, $this->fieldName(), "");
    }

    public function value2db($rec)
    {
        if (is_array($rec) && isset($rec[$this->fieldName()])) {
            $dbval = $this->escapeSQL(preg_replace("/\&quot;/Ui", "\"", $rec[$this->fieldName()]));

            return $dbval;
        }

        return null;
    }

    /**
     * Check if a record has an empty value for this attribute.
     *
     * If the record only contains tags or spaces, we consider it empty. We exclude the div
     * tag, since it is often used as (for instance) a placeholder for script results.
     *
     * @param array $record The record that holds this attribute's value.
     * @return boolean
     */
    public function isEmpty($record)
    {
        $record[$this->fieldName()] = trim(strip_tags($record[$this->fieldName()], '<div>'));

        return parent::isEmpty($record);
    }

    private function getSpellCheckerLang($atkLang)
    {
        switch ($atkLang) {
            case 'da';
                return 'da_DK'; // Danish
            case 'de':
                return 'de_DE'; // German
            case 'el':
                return 'el_GR'; // Greek
            case 'en':
                return 'en_US'; // English
            case 'es':
                return 'es_ES'; // Spanish
            case 'fi':
                return 'fi_FI'; // Finnish
            case 'fr':
                return 'fr_FR'; // French
            case 'it':
                return 'it_IT'; // Italian
            case 'nl':
                return 'nl_NL'; // Dutch
            case 'no':
                return 'nb_NO'; // Norwegian
            case 'pt':
                return 'pt_PT'; // Portuguese
            case 'sv':
                return 'sv_SE'; // Swedish
            default:
                return 'en_US'; // Default: English
        }
    }
}
