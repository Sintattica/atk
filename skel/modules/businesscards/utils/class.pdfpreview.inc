<?php

  /**
   * The node class for import.
   */
  class pdfpreview extends atkNode
  {
    function pdfpreview()
    {
      $this->atkNode("pdfpreview");
      $this->m_securityMap["preview"] = "selectfile";
      $this->m_securityMap["fillfields"] = "selectfile";
    }
    
    function action_selectfile(&$handler)
    {
      global $g_layout;
      
      $postvars = pf_getPostVar();
      $g_layout->ui_top(text("select_file"));
      $g_layout->output($this->statusbar());
      $g_layout->register_script(atkconfig("platformroot")."atk/javascript/formsubmit.js");
      $g_layout->register_script(atkconfig("platformroot")."atk/javascript/formfocus.js");
      
      $output .= '<form name="entryform" action="dispatch.php?" enctype="multipart/form-data" method="post" onsubmit="return globalSubmit(this)">
                    <input type="hidden" name="atklevel" value="'.$postvars["atklevel"].'">
                    <input type="hidden" name="atkprevlevel" value="'.$postvars["atkprevlevel"].'">
                    <input type="hidden" name="atkstackid" value="'.$postvars["atkstackid"].'">
                    <input type="hidden" name="atkaction" value="fillfields">
                    <input type="hidden" name="atknodetype" value="densito.pdfpreview">';
      $output .= $g_layout->ret_table_simple();
      $output .= "<tr align='center'><td class='table'>".text("pdffile").": ";
      $output .= "</td><td class='table'><input type=\"file\" name=\"pdffile\"></td></tr>";
      $output .= "</table>";
      $output .= '<br><input type="submit" value="'.text("next").'"></form>';
      
      $g_layout->output($output);
      $g_layout->output('<br>');
      $g_layout->ui_bottom();
    }
    
    function action_fillfields(&$handler)
    {
      global $g_layout, $HTTP_POST_FILES, $g_user;
      $postvars = $HTTP_POST_FILES;

      $g_layout->ui_top(text("select_file"));
      $g_layout->output($this->statusbar());
      $g_layout->register_script(atkconfig("platformroot")."atk/javascript/formsubmit.js");
      $g_layout->register_script(atkconfig("platformroot")."atk/javascript/formfocus.js");
      
      $output .= '<form name="entryform" action="dispatch.php?" target="_new" method="get" onsubmit="return globalSubmit(this)">
                    <input type="hidden" name="atklevel" value="'.$postvars["atklevel"].'">
                    <input type="hidden" name="atkprevlevel" value="'.$postvars["atkprevlevel"].'">
                    <input type="hidden" name="atkstackid" value="'.$postvars["atkstackid"].'">
                    <input type="hidden" name="atkaction" value="preview">
                    <input type="hidden" name="atknodetype" value="densito.pdfpreview">
      				      <input type="hidden" name="atkfilter" value="'.$postvars["atkfilter"].'">';
      $output .= $g_layout->ret_table_simple();
      
      $filename = atkconfig("platformroot")."../uploaded_files/preview_".$g_user["userid"].".pdf";
      copy($postvars["pdffile"]["tmp_name"], $filename);
      
      include_once(moduleDir("densito")."class.pdf.inc");
      if (isset($postvars["pdffile"]["tmp_name"]) && $postvars["pdffile"]["tmp_name"] != "") 
      {
        $pdf = new PDF($postvars["pdffile"]["tmp_name"]);
        $pdfcontents = file_get_contents($postvars["pdffile"]["tmp_name"]);
        $fields = $pdf->getFields();
        $pdf->close();
      }

      // CHECK THIS! $value is saved in sub-array
      foreach ($fields as $name => $value)
      {
        $output .= '<tr align="center"><td class="table">'.$name.': ';
        $output .= '</td><td class="table"><input type="text" name="fields['.$name.']" value="'.$value['defaultvalue'].'"></td></tr>';
      }
      $output .= "</table>";
      $output .= "<input type='hidden' name='pdffile' value='".$filename."'>";
      $output .= '<br><input type="submit" value="'.text("preview").'"></form>';
      
      $g_layout->output($output);
      $g_layout->output('<br>');
      $g_layout->ui_bottom();
    }
    
    function action_preview()
    {
      set_time_limit(0);
      $postvars = pf_getPostVar();
      atk_var_dump($postvars);
      
      include_once(moduleDir("densito")."class.pdf.inc");
      if (isset($postvars["pdffile"]) && $postvars["pdffile"] != "")
      {
        $pdf = new PDF($postvars["pdffile"]);
  
        $pdf->loadFonts(atkconfig("pdffont_dir"));
        $pdf->exportToBrowserAndClose($postvars["fields"]);
      }
      die;
    }
  }

?>