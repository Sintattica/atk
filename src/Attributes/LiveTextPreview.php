<?php namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Ui\Page;

/**
 * The LiveTextPreview adds a preview to the page that previews realtime
 * the content of any Attribute or atkTextAttribute while it is being
 * edited.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage attributes
 *
 */
class LiveTextPreview extends DummyAttribute
{
    /**
     * Custom flags
     */
    const AF_LIVETEXT_SHOWLABEL = DummyAttribute::AF_DUMMY_SHOW_LABEL;
    const AF_LIVETEXT_NL2BR = 67108864;

    var $m_masterattribute = "";

    /**
     * Constructor
     * @param string $name The name of the attribute
     * @param string $masterattribute The attribute that should be previewed.
     * @param int $flags Flags for this attribute. Use self::AF_LIVETEXT_SHOWLABEL if the
     *                   preview should be labeled.
     *                   Use self::AF_LIVETEXT_NL2BR if the data should be nl2br'd before
     *                   display.
     */
    function __construct($name, $masterattribute, $flags = 0)
    {
        parent::__construct($name, '', $flags);
        $this->m_masterattribute = $masterattribute;
    }

    /**
     * Edit record
     * Thie method will display a live preview.
     * @param array $record Array with fields
     * @param string $fieldprefix Fieldprefix for embedded forms.
     * @return String Parsed string
     */
    function edit($record, $fieldprefix = "")
    {
        $page = Page::getInstance();
        $id = $fieldprefix . $this->fieldName();
        $master = $fieldprefix . $this->m_masterattribute;
        $page->register_scriptcode("function {$id}_ReloadTextDiv()
                                  {
                                    var NewText = document.getElementById('{$master}').value;
                                    var DivElement = document.getElementById('{$id}_preview');
                                    " . ($this->hasFlag(self::AF_LIVETEXT_NL2BR) ? "NewText = NewText.split(/\\n/).join('<br />');"
                : "") . "
                                    DivElement.innerHTML = NewText;
                                  }                                                                    
                                  ");
        $page->register_loadscript("document.entryform.{$this->m_masterattribute}.onkeyup = {$id}_ReloadTextDiv;");

        return '<span id="' . $id . '_preview">' . $record[$this->m_masterattribute] . '</span>';
    }

}

