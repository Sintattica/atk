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
  atkimport("atk.recordlist.atkrecordlist");

  /**
   * Custom recordlist renderer.
   *
   * @author Paul Verhoef <paul@ibuildings.nl>
   * @package atk
   * @subpackage recordlist
   *
   */
  class atkCustomRecordList extends atkRecordList
  {

  	var $m_exportcsv = true;

    /**
     * Creates a special Recordlist that can be used for exporting to files or to make it printable
     * @param atkNode $node       The node to use as definition for the columns.
     * @param array $recordset    The records to render
     * @param String $sol         String to use at start of each row
     * @param String $sof         String to use at start of each field
     * @param String $eof         String to use at end of each field
     * @param String $eol         String to use at end of each row
     * @param int $type           0=Render rows in simple html tabl; 1= raw export
     * @param $compression        Compression technique (bzip / gzip)
     * @param array $suppressList List of attributes from $node that should be ignored
     * @param array $outputparams Key-Value parameters for output. Currently existing:
     *                               filename - the name of the file (without extension .csv)
     * @param String $mode	      The mode that is passed to attributes' display() method
     *                            (for overrides). Defaults to 'list'.
     * @param Boolean $titlerow   Should titlerow be rendered or not
     * @param Boolean $decode     Should data be decoded or not (for exports)
     * @param String $fsep        String to use between fields
     * @param String $lfreplace   String for replacing line feeds in recordset field values (null = do not replace)
    */
    function render(&$node, $recordset, $sol, $sof, $eof, $eol, $type="0", $compression="", $suppressList="",
                    $outputparams=array(), $mode="list", $titlerow=true, $decode=false, $fsep="", $rfeplace=null)
    {
      $this->setNode($node);
      // example      html         csv

      // $sol     = '<tr>'         or  ''
      // $sof     = '<td>'         or  '"'
      // $eof     = '</td>'        or  '"'
      // $eol     = '</tr>'        or  '\r\n'
      // $fsep    = ''             or  ';'
      //$empty  om lege tabelvelden op te vullen;

      // stuff for the totals row..
      $totalisable = false;
      $totals = array();
      if ($type=="0")
      {
        $empty = "&nbsp;";
      }
      if ($type=="1")
      {
        $output="";
        $empty="";
      }

      if ($titlerow)
      {
        $output .= $sol;

        // display a headerrow with titles.
        // Since we are looping the attriblist anyway, we also check if there
        // are totalisable collumns.
        foreach (array_keys($this->m_node->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_node->m_attribList[$attribname];
          $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
          if (
            ($p_attrib->hasFlag(AF_HIDE)==false)
            &&
            (
            ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
            ||($this->m_node->m_action!="select")
            )
            &&$musthide==false
          )
          {
            $output.=$sof.$this->eolreplace(atktext($p_attrib->fieldName(), $this->m_node->m_module, $this->m_node->m_type), $rfeplace).$eof.$fsep;

            // the totalisable check..
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $totalisable = true;
            }
          }
        }

        $output.=$eol;
      }

      // Display the values
      for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
      {
        $output.=$sol;
        foreach (array_keys($this->m_node->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_node->m_attribList[$attribname];
          $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
          if (
              ($p_attrib->hasFlag(AF_HIDE)==false)
              &&
              (
                ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
                ||($this->m_node->m_action!="select")
              )
              &&$musthide==false
             )
          {
            // An <attributename>_display function may be provided in a derived
            // class to display an attribute.
            $funcname = $p_attrib->m_name."_display";

            if (method_exists($this->m_node,$funcname))
            {
              $value=$this->eolreplace($this->m_node->$funcname($recordset[$i], $mode), $rfeplace);
            }
            else
            {
              // otherwise, the display function of the particular attribute
              // is called.
              $value = $this->eolreplace($p_attrib->display($recordset[$i], $mode), $rfeplace);
            }
            if (atktext("charset") != "" && $decode) $value = html_entity_decode(htmlentities($value, ENT_NOQUOTES, atktext("charset")));
            $output.=$sof.($value==""?$empty:$value).$eof.$fsep;

            // Calculate totals..
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $totals[$attribname] = $p_attrib->sum($totals[$attribname], $recordset[$i]);
            }
          }
        }

        $output.=$eol;
      }

      // totalrow..
      if ($totalisable)
      {
        $totalRow = $sol;

        // Third loop.. this time for the totals row.
        foreach (array_keys($this->m_node->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_node->m_attribList[$attribname];
        $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
          if (
              ($p_attrib->hasFlag(AF_HIDE)==false)
              &&
              (
                ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
                ||($this->m_node->m_action!="select")
              )
              &&$musthide==false
             )
          {
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $value = $this->eolreplace($p_attrib->display($totals[$attribname], $mode), $rfeplace);
              $totalRow.=$sof.($value=="" ? $empty : $value).$eof.$fsep;

            }
            else
            {
              $totalRow.= $sof.$empty.$eof.$fsep;
            }
          }
        }

        $totalRow .= $eol;

        $output .= $totalRow;

      }

      // html requires table tags
      if ($type == "0")
      {
        $output = '<table border="1" cellspacing="0" cellpadding="2">'.$output."</table>";
      }

      atkdebug(html_entity_decode($output));

      // To a File
      if (!array_key_exists("filename", $outputparams))
        $outputparams["filename"] = "achievo";

      if ($this->m_exportcsv)
      {
        $ext = ($type == "0" ? "html" : "csv");
        $exporter = &atknew("atk.utils.atkfileexport");
        $exporter->export($output, $outputparams["filename"], $ext, $ext, $compression);
      }
      else
      {
      	return $output;
      }
    }

    function setExportingCSVToFile($export=true)
    {
    	if (is_bool($export)) $this->m_exportcsv = $export;
    }

    /**
     * Replace any eol character(s) by something else
     *
     * @param String $string        The string to process
     * @param String $replacement   The replacement string for '\r\n', '\n' and/or '\r'
     */
    function eolreplace($string, $replacement)
    {
      if (!is_null($replacement))
      {
        $string = str_replace("\r\n", $replacement, $string); // prevent double replacement in the next lines!
        $string = str_replace("\n", $replacement, $string);
        $string = str_replace("\r", $replacement, $string);
      }

      return $string;
    }
  }

?>