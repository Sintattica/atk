<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage handlers
   *
   * @copyright (c)2004 Ivo Jansch
   * @copyright (c)2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Handler for the 'import' action of a node. The import action is a
   * generic tool for importing CSV files into a table.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkImportHandler extends atkActionHandler 
  {
    /** 
     * The action handler.
     */
    function action_import()
    {          
      $phase = ($this->m_postvars["phase"]!=""?$this->m_postvars["phase"]:"init");
      
      switch ($phase)
      {
        case "init": $this->doInit(); break;
        case "upload": $this->doUpload(); break;
        case "process": $this->doProcess(); break;
      }
      return true;
    }
    
    function doInit()
    {
      $content = atktext("import_upload_explanation", "atk", $this->m_node->m_type);
      $content.= '<br><br><form name="entryform" enctype="multipart/form-data" action="'.$_SERVER["PHP_SELF"].'?'.SID.'"'.
                                  ' method="post">'.session_form();
      $content.= '<input type="hidden" name="phase" value="upload">';      
      $content.= '<input type="file" name="csvfile">';
      $content.= '<br><br><input type="submit" value="Upload"> ';
      $content.= atkButton("Back", "", SESSION_BACK, true);
      
      $content.= '</form><br><br>';
      
      
      $page = &$this->getPage();
      $page->addContent($this->m_node->genericPage(atktext("import", "atk"), $content));
      return true;
    }
    
    function doUpload()
    {      
      $fileid = uniqid("file_");
      $filename = $this->getTmpFileDestination($fileid);
      if (!move_uploaded_file($_FILES['csvfile']['tmp_name'], $filename))
      {
        $this->m_node->redirect($this->m_node->feedbackUrl("import", ACTION_FAILED));
      }
      else
      {
        // file uploaded
        return $this->doAnalyze($fileid);
      }
    }
    
    function doProcess()
    {
      $filename = $this->getTmpFileDestination($this->m_postvars["fileid"]);
      if ($this->m_postvars["import"]!="")
      {
        return $this->doImport($filename);
      }
      else 
      {
        // reanalyze
        return $this->doAnalyze($this->m_postvars["fileid"]);
      }
    }
    
    function doAnalyze($fileid)
    {
      global $g_sessionManager;
      $filename = $this->getTmpFileDestination($fileid);
      $rows = $this->getSampleRows($filename);
      $delimiter = $g_sessionManager->pageVar("delimiter");
      if ($delimiter=="") $delimiter = $this->estimateDelimiter($rows);
      $enclosure = $g_sessionManager->pageVar("enclosure");
      if ($enclosure=="") $enclosure = $this->estimateEnclosure($rows);
      
      $columncount = $this->estimateColumnCount($rows, $delimiter);      
      
      $csv_data = $this->fgetcsvfromarray($rows, $columncount, $delimiter, $enclosure);     
                  
      // Display sample
      $sample = '<br><br>Sample of data from csv file:<br><br><table class="recordlist">';
      
      $col_map = $this->m_postvars["col_map"];  
      if (!is_array($col_map))
      {
        // init colmap
        $col_map = $this->getUsableAttributes();
      }
      
      // header
      $sample.= '<tr>';
      for ($j=1; $j<=$columncount; $j++)
      {
        $sample.='<th>';
        $sample.='kolom '.$j;
        $sample.='</th>';
      }      
      $sample.= '</tr>';
      
      // column assign
      $sample.= '<tr>';
      for ($j=0; $j<$columncount; $j++)
      {
        $sample.='<th>';
        $sample.=$this->getAttributeSelector($j, $col_map[$j]);
        $sample.='</th>';
      }      
      $sample.= '</tr>';
            
      // sample data
      for ($i=0; $i<count($csv_data); $i++)
      {
        $line = $csv_data[$i];
                
        $sample.='<tr class="row'.(($i%2)+1).'">';
        for ($j=0; $j<$columncount; $j++)
        {
          $sample.='<td>';
          if ($col_map[$j]!="" && $col_map[$j]!="-")
          {
            $attr = &$this->m_node->getAttribute($col_map[$j]);
            $sample.=$attr->display(array($col_map[$j]=>$attr->parseStringValue(trim($line[$j+1]))));
          }
          else if ($col_map[$j]=="-")
          {
            // ignoring.
            $sample.='<div style="color: #777777">'.trim($line[$j+1]).'</div>';
          }
          else
          {
            $sample.=trim($line[$j+1]);
          }
          $sample.='</td>';
        }
        $sample.='</tr>';
      }
      $sample.= '</table><br><br>';
            
      
      $content.= '<br><br><form name="entryform" enctype="multipart/form-data" action="'.$_SERVER["PHP_SELF"].'?'.SID.'"'.
                                  ' method="post">'.session_form();
      $content.= '<input type="hidden" name="phase" value="process">';      
      $content.= '<input type="hidden" name="fileid" value="'.$fileid.'">';            
      $content.= '<input type="hidden" name="columncount" value="'.$columncount.'">';            
      $content.= 'Delimiter: <input type="text" size="2" name="delimiter" value="'.htmlentities($delimiter).'">';
      $content.= ' &nbsp; Enclosure: <input type="text" size="2" name="enclosure" value="'.htmlentities($enclosure).'">';
      $content.= '<br><br>Gedetecteerde aantal kolommen: '.$columncount.'<br><br>';
      $content.= $sample;            
      
      $content.= ' <input type="submit" name="analyse" value="Analyze again">';
      $content.= ' <input type="submit" name="import" value="Import"> ';
      $content.= atkButton("Back", "", SESSION_BACK, true);
      
      $content.= '</form><br><br>';
      
      $page = &$this->getPage();
      $theme = &atkTheme::getInstance();      
      $page->register_style($theme->stylePath("recordlist.css"));
      $page->addContent($this->m_node->genericPage(atktext("import", "atk"), $content));
    }
    
    function getTmpFileDestination($fileid)
    {
      return atkconfig("atktempdir")."csv_import_$fileid.csv";
    }
    
    function fgetcsvfromarray ($arr, $columncount, $delimiterChar = ',', $enclosureChar = '"')
    {
      $result = array();
      foreach ($arr as $line) { $result[] = $this->fgetcsvfromline($line, $columncount, $delimiterChar, $enclosureChar); }
      return $result;
    }
    
    function estimateDelimiter($rows)
    {
      if (!is_array($rows)||count($rows)==0) return ",";
      if (strpos($rows[0], ";")!==false) return ";";
      if (strpos($rows[0], ",")!==false) return ",";
      if (strpos($rows[0], ":")!==false) return ":";
    }
    
    function estimateEnclosure($rows)
    {
      if (!is_array($rows)||count($rows)==0) return '"';
      if (substr_count($rows[0], '"')>=2) return '"';
      return '"';
    }
    
    function estimateColumnCount($rows, $delimiter)
    {
      if (!is_array($rows)||count($rows)==0) return 0;
      return (substr_count($rows[0], $delimiter)+1);
      
    }
    
    function getSampleRows($file)
    {
      $result = array();
      $fp = fopen($file, "r");
      for ($i=0; $i<5; $i++)
      {
        $line = fgets($fp);
        if ($line!==false)
        {
          $result[] = $line;
        }
      }
      fclose($fp);
      return $result;      
    }
    
    // source: http://bu.orbitel.bg/fgetcsvfromline.php (dawa@did-it.com)
    function fgetcsvfromline ($line, $columncount, $delimiterChar = ',', $enclosureChar = '"') 
    {
      $regExpSpecialChars = array (
          "|" => "\\|",
          "&" => "\\&",
          "$" => "\\$",
          "(" => "\\(",
          ")" => "\\)",
          "^" => "\\^",
          "[" => "\\[",
          "]" => "\\]",
          "{" => "\\{",
          "}" => "\\}",
          "." => "\\.",
          "*" => "\\*",
          "\\" => "\\\\",
          "/" => "\\/"
      );
  
      $matches = array();
  
      $delimiterChar = strtr($delimiterChar, $regExpSpecialChars);
      $enclosureChar = strtr($enclosureChar, $regExpSpecialChars);
  
      $regExp = "/^";
  
      for ($i = 0; $i < $columncount; $i++) {
          $regExp .= '('.$enclosureChar.'?)(.*)\\'.(2*$i + 1).$delimiterChar; // construct the regular expression
      }
  
      $regExp = substr($regExp, 0, (strlen($regExp) - strlen($delimiterChar)))."/"; // format the regular expression
  
      if (preg_match($regExp, $line, $matches)) {
          $result = array();
          for ($i = 1; $i < count($matches)/2; $i++) {
              $result[$i] = $matches[2*$i]; // get only the fields but not the delimiters
          }
  
          return $result;
      }
  
      return FALSE;
    }
    
    function getUsableAttributes()
    {
      $attrs = array();
      foreach (array_keys($this->m_node->m_attribList) as $attribname)
      {
        $attrib = &$this->m_node->getAttribute($attribname);
        if ($attrib->storageType("add")==ADDTOQUERY && !$attrib->hasFlag(AF_AUTOINCREMENT))
        {
          $attrs[] = $attribname;
        }        
      }
      return $attrs;
    }
    
    function getAttributeSelector($index=0, $value="")
    {
      $res = '<select name="col_map['.$index.']">';
      
      $j=0;      
      $attrs = $this->getUsableAttributes();
      foreach ($attrs as $attribname)
      {               
        $selected = "";
        if (($value=="" && $index==$j) || ($value!="" && $value==$attribname))
        {
          // select the next.
          $selected="selected";
        }
          
        $res.= '<option value="'.$attribname.'" '.$selected.'>'.atktext($attribname, $this->m_node->m_module, $this->m_node->m_type);
        $j++;
      }        
      
      $res.= '<option value="-" '.($value=="-"?"selected":"").' style="font-style: italic">Ignore this column';
      
      $res.= '</select>';
      return $res;      
    }
    
    function doImport()
    {
      $col_map = $this->m_postvars["col_map"];
      $fileid = $this->m_postvars["fileid"];
      $file = $this->getTmpFileDestination($fileid);
      $enclosure = $this->m_postvars["enclosure"];
      $delimiter = $this->m_postvars["delimiter"];
      $columncount = $this->m_postvars["columncount"];
      
      $fp = fopen($file, "r");
      for($line = fgets($fp); $line!==false; $line = fgets($fp))
      {
        $data = $this->fgetcsvfromline($line, $columncount, $delimiter, $enclosure);
        
        $rec = $this->m_node->initial_values();
        for ($i=0, $_i=count($col_map); $i<$_i; $i++)
        {
          if ($col_map[$i]!="-")
          {
            // column is mapped
            $attr = &$this->m_node->getAttribute($col_map[$i]);
            
            $rec[$col_map[$i]] = $attr->parseStringValue(trim($data[$i+1]));
          }
        }
        
        if (!$this->m_node->addDb($rec))
        {
          $this->m_node->redirect($this->m_node->feedbackUrl("import", ACTION_FAILED));
        }
      }
      
      fclose($fp);
      
      // remove the csv file
      unlink($file);
      
      $this->m_node->redirect();
      
    }
        
  }
?>