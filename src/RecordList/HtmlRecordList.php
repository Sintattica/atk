<?php

namespace Sintattica\Atk\RecordList;

use Sintattica\Atk\Core\Node;

/**
 * HTML recordlist renderer.
 *
 * @author Patrick van der Velden <patrick@ibuildings.nl>
 */
class HtmlRecordList extends CustomRecordList
{
    public $m_exportcsv = true;

    /**
     * Creates a special Recordlist that can be used for exporting to files or to make it printable.
     *
     * @param Node $node The node to use as definition for the columns.
     * @param array $recordset The records to render
     * @param string $compression Compression technique (bzip / gzip)
     * @param array $suppressList List of attributes from $node that should be ignored
     * @param array $outputparams Key-Value parameters for output. Currently existing:
     *                             filename - the name of the file (without extension .csv)
     * @param bool $titlerow Should titlerow be rendered or not
     * @param bool $decode Should data be decoded or not (for exports)
     *
     * @return string
     */
    public function render(
        &$node,
        $recordset,
        $compression = '',
        $suppressList = [],
        $outputparams = [],
        $titlerow = true,
        $decode = false
    ) {
        return parent::render($node, $recordset, '<tr>', '<td>', '</td>', "<tr>\n", '0', $compression, $suppressList, $outputparams, 'list', $titlerow, $decode,
            '', '<br>');
    }
}
