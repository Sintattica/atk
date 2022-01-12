<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Utils\BrowserInfo;
use Sintattica\Atk\Core\Tools;

/**
 * The atkTextAttribute class represents an attribute of a node
 * that is a big text field.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 *
 * @todo autadjust needs to be modified as not every character == 1 column,
 *       perhaps forcing every textattribute to use a non-proportional font?
 */
class TextAttribute extends Attribute
{
    /**
     * Display long text in row.
     * You can specify maxChars for the length of the text to show.
     */
    public const MODE_INLINE = 'inline';

    /**
     * Display the text stretched in height.
     * You must specify a min-width.
     */
    public const MODE_SCROLL = 'scroll';

    /**
     * Display the text as wrapped. As default it truncates the text at
     * specified length, it can be set with setMaxChars()
     */
    public const MODE_DEFAULT = 'default';

    private const MODES_ALLOWED = [self::MODE_DEFAULT, self::MODE_SCROLL, self::MODE_INLINE];

    private $displayMode = self::MODE_DEFAULT;
    private $wrapMode = 'soft';
    private $minWidth = "200px";
    private $maxHeight = "200px";
    private $maxChars = null;

    // number of rows of the edit box
    public $m_rows = 10;
    public $m_cols;
    public $m_autoadjust;

    /**
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param array $options : rows, cols, autoadjust
     */
    public function __construct($name, $flags = 0, $options = [])
    {
        parent::__construct($name, $flags);

        if (isset($options['rows'])) {
            $this->m_rows = $options['rows'];
        }
        if (isset($options['cols'])) {
            $this->m_cols = $options['cols'];
        }
        if (isset($options['autoadjust'])) {
            $this->m_autoadjust = $options['autoadjust'];
        }

    }

    /**
     * Returns the current wrap mode.
     *
     * @return string wrap mode
     */
    public function getWrapMode()
    {
        return $this->wrapMode;
    }

    /**
     * Sets the wrap mode for the text area.
     *
     * @param string $mode wrap mode ('soft', 'hard' or 'off')
     */
    public function setWrapMode($mode)
    {
        $this->wrapMode = $mode;
    }


    public function display($record, $mode)
    {
        $display = parent::display($record, $mode);

        if ($mode == 'list') {
            $style = "min-width: {$this->minWidth};";
            $classes = '';

            switch ($this->displayMode) {
                case self::MODE_INLINE:
                    if ($this->maxChars) {
                        $record[$this->fieldName()] = $record[$this->fieldName()] != null ? Tools::truncateHTML($record[$this->fieldName()], $this->maxChars, '...') : null;
                        $display = parent::display($record, $mode);
                    }
                    break;
                case self::MODE_SCROLL:
                    $classes = 'text-wrap';
                    $style .= " max-height: {$this->maxHeight}; overflow-y: auto; ";
                    break;
                default:
                    $classes = 'text-wrap';
                    $maxChars = $this->maxChars ?: '200';
                    $record[$this->fieldName()] = $record[$this->fieldName()] != null ? Tools::truncateHTML($record[$this->fieldName()], $maxChars, '...') : null;
                    $display = parent::display($record, $mode);

            }

            $display = "<div class='$classes' style='$style'>$display</div>";
        }

        return $display;
    }


    public function edit($record, $fieldprefix, $mode)
    {

        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);

        $style = '';
        foreach ($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }

        // list mode, show a small textarea, until it get's focus
        // and is inflated to a big textarea
        if ($mode == 'list') {
            $page = $this->m_ownerInstance->getPage();

            // NOTE:
            // The onblur function uses a small timeout to make sure a click on a
            // new element is performed earlier then the swap of fields.
            // If we don't do this the click might end up wrong.
            $page->register_loadscript("
          \$('{$id}').onfocus = function() {
            \$('{$id}_textarea').value = this.value;
            Element.hide(this);
            Element.show('{$id}_textarea');
            \$('{$id}_textarea').focus();
          };

          \$('{$id}_textarea').onblur = function() {
            \$('{$id}').value = this.value;
            window.setTimeout(function() {
              Element.hide('{$id}_textarea');
              Element.show('{$id}'); }, 500
            );
          };
        ");

            $result = '<textarea id="' . $id . '" name="' . $name . '" wrap="soft" rows="1" cols="20" style="overflow:hidden;' . $style . '">';
            $result .= htmlspecialchars($record[$this->fieldName()]);
            $result .= '</textarea>';
            $result .= '<textarea id="' . $id . '_textarea" wrap="' . $this->getWrapMode() . '" rows="5" cols="40" style="display: none"></textarea>';

            return $result;
        }

        $text = isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : '';

        if ($this->m_cols != 0) {
            $cols = $this->m_cols;
        } else {
            $cols = $this->maxInputSize();
        }
        $rows = $this->m_rows;
        if ($rows == '' || $rows == 0) {
            $rows = 10;
        }

        if ($this->m_autoadjust) {
            $this->doAutoAdjust(htmlspecialchars($text), $rows, $cols);
        }

        $result = sprintf('<textarea id="%s" name="%s" wrap="%s" ', $id, $name, $this->getWrapMode());
        if ($rows) {
            $result .= 'rows="' . $rows . '" ';
        }
        if ($cols) {
            $result .= 'cols="' . $cols . '" ';
        }
        if ($this->m_maxsize > 0) {
            $result .= 'maxlength="' . $this->m_maxsize . '" '; // now supported in HTML5
        }
        $result .= $this->getCSSClassAttribute();
        if ($style != '') {
            $result .= ' style="' . $style . '"';
        }
        $result .= ">" . htmlspecialchars($text) . '</textarea>';

        return $result;
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        if ($mode == 'add' || $mode == 'update') {
            $query->addField($this->fieldName(), $this->value2db($record), '', '', !$this->hasFlag(self::AF_NO_QUOTES), true);
        } else {
            $query->addField($this->fieldName(), '', $tablename, $fieldaliasprefix, !$this->hasFlag(self::AF_NO_QUOTES), true);
        }
    }

    /**
     * Add's slashes to the string for the database.
     *
     * @param array $rec Array with values
     *
     * @return string with slashes
     */
    public function value2db(array $rec)
    {
        $db = $this->getDb();
        if ($db->getType() != 'oci9' || $this->dbFieldType() != 'text') {
            return $db->escapeSQL($rec[$this->fieldName()]);
        } else {
            return $rec[$this->fieldName()];
        } //CLOB in oci9 don't need quotes to be escaped EVIL HACK! THIS IS NOT ATKTEXTATTRIBUTE's PROBLEM!
    }

    /**
     * Removes slashes from the string.
     *
     * @param array $rec Array with values
     *
     * @return string without slashes
     */
    public function db2value($rec)
    {
        if (isset($rec[$this->fieldName()])) {
            return $rec[$this->fieldName()];
        }

        return null;
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return string The 'generic' type of the database field for this
     *                attribute.
     */
    public function dbFieldType()
    {
        // make sure our metadata is set
        if (is_object($this->m_ownerInstance)) {
            $this->m_ownerInstance->setAttribSizes();
        }

        if ($this->m_dbfieldtype == '') {
            return 'text';
        }

        return $this->m_dbfieldtype;
    }

    /**
     * Fetch the metadata about this attrib from the table metadata, and
     * process it.
     *
     * @param array $metadata The table metadata from the table for this
     *                        attribute.
     */
    public function fetchMeta($metadata)
    {
        $this->m_dbfieldtype = isset($metadata[$this->fieldName()]['gentype']) ? $metadata[$this->fieldName()]['gentype'] : null;
        if ($this->m_dbfieldtype == 'string') {
            parent::fetchMeta($metadata);
        }
    }

    /**
     * Parses the data that we are going to display in the textfield
     * and adjust rows to ensure that all the data is actually displayed.
     *
     * @param string $data Data we want to display
     * @param int $rows Rows of the textarea
     * @param int $cols Columns of the textarea
     */
    public function doAutoAdjust($data, &$rows, &$cols)
    {
        $browser = new BrowserInfo();
        $maxlinechars = 0;
        for ($counter = 0, $linecharacters = 0, $rowsrequired = 1; $counter < Tools::atk_strlen($data); $counter++, $linecharacters++) {
            // Current character we are parsing
            $character = substr($data, $counter, 1);

            // If we encounter a newline character or the number of characters
            // equals the number of columns we have (with IE)...
            if ($character == chr(13) || ($linecharacters == $cols && $browser->browser == 'MSIE')) {
                if ($linecharacters > $maxlinechars) {
                    $maxlinechars = $linecharacters;
                }
                // We start another line
                $linecharacters = 0;
                // But need another row
                ++$rowsrequired;
            }
        }
        // If we need more rows, we set them
        if ($rowsrequired > $rows) {
            $rows = $rowsrequired;
        }
        // IE wraps characters, other don't, so if we're not dealing with IE
        // we need more columns
        if ($maxlinechars > $cols && $browser->browser !== 'MSIE') {
            $cols = $maxlinechars;
        }
    }

    /**
     * @return int|mixed
     */
    public function getMRows(): int
    {
        return $this->m_rows;
    }

    /**
     * @param int|mixed $m_rows
     * @return TextAttribute
     */
    public function setMRows(int $m_rows): TextAttribute
    {
        $this->m_rows = $m_rows;
        return $this;
    }

    /**
     * @return mixed
     */
    public function getMCols()
    {
        return $this->m_cols;
    }

    /**
     * @param mixed $m_cols
     * @return TextAttribute
     */
    public function setMCols($m_cols)
    {
        $this->m_cols = $m_cols;
        return $this;
    }

    /**
     * @return mixed
     */
    public function getMAutoadjust()
    {
        return $this->m_autoadjust;
    }

    /**
     * @param mixed $m_autoadjust
     * @return TextAttribute
     */
    public function setMAutoadjust($m_autoadjust)
    {
        $this->m_autoadjust = $m_autoadjust;
        return $this;
    }

    /**
     * @return mixed|string
     */
    public function getDisplayMode(): string
    {
        return $this->displayMode;
    }

    /**
     * @param mixed|string $displayMode
     * @return TextAttribute
     */
    public function setDisplayMode(string $displayMode): self
    {

        if (in_array($displayMode, self::MODES_ALLOWED)) {
            $this->displayMode = $displayMode;
        }

        return $this;
    }

    /**
     * @return mixed|string
     */
    public function getMinWidth(): string
    {
        return $this->minWidth;
    }

    /**
     * @param mixed|string $minWidth
     * @return TextAttribute
     */
    public function setMinWidth(string $minWidth): self
    {
        $this->minWidth = $minWidth;
        return $this;
    }

    /**
     * @return mixed|string
     */
    public function getMaxHeight(): string
    {
        return $this->maxHeight;
    }

    /**
     * @param mixed|string $maxHeight
     * @return TextAttribute
     */
    public function setMaxHeight(string $maxHeight): self
    {
        $this->maxHeight = $maxHeight;
        return $this;
    }

    /**
     * @return int|mixed
     */
    public function getMaxChars(): int
    {
        return $this->maxChars;
    }

    /**
     * @param int|mixed $maxChars
     * @return TextAttribute
     */
    public function setMaxChars(int $maxChars): self
    {
        $this->maxChars = $maxChars;
        return $this;
    }


}
