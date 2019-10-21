<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Language;
use Sintattica\Atk\Core\Tools;

/**
 * Attribute wrapper for CKEditor (the successor of FCK Editor)
 * See http://ckeditor.com.
 */
class CkAttribute extends HtmlAttribute
{
    /**
     * @var array CKEditor configuration (default)
     */
    protected $ckOptions = [
        'toolbar' => [
            ['name' => 'clipboard', 'items' => ['Cut', 'Copy', 'Paste', 'PasteText', 'PasteFromWord', '-', 'Undo', 'Redo', '-', 'Print']],
            ['name' => 'editing', 'items' => ['Find', 'Replace', '-', 'SelectAll', '-', 'Scayt']],
            ['name' => 'links', 'items' => ['Link', 'Unlink']],
            ['name' => 'insert', 'items' => ['Image', 'Table', 'HorizontalRule', 'SpecialChar']],
            '/',
            ['name' => 'basicstyles', 'items' => ['Bold', 'Italic', 'Underline', 'Strike', 'Subscript', 'Superscript', '-', 'RemoveFormat']],
            ['name' => 'paragraph', 'items' => ['NumberedList', 'BulletedList', '-', 'Outdent', 'Indent', '-', 'Blockquote', '-', 'JustifyLeft', 'JustifyCenter', 'JustifyRight', 'JustifyBlock']],
            ['name' => 'styles', 'items' => ['Format', 'FontSize']],
            ['name' => 'colors', 'items' => ['TextColor', 'BGColor']],
        ],
        'removePlugins' => 'elementspath',
        'height' => 300
    ];

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for the attribute
     * @param array $options CKEditor configuration options
     */
    public function __construct($name, $flags = 0, $options = [])
    {
        $this->ckOptions['language'] = Language::getLanguage();
        $this->ckOptions['wsc_lang'] = $this->ckOptions['scayt_sLang'] = Tools::atktext('locale');
        $this->ckOptions = array_merge($this->ckOptions, Config::getGlobal('ck_options'), $options);

        parent::__construct($name, $flags);
    }

    public function edit($record, $fieldprefix, $mode)
    {
        $page = $this->getOwnerInstance()->getPage();
        $id = $this->getHtmlId($fieldprefix);

        // register CKEditor main script
        $page->register_script(Config::getGlobal('assets_url').'lib/ckeditor/ckeditor.js');
        $page->register_script(Config::getGlobal('assets_url').'lib/ckeditor/adapters/jquery.js');

        // activate CKEditor
        $options = json_encode($this->ckOptions);
        $result = parent::edit($record, $fieldprefix, $mode);

        $result .= '<script>';
        $result .= "jQuery('#$id').ckeditor($options);";
        $result .= '</script>';

        return $result;
    }

    /**
     * Check if a record has an empty value for this attribute.
     *
     * If the record only contains tags or spaces, we consider it empty. We exclude the div
     * tag, since it is often used as (for instance) a placeholder for script results.
     *
     * @param array $record The record that holds this attribute's value.
     *
     * @return bool
     */
    public function isEmpty($record)
    {
        $record[$this->fieldName()] = trim(strip_tags($record[$this->fieldName()], '<div>'));

        return parent::isEmpty($record);
    }
}
