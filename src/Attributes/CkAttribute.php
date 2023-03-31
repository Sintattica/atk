<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Language;
use Sintattica\Atk\Core\Tools;

/**
 * Attribute wrapper for CKEditor (the successor of FCK Editor)
 * See https://ckeditor.com
 *
 * ckeditor v5
 *
 * Plugins:
 * Autoformat, BlockQuote, Bold, CloudServices, Code, CodeBlock, Essentials, FontBackgroundColor,
 * FontColor, FontFamily, FontSize, GeneralHtmlSupport, Heading, HorizontalLine, HtmlEmbed, Image,
 * ImageCaption, ImageInsert, ImageResize, ImageStyle, ImageToolbar, ImageUpload, Indent, Italic, Link, List,
 * MediaEmbed, Paragraph, PasteFromOffice, RemoveFormat, SourceEditing, SpecialCharacters, SpecialCharactersArrows,
 * SpecialCharactersCurrency, SpecialCharactersEssentials, SpecialCharactersLatin, SpecialCharactersText, Style,
 * Subscript, Table, TableCaption, TableCellProperties, TableColumnResize, TableProperties, TableToolbar,
 * TextTransformation, Underline, WordCount
 */
class CkAttribute extends HtmlAttribute
{
    const ENTER_MODE_P = 1;     // new <p> paragraphs are created
    const ENTER_MODE_BR = 2;    // replaces all <p> tags with <br><br>
    const ENTER_MODE_DIV = 3;   // replaces all <p> tags with <div>

    private $enterMode = self::ENTER_MODE_P;

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
                'specialCharacters', 'sourceEditing', 'generalHtmlSupport'
            ],
            'image' => [
                'toolbar' => ['imageTextAlternative', 'imageStyle:full', 'imageStyle:side']
            ],
            'table' => [
                'contentToolbar' => ['tableColumn', 'tableRow', 'mergeTableCells', 'tableCellProperties', 'tableProperties']
            ],
        ],
        'htmlSupport' => [ // useful to insert classes and styles in html tags 
            'allow' => [
                ["name" => "table", "styles" => true, "classes" => true]
            ]
        ],
        'height' => '200px',
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

        // scroll mode by default
        $this->setDisplayMode(self::MODE_SCROLL);

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

    public function getEnterMode(): int
    {
        return $this->enterMode;
    }

    public function setEnterMode(int $enterMode): self
    {
        $this->enterMode = $enterMode;
        return $this;
    }

    public function value2db(array $record)
    {
        if ($record[$this->fieldName()]) {

            switch ($this->enterMode) {
                case self::ENTER_MODE_BR:
                    $record[$this->fieldName()] = Tools::strReplaceOccurrence('</p>', '', $record[$this->fieldName()]);
                    $record[$this->fieldName()] = str_replace('<p>', '', $record[$this->fieldName()]);
                    $record[$this->fieldName()] = str_replace('</p>', '</br></br>', $record[$this->fieldName()]);
                    break;

                case self:: ENTER_MODE_DIV:
                    $record[$this->fieldName()] = str_replace('<p>', '<div>', $record[$this->fieldName()]);
                    $record[$this->fieldName()] = str_replace('</p>', '</div>', $record[$this->fieldName()]);
                    break;
            }
        }

        return parent::value2db($record);
    }
}
