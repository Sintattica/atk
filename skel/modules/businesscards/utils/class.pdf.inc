<?php

class PDF
{
  var $m_file;
  
  var $m_pdf;
  var $m_container;
  var $m_page;

  /**
   * Constructor
   *
   * @param string $file
   * @return PDF
   */
  function __construct($file=null)
  {
    if (is_null($file))
      return;
      
    $this->m_file = $file;
    $this->m_pdf = pdf_new();
    
    if (pdf_begin_document($this->m_pdf, "", "") == 0)
    {
      $this->handleError();
    }  
    
    if ($file != "") $this->m_container = pdf_open_pdi_document($this->m_pdf, $file, "");
    if ($this->m_container == 0)
    {
      $this->handleError();
    }  

    $this->loadPages();
  }
  
  /**
   * Load all pages of PDF template in m_page array
   */
  private function loadPages()
  {
    $nof_pages = PDF_pcos_get_number($this->m_pdf, $this->m_container, "length:pages");    
    if ($nof_pages == 0) $this->handleError(); // no pages found    

    for ($pagenr = 1 ; $pagenr <= $nof_pages ; $pagenr++)
    {
      $result = pdf_open_pdi_page($this->m_pdf, $this->m_container, $pagenr, "");
      
      if ($result != 0) $this->m_page[$pagenr] = $result;
      else              $this->handleError();
    }
    
    atkdebug("PDF::loadPages() | ".count($this->m_page)." page(s) in PDF template");
  }

  private function handleError()
  {
    atkerror("PDF Error: ".pdf_get_errmsg($this->m_pdf));  
  }
  
  /**
   * Read fieldname, -description and -subtype from PDF Template
   *
   * @return array $result
   */
  public function getFields()
  {
    $result = array();
    
    foreach ($this->m_page as $pagenr => $page_handle)
    {
      $nof_blocks = PDF_pcos_get_number($this->m_pdf, $this->m_container, "length:pages[".($pagenr-1)."]/blocks");
      
      for ($i = 0; $i < $nof_blocks; $i++)
      {
        $blockname = PDF_pcos_get_string($this->m_pdf, $this->m_container, "pages[".($pagenr-1)."]/blocks[$i]/Name");
        $desc      = PDF_pcos_get_string($this->m_pdf, $this->m_container, "pages[".($pagenr-1)."]/blocks[$i]/Description");
        $subtype   = PDF_pcos_get_string($this->m_pdf, $this->m_container, "pages[".($pagenr-1)."]/blocks[$i]/Subtype");
        $def_value = $this->getBlockProperty($subtype, "pages[".($pagenr-1)."]/blocks/$blockname/defaulttext");
        $multiline = $this->getBlockProperty($subtype, "pages[".($pagenr-1)."]/blocks/$blockname/textflow", 'boolean');
  
        $result[$blockname]['page_nr']         = $pagenr;
        $result[$blockname]['field_nr']        = $i; // maybe not used?
        $result[$blockname]['field_multiline'] = $multiline;
        $result[$blockname]['field_desc']      = $desc;
        $result[$blockname]['subtype']         = $subtype;
        $result[$blockname]['defaultvalue']    = $def_value;
      }
    }
        
    return $result;
  }

  /**
   * Use this function to retrieve block-properties
   * 
   * Checks if subtype is Text (and not Image or PDF)
   * Returns found value
   *
   * @param string $subtype Text, Image or PDF
   * @param string $querypath
   * @param string $returntype used when result is empty; 'boolean' will be returned as '0', 'string' as ""
   * @return string $value
   */
  private function getBlockProperty($subtype, $querypath, $returntype='string')
  {
    $value = "";
    
    if ($subtype == "Text")
    {
      $type = PDF_pcos_get_string($this->m_pdf, $this->m_container, "type:{$querypath}"); // Beware: 'null' when field is empty 
      
      switch ($type)
      {
        case 'string':
          $value = PDF_pcos_get_string($this->m_pdf, $this->m_container, $querypath);
          break;
        
        case 'boolean':
          $value = PDF_pcos_get_number($this->m_pdf, $this->m_container, $querypath);
          break;
      }
    }    
    
    if (empty($value) && $returntype == 'boolean') $value = 0;
    atkdebug("PDF::getBlockProperty for $querypath | returnvalue = $value (Blocktype=$subtype | fieldtype=$type)</h2>");
    
    return $value;
  }

  
  
  public function loadFonts($path)
  {
    foreach (glob($path."/*.{pfb,PFB}", GLOB_BRACE) as $pfbFile)
    {
      $content = file_get_contents("$pfbFile");
      preg_match('/FontName[ ]+\/([^ ]+) def/', $content, $matches);
      $fontName = $matches[1];
      list($filePrefix) = explode('.', basename($pfbFile));
      $this->loadFont($fontName, $path."/".$filePrefix);
    }
    foreach (glob($path."/*.{ttf}", GLOB_BRACE) as $pfbFile)
    {
      list($filePrefix) = explode('.', basename($pfbFile));
      $this->loadFont($filePrefix, $path."/".$filePrefix, "TTF");
    }
  }
  
  private function loadFont($name, $file, $type="PFB")
  {
    if ($type == "PFB")
    {
      pdf_set_parameter($this->m_pdf, "FontPFM", "$name=$file.pfm");
      pdf_set_parameter($this->m_pdf, "FontOutline", "$name=$file.pfb");  
    }
    else 
    {  
      pdf_set_parameter($this->m_pdf, "FontOutline", "$name=$file.ttf");    
    }
  }
  
  public function close()
  {
    pdf_delete($this->m_pdf);
  }
  
  private function exportAndClose($fields=array())
  {
    pdf_set_parameter($this->m_pdf, "hypertextencoding", "winansi");
    pdf_set_info($this->m_pdf, "Creator", "Ibuildings PDFlib Export");
   
    pdf_begin_page_ext($this->m_pdf, 20, 20, ""); // dummy page size
    
    foreach ($this->m_page as $pagenr => $page_handle)
    {
      pdf_fit_pdi_page($this->m_pdf, $page_handle, 0, 0, "adjustpage"); // adjust size to container
  
      // fill fields
      foreach ($fields as $name => $value)
        if (pdf_fill_textblock($this->m_pdf, $page_handle, $name, $value, "embedding encoding=winansi") == 0)
          $this->handleError();
  
      pdf_end_page_ext($this->m_pdf, "");
  
      pdf_close_pdi_page($this->m_pdf, $page_handle);
    }

    pdf_end_document($this->m_pdf, "");
    pdf_close_pdi($this->m_pdf, $this->m_container);

    $data = pdf_get_buffer($this->m_pdf);
    $this->close();
    return $data;
  }
  
  public function exportToFileAndClose($file, $fields=array())
  {
    $data = $this->exportAndClose($fields);
    $fp = fopen($file, "w+");
    fwrite($fp, $data);
    fclose($fp);
  }
  
  public function exportToBrowserAndClose($fields=array())
  {
    $data = $this->exportAndClose($fields);
    $length = strlen($data);
    
    header("Content-type: application/pdf");
    header("Content-Length: $length");
    header("Content-Disposition: inline; filename=".basename($this->m_file));  
    print $data;
    exit();
  }
}

?>