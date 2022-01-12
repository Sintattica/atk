<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;

/**
 * The HtmlAttribute class is the same as a normal Attribute. It only
 * (has a different display function. For this attribute, the value is
 * rendered as-is, which means you can use html codes in the text.
 *
 * There might me times where you want the user to be able to use html tags,
 * but you don't want to have the inconvenience of using br's for each line.
 * For this reason the constructor accepts a parameter which tells it to do
 * a newline-to-br conversion.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class HtmlAttribute extends TextAttribute
{

    private $previewSkipFilteringHTMLTags = "<a><p><br>";


    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags of the attribute
     * @param bool $nl2br nl2br boolean
     */
    public function __construct($name, $flags = 0, $nl2br = false)
    {
        parent::__construct($name, $flags);
        $this->setNl2br($nl2br);
    }

    public function display($record, $mode)
    {
        $record[$this->fieldName()] = Tools::atkArrayNvl($record, $this->fieldName(), '');

        if ($this->getDisplayMode() !== self::MODE_SCROLL && $mode === 'list') {
            $record[$this->fieldName()] = strip_tags($record[$this->fieldName()], '<a><p><br>');
        }

        return parent::display($record, $mode);
    }


    /**
     * @return string
     */
    public function getPreviewSkipFilteringHTMLTags(): string
    {
        return $this->previewSkipFilteringHTMLTags;
    }

    /**
     * @param array $skipTags
     * @return CkAttribute
     */
    public function setPreviewSkipFilteringHTMLTags(array $skipTags): CkAttribute
    {
        foreach ($skipTags as $tag) {
            $this->previewSkipFilteringHTMLTags .= $tag;
        }

        return $this;
    }
}
