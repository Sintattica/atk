<?php

  atkimport("atk.recordlist.atkrecordlist");

  class atkCustomRecordList extends atkRecordList 
  {    

    /**
     * Creates a special Recordlist that can be used for exporting to files or to make it printable
     * @param $recordset $suppresslist

     * @param $sol                -- start row/record/line
     * @param $eof                -- end field
     * @param $sof                -- start field
     * @param $eol                -- end record/line
     * @param $type               -- 0=in simple table ; 1= export
     * @param outputparamKeyVal   -- Key-Value parameters for output. Currently existening: 
     *                               filename   -- the name of the file (without extension .cvs)
    *@param $compression         -- Compression technique (bzip / gzip)
    */
    function render(&$node, $recordset, $sol, $sof, $eof, $eol, $type="0", $compression="",$suppressList="", $outputparamKeyVal = Array(), $flags=0)
    {
      $this->setNode($node);
      $this->m_flags = $flags;
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
              $value=$this->m_node->$funcname($recordset[$i], "list");
            }
            else
            {
              // otherwise, the display function of the particular attribute
              // is called.
              $value=$p_attrib->display($recordset[$i], "list");
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
              $value = $p_attrib->display($totals[$attribname], "list");
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
        if (!array_key_exists("filename", $outputparamKeyVal))
          $outputparamKeyVal["filename"] = "achievo";

        $exporter = &atknew("atk.utils.atkfileexport");        
        $exporter->export($output, $outputparamKeyVal["filename"],"csv","csv",$compression);        
      }
    }
  }

?>