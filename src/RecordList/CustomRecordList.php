<?php

namespace Sintattica\Atk\RecordList;

use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Utils\FileExport;

/**
 * Custom recordlist renderer.
 *
 * @author Paul Verhoef <paul@ibuildings.nl>
 */
class CustomRecordList extends RecordList
{
    public $m_exportcsv = true;
    protected $m_mode;

    /**
     * Creates a special Recordlist that can be used for exporting to files or to make it printable.
     *
     * @param Node $node The node to use as definition for the columns.
     * @param array $recordset The records to render
     * @param string $sol String to use at start of each row
     * @param string $sof String to use at start of each field
     * @param string $eof String to use at end of each field
     * @param string $eol String to use at end of each row
     * @param int $type 0=Render rows in simple html tabl; 1= raw export
     * @param string $compression Compression technique (bzip / gzip)
     * @param array $suppressList List of attributes from $node that should be ignored
     * @param array $outputparams Key-Value parameters for output. Currently existing:
     *                             filename - the name of the file (without extension .csv)
     * @param string $mode The mode that is passed to attributes' display() method
     *                             (for overrides). Defaults to 'list'.
     * @param bool $titlerow Should titlerow be rendered or not
     * @param bool $decode Should data be decoded or not (for exports)
     * @param string $fsep String to use between fields
     * @param string $rfeplace String for replacing line feeds in recordset field values (null = do not replace)
     *
     * @return string|null
     */
    public function render(
        $node,
        $recordset,
        $sol,
        $sof,
        $eof,
        $eol,
        $type = 0,
        $compression = '',
        $suppressList = '',
        $outputparams = [],
        $mode = 'list',
        $titlerow = true,
        $decode = false,
        $fsep = '',
        $rfeplace = null
    ) {
        $this->setNode($node);
        $this->m_mode = $mode;
        // example      html         csv
        // $sol     = '<tr>'         or  ''
        // $sof     = '<td>'         or  '"'
        // $eof     = '</td>'        or  '"'
        // $eol     = '</tr>'        or  '\r\n'
        // $fsep    = ''             or  ';'
        //$empty  om lege tabelvelden op te vullen;
        // stuff for the totals row..

        $output = '';

        $empty = '';

        if ($type == '0') {
            $empty = '&nbsp;';
        }


        if ($titlerow) {
            $output .= $sol;

            // display a headerrow with titles.
            // Since we are looping the attriblist anyway, we also check if there
            // are totalisable collumns.
            foreach (array_keys($this->m_node->m_attribList) as $attribname) {
                $p_attrib = $this->m_node->m_attribList[$attribname];
                $musthide = (is_array($suppressList) && Tools::count($suppressList) > 0 && in_array($attribname, $suppressList));
                if (!$this->isHidden($p_attrib) && !$musthide) {
                    $output .= $sof.$this->eolreplace($p_attrib->label(), $rfeplace).$eof.$fsep;
                }
            }

            if ($fsep) {
                // remove separator at the end of line
                $output = substr($output, 0, -strlen($fsep));
            }

            $output .= $eol;
        }

        // Display the values
        for ($i = 0, $_i = Tools::count($recordset); $i < $_i; ++$i) {
            $output .= $sol;
            foreach (array_keys($this->m_node->m_attribList) as $attribname) {
                $p_attrib = $this->m_node->m_attribList[$attribname];
                $musthide = (is_array($suppressList) && Tools::count($suppressList) > 0 && in_array($attribname, $suppressList));

                if (!$this->isHidden($p_attrib) && !$musthide) {
                    // An <attributename>_display function may be provided in a derived
                    // class to display an attribute.
                    $funcname = $p_attrib->m_name.'_display';

                    if (method_exists($this->m_node, $funcname)) {
                        $value = $this->eolreplace($this->m_node->$funcname($recordset[$i], $this->m_mode), $rfeplace);
                    } else {
                        // otherwise, the display function of the particular attribute
                        // is called.
                        $value = $this->eolreplace($p_attrib->display($recordset[$i], $this->m_mode), $rfeplace);
                    }
                    if (Tools::atkGetCharset() != '' && $decode) {
                        $value = Tools::atk_html_entity_decode(htmlentities($value, ENT_NOQUOTES), ENT_NOQUOTES);
                    }
                    $output .= $sof.($value == '' ? $empty : $value).$eof.$fsep;
                }
            }

            if ($fsep) {
                // remove separator at the end of line
                $output = substr($output, 0, -strlen($fsep));
            }

            $output .= $eol;
        }


        // html requires table tags
        if ($type == '0') {
            $output = '<table border="1" cellspacing="0" cellpadding="2">'.$output.'</table>';
        }

        Tools::atkdebug(Tools::atk_html_entity_decode($output));

        // To a File
        if (!array_key_exists('filename', $outputparams)) {
            $outputparams['filename'] = 'achievo';
        }

        if ($this->m_exportcsv) {
            $ext = ($type == '0' ? 'html' : 'csv');
            $exporter = new FileExport();
            $exporter->export($output, $outputparams['filename'], $ext, $ext, $compression);
        } else {
            return $output;
        }

        return;
    }

    /**
     * Is this attribute hidden?
     *
     * @param Attribute $attribute
     *
     * @return bool Boolean to indicate if attribute is hidden or not
     */
    protected function isHidden(Attribute $attribute)
    {
        if ($attribute->hasFlag(Attribute::AF_HIDE)) {
            return true;
        }
        if ($attribute->hasFlag(Attribute::AF_HIDE_SELECT) && $this->m_node->m_action === 'select') {
            return true;
        }
        if ($attribute->hasFlag(Attribute::AF_HIDE_LIST) && ($this->m_node->m_action === 'export' || $this->m_mode === 'export')) {
            return true;
        }

        return false;
    }

    /**
     * Set exporting csv to file.
     *
     * @param bool $export
     */
    public function setExportingCSVToFile($export = true)
    {
        if (is_bool($export)) {
            $this->m_exportcsv = $export;
        }
    }

    /**
     * Replace any eol character(s) by something else.
     *
     * @param string $string The string to process
     * @param string $replacement The replacement string for '\r\n', '\n' and/or '\r'
     */
    public function eolreplace($string, $replacement)
    {
        if (!is_null($replacement)) {
            $string = str_replace("\r\n", $replacement, $string); // prevent double replacement in the next lines!
            $string = str_replace("\n", $replacement, $string);
            $string = str_replace("\r", $replacement, $string);
        }

        return $string;
    }
}
