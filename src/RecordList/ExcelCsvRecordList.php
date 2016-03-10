<?php namespace Sintattica\Atk\RecordList;

/**
 * CVS for Excel recordlist renderer.
 * Note: End of line characters within values are replaced for excel 2003 compatibility reasons!
 *
 * @author Patrick van der Velden <patrick@ibuildings.nl>
 * @package atk
 * @subpackage recordlist
 *
 */
class ExcelCsvRecordList extends CustomRecordList
{
    public $lfreplace = null;

    /**
     * Creates a special Recordlist that can be used for exporting to files or to make it printable
     * @param Node $node The node to use as definition for the columns.
     * @param array $recordset The records to render
     * @param string $compression Compression technique (bzip / gzip)
     * @param array $suppressList List of attributes from $node that should be ignored
     * @param array $outputparams Key-Value parameters for output. Currently existing:
     *                               filename - the name of the file (without extension .csv)
     * @param Boolean $titlerow Should titlerow be rendered or not
     * @param Boolean $decode Should data be decoded or not (for exports)
     */
    public function render(
        &$node,
        $recordset,
        $compression = "",
        $suppressList = "",
        $outputparams = array(),
        $titlerow = true,
        $decode = false
    ) {
        parent::render($node, $recordset, "", "\"", "\"", "\n", "1", $compression, $suppressList, $outputparams, "csv", $titlerow, $decode, ";",
            $this->lfreplace);
    }

    /**
     * Replace line feeds within cell values with another string.
     * Note: Excel 2003 requires this feature for csv imports.
     *
     * @param string $string The line feed replacement string
     */
    public function setLfReplacementString($string)
    {
        $this->lfreplace = $string;
    }
}
