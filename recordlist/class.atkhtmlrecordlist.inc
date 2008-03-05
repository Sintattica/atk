<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage recordlist
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /** @internal include base class */
  atkimport("atk.recordlist.atkcustomrecordlist");

  /**
   * HTML recordlist renderer.
   *
   * @author Patrick van der Velden <patrick@ibuildings.nl>
   * @package atk
   * @subpackage recordlist
   *
   */
  class atkHTMLRecordList extends atkCustomRecordList
  {

  	var $m_exportcsv = true;

    /**
     * Creates a special Recordlist that can be used for exporting to files or to make it printable
     * @param atkNode $node       The node to use as definition for the columns.
     * @param array $recordset    The records to render
     * @param $compression        Compression technique (bzip / gzip)
     * @param array $suppressList List of attributes from $node that should be ignored
     * @param array $outputparams Key-Value parameters for output. Currently existing:
     *                               filename - the name of the file (without extension .csv)
     * @param Boolean $titlerow   Should titlerow be rendered or not
     * @param Boolean $decode     Should data be decoded or not (for exports)
    */
    function render(&$node, $recordset, $compression="", $suppressList="", $outputparams=array(), $titlerow=true, $decode=false)
    {
      parent::render($node, $recordset, "<tr>", "<td>", "</td>", "<tr>\n", "0", $compression, $suppressList, $outputparams, "list", $titlerow, $decode, "", "<br>");
    }

  }

?>