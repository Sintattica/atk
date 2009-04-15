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
   * @version $Revision: 6323 $
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
    var $m_importNode;

    /**
     * The action handler.
     * @param bool Always true
     */
    function action_import()
    {
      global $ATK_VARS;

      //need to keep the postdata after a AF_LARGE selection in the allfield
      if(!isset($this->m_postvars["phase"]) && isset($ATK_VARS['atkformdata']))
        foreach($ATK_VARS['atkformdata'] as $key=>$value)
          $this->m_postvars[$key] = $value;

      $keys = array();
      //need to keep the selected item after an importerror
      if (is_array($ATK_VARS['allFields'])) $keys = array_keys($ATK_VARS['allFields']);
      foreach ($keys as $key)
      {
        if(!isset($ATK_VARS[$ATK_VARS['allFields'][$key]."_newsel"]))
          $ATK_VARS[$ATK_VARS['allFields'][$key]."_newsel"] = $ATK_VARS[$ATK_VARS['allFields'][$key]];
      }

      $phase = ($this->m_postvars["phase"]!=""?$this->m_postvars["phase"]:"init");

      switch ($phase)
      {
        case "init": $this->doInit(); break;
        case "upload": $this->doUpload(); break;
        case "process": $this->doProcess(); break;
      }
    }

    /**
     * Sets the node for this handler. Implicitly sets the
     * import node too!
     *
     * @param atkNode $node node instance
     *
     * @see setImportNode
     */
    function setNode(&$node)
    {
      parent::setNode($node);
      $this->setImportNode($node);
    }

    /**
     * Sets the import node. By default this is the same node
     * as set by setNode, but if you call this method after the setNode
     * call you can override the import node.
     *
     * @param atkNode $node node instance
     *
     * @see setNode
     */
    function setImportNode(&$node)
    {
      $this->m_importNode = &$node;
    }

    /**
     * Create import page for the given phase.
     *
     * @param string $phase import phase (init, upload, process)
     * @param string $content page content
     */
    function importPage($phase, $content)
    {
      $controller = &atkController::getInstance();
      $action = $controller->getPhpFile().'?'.SID;

      $formStart =
        '<form id="entryform" name="entryform" enctype="multipart/form-data" action="'.$action.'" method="post">'.
        session_form(atkLevel() == 0 ? SESSION_NESTED : SESSION_REPLACE).
        '<input type="hidden" name="atknodetype" value="'.$this->m_node->atkNodeType().'" />'.
        '<input type="hidden" name="atkaction" value="'.$this->m_node->m_action.'" />'.
        $controller->getHiddenVarsString();

      $buttons = $this->invoke('getImportButtons', $phase);

      $ui = &$this->m_node->getUi();
      $page = &$this->m_node->getPage();

      $this->m_node->addStyle("style.css");

      $params = $this->m_node->getDefaultActionParams(false);
      $params['header'] = $this->invoke('importHeader', $phase);
      $params['formstart'] = $formStart;
      $params['content'] = $content;
      $params['buttons'] = $buttons;
      $output = $ui->renderAction('import', $params);

      $params = array();
      $params['title'] = $this->m_node->actionTitle('import');
      $params['content'] = $output;
      $output = $ui->renderBox($params);

      $output = $this->m_node->renderActionPage('import', $output);
      $page->addContent($output);
    }

    /**
     * Import header.
     *
     * @param string $phase import phase ('init', 'upload', 'process', 'analyze')
     */
    function importHeader($phase)
    {
      return '';
    }

    /**
     * Get import buttons.
     *
     * @param string $phase import phase ('init', 'upload', 'process', 'analyze')
     */
    function getImportButtons($phase)
    {
      $result = array();

      if ($phase == 'init')
      {
        $result[] = '<input class="btn" type="submit" value="'.$this->m_node->text("import_upload").'">';
      }
      else if ($phase == 'analyze')
      {
        $result[] = '<input type="submit" class="btn" name="analyse" value="'.$this->m_node->text("import_analyse").'">';
        $result[] = '<input type="submit" class="btn" name="import" value="'.$this->m_node->text("import_import").'"> ';
      }

      if (atkLevel() > 0)
      {
        $result[] = atkButton($this->m_node->text("cancel","atk"), "", SESSION_BACK, true);
      }

      return $result;
    }

    /**
     * This function shows a form to upload a .csv
     * @param bool Always true
     */
    function doInit()
    {
      $content = '
        <input type="hidden" name="phase" value="upload">
        <table border="0">
          <tr>
            <td style="text-align: left">
              '.$this->m_node->text("import_upload_explanation").'
              <br /><br />
              <input type="file" name="csvfile">
            </td>
          </tr>
        </table>';

      $this->invoke('importPage', 'init', $content);
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
        $this->doAnalyze($fileid);
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
        $this->doImport($filename);
      }
      else
      {
        // reanalyze
        $this->doAnalyze($this->m_postvars["fileid"]);
      }
    }

    /**
     * This function shows a form where the user can choose the mapping of the column,
     * an allfield and if the first record must be past over
     *
     * @param string $fileid       the id of the uploaded file
     * @param array $importerrors  An array with the import errors
     */
    function doAnalyze($fileid,$importerrors=array())
    {
      $sessionMgr = &atkGetSessionManager();
      $filename = $this->getTmpFileDestination($fileid);

      $rows =             $this->getSampleRows($filename);
      $delimiter =        $sessionMgr->pageVar("delimiter");
      if ($delimiter=="") $delimiter = $this->estimateDelimiter($rows);
      $enclosure =        $sessionMgr->pageVar("enclosure");
      if ($enclosure=="") $enclosure = $this->estimateEnclosure($rows);

      $allFields =          $sessionMgr->pageVar("allFields");
      if($allFields=="")    $allFields = array();
      $skipfirstrow =       $this->m_postvars['skipfirstrow'];
      $doupdate =           $this->m_postvars['doupdate'];
      $updatekey1 =         $this->m_postvars['updatekey1'];
      $onfalseidentifier =  $this->m_postvars['onfalseid'];
      $novalidatefirst =    $this->m_postvars['novalidatefirst'];

      $columncount =    $this->estimateColumnCount($rows, $delimiter);

      $csv_data = $this->fgetcsvfromarray($rows, $columncount, $delimiter, $enclosure);

      $col_map = $this->m_postvars["col_map"];
      if (!is_array($col_map))
      {
        // init colmap
        $col_map = $this->initColmap($csv_data[0], $matchFound);
      }

      if ($skipfirstrow === null)
      {
        $skipfirstrow = $matchFound;
      }

      if ($columncount>count($col_map))
      {
        // fill with ignored
          for ($i=0, $_i=($columncount-count($col_map)); $i<$_i; $i++) $col_map[] = "-";
      }

      $rowCount = $this->getRowCount($filename, $skipfirstrow);

      // Display sample
      $sample =
        atktext("import_sample").':<br><br><table class="recordlist">'.
        $this->_getAnalyseSample($columncount, $col_map, $csv_data, $skipfirstrow);

      $content = '
        <input type="hidden" name="phase" value="process">
        <div style="text-align: left; margin-left: 10px;">
          '.$this->_getAnalyseHeader($fileid, $columncount, $delimiter, $enclosure, $rowCount).'
          <br />
          '.$this->_getErrors($importerrors).'
          '.$sample.'
          <br />
          '.$this->_getAnalyseExtraOptions($skipfirstrow, $doupdate, $updatekey1, $onfalseidentifier, $allFields, $novalidatefirst).'
        </div>';

      $page = &$this->m_node->getPage();
      $theme = &atkinstance("atk.ui.atktheme");
      $page->register_style($theme->stylePath("recordlist.css"));
      $this->invoke('importPage', 'analyze', $content);
    }

    /**
     * Transforms the $importerrors array into displayable HTML
     *
     * @todo make this use templates
     *
     * @param Array $importerrors A special array with arrays in it
     *                            $importerrors[0] are general errors, other than that
     *                            the numbers stand for recordnumbers
     * @return String HTML table with the errors
     */
    function _getErrors($importerrors)
    {
      if(is_array($importerrors))
      {
        $content.="\n<table>";

        $errorCount = 0;
        foreach ($importerrors as $record => $errors)
        {
          $errorCount++;

          if ($errorCount > atkconfig("showmaximporterrors",50)) break;

          if ($record==0 && atk_value_in_array($errors))
          {
            $content.="<tr><td colSpan=2>";
            foreach ($errors as $error)
            {
              if (!empty($error)) $content.= "<span class=\"error\">".text($error['msg']).$error['spec']."</span><br />";
            }
            $content.="</td></tr>";
          }
          else if (atk_value_in_array($errors))
          {
            $content.="<tr><td valign=\"top\" class=\"error\">";
            $content.="<b>Record $record:</b>&nbsp;";
            $content.="</td><td valign=\"top\" class=\"error\">";
            $counter = 0;
            for ($counter=0;$counter<count($errors)&&$counter<atkconfig("showmaximporterrors",50);$counter++)
            {
              $content.= $this->m_node->text($errors[$counter]['msg']).$errors[$counter]['spec']."<br />";
            }
            $content.="</td></tr>";
          }
        }
        $content.="</tr></table><br />";
      }

      return $content;
    }

    /**
     * Returns the HTML header for the 'analyse' mode of the import handler
     * @param String $fileid      The 'id' (name) of the file we are importing
     * @param String $columncount The number of columns we have
     * @param String $delimiter   The delimiter in the file
     * @param String $enclosure   The enclosure in the file
     * @param int    $rowcount    The number of rows in the CSV file
     * @return String The HTML header
     */
    function _getAnalyseHeader($fileid, $columncount, $delimiter, $enclosure, $rowcount)
    {
      $content = '<br>';
      $content.= '<input type="hidden" name="fileid" value="'.$fileid.'">';
      $content.= '<input type="hidden" name="columncount" value="'.$columncount.'">';
      $content.= '<table border="0">';
      $content.= '<tr><td>'.text("delimiter").': </td><td><input type="text" size="2" name="delimiter" value="'.atk_htmlentities($delimiter).'"></td></tr>';
      $content.= '<tr><td>'.text("enclosure").': </td><td><input type="text" size="2" name="enclosure" value="'.atk_htmlentities($enclosure).'"></td></tr>';
      $content.= '<tr><td>'.atktext("import_detectedcolumns").': </td><td>'.$columncount.'</td></tr>';
      $content.= '<tr><td>'.atktext("import_detectedrows").': </td><td>'.$rowcount.'</td></tr>';
      $content.= '</table>';
      return $content;
    }

    /**
     * Returns a sample of the analysis
     * @param String $columncount  The number of columns we have
     * @param String $col_map      A mapping of the column
     * @param String $csv_data     The CSV data
     * @param String $skipfirstrow Wether or not to skip the first row
     */
    function _getAnalyseSample($columncount, $col_map, $csv_data, $skipfirstrow)
    {
      // header
      $sample.= '<tr>';
      for ($j=1; $j<=$columncount; $j++)
      {
        $sample.='<th>';
        $sample.= ucfirst(atktext("column")).' '.$j;
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
          if ($i==0&&$skipfirstrow)
          {
            $sample.='<th>';
            $sample.=atktext(trim($line[$j]));
          }
          else
          {
            $sample.='<td>';
            if ($col_map[$j]!="" && $col_map[$j]!="-")
            {
              $display = $this->_getSampleValue($col_map[$j],trim($line[$j]));
              if ($display) $sample.= $display;
              else $sample.= atktext($col_map[$j]);

              if ((string)$display!==(string)$line[$j])
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
          }
          $sample.=($i==0&&$skipfirstrow)?'</th>':'</td>';
        }
        $sample.='</tr>';
      }
      $sample.= '</table>';
      return $sample;
    }

    /**
     * Gets the displayable value for the attribute
     * @param String $attributename The name of the attribute
     * @param String $value         The value of the attribute
     * @return String The displayable value for the attribute
     */
    function _getSampleValue($attributename, $value)
    {
      $attr = &$this->getUsableAttribute($attributename);

      if(method_exists($attr, "parseTime"))
        $newval = $attr->parseTime($value);
      else
        $newval = $attr->parseStringValue($value);

      if (method_exists($attr, "createDestination"))
      {
        $attr->createDestination();

        // If we can create a destination, then we can be reasonably sure it's a relation
        // and importing in a relation is a different matter altogether
        $searchresults = $attr->m_destInstance->searchDb($newval);
        if (count($searchresults)==1)
        {
          $atkval = array($attributename=>array($attr->m_destInstance->primaryKeyField()=>$searchresults[0][$attr->m_destInstance->primaryKeyField()]));
        }
      }
      else
      {
        $atkval = array($attributename=>$newval);
      }
      return $attr->display($atkval);
    }

    /**
     * Returns the extra options of the importhandler
     * @param String $skipfirstrow      Wether or not to skip the first row
     * @param String $doupdate          Wether or not to do an update
     * @param String $updatekey1        The key to update on
     * @param String $onfalseidentifier What to do on a false identifier
     * @param String $allFields					The fields to import
     * @param Bool	 $novalidatefirst 	Validate before the import 
     * @return String The HTML with the extra options
     */
    function _getAnalyseExtraOptions($skipfirstrow, $doupdate, $updatekey1, $onfalseidentifier, $allFields, $novalidatefirst)
    {
      $content.= '<br /><table id="importoptions">';
      $content.= '  <tr>';
      $content.= '    <td>';

      foreach ($allFields as $allfield)
      {
        if (!$this->m_postvars[$allfield]) $noallfieldvalue=true;
      }

      if (empty($allFields) || !$noallfieldvalue) $allFields[] = '';
      foreach ($allFields as $allField)
      {
        $content.= atktext("import_allfield").': </td><td>'.$this->getAttributeSelector(0,$allField,"allFields[]");

        if ($allField!="")
        {
          $attr = $this->getUsableAttribute($allField);

          if(is_object($attr))
          {
            $fakeeditarray = array($allField=>$this->m_postvars[$allField]);
            $content.= ' '.atktext("value").': '.$attr->edit($fakeeditarray,"","edit").'<br/>';
          }
        }
        $content.= '</td></tr><tr><td>';
      }

      $content.= atktext("import_skipfirstrow").': </td><td><input type="checkbox" name="skipfirstrow" class="atkcheckbox" value="1" '.($skipfirstrow?"CHECKED":"").'/>';
      $content.= '</td></tr><tr><td>';
      $content.= atktext("import_doupdate").': </td><td> <input type="checkbox" name="doupdate" class="atkcheckbox" value="1" '.($doupdate?"CHECKED":"").'/>';
      $content.= '</td></tr><tr><td>';
      $content.= atktext("import_update_key").': </td><td>'.$this->getAttributeSelector(0,$updatekey1,"updatekey1",2).'</td>';
      $content.= '</td></tr><tr><td>';
      $content.= atktext("import_onfalseidentifier").': </td><td> <input type="checkbox" name="onfalseid" class="atkcheckbox" value="1" '.($onfalseidentifier?"CHECKED":"").'/>';
      $content.= '</td></tr><tr><td>';
      $content.= atktext("import_novalidatefirst").': </td><td> <input type="checkbox" name="novalidatefirst" class="atkcheckbox" value="1" '.($novalidatefirst?"CHECKED":"").'/>';

      $content.= '    </td>';
      $content.= '  </tr>';
      $content.= '</table><br /><br />';
      return $content;
    }

    /**
     * Get the destination of the uploaded csv-file
     * @param string $fileid  The id of the file
     * @return string         The path of the file
     */
    function getTmpFileDestination($fileid)
    {
      return atkconfig("atktempdir")."csv_import_$fileid.csv";
    }

    /**
     * Get data from each line
     * @param Array  $arr           An array with the lines from the CSV file
     * @param int    $columncount   The number of columns in the file
     * @param String $delimiterChar The delimeter character
     * @param String $enclosureChar The enclosure character
     * @return Array An array with the CSV data
     */
    function fgetcsvfromarray ($arr, $columncount, $delimiterChar = ',', $enclosureChar = '"')
    {
      $result = array();
      foreach ($arr as $line)
      {
        $result[] = $this->fgetcsvfromline($line, $columncount, $delimiterChar, $enclosureChar);
      }
      return $result;
    }

    /**
     * Gets the char which is used for enclosure in the csv-file
     * @param Array $rows The rows from the csv-file
     * @return String     The enclosure
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
     * @param Array $rows The rows from the csv-file
     * @return String     The enclosure
     */
    function estimateEnclosure($rows)
    {
      if (!is_array($rows)||count($rows)==0) return '"';
      if (substr_count($rows[0], '"')>=2) return '"';
      return '';
    }

    /**
     * Counts the number of columns in the first row
     * @param Array $rows     The rows from the csv-file
     * @param String $delimiter The char which seperate the fields
     * @return int  The number of columns
     */
    function estimateColumnCount($rows, $delimiter)
    {
      if (!is_array($rows)||count($rows)==0) return 0;
      if ($delimiter == "") return 1;
      return (substr_count($rows[0], $delimiter)+1);
    }

    /**
     * Get the first 5 lines from the csv-file
     * @param String $file   The path to the csv-file
     * @return Array   The 5 lines from the csv file
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
     * Returns the CSV line count.
     *
     * @param string $file the path to the csv-file
     * @param bool $skipFirstRow Skip the first row?
     * @return int row count
     */
    function getRowCount($file, $skipFirstRow)
    {
      $count = 0;

      $fp = fopen($file, "r");
      while ($line = fgets($fp))
      {
        if (trim($line) == "") continue;
        $count++;
      }

      return $count - ($count > 0 && $skipFirstRow ? 1 : 0);
    }

    /**
     * Gets the csvdata from one line
     *
     * source: http://bu.orbitel.bg/fgetcsvfromline.php (bu@orbitel.bg)
     *
     * @param String $line           One line from the csv-file
     * @param int    $columncount    Number of columns in the cvs-file
     * @param String $delimiterChar  The char which seperate the fields
     * @param String $enclosureChar  The char which is in front and after each field
     * @return Array The fields
     */
    function fgetcsvfromline ($line, $columncount, $delimiterChar = ',', $enclosureChar = '"')
    {
      $line = trim($line);

      if (substr($line, -1) == $delimiterChar)
        $line .= $enclosureChar.$enclosureChar;

      if ($enclosureChar!='')
      {
        $start = (($line[0]==$enclosureChar)?1:0);
        $end = (($line[atk_strlen($line)-1]==$enclosureChar)?-1:NULL);
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
     * @param bool $obligatoryOnly    if false then give all attributes, if true then give only the obligatory ones
     *                                defaults to false
     * @return Array the attributes
     */
    function getUsableAttributes($obligatoryOnly=false)
    {
      $attrs = array();
      foreach (array_keys($this->m_importNode->m_attribList) as $attribname)
      {
        $attrib = &$this->m_importNode->getAttribute($attribname);

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
     * @param bool   $obligatoryOnly  Wether or not we should concider obligatory attributes
     * @param Object &$attrib         The attribute
     * @return bool Wether or not the attribute is usable for import
     */
    function _usableForImport($obligatoryOnly, &$attrib)
    {
      return ((!$obligatoryOnly || $this->isObligatory($attrib))
              && !$attrib->hasFlag(AF_AUTOINCREMENT) && !$this->isHide($attrib)
              && !is_a($attrib, 'atkdummyattribute'));
    }

    /**
     * Gives all obligatory attributes
     *
     * Same as getUsableAttributes with parameter true
     * @return Array An array with all the obligatory attributes
     */
    function getObligatoryAttributes()
    {
      return $this->getUsableAttributes(true);
    }

    /**
     * Checks whether the attribute is obligatory
     * @param Object $attr  The attribute to check
     * @return boolean The result of the check
     */
    function isObligatory($attr)
    {
      return ($attr->hasFlag(AF_OBLIGATORY) && !$this->isHide($attr));
    }

    /**
     * Checks whether the attribute is hiden by a flag
     * @param Object $attr  The attribute to check
     * @return boolean    The result of the check
     */
    function isHide($attr)
    {
      return (($attr->hasFlag(AF_HIDE) || ($attr->hasFlag(AF_HIDE_ADD) && $attr->hasFlag(AF_HIDE_EDIT)))&& !$attr->hasFlag(AF_FORCE_LOAD));
    }

    /**
     * Checks whether the attribute has the flag AF_ONETOONE_INTEGRATE
     * @param Object $attr  The attribute to check
     * @return boolean    The result of the check
     */
    function integrateAttribute($attr)
    {
      return in_array(get_class($attr),array("atkonetoonerelation","atksecurerelation")) && $attr->hasFlag(AF_ONETOONE_INTEGRATE);
    }

    /**
     * Get al attributes from the import node that have the flag AF_ONETOONE_INTEGRATE
     * @return array  A list with all attributes from the import node that have the flag AF_ONETOONE_INTEGRATE
     */
    function getIntegratedAttributes()
    {
      $attrs = array();
      foreach (array_keys($this->m_importNode->m_attribList) as $attribname)
      {
        $attrib = &$this->m_importNode->getAttribute($attribname);

        if($this->integrateAttribute($attrib))
        {
          $attrs[] = $attribname;
        }
      }
      return $attrs;
    }

    /**
     * Check whether the attribute is part of a relation
     * @param String $attrname  name of the attribute
     * @return mixed            false if not, relation name if yes
     */
    function isRelationAttribute($attrname)
    {
      if(array_key_exists($attrname,$this->m_importNode->m_attribList))
        return false;

      foreach($this->getIntegratedAttributes() as $attr)
      {
        $relattr = $this->m_importNode->getAttribute($attr);
        $relattr->createDestination();
        if(array_key_exists($attrname,$relattr->m_destInstance->m_attribList))
          return $attr;
      }
      return false;
    }

    /**
     * Check whether the attribute has a relation (only manytoonerelations)
     * @param String $attrname  name of the attribute
     * @return boolean          result of the check
     */
    function hasRelationAttribute($attrname)
    {
      return in_array(get_class($this->getUsableAttribute($attrname)),array("atkmanytoonerelation","atkmanytoonetreerelation"));
    }

    /**
     * Get the real attribute (instance) by his name
     * @param String $name    name of the attribute
     * @return object         instance of the attribute
     */
    function &getUsableAttribute($name)
    {
      if(array_key_exists($name,$this->m_importNode->m_attribList))
        return $this->m_importNode->getAttribute($name);

      foreach($this->getIntegratedAttributes() as $attr)
      {
        $relattr = $this->m_importNode->getAttribute($attr);
        $relattr->createDestination();
        if(array_key_exists($name,$relattr->m_destInstance->m_attribList))
          return $relattr->m_destInstance->getAttribute($name);
      }
      return null;
    }

    /**
     * Add one value to the record
     * @param Array $record     the record wich will be changed
     * @param String $attrname  the name of the attribute
     * @param String $value     the value of that attribute
     */
    function addToRecord(&$record,$attrname,$value)
    {
      $attr = &$this->getUsableAttribute($attrname);

      if(!is_object($attr)) return;

      foreach($this->getIntegratedAttributes() as $intattr)
      {
        if(!isset($record[$intattr]))
          $record[$intattr] = array('mode'=>"add",'atkaction'=>"save");
      }

      $record[$attrname] = $value;
    }

    /**
     * Returns a dropdownlist with all possible field in the importnode
     * @param int $index         the number of the column
     * @param String $value      the name of the attribute that is selected in the list (if empty then select the last one)
     * @param String $othername  if set, use a other name for the dropdown, else use the name "col_map[index]"
     * @param int $emptycol      mode for empty column (0 = no empty column, 1= empty column, 2= an 'ignore this column' (default))
     * @return String            the html-code for the dropdownlist (<select>...</sekect>)
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
        $attr = &$this->getUsableAttribute($attribname);
        $label = $attr->label();

        $selected = "";
        if ($value!="" && $value==$attribname)
        {
          // select the next.
          $selected="selected";
          $hasoneselected = true;
        }

        $res.= '<option value="'.$attribname.'" '.$selected.'>'.$label."\n";
        $j++;
      }

      if ($emptycol==2)      $res.= '<option value="-" '.(($value=="-"||!$hasoneselected)?"selected":"").' style="font-style: italic">'.atktext("import_ignorecolumn");
      elseif ($emptycol==1)  $res.= '<option value="" '.((!$value||!$hasoneselected)?"selected":"").'>';

      $res.= '</select>';
      return $res;
    }

    /**
     * The same als the php function array_search, but now much better.
     * This function is not case sensitive
     * @param Array $array The array to search through
     * @param mixed $value The value to search for
     * @return mixed The key if it is in the array, else false
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

    /**
     * Make a record of translations of the given attributes
     * @param Array $attributes The attributes to translate
     * @return Array The result of the translation
     */
    function getAttributesTranslation($attributes)
    {
      $result = array();

      foreach($attributes as $attribute)
      {
        $attr = &$this->getUsableAttribute($attribute);
        $label = $attr->label();
        $result[] = $label;
      }

      return $result;
    }

    /**
     * Tries to make a default col_map with the first record of the csv-file
     * @param Array $firstRecord The first record of the CSV file
     * @param Bool &$matchFound Found a match?
     * @return Array The default col_map
     */
    function initColmap($firstRecord, &$matchFound)
    {
      $result = array();

      $attributes = $this->getUsableAttributes();
      $translations = $this->getAttributesTranslation($attributes);

      $matchFound = false;

      foreach($firstRecord as $value)
      {
        $key = $this->inArray($attributes,$value);
        if($key)
        {
          $result[] = $attributes[$key];
          $matchFound = true;
        }
        else
        {
          //checks the translation
          $key = $this->inArray($translations,$value);

          if($key!==false)
          {

            $result[] = $attributes[$key];
            $matchFound = true;
          }
          else
            $result[] = "-";
        }
      }

      return $result;
    }

    /**
     * Add the allField to the col_map array
     * but only if a valid field is selected
     * @param Array $col_map The map of columns (!stub)
     * @return mixed The value for the field to use with all records
     */
    function getAllFieldsValues(&$col_map)
    {
      $allFields =     $this->m_postvars["allFields"];

      foreach ($allFields as $key=>$allField)
      {
        if ($allField!="")
        {
          $attr = &$this->getUsableAttribute($allField);
          if($attr)
          {
            $col_map[] = $allField;
          }

          //get the value from the postvars
          $allFieldValue = $this->m_postvars[$allField];
          if(strstr($allFieldValue,"="))
          {
            $allFieldValue = substr(strstr($allFieldValue,"="),2);
            $allFieldValue = substr($allFieldValue,0,atk_strlen($allFieldValue)-1);
          }
          $allFieldsValues[$allField] = $allFieldValue;
        }
      }
      return $allFieldsValues;
    }

    /**
     * The real import function actually imports the importfile
     * 
     * @param Bool $nopost 
     */
    function doImport($nopost=false)
    {
      ini_set('max_execution_time',300);
      $db = &$this->m_importNode->getDb();
      $fileid =       $this->m_postvars["fileid"];
      $file =         $this->getTmpFileDestination($fileid);

      $validated = $this->getValidatedRecords($file);

      if (!$this->m_postvars['novalidatefirst'] && $this->showErrors($validated['importerrors']))
      {
        $db->rollback();
        return;
      }

      $this->addRecords($validated['importerrors'], $validated['validatedrecs']);

      if (!$this->m_postvars['novalidatefirst'] && $this->showErrors($validated['importerrors']))
      {
        $db->rollback();
        return;
      }

      $db->commit();

      // clean-up
      @unlink($file);

      // clear recordlist cache
      $this->clearCache();

      // register message
      atkimport('atk.utils.atkmessagequeue');
      $messageQueue = &atkMessageQueue::getInstance();

      $count = count((array)$validated['validatedrecs']['add']) + count((array)$validated['validatedrecs']['update']);
      if ($count == 0)
      {
        $messageQueue->addMessage(sprintf($this->m_node->text('no_records_to_import'), $count), AMQ_GENERAL);
      }
      else if ($count == 1)
      {
        $messageQueue->addMessage($this->m_node->text('successfully_imported_one_record'), AMQ_SUCCESS);
      }
      else
      {
        $messageQueue->addMessage(sprintf($this->m_node->text('successfully_imported_x_records'), $count), AMQ_SUCCESS);
      }

      $this->m_node->redirect();
    }

    /**
     * Get the validated records
     *
     * @param String $file The import csv file
     * @return Array with importerrors and validatedrecs
     */
    function getValidatedRecords($file)
    {
      $enclosure =    $this->m_postvars["enclosure"];
      $delimiter =    $this->m_postvars["delimiter"];
      $columncount =  $this->m_postvars["columncount"];
      $skipfirstrow = $this->m_postvars['skipfirstrow'];
      $allFields =     $this->m_postvars["allFields"];
      $col_map =      $this->m_postvars["col_map"];

      $allFieldsValues = $this->getAllFieldsValues($col_map);
      $initial_values = $this->m_importNode->initial_values();

      $validatedrecs = array();
      $validatedrecs["add"]=array();
      $validatedrecs["update"] = array();
      $importerrors = array();
      $importerrors[0]=array();

      $importerrors[0] = array_merge($importerrors[0],$this->checkImport($col_map,$initial_values));
      $allfielderror = $this->checkAllFields($allFields,$allFieldsValues);
      if ($allfielderror)
      {
        $importerrors[0][] = $allfielderror;
      }

      if (count($importerrors[0]) > 0)
      {
        // don't start importing if even the minimum requirements haven't been met
        return array('importerrors'=>&$importerrors, 'validatedrecs'=>array());
      }

      //copy the csv in a record and add it to the db
      $fp = fopen($file, "r");
      if($skipfirstrow == "1") $line = fgets($fp);
      for($line = fgets($fp),$counter=1; $line!==false; $line = fgets($fp),$counter++)
      {
        atkdebug("Validating record nr. $counter");
        //if we have an empty line, pass it
        if(trim($line)=="")
          continue;

        //large import are a problem for the maximum execution time, so we want to set for each
        //loop of the for-loop an maximum execution time
        set_time_limit(60);
        atkdebug('set_time_limit(60)');

        if (atkGetCharset() != '' && function_exists("mb_convert_encoding")) $line = mb_convert_encoding($line, atkGetCharset());

        $data = $this->fgetcsvfromline($line, $columncount, $delimiter, $enclosure);

        $rec = $initial_values;
        atkimport("atk.utils.atkstringparser");

        for ($i=0, $_i=count($col_map); $i<$_i; $i++)
        {
          if ($col_map[$i]!="-")
          {
            if(!in_array($col_map[$i],$allFields))// column is mapped
            {
              $value = $this->_getAttributeValue($col_map[$i], $allFields, $data[$i], $importerrors,$counter,$rec);
            }
            else //this is the allField
            {
              $value = $allFieldsValues[$col_map[$i]];
            }
            $this->addToRecord($rec,$col_map[$i],$value);
          }
        }
        $this->validateRecord($rec, $validatedrecs, $importerrors, $counter);
      }

      // close file
      @fclose($fp);

      return array('importerrors'=>&$importerrors, 'validatedrecs'=>&$validatedrecs);
    }

    /**
     * Gets the ATK value of the attribute
     * 
     * @param String $attributename The name of the attribute
     * @param Array $allFields      Array with all the fields
     * @param mixed $value          The value from the CSV file
     * @param Array  &$importerrors  Any import errors which may occur or may have occured
     * @param Integer $counter			The counter of the validatedrecords
     * @param Array $rec            The record
     * 
     * @return mixed The ATK value of the field
     */
    function _getAttributeValue($attributename, $allFields, $value, &$importerrors, $counter, $rec)
    {
      $updatekey1 =     $this->m_postvars['updatekey1'];
      $attr = &$this->getUsableAttribute($attributename);

      if (method_exists($attr, "createDestination") && $attr->createDestination() && !in_array($attributename,$allFields))
      {
        $primaryKeyAttr = $attr->m_destInstance->getAttribute($attr->m_destInstance->primaryKeyField());
        $isNumeric = $attr->hasFlag(AF_AUTO_INCREMENT) || is_a($primaryKeyAttr, 'atknumberattribute');

        $relationselect = array();

        // this check only works if either the primary key column is non-numeric or the given value is numeric
        if (!$isNumeric || is_numeric($value))
        {
          $relationselect = $attr->m_destInstance->selectDb($attr->m_destInstance->m_table.".".$attr->m_destInstance->primaryKeyField().' = \''.escapeSQL($value)."'");
        }

        if (count($relationselect)==0 || count($relationselect)>1)
        {
          $searchresults = $attr->m_destInstance->searchDb($value);
          if (count($searchresults)==1)
          {
            $value = array($attr->m_destInstance->primaryKeyField()=>$searchresults[0][$attr->m_destInstance->primaryKeyField()]);
          }
          else
          {
            $relation = $this->isRelationAttribute($attributename);

            if ($relation) $rec[$relation][$attributename] = $value;
            else $rec[$attributename] = $value;

            $importerrors[$counter][] = array("msg"=>atktext("error_formdataerror"),
            "spec"=>sprintf(atktext("import_nonunique_identifier"), $this->getValueFromRecord($rec, $attributename)));
          }
        }
      }
      else if (is_object($attr) && method_exists($attr,"parseStringValue"))
      {
        $value = $attr->parseStringValue($value);
      }
      else
      {
        $value = trim($value);
      }
      return $value;
    }

    /**
     * Determines wether or not errors occurred and shows the analyze screen if errors occurred.
     * @param Array $importerrors An array with the errors that occurred
     * @param Array $extraerror   An extra error, if we found errors
     * @return bool Wether or not errors occurred
     */
    function showErrors($importerrors, $extraerror=null)
    {
      foreach ($importerrors as $importerror)
      {
        if (is_array($importerror) && !empty($importerror[0]))
        {
          $errorfound=true;
        }
      }
      if($errorfound)
      {
        if ($extraerror) $importerrors[0][] = $extraerror;
        $this->doAnalyze($this->m_postvars["fileid"],$importerrors);
        return true;
      }
    }

    /**
     * Adds the validated records but checks for errors first
     *
     * @param Array  $importerrors   Errors that occurred during validation of importfile
     * @param Array  $validatedrecs  Records that were validated
     */
    function addRecords(&$importerrors, &$validatedrecs)
    {
      $counter=0;
      foreach ($validatedrecs as $action=>$validrecs)
      {
        foreach ($validrecs as $validrec)
        {
          $counter++;
          atkdebug("Doing $action for record nr $counter");

          $this->$action($validrec);
          if (!empty($validrec['atkerror']))
          {
            foreach ($validrec['atkerror'] as $atkerror)
            {
              $importerrors[$counter][] = array("msg"=>"Fouten gedetecteerd op rij $counter: ",
              "spec"=>$atkerror['msg']);
            }
          }
          unset($validrec);
        }
        unset($validrecs);
      }

      unset($validatedrecs);
    }

    /**
     * Add a valid record to the db
     * @param Array $record   The record to add
     * @return bool Wether or not there were errors
     */
    function add(&$record)
    {
      $this->m_importNode->preAdd($record);

      if(isset($record['atkerror']))
      {
        return false;
      }

      $this->m_importNode->addDb($record);

      if(isset($record['atkerror']))
      {
        return false;
      }

      return true;
    }

    /**
     * Update a record in the db
     * @param Array $record    the record to update
     * @return bool Wether or not there were errors
     */
    function update(&$record)
    {
      $this->m_importNode->preUpdate($record);

      if(isset($record['atkerror']))
      {
        return false;
      }

      $this->m_importNode->updateDb($record);

      if(isset($record['atkerror']))
      {
        return false;
      }
      return true;
    }

    /**
     * Check whether the record if valide to import
     * @param array $record   the record
     * @return bool Wether or not there were errors
     */
    function validate(&$record)
    {
      if ($this->m_postvars['doupdate']) $mode = "update";
      else $mode = "add";

      $this->m_importNode->validate($record,$mode);

      foreach (array_keys($record) as $key)
      {
         $error = $error || (is_array($record[$key]) && array_key_exists('atkerror', $record[$key]) && count($record[$key]['atkerror']) > 0);
      }

      if(isset($error))
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
     * 
     * @param array $col_map        The use of the fields for the columns in the csv
     * @param array $initial_values The initial_values of the importnode
     * @return array          An array with errors, if there are any
     */
    function checkImport($col_map,$initial_values)
    {
      $errors = array();
      //get the unused obligatory fields
      $unused = array_values(array_diff($this->getObligatoryAttributes(),$col_map));

      $this->_returnErrors(array_values(array_diff($unused,array_keys($initial_values))),"import_error_fieldisobligatory","import_error_fieldsareobligatory",$errors);
      $this->_returnErrors($this->_getDuplicateColumns($col_map),"import_error_fieldusedtwice","import_error_fieldsusedtwice",$errors);

      return $errors;
    }

    /**
     * Checks if there are errors and if there are then it adds it to the collection
     * @param Array  $errors      The errors to check
     * @param String $singleerror The language code to use for a single error
     * @param String $doubleerror The language code to use for multiple errors
     * @param Array  &$collection The collection of errors thus far
     */
    function _returnErrors($errors, $singleerror,$doubleerror, &$collection)
    {
      if(count($errors) > 0)
      {
        $msg = atktext((count($errors)==1)?$singleerror:$doubleerror).": ";
        foreach ($errors as $key=>$field)
        {
          $attr = &$this->getUsableAttribute($field);
          $errors[$key] = $attr->label();
        }
        $collection[] = array("msg"=>$msg,"spec"=>implode(", ",$errors));
      }
    }

    /**
     * Array with columns
     * @param Array $array  The array the columns to check
     * @return Array The duplicate columns
     */
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

    /**
     * Checks the allfield for correct data
     * @param Array  $fields  The fields
     * @param Array  &$values The values of the fields
     * @return Array An array with an error message, if an error occurred
     */
    function checkAllFields($fields, &$values)
    {
      foreach ($fields as $field)
      {
        $attr = &$this->getUsableAttribute($field);
        if(!$attr)
        return;

        $record = array();
        $this->addToRecord($record,$field,$values[$field]);

        $result = $attr->display($record);

        if(!$result)
        if(in_array($field,$this->getObligatoryAttributes()))
        {
          return array('msg' => sprintf(atktext("import_error_allfieldnocorrectdata"),atktext($field,$this->m_node->m_module,$this->m_node->m_type),var_export($values[$field],1)));
        }
        else
        {
          $value = "";
        }
      }

      return;
    }

    /**
     * Validates a record
     * @param Array &$rec           The record to validate
     * @param Array &$validatedrecs The records thus far validated
     * @param Array &$importerrors  The errors so far in the import process
     * @param int   $counter        The number that the record is
     */
    function validateRecord(&$rec, &$validatedrecs, &$importerrors, $counter)
    {
      // Update variables
      $doupdate   =     $this->m_postvars['doupdate'];
      $updatekey1 =     $this->m_postvars['updatekey1'];
      $onfalseidentifier = $this->m_postvars['onfalseid'];
      $errors=array();
      if (!$this->validate($rec))
      {
        if ($rec['atkerror'][0])
        foreach ($rec['atkerror'] as $atkerror)
        {
          $errors[] = $atkerror;
        }
        foreach (array_keys($rec) as $key)
        {
          if (is_array($rec[$key]) && array_key_exists('atkerror', $rec[$key]) && count($rec[$key]['atkerror']) > 0)
          {
            foreach ($rec[$key]['atkerror'] as $atkerror)
            {
              $errors[] = $atkerror;
            }
          }
        }

        if ($errors[0])
        {
          foreach ($errors as $error)
          {
            $attr = &$this->getUsableAttribute($error['attrib_name']);

            $importerrors[$counter][] = array("msg"=>$error['msg'].": ",
                                    "spec"=>$attr->label());
          }
        }
      }

      if ($doupdate) $prepareres = $this->prepareUpdateRecord($rec);

      if (empty($importerrors[$counter][0]))
      {
        if ($prepareres==true)
        {
          $validatedrecs["update"][] = $rec;
        }
        else if (!$prepareres || $onfalseidentifier)
        {
          $validatedrecs["add"][] = $rec;
        }
        else
        {
          $importerrors[] = array("msg"=>atktext("error_formdataerror"),
                                  "spec"=>sprintf(atktext("import_nonunique_identifier"),
                                                  $this->getValueFromRecord($rec, $updatekey1)));
        }
      }
    }

    /**
     * Here we prepare our record for updating or return false,
     * indicating that we need to insert the record instead of updating it
     * @param Array &$record The record to prepare
     * @return bool If the record wasn't prepared we return false, otherwise true
     */
    function prepareUpdateRecord(&$record)
    {
      global $g_sessionManager;
      // The keys to update the record on
      $updatekey1 =     $this->m_postvars['updatekey1'];
      $updatekey1val =  $this->getValueFromRecord($record, $updatekey1);
      $allFields =       $g_sessionManager->pageVar("allFields");
      foreach ($allFields as $allField)
      {
        $allFieldsValues[$allField] =  $this->m_postvars[$allField];
      }

      $this->m_importNode->m_postvars["atksearchmode"] = "exact";
//      if (!in_array($allFieldValue)) $this->m_importNode->m_fuzzyFilters[] = $allFieldValue;

      $dbrec = $this->m_importNode->searchDb(array($updatekey1=>$updatekey1val));

      if (count($dbrec)==1)
      {
        $record[$this->m_importNode->primaryKeyField()] = $dbrec[0][$this->m_importNode->primaryKeyField()];
        $record['atkprimkey'] = $dbrec[0]['atkprimkey'];
        return true;
      }
      return false;
    }

    /**
     * Gets a raw value from a record with ATK values for a specific attribute
     * @param String $fieldname The name of the attribute to get the value for
     * @param Array $record     The record to search through
     * @return mixed The value
     */
    function getValueFromRecord($record, $fieldname)
    {
      $attr = &$this->getUsableAttribute($fieldname);
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

      if (is_object($attr))
        return $attr->value2db(array($fieldname=>$value));
      else
        return $value;
    }
  }
?>
