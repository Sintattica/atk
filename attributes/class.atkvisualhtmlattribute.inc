<?php
  /**
   * The atkWysiwygAttribute class is an MS IE html edit control thingee
   *
   * @author Ivo Jansch (ivo@achievo.com)
   * @version 1.1
   *
   * $Id$
   * $Log$
   * Revision 4.1  2001/10/22 13:07:48  ivo
   * New initial visualhtml attribute. Needs quite a cleanup yet, and the table
   * features have been disabled because they don't seem to work.
   *
   */
   
  include_once($config_atkroot."atk/attributes/wysiwyg/property_accessor.php");
   
  class atkVisualHtmlAttribute extends atkTextAttribute
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function atkVisualHtmlAttribute($name, $flags=0)
    {
      $this->atkTextAttribute($name,$flags); // base class constructor
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a textarea
     */
    function edit($record="", $fieldprefix="")
    {
      global $config_atkroot, $g_layout;
      $g_layout->register_script("atk/javascript/class.atkvisualhtmlattribute.js");      
      $nom_editor = $this->fieldname()."_editor";
      $g_layout->register_submitscript($nom_editor."_save();");
      $idi_edit = 'eng';
      $editor_height = 100;
      $editor_width= 700;
      //$funcio_save= 'guardaho(codi);';
      //$action_submit = "sample_submit.php";
      $document_complet = 0;
      $content_inicial = $record[$this->fieldname()];
      $result = '<input type="hidden" name="'.$this->fieldname().'" value="">';
      include $config_atkroot."atk/attributes/wysiwyg/editor.php";
      return $result;
    }
    
    function display($record)
    {
      return $record[$this->fieldName()];
    }
        
  }
?>