<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Language;

/**
 * Attribute wrapper for CKEditor (the successor of FCK Editor)
 * See http://ckeditor.com.
 */
class CkAttribute extends HtmlAttribute
{
    /**
     * @var array CKEditor configuration (default)
     */
    private $ckOptions = [
        'removePlugins' => ['Title', 'MathType', 'ChemType'],
        'toolbar' => [
            'items' => [
                'heading', '|',
                'bold', 'italic', 'underline', 'strikethrough', 'subscript', 'superscript', 'horizontalLine', 'blockQuote', '|',
                'highlight', 'fontBackgroundColor', 'fontColor', 'fontSize', 'fontFamily', 'removeFormat', '|',
                'bulletedList', 'numberedList', 'indent', 'outdent', 'alignment', '|',
                'link', 'insertTable', 'imageInsert', 'mediaEmbed', '|',
                'undo', 'redo', '|',
                'htmlEmbed', 'code', 'codeBlock', '|',
                'specialCharacters',
            ],
            'image' => [
                'toolbar' => ['imageTextAlternative', 'imageStyle:full', 'imageStyle:side']
            ],
            'table' => [
                'contentToolbar' => ['tableColumn', 'tableRow', 'mergeTableCells', 'tableCellProperties', 'tableProperties']
            ],
        ],
        'height' => '200px'
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
        $this->ckOptions['toolbar']['language'] = Language::getLanguage();
        //$this->ckOptions['wsc_lang'] = $this->ckOptions['scayt_sLang'] = Tools::atktext('locale');
        $this->ckOptions = array_merge($this->ckOptions, Config::getGlobal('ck_options'), $options);

        $this->setNl2br(false);
        $this->setHtmlSpecialChars(false);

        parent::__construct($name, $flags);
    }

    public function edit($record, $fieldprefix, $mode): string
    {
        $page = $this->getOwnerInstance()->getPage();
        $id = $this->getHtmlId($fieldprefix);

        // register CKEditor main script
        $page->register_script(Config::getGlobal('assets_url') . 'lib/ckeditor5/ckeditor.js');

        // activate CKEditor
        $options = json_encode($this->ckOptions);

        $page->register_loadscript("ClassicEditor
            .create( document.querySelector( '#$id' ), $options)
            .then( editor => {
                editor.editing.view.change( writer => writer.setStyle( 'height', '" . $this->ckOptions['height'] . "', editor.editing.view.document.getRoot() ));
                window.editor = editor
            })
			.catch( error => console.error( 'Oops, something went wrong: ', error) );"
        );

        return parent::edit($record, $fieldprefix, $mode);
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
    public function isEmpty($record): bool
    {
        $record[$this->fieldName()] = trim(strip_tags($record[$this->fieldName()], '<div>'));

        return parent::isEmpty($record);
    }

    /**
     * Set the ckEditor height
     * Ex: 200px or 10rem ...
     *
     * @param string $height
     * @return CkAttribute
     */
    public function setHeight(string $height): self
    {
        $this->ckOptions['height'] = $height;
        return $this;
    }

    protected function formatPostfixLabel(): string
    {
        return "";
    }
}
