<?php
  /**
   * The atkVisualHtmlAttribute class is an MS IE html edit control thingee
   *
   * @author Ivo Jansch <ivo@achievo.com>
   * @version $Revision$
   *
   * $Id$   
   *
   */
   
  include_once($config_atkroot."atk/attributes/wysiwyg/property_accessor.php");
  include_once($config_atkroot."atk/atkbrowsertools.inc");
   
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
      global $config_atkroot, $g_layout, $config_enable_ie_extensions;
      
      $browser = getBrowserInfo();
                              
															
      if ($browser["family"]!="ie5up"||$config_enable_ie_extensions==false)
      {
        return atkTextAttribute::edit($record, $fieldprefix);
      }
      else
      {    
        $g_layout->register_script($config_atkroot."atk/javascript/class.atkvisualhtmlattribute.js");      
        $nom_editor = $fieldprefix.$this->fieldname()."_editor";
        $g_layout->register_submitscript($nom_editor."_save();");
        $idi_edit = 'eng';
        $editor_height = 100;
        $editor_width= 700;
        //$funcio_save= 'guardaho(codi);';
        //$action_submit = "sample_submit.php";
        $document_complet = 0;
        $content_inicial = $record[$this->fieldname()];
        $result = '<input type="hidden" name="'.$fieldprefix.$this->fieldname().'" value="">';
        include $config_atkroot."atk/attributes/wysiwyg/editor.php";
        return $result;
      }
    }
    
    function display($record)
    {
      return $record[$this->fieldName()];
    }
        
  }
?>