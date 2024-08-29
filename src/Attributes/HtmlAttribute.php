<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;

/**
 * The HtmlAttribute class is the same as a normal Attribute. It only
 * (has a different display function. For this attribute, the value is
 * rendered as-is, which means you can use html codes in the text).
 *
 * There might be times when you want the user to be able to use html tags,
 * but you don't want to have the inconvenience of using br's for each line.
 * For this reason the constructor accepts a parameter which tells it to do
 * a newline-to-br conversion.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class HtmlAttribute extends TextAttribute
{
    private $previewSkipFilteringHTMLTags = "<a><p><br>";

    private $stripTagsEnabled = true;

    public function __construct($name, $flags = 0, $nl2br = false)
    {
        parent::__construct($name, $flags);
        $this->setNl2br($nl2br);
    }

    public function display(array $record, string $mode): string
    {
        $record[$this->fieldName()] = Tools::atkArrayNvl($record, $this->fieldName(), '');

        if ($this->stripTagsEnabled && $this->getDisplayMode() !== self::MODE_SCROLL && $mode === 'list') {
            $record[$this->fieldName()] = strip_tags($record[$this->fieldName()], $this->previewSkipFilteringHTMLTags);
        }

        return parent::display($record, $mode);
    }

    public function getPreviewSkipFilteringHTMLTags(): string
    {
        return $this->previewSkipFilteringHTMLTags;
    }

    public function setPreviewSkipFilteringHTMLTags(array $skipTags = []): self
    {
        $this->previewSkipFilteringHTMLTags = $skipTags ? implode('', $skipTags) : '';

        return $this;
    }

    public function isStripTagsEnabled(): bool
    {
        return $this->stripTagsEnabled;
    }

    public function setStripTagsEnabled(bool $stripTagsEnabled): self
    {
        $this->stripTagsEnabled = $stripTagsEnabled;
        return $this;
    }
}
