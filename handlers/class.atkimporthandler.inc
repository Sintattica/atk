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
      global $ATK_VARS;
    
      //need to keep the postdata after a AF_LARGE selection in the allfield
      if(!isset($this->m_postvars["phase"]) && isset($ATK_VARS['atkformdata']))
        foreach($ATK_VARS['atkformdata'] as $key=>$value)
          $this->m_postvars[$key] = $value;

      //need to keep the selected item after an importerror
      if(!isset($ATK_VARS[$ATK_VARS['allField']."_newsel"]))
        $ATK_VARS[$ATK_VARS['allField']."_newsel"] = $ATK_VARS[$ATK_VARS['allField']];

      $phase = ($this->m_postvars["phase"]!=""?$this->m_postvars["phase"]:"init");

      switch ($phase)
      {
        case "init": $this->doInit(); break;
        case "upload": $this->doUpload(); break;
        case "process": $this->doProcess(); break;
      }
      return true;
    }

    /**
     * This function shows a form to upload a .csv
     */
    function doInit()
    {
      $content = atktext("import_upload_explanation", "atk", $this->m_node->m_type);
      $content.= '<br><br><form name="entryform" enctype="multipart/form-data" action="'.$_SERVER["PHP_SELF"].'?'.SID.'"'.
                                  ' method="post">'.session_form();
      $content.= '<input type="hidden" name="phase" value="upload">';
      $content.= '<input type="file" name="csvfile">';
      $content.= '<br><br><input type="submit" value="'.atktext("import_upload","atk").'"> ';
      $content.= atkButton(atktext("back","atk"), "", SESSION_BACK, true);

      $content.= '</form><br><br>';

      $page = &$this->getPage();
      $page->addContent($this->m_node->genericPage(atktext("import", "atk"), $content));
      return true;
    }

    /**
     * This function takes care of uploaded file
     */
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

    /**
     * This function checks if there is enough information to import the date
     * else it wil shows a form to set how the file wil be imported
     */
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

    /**
     * This function shows a form where the user can choose the mapping of the column,
     * an allfield and if the first record must be past over
     *
     * @param $fileid      string  the id of the uploaded file
     * @param $importerror string  if there is an importerror to show
     */
    function doAnalyze($fileid,$importerror="")
    {
      global $g_sessionManager;
      $filename = $this->getTmpFileDestination($fileid);

      $rows =             $this->getSampleRows($filename);      
      $delimiter =        $g_sessionManager->pageVar("delimiter");
      if ($delimiter=="") $delimiter = $this->estimateDelimiter($rows);
      $enclosure =        $g_sessionManager->pageVar("enclosure");
      if ($enclosure=="") $enclosure = $this->estimateEnclosure($rows);
      
      $allField =           $g_sessionManager->pageVar("allField");
      $skipfirstrow =       $this->m_postvars['skipfirstrow'];
      $doupdate =           $this->m_postvars['doupdate'];
      $updatekey1 =         $this->m_postvars['updatekey1'];
      $onfalseidentifier =  $this->m_postvars['onfalseid'];
     
      $columncount =    $this->estimateColumnCount($rows, $delimiter);

      $csv_data = $this->fgetcsvfromarray($rows, $columncount, $delimiter, $enclosure);
      

      // Display sample
      $sample = '<br><br>'.atktext("import_sample").':<br><br><table class="recordlist">';

      $col_map = $this->m_postvars["col_map"];
      if (!is_array($col_map))
      {
        // init colmap
        $col_map = $this->initColmap($csv_data[0]);
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
        $sample.=atktext("column").' '.$j;
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
          $sample.=($i==0&&$skipfirstrow)?'<th>':'<td>';
          if ($col_map[$j]!="" && $col_map[$j]!="-")
          {            
            $attr = &$this->getUsableAttribute($col_map[$j]);
            $newval = $attr->parseStringValue(trim($line[$j]));
            $sample.=$attr->display(array($col_map[$j]=>$newval));

            if ((string)$newval!=(string)$line[$j])
            {
              // Also display raw value so we can verify
              $sample.= ' <i style="color: #777777">('.trim($line[$j]).")</i>";
            }
          }
          else if ($col_map[$j]=="-")
          {
            // ignoring.
            $sample.='<div style="color: #777777">'.trim($line[$j]).'</div>';
          }
          else
          {
            $sample.=trim($line[$j]);
          }
          $sample.=($i==0&&$skipfirstrow)?'</th>':'</td>';
        }
        $sample.='</tr>';
      }
      $sample.= '</table><br><br>';


      $content.= '<br><form name="entryform" enctype="multipart/form-data" action="'.$_SERVER["PHP_SELF"].'?'.SID.'"'.
                                  ' method="post">'.session_form();
      $content.= '<input type="hidden" name="phase" value="process">';
      $content.= '<input type="hidden" name="fileid" value="'.$fileid.'">';
      $content.= '<input type="hidden" name="columncount" value="'.$columncount.'">';
      $content.= text("delimiter").': <input type="text" size="2" name="delimiter" value="'.htmlentities($delimiter).'">';
      $content.= '&nbsp; '.text("enclosure").': <input type="text" size="2" name="enclosure" value="'.htmlentities($enclosure).'">';
      $content.= '<br><br>'.atktext("import_detectedcolumns").': '.$columncount.'<br />';

      if(is_array($importerror))
        $content.= "<b>".text($importerror['msg']).$importerror['spec']."</b>";

      $content.= $sample;

      $content.= atktext("import_allfield").': <br/>'.atktext("import_field").': '.$this->getAttributeSelector(0,$allField,"allField").'<br/>';      
      if ($allField!="")
      {
        $attr = $this->getUsableAttribute($allField);
        
        if(is_object($attr))
        {
          $record = array();
  
          $content.= atktext("value").': '.$attr->edit($record,"","edit").'<br/>';
        }
      }

      $content.= '<br /><table id="importoptions">';
      $content.= '  <tr>';
      $content.= '    <td>';
      $content.= atktext("import_skipfirstrow").' </td><td><input type="checkbox" name="skipfirstrow" value="1" '.($skipfirstrow?"CHECKED":"").'/>';
      $content.= '</td></tr><tr><td>';
      $content.= atktext("import_doupdate").'    </td><td> <input type="checkbox" name="doupdate" value="1" '.($doupdate?"CHECKED":"").'/>';
      $content.= '</td></tr><tr><td>';
      $content.= atktext("import_update_key").' 1</td><td>'.$this->getAttributeSelector(0,$updatekey1,"updatekey1",0).'</td>';
      $content.= '</td></tr><tr><td>';
      $content.= atktext("import_onfalseidentifier").'    </td><td> <input type="checkbox" name="onfalseid" value="1" '.($onfalseidentifier?"CHECKED":"").'/>';
      
      
      $content.= '    </td>';
      $content.= '  </tr>';
      $content.= '</table><br /><br />';
      
      $content.= ' <input type="submit" name="analyse" value="'.atktext("import_analyse").'">';
      $content.= ' <input type="submit" name="import" value="'.atktext("import_import").'"> ';
      $content.= atkButton(atktext("back","atk"), "", SESSION_BACK, true);

      $content.= '</form><br><br>';

      $page = &$this->getPage();
      $theme = &atkTheme::getInstance();
      $page->register_style($theme->stylePath("recordlist.css"));
      $page->addContent($this->m_node->genericPage(atktext("import", "atk"), $content));
    }

    /**
     * Get the destination of the uploaded csv-file
     * @param $fileid string  The id of the file
     * @return string         The path of the file
     */
    function getTmpFileDestination($fileid)
    {
      return atkconfig("atktempdir")."csv_import_$fileid.csv";
    }

    /**
     * Get data from each line
     */
    function fgetcsvfromarray ($arr, $columncount, $delimiterChar = ',', $enclosureChar = '"')
    {
      $result = array();
      foreach ($arr as $line) { $result[] = $this->fgetcsvfromline($line, $columncount, $delimiterChar, $enclosureChar); }
      return $result;
    }

    /**
     * Gets the char which is used for enclosure in the csv-file
     * @param $rows     The rows from the csv-file
     * @return char     The enclosure
     */
    function estimateDelimiter($rows)
    {
      if (!is_array($rows)||count($rows)==0) return ",";
      if (strpos($rows[0], ";")!==false) return ";";
      if (strpos($rows[0], ",")!==false) return ",";
      if (strpos($rows[0], ":")!==false) return ":";
      else return ";";
    }

    /**
     * Gets the char which is used for enclosure in the csv-file
     * @param $rows     The rows from the csv-file
     * @return char     The enclosure
     */
    function estimateEnclosure($rows)
    {
      if (!is_array($rows)||count($rows)==0) return '"';
      if (substr_count($rows[0], '"')>=2) return '"';
      return '';
    }

    /**
     * Counts the number of columns in the first row
     * @param $rows     The rows from the csv-file
     * @return integer  The number of columns
     */
    function estimateColumnCount($rows, $delimiter)
    {
      if (!is_array($rows)||count($rows)==0) return 0;
      if ($delimiter == "") return 1;
      return (substr_count($rows[0], $delimiter)+1);

    }

    /**
     * Get the first 5 lines from the csv-file
     * @param          The path to the csv-file
     * @return array   The 5 lines
     */
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

    /**
     * source: http://bu.orbitel.bg/fgetcsvfromline.php (dawa@did-it.com)
     * Gets the csvdata from one line
     * @param $line           One line from the csv-file
     * @param $columncount    Number of columns in the cvs-file
     * @param $delimiterChar  The char which seperate the fields
     * @param $enclosureChar  The char which is in front and after each field
     * @return array          The fields
     */
    function fgetcsvfromline ($line, $columncount, $delimiterChar = ',', $enclosureChar = '"')
    {      
      $line = trim($line);
      
      if (substr($line, -1) == $delimiterChar)
        $line .= $enclosureChar.$enclosureChar;
      
      if ($enclosureChar!='')
      {
        $start = (($line[0]==$enclosureChar)?1:0);
        $end = (($line[strlen($line)-1]==$enclosureChar)?-1:NULL);
        $line = substr($line, $start, $end);
      }
                  
      $regExpSpecialChars = array 
      (
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

      $delimiterChar = strtr($delimiterChar, $regExpSpecialChars);
      $enclosureChar = strtr($enclosureChar, $regExpSpecialChars);
      
      // if we haven't got an enclosure char, the only thing we can do is
      // splitting it using the delimiterChar - no further action needed
      if (!$enclosureChar)
      {
        return explode($delimiterChar,$line);
      }
      
      // Some exports don't enclose empty or numeric fields with the enclosureChar. Let's fix
      // that first so we can use one preg_split statement that works in those cases too.      
      // loop until all occurrences are replaced. Contains an infinite loop prevention.
      for ($fix="", $i=0, $_i=substr_count($line, $enclosureChar); $fix!=$line && $i<$_i;$i++)
      {
        if ($fix!="") $line = $fix;
        $fix = preg_replace('/'.$delimiterChar.'(\d*)'.$delimiterChar.'/',
                           $delimiterChar.$enclosureChar.'\\1'.$enclosureChar.$delimiterChar, $line);
      }       
      // now split by delimiter
      $expression = '#'.$enclosureChar.' *'.$delimiterChar.' *'.$enclosureChar.'#';
      return preg_split($expression, $line);                
    }

    /**
     * Gives all the attributes that can be used for the import
     * @param $obligatoryOnly     if false then give all attributes, if true then give only the obligatory ones
     * @return array              the attributes
     */
    function getUsableAttributes($obligatoryOnly=false)
    {
      $attrs = array();
      foreach (array_keys($this->m_node->m_attribList) as $attribname)
      {
        $attrib = &$this->m_node->getAttribute($attribname);

        if($this->integrateAttribute($attrib))
        {
          $attrib->createDestination();
          foreach(array_keys($attrib->m_destInstance->m_attribList) as $relattribname)
          {
            $relattrib = &$attrib->m_destInstance->getAttribute($relattribname);
            
            if ($this->_usableForImport($obligatoryOnly, $relattrib))
            {
              $attrs[] = $relattribname;
            }
          }
        }
        else
        {        
          if ($this->_usableForImport($obligatoryOnly, $attrib))
          {
            $attrs[] = $attribname;
          }
        }
      }
      return $attrs;
    }
    
    /**
     * Check if an attribute is usable for import.
     */
    function _usableForImport($obligatoryOnly, &$attrib)
    {
      return ((!$obligatoryOnly || $this->isObligatory($attrib))
              && $attrib->storageType("add")==ADDTOQUERY && !$attrib->hasFlag(AF_AUTOINCREMENT) && !$this->isHide($attrib));
    }
    
    /**
     * Gives all obligatory attributes
     * same as getUsableAttributes with parameter true
     */
    function getObligatoryAttributes()
    {
      return $this->getUsableAttributes(true);
    }
    
    /**
     * Checks whether the attribute is obligatory
     * @return boolean    The result of the check
     */
    function isObligatory($attr)
    {
      return ($attr->hasFlag(AF_OBLIGATORY) && !$this->isHide($attr));
    }
    
    /**
     * Checks whether the attribute is hided by flag
     * @return boolean    The result of the check
     */
    function isHide($attr)
    {
      return ($attr->hasFlag(AF_HIDE) || ($attr->hasFlag(AF_HIDE_ADD) && $attr->hasFlag(AF_HIDE_EDIT)));
    }
    
    /**
     * Checks whether the attribute has the flag AF_ONETOONE_INTEGRATE
     * @return boolean    The result of the check
     */
    function integrateAttribute($attr)
    {
      return in_array(get_class($attr),array("atkonetoonerelation","atksecurerelation")) && $attr->hasFlag(AF_ONETOONE_INTEGRATE);
    }
    
    /**
     * Get al attributes from the import node that have the flag AF_ONETOONE_INTEGRATE
     * @return array  A list with all these attributes
     */
    function getIntegratedAttributes()
    {
      $attrs = array();
      foreach (array_keys($this->m_node->m_attribList) as $attribname)
      {
        $attrib = &$this->m_node->getAttribute($attribname);

        if($this->integrateAttribute($attrib))
        {
          $attrs[] = $attribname;
        }
      }

      return $attrs;
    }
    
    /**
     * Check whether the attribute is part of a relation
     * @param $attrname string  name of the attribute
     * @return mixed            false if not, relation name if yes
     */
    function isRelationAttribute($attrname)
    {
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

    /**
     * Check whether the attribute has a relation (only manytoonerelations)
     * @param $attrname string  name of the attribute
     * @return boolean          result of the check
     */
    function hasRelationAttribute($attrname)
    {      
      return in_array(get_class($this->getUsableAttribute($attrname)),array("atkmanytoonerelation","atkmanytoonetreerelation"));
    }

    /**
     * Get the real attribute (instance) by his name
     * @param $name string    name of the attribute
     * @return object         instance of the attribute
     */
    function getUsableAttribute($name)
    {      
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
    
    /**
     * Add one value to the record
     * @param $record array     the record wich will be changed
     * @param $attrname string  the name of the attribute
     * @param $value string     the value of that attribute
     */
    function addToRecord(&$record,$attrname,$value)
    {
      $attr = $this->getUsableAttribute($attrname);
    
      if(!is_object($attr)) return;
    
      foreach($this->getIntegratedAttributes() as $intattr)
      {
        if(!isset($record[$intattr]))
          $record[$intattr] = array('mode'=>"add",'atkaction'=>"save");
      }

      if(!$this->isRelationAttribute($attrname))
      {
        if(!$this->hasRelationAttribute($attrname))
        {
          $record[$attrname] = $attr->parseStringValue($value);
        }
        else 
        {
          $attr->createDestination();
          $key = $attr->m_destInstance->m_primaryKey;
          $record[$attrname] = array($key[0]=>$attr->parseStringValue($value));
        }
      }
      else
      {
        $record[$this->isRelationAttribute($attrname)][$attrname] = $attr->parseStringValue($value);
      }
    }

    /**
     * Returns a dropdownlist with all possible field in the importnode
     * @param index int         the number of the column
     * @param value string      the name of the attribute that is selected in the list (if empty then select the last one)
     * @param othername string  if set, use a other name for the dropdown, else use the name "col_map[index]"
     * @param int emptycol      mode for empty column (0 = no empty column, 1= empty column, 2= an 'ignore this column' (default))
     * @return string           the html-code for the dropdownlist (<select>...</sekect>)
     */
    function getAttributeSelector($index=0, $value="",$othername="", $emptycol=2)
    {
      if(!$othername)
        $res = '<select name="col_map['.$index.']">';
      else
        $res = '<select name="'.$othername.'" onchange="entryform.submit()">';

      $j=0;
      $hasoneselected = false;
      $attrs = $this->getUsableAttributes();
      foreach ($attrs as $attribname)
      {
        $selected = "";
        if ($value!="" && $value==$attribname)
        {
          // select the next.
          $selected="selected";
          $hasoneselected = true;
        }

        $res.= '<option value="'.$attribname.'" '.$selected.'>'.atktext($attribname, $this->m_node->m_module, $this->m_node->m_type)."\n";
        $j++;
      }
     
      if ($emptycol==2)      $res.= '<option value="-" '.(($value=="-"||!$hasoneselected)?"selected":"").' style="font-style: italic">'.atktext("import_ignorecolumn");
      elseif ($emptycol==1)  $res.= '<option value="" '.((!$value||!$hasoneselected)?"selected":"").'>';

      $res.= '</select>';
      return $res;
    }
    
    /**
     * the same als the php function array_search, but now much better.
     * This function is not case sensitive
     */
    function inArray($array,$value)
    {
      foreach($array as $key=>$item)
      {
        if(strtolower($item) == strtolower($value))
          return $key;

        if(strtolower($item) == strtolower(atktext($value, $this->m_node->m_module, $this->m_node->m_type)))
          return $key;
      }
      
      return false;
    }
    
    //make a record of translations of the given attributes
    function getAttributesTranslation($attributes){
      $result = array();
      
      foreach($attributes as $attribute)
      {
        $result[] = atktext($attribute, $this->m_node->m_module, $this->m_node->m_type);
      }
      
      return $result;
    }
    
    //tries to make a default col_map with the first record of the csv-file
    function initColmap($firstRecord)
    {
      $result = array();

      $attributes = $this->getUsableAttributes();
      $translations = $this->getAttributesTranslation($attributes);
      
      foreach($firstRecord as $value)
      {
        $key = $this->inArray($attributes,$value);
        if($key)
        {
          $result[] = $attributes[$key];
        }
        else
        {
          //checks the translation
          $key = $this->inArray($translations,$value);
          if($key)
            $result[] = $attributes[$key];
          else
            $result[] = "-";
        }
      }
    
      return $result;
    }

    /**
     * the real import function
     * import the uploaded csv file for real
     */
    function doImport()
    {
      $col_map =      $this->m_postvars["col_map"];
      $fileid =       $this->m_postvars["fileid"];
      $file =         $this->getTmpFileDestination($fileid);
      $enclosure =    $this->m_postvars["enclosure"];
      $delimiter =    $this->m_postvars["delimiter"];
      $columncount =  $this->m_postvars["columncount"];
      $allField =     $this->m_postvars["allField"];
      $skipfirstrow = $this->m_postvars['skipfirstrow'];
      
      //add the allField to the col_map array
      //but only if a valid field is selected      
      if ($allField!="")
      {        
        $attr = $this->getUsableAttribute($allField);
        if($attr)
        {
          $col_map[] = $allField;      
        }
      
        //get the value from the postvars
        $allFieldValue = $this->m_postvars[$allField];
        if(strstr($allFieldValue,"="))
        {
          $allFieldValue = substr(strstr($allFieldValue,"="),2);
          $allFieldValue = substr($allFieldValue,0,strlen($allFieldValue)-1);
        }
      }

      $initial_values = $this->m_node->initial_values();

      $importerror = "";

      $importerror = $this->checkImport($col_map,$initial_values);
      
      if(is_array($importerror))
      {
        $this->doAnalyze($this->m_postvars["fileid"],$importerror);
        return;
      }
      
      $importerror = $this->checkAllField($allField,$allFieldValue);

      if(is_array($importerror))
      {
        $this->doAnalyze($this->m_postvars["fileid"],$importerror);
        return;
      }

      //copy the csv in a record and add it to the db
      $fp = fopen($file, "r");
      if($skipfirstrow == "1") $line = fgets($fp);
      for($line = fgets($fp); $line!==false; $line = fgets($fp))
      {        
        //if we have an empty line, pass it
        if(trim($line)=="")
          continue;
          
        //large import are a problem for the maximum execution time, so we want to set for each
        //loop of the for-loop an maximum execution time
        set_time_limit(30);        
      
        $data = $this->fgetcsvfromline($line, $columncount, $delimiter, $enclosure);
        
        $rec = $initial_values;

        for ($i=0, $_i=count($col_map); $i<$_i; $i++)
        {
          if ($col_map[$i]!="-")
          {
            if($i != $_i-1 || $col_map[$i] != $allField)// column is mapped
            {
              $value = $data[$i]; 
            }
            else //this is the allField
            {
              $value = $allFieldValue;
            }
            $this->addToRecord($rec,$col_map[$i],trim($value));
            unset($value);
          }
        }

        $importerror = $this->validateAndAddRecord($rec);
        if ($importerror) break;
        unset($rec);
        unset($line);
        unset($data);
      }

      if(is_array($importerror))
      {
        $this->doAnalyze($this->m_postvars["fileid"],$importerror);
        return;
      }
      
      //return;
      fclose($fp);

      // remove the csv file
      unlink($file);

      $this->m_node->redirect();
    }
    
    /**
     * add a valid record to the db
     * @param $record array   the record
     */
    function add(&$record)
    {
      //atkerror(arrayToString($record));
      //return false;

      $this->m_node->preAdd($record);

      if(isset($record['atkerror']))
      {
        return false;
      }

      $this->m_node->addDb($record);
      
      if(isset($record['atkerror']))
      {
        return false;
      }

      return true;
    }
    
    /**
     * Update a record in the db
     * @param $record array   the record to update
     */
    function update(&$record)
    {
      $this->m_node->preUpdate($record);

      if(isset($record['atkerror']))
      {
        return false;
      }

      $this->m_node->updateDb($record);
      
      if(isset($record['atkerror']))
      {
        return false;
      }
      return true;
    }
    
    /**
     * check whether the record if valide to import
     * @param $record array   the record
     */
    function validate(&$record)
    {
      if ($this->m_postvars['doupdate']) $mode = "update";
      else $mode = "add";
        
      $this->m_node->validate($record,$mode);
      
      if(isset($record['atkerror']))
      {
        return false;
      }
      else
      {
        return true;
      }
    }
    
    /**
     * Checks the import by the col_map and the initial_values
     * Check if all obligatory fields are used in the col_map or the initial_values
     * Check if there are no fields used twice
     * @param $col_map        The use of the fields for the columns in the csv
     * @param $initial_values The initial_values of the importnode
     * @return boolean        If the check passed or not (true or false)
     */
    function checkImport($col_map,$initial_values)
    {
      //get the unused obligatory fields
      $unused = array_values(array_diff($this->getObligatoryAttributes(),$col_map));
      $unused = array_values(array_diff($unused,array_keys($initial_values)));
      
      if(count($unused) > 0)
      {
        $msg = atktext((count($unused)==1)?"import_error_fieldisobligatory":"import_error_fieldsareobligatory").": ";        
        return array("msg"=>$msg,"spec"=>implode(", ",$unused));
      }

      $doubles = $this->_getDuplicateColumns($col_map);
      
      if(count($doubles) > 0)
      {        
        $msg = atktext((count($doubles)==1)?"import_error_fieldusedtwice":"import_error_fieldsusedtwice").": ";        
        return array("msg"=>$msg,"spec"=>implode(", ",$doubles));
      }      
    }
    
    function _getDuplicateColumns($array)
    {
      $result = array();
      $frequencies = array_count_values($array);
      foreach ($frequencies as $key=>$count) 
      {
        if ($count>1 && $key!="-") $result[] = $key;
      }
      return $result;
    }
    
    function checkAllField($field, &$value)
    {
      $attr = $this->getUsableAttribute($field);
      if(!$attr)
        return;

      $record = array();
      $this->addToRecord($record,$field,$value);
      $result = $attr->display($record);

      if(!$result)
        if(in_array($field,$this->getObligatoryAttributes()))
        {
          return array('msg' => atktext("import_error_allfieldnocorrectdata"));
        }
        else
        {
          $value = "";
        }

      return;
    }
    
    function validateAndAddRecord(&$rec)
    {
      // Update variables
      $doupdate   =     $this->m_postvars['doupdate'];
      $updatekey1 =     $this->m_postvars['updatekey1'];
      $onfalseidentifier = $this->m_postvars['onfalseid'];
      
      if (!$this->validate($rec))
      {
        $importerror = array("msg"=>$rec['atkerror'][0]['msg'].": ",
        "spec"=>text($rec['atkerror'][0]['attrib_name']));
      }

      if ($doupdate) $prepareres = $this->prepareUpdateRecord($rec);

      if ($prepareres==true)
      {
        if (!$this->update($rec))
        {
          //continue;
          $importerror = array("msg"=>$rec['atkerror'][0]['msg'].": ",
          "spec"=>text($rec['atkerror'][0]['attrib_name']));
        }
      }
      else if (!$prepareres || $onfalseidentifier)
      {
        if (!$this->add($rec))
        {
          //continue;
          $importerror = array("msg"=>$rec['atkerror'][0]['msg'].": ",
          "spec"=>text($rec['atkerror'][0]['attrib_name']));
        }
      }
      else
      {
        $importerror = array("msg"=>atktext("error_formdataerror"),
        "spec"=>sprintf(atktext("import_nonunique_identifier"), $this->getValueFromRecord($rec, $updatekey1)));
      }
      return $importerror;
    }
    
    /**
     * Here we prepare our record for updating or return false,
     * indicating that we need to insert the record instead of updating it
     */
    function prepareUpdateRecord(&$record)
    {
      global $g_sessionManager;
      // The keys to update the record on
      $updatekey1 =     $this->m_postvars['updatekey1'];
      $updatekey1val =  $this->getValueFromRecord($record, $updatekey1);
      $allField =       $g_sessionManager->pageVar("allField");
      $allFieldValue =  $this->m_postvars[$allField];
      
      $this->m_node->m_postvars["atksearchmode"] = "exact";
      if (!in_array($allFieldValue)) $this->m_node->m_fuzzyFilters[] = $allFieldValue;
      
      $dbrec = $this->m_node->searchDb(array($updatekey1=>$updatekey1val));
      
      if (count($dbrec)==1)
      {
        $record[$this->m_node->primaryKeyField()] = $dbrec[0][$this->m_node->primaryKeyField()];
        $record['atkprimkey'] = $dbrec[0]['atkprimkey'];
        return true;
      }
      return false;
    }
    
    function getValueFromRecord($record, $fieldname)
    {
      $attr = $this->getUsableAttribute($fieldname);
      if(!$this->isRelationAttribute($fieldname))
      {
        if(!$this->hasRelationAttribute($fieldname))
        {
          $value = $record[$fieldname];
        }
        else
        {
          if (is_object($attr) && $attr->createDestination())
          {
            $key = $attr->m_destInstance->m_primaryKey;
            $value = $record[$fieldname][$key[0]];
          }
        }
      }
      else
      {
        $value = $record[$this->isRelationAttribute($fieldname)][$fieldname];
      }
      return $attr->value2db(array($fieldname=>$value));
    }
  }
?>
