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
      $content.= '<br><br><input type="submit" value="Uploaden"> ';
      $content.= atkButton("Terug", "", SESSION_BACK, true);

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

    function doAnalyze($fileid,$importerror="")
    {
      global $g_sessionManager;
      $filename = $this->getTmpFileDestination($fileid);
      $rows = $this->getSampleRows($filename);
      $delimiter = $g_sessionManager->pageVar("delimiter");
      if ($delimiter=="") $delimiter = $this->estimateDelimiter($rows);
      $enclosure = $g_sessionManager->pageVar("enclosure");
      if ($enclosure=="") $enclosure = $this->estimateEnclosure($rows);
      
      $allFieldValue = $g_sessionManager->pageVar("allFieldValue");
      $allField = $g_sessionManager->pageVar("allField");

      $columncount = $this->estimateColumnCount($rows, $delimiter);

      $csv_data = $this->fgetcsvfromarray($rows, $columncount, $delimiter, $enclosure);

      // Display sample
      $sample = '<br><br>Gegevens sample uit de csv-file:<br><br><table class="recordlist">';

      $col_map = $this->m_postvars["col_map"];
      if (!is_array($col_map))
      {
        // init colmap
        $col_map = $this->getUsableAttributes();
      }

      if ($columncount>count($col_map))
      {
        // fill with ignored
          for ($i=0, $_i=($columncount-count($col_map)); $i<$_i; $i++) $col_map[] = "-";
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
            $attr = &$this->getUsableAttribute($col_map[$j]);
            $newval = $attr->parseStringValue(trim($line[$j+1]));
            $sample.=$attr->display(array($col_map[$j]=>$newval));

            if ((string)$newval!=(string)$line[$j+1])
            {
              // Also display raw value so we can verify
              $sample.= ' <i style="color: #777777">('.trim($line[$j+1]).")</i>";
            }
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
      $content.= text("delimiter").': <input type="text" size="2" name="delimiter" value="'.htmlentities($delimiter).'">';
      $content.= '&nbsp; '.text("enclosure").': <input type="text" size="2" name="enclosure" value="'.htmlentities($enclosure).'">';
      $content.= '<br><br>Gedetecteerde aantal kolommen: '.$columncount.'<br><br>';

      if(is_array($importerror))
        $content.= "<b>".text($importerror['msg']).$importerror['spec']."</b>";

      $content.= $sample;

      $content.= 'Veld voor gebruik in iedere record: <br/>Veld: '.$this->getAttributeSelector(0,$allField,"allField").'<br/>';
      $content.= 'Waarde: <input type="text" size="50" name="allFieldValue" value="'.htmlentities($allFieldValue).'"><br/>';
      $attr = $this->getUsableAttribute($allField);
      if(is_object($attr) && $attr->display($attr->parseStringValue($allFieldValue)))
        $content.= 'Resultaat: <i>'.$attr->display($attr->parseStringValue($allFieldValue)).'</i><br/><br/>';
        
      $content.= ' <input type="submit" name="analyse" value="Analyseer opnieuw">';
      $content.= ' <input type="submit" name="import" value="Importeer"> ';
      $content.= atkButton("Terug", "", SESSION_BACK, true);

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
//"
      $matches = array();

      $delimiterChar = strtr($delimiterChar, $regExpSpecialChars);
      $enclosureChar = strtr($enclosureChar, $regExpSpecialChars);

      $regExp = "/^";

      for ($i = 0; $i < $columncount; $i++) {
          $regExp .= '('.$enclosureChar.'?)(.*)\\'.(2*$i + 1).$delimiterChar; // construct the regular expression
      }
//"'
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

    function getUsableAttributes($obligatoryOnly=false)
    {
      $attrs = array();
      foreach (array_keys($this->m_node->m_attribList) as $attribname)
      {
        $attrib = &$this->m_node->getAttribute($attribname);

        if($this->integrateAttribute($attrib)){
          $attrib->createDestination();
          foreach(array_keys($attrib->m_destInstance->m_attribList) as $relattribname){
            $relattrib = &$attrib->m_destInstance->getAttribute($relattribname);
            
            if($obligatoryOnly && !$this->isObligatory($relattrib))
              continue;

            if ($relattrib->storageType("add")==ADDTOQUERY && !$relattrib->hasFlag(AF_AUTOINCREMENT) && !$this->isHide($relattrib))
            {
              $attrs[] = $relattribname;
            }
          }
          continue;
        }
        
        if($obligatoryOnly && !$this->isObligatory($attrib))
          continue;
        
        if ($attrib->storageType("add")==ADDTOQUERY && !$attrib->hasFlag(AF_AUTOINCREMENT) && !$this->isHide($attrib))
        {
          $attrs[] = $attribname;
        }
      }

      return $attrs;
    }
    
    function getObligatoryAttributes()
    {
      return $this->getUsableAttributes(true);
    }
    
    function isObligatory($attr){
      return ($attr->hasFlag(AF_OBLIGATORY) && !$this->isHide($attr));
    }
    
    function isHide($attr){
      return ($attr->hasFlag(AF_HIDE) || ($attr->hasFlag(AF_HIDE_ADD) && $attr->hasFlag(AF_HIDE_EDIT)));
    }
    
    function integrateAttribute($attr){
      return in_array(get_class($attr),array("atkonetoonerelation","atksecurerelation")) && $attr->hasFlag(AF_ONETOONE_INTEGRATE);
    }
    
    function getIntegratedAttributes(){
      $attrs = array();
      foreach (array_keys($this->m_node->m_attribList) as $attribname)
      {
        $attrib = &$this->m_node->getAttribute($attribname);

        if($this->integrateAttribute($attrib)){
          $attrs[] = $attribname;
        }
      }

      return $attrs;
    }
    
    function isRelationAttribute($attrname){
      if(array_key_exists($attrname,$this->m_node->m_attribList))
        return false;

      foreach($this->getIntegratedAttributes() as $attr)
      {
        $relattr = $this->m_node->getAttribute($attr);
        $relattr->createDestination();
        if(array_key_exists($attrname,$relattr->m_destInstance->m_attribList))
          return $attr;
      }

      return false;
    }

    function getUsableAttribute($name){
      if(array_key_exists($name,$this->m_node->m_attribList))
        return $this->m_node->getAttribute($name);

      foreach($this->getIntegratedAttributes() as $attr)
      {
        $relattr = $this->m_node->getAttribute($attr);
        $relattr->createDestination();
        if(array_key_exists($name,$relattr->m_destInstance->m_attribList))
          return $relattr->m_destInstance->getAttribute($name);
      }

      return;
    }
    
    function addToRecord(&$record,$attrname,$value){
      $attr = $this->getUsableAttribute($attrname);
    
      if(!is_object($attr)){
        return;
      }
    
      foreach($this->getIntegratedAttributes() as $intattr)
      {
        if(!isset($record[$intattr]))
          $record[$intattr] = array('mode'=>"add",'atkaction'=>"save");
      }

      if(!$this->isRelationAttribute($attrname))
        $record[$attrname] = $attr->parseStringValue($value);
      else{
      
        $record[$this->isRelationAttribute($attrname)][$attrname] = $attr->parseStringValue($value);
      }
    }

    function getAttributeSelector($index=0, $value="",$othername="")
    {
      if(!$othername)
        $res = '<select name="col_map['.$index.']">';
      else
        $res = '<select name="'.$othername.'">';

      $j=0;
      $hasoneselected = false;
      $attrs = $this->getUsableAttributes();
      foreach ($attrs as $attribname)
      {
        $selected = "";
        if (($value=="" && $index==$j) || ($value!="" && $value==$attribname))
        {
          // select the next.
          $selected="selected";
          $hasoneselected = true;
        }

        $res.= '<option value="'.$attribname.'" '.$selected.'>'.atktext($attribname, $this->m_node->m_module, $this->m_node->m_type);
        $j++;
      }

      $res.= '<option value="-" '.(($value=="-"||!$hasoneselected)?"selected":"").' style="font-style: italic">Ignore this column';

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
      $allField = $this->m_postvars["allField"];
      $allFieldValue = $this->m_postvars["allFieldValue"];

      //add the allField to the col_map array
      $col_map[] = $allField;

      $importerror = "";

      $importerror = $this->checkImport($col_map);
      
      if(is_array($importerror)){
        $this->doAnalyze($this->m_postvars["fileid"],$importerror);
        return;
      }
      
      $importerror = $this->checkAllField($allField,$allFieldValue);

      if(is_array($importerror)){
        $this->doAnalyze($this->m_postvars["fileid"],$importerror);
        return;
      }

      //copy the csv in a record and add it to the db
      $fp = fopen($file, "r");
      for($line = fgets($fp); $line!==false; $line = fgets($fp))
      {
        $data = $this->fgetcsvfromline($line, $columncount, $delimiter, $enclosure);

        $rec = $this->m_node->initial_values();

        for ($i=0, $_i=count($col_map); $i<$_i; $i++)
        {
          if ($col_map[$i]!="-")
          {
            if($i != $_i-1)// column is mapped
              $value = $data[$i+1];
            else //this is the allField
              $value = $allFieldValue;

            $this->addToRecord($rec,$col_map[$i],trim($value));
          }
        }

        if (!$this->validate($rec)){
          $importerror = array("msg"=>$rec['atkerror'][0]['msg'].": ","spec"=>text($rec['atkerror'][0]['attrib_name']));
          break;
        }
        
        if (!$this->add($rec))
        {
          $importerror = array("msg"=>$rec['atkerror'][0]['msg'].": ","spec"=>text($rec['atkerror'][0]['attrib_name']));
          break;
        }
      }

      if(is_array($importerror)){
        $this->doAnalyze($this->m_postvars["fileid"],$importerror);
        return;
      }
      
      fclose($fp);

      // remove the csv file
      unlink($file);

      $this->m_node->redirect();

    }
    
    function add($record)
    {
      $this->m_node->preAdd($record);
      
      if(isset($record['atkerror']))
        return false;

      $this->m_node->addDb($record);
      
      if(isset($record['atkerror']))
        return false;

      return true;
    }
    
    function validate(&$record)
    {
      $this->m_node->validate($record,"add");
      if(isset($record['atkerror']))
        return false;
      else
        return true;
    }
    
    function checkImport($col_map)
    {
      //get the unused obligatory fields
      $unused = array_values(array_diff($this->getObligatoryAttributes(),$col_map));
      if(count($unused) > 0){
        if(count($unused) == 1)
          $msg = "Het volgende veld moet verplicht gebruikt worden: ";
        else
          $msg = "De volgende velden moeten verplicht gebruikt worden: ";

        foreach($unused as $field)
        {
          $spec .= text($field).", ";
        }
        $spec = substr($spec,0,strlen($spec)-2);

        return array("msg"=>$msg,"spec"=>$spec);
      }

      //check if there ar double values in col_map
      $doubles = array_values(array_diff_assoc($col_map,array_unique($col_map)));
      if(count($doubles) > 0)
      {
        if(count($doubles) == 1)
          $msg = "Het volgende veld is twee maal gebruikt: ";
        else
          $msg = "De volgende velden zijn twee maal gebruikt: ";

        foreach($doubles as $field)
        {
          $spec .= text($field).", ";
        }
        $spec = substr($spec,0,strlen($spec)-2);

        return array("msg"=>$msg,"spec"=>$spec);
      }
      
    }
    
    function checkAllField($field, &$value)
    {
      $attr = $this->getUsableAttribute($field);
      $result = $attr->display($attr->parseStringValue($value));
      
      if(!$result)
        if(in_array($field,$this->getObligatoryAttributes()))
          return array('msg' => "Het veld voor gebruik in iedere record bevat geen correcte invoer.");
        else
          $value = "";

      return;
    }

  }
?>
