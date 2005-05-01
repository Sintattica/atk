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
     * @param array $outputparams Key-Value parameters for output. Currently existing: 
     *                               filename - the name of the file (without extension .csv)     
     * @param array $suppressList List of attributes from $node that should be ignored     
     * @param String $mode	      The mode that is passed to attributes' display() method 
     *                            (for overrides). Defaults to 'list'.
    */
    function render(&$node, $recordset, $sol, $sof, $eof, $eol, $type="0", $compression="", $suppressList="", $outputparams=array(), $mode="list")
    {
      $this->setNode($node);
      // example      html         csv

      // $sol     = "<tr>"         or  ""
      // $eof     = "</td>"        or  ";"
      // $sof     = "<td>"         or  ""
      // $eol     = "</tr>"        or  "\r\n"
      //$empty  om lege tabelvelden op te vullen;

      // stuff for the totals row..
      $totalisable = false;
      $totals = array();
      if ($type=="0")
       {
        $output = '<table border="1" cellspacing="0" cellpadding="4">';
        $empty = "&nbsp;";
       }
      if ($type=="1")
       {
        $output="";
        $empty="";
       }

       $output .= $sol;

      // display a headerrow with titles.
      // Since we are looping the attriblist anyway, we also check if there
      // are totalisable collumns.
      foreach (array_keys($this->m_node->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_node->m_attribList[$attribname];
        $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
        if (
            ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
            &&
            (
              ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
              ||($this->m_node->m_action!="select")
            )
            &&$musthide==false
           )
        {
          $output.=$sof.text($p_attrib->fieldName(),$this->m_node->m_type).$eof;

          // the totalisable check..
          if ($p_attrib->hasFlag(AF_TOTAL))
          {
            $totalisable = true;
          }

        }
      }

      $output.=$eol;

      for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
      {
        $output.=$sol;
        foreach (array_keys($this->m_node->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_node->m_attribList[$attribname];
          $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
          if (
              ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
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
              $value=$this->m_node->$funcname($recordset[$i], $mode);
            }
            else
            {
              // otherwise, the display function of the particular attribute
              // is called.
              $value=$p_attrib->display_plain($recordset[$i], $mode);
              if (!$value)
                $value=$p_attrib->display($recordset[$i], $mode);
            }
            $output.=$sof.($value==""?$empty:$value).$eof;

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
              ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
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
              $value = $p_attrib->display_plain($totals[$attribname], $mode);
              if (!$value)
                $value = $p_attrib->display($totals[$attribname], $mode);
              $totalRow.=$sof.($value==""?$empty:$value).$eof;

            }
            else
            {
              $totalRow.= $sof.$empty.$eof;
            }
          }
        }

        $totalRow.=$eol;

        $output.=$totalRow;
       
      }

      // To the Screen
      if ($type=="0")
      {
        
        $output .= "</table>";
        return $output;
      }
      // To a File
      if ($type=="1") 
      {
        if (!array_key_exists("filename", $outputparams))
          $outputparams["filename"] = "achievo";

        if ($this->m_exportcsv)
        {
          $exporter = &atknew("atk.utils.atkfileexport");        
          $exporter->export($output, $outputparams["filename"],"csv","csv",$compression);        
        }
        else 
        {
        	return $output;
        }
      }
    }
        
    function setExportingCSVToFile($export = false)
    {
    	if (is_bool($export)) $this->m_exportcsv = $export;
    }
  }

?>