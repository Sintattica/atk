<?php

  class atkDocxWriter extends atkOpenDocumentWriter 
  {
    function _parse($tpl_file, $tpl_vars = null)
    {
      return parent::_parse($tpl_file, $tpl_vars, $content_file="word/document.xml");
    }
  }
?>