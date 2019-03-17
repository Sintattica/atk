<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Ui\Output;
use Sintattica\Atk\Core\Tools;

/**
 * Handler class for the exporting a record to an XML file.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class XmlHandler extends ActionHandler
{
    /**
     * The action handler method. Creates an xml document and outputs it to the browser.
     */
    public function action_xml()
    {
        $recordset = $this->m_node->select($this->m_postvars['atkselector'])->mode('xml')->fetchAll();

        $output = Output::getInstance();

        $document = '<?xml version="1.0"?>'."\n";

        for ($i = 0, $_i = Tools::count($recordset); $i < $_i; ++$i) {
            $document .= $this->invoke('xml', $recordset[$i])."\n";
        }
        $output->output($document);
    }

    /**
     * Convert a record to an XML fragment.
     *
     * @param array $record The record to convert to xml.
     *
     * @return string XML document.
     *
     * @todo This handler can only handle 'simple' key/value attributes
     *       like Attribute. Relation support should be added.
     */
    public function xml($record)
    {
        $node = $this->m_node;
        $xml = '<'.$node->m_type.' ';

        $attrs = [];
        foreach (array_keys($node->m_attribList) as $attribname) {
            $p_attrib = $node->m_attribList[$attribname];
            if (!$p_attrib->isEmpty($record)) {
                $attrs[] = $attribname.'="'.$p_attrib->display($record, 'xml').'"';
            }
        }
        if (Tools::count($attrs)) {
            $xml .= implode(' ', $attrs);
        }

        $xml .= '/>';

        if (isset($node->m_postvars['tohtml']) && $node->m_postvars['tohtml'] == 1) {
            return htmlspecialchars($xml).'<br>';
        } else {
            return $xml;
        }
    }
}
