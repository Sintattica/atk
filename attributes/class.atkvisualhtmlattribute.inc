<?php
  /**
   * The atkVisualHtmlAttribute class is an MS IE html edit control thingee
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @version $Revision$
   *
   * $Id$   
   *
   */
   
  include_once($config_atkroot."atk/attributes/wysiwyg/property_accessor.php");
  include_once($config_atkroot."atk/atkbrowsertools.inc");
  include_once($config_atkroot."atk/attributes/class.atktextattribute.inc");
  
  /**
   * These defines control the behaviour of the visual html control. You can 
   * pass these defines as second parameter to the atkVisualHtmlAttribute.
   * Use a | to specify multiple values (like flags).
   */
  define("WW_ALL"      ,   0); 
  define("WW_STYLE"    ,   1); // style options (bold, italic, underline)
  define("WW_FONT_FACE",   2); // font face (helvetica, arial, etc)
  define("WW_FONT_SIZE",   4); // font size
  define("WW_FONT"     ,WW_FONT_SIZE|WW_FONT_FACE); // all font options (font face, font size)
  define("WW_TOOLS"    ,   8); // cut, copy, paste, undo, redo and find options
  define("WW_LINK"     ,  16); // link button
  //define("WW_IMAGE"    ,  32); // image button (TODO: IMAGE EDITOR IS CURRENTLY BROKEN!!!!!)
  //define("WW_TABLE"    ,  64); // table editor (TODO: TABLE EDITOR IS CURRENTLY BROKEN!!!!!!!!)
  define("WW_COLOR"    , 128); // text background and foreground color
  define("WW_ALIGN"    , 256); // text alignment (left, right, center)
  define("WW_LIST"     , 512); // numbered and bulleted lists
  define("WW_INDENT"   ,1024); // Indentation buttons
  define("WW_CLEAR"    ,2048); // clear document button

  
  // predefine for a 'simple' visual editor that only supports basic options
  // that don't allow a user to crap a sites layout.
  define("WW_LIGHT", WW_STYLE|WW_TOOLS|WW_LIST|WW_LINK);
   
  class atkVisualHtmlAttribute extends atkTextAttribute
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $editflags These define what buttons the toolbar contains.
     * @param $flags Flags for this attribute
     */
    function atkVisualHtmlAttribute($name, $editflags=WW_ALL, $flags=0)
    {
      $this->m_editflags = $editflags;
      $this->atkTextAttribute($name,12,$flags); // base class constructor
    }
    
    function editmode($flag)
    {
      // first check special WW_ALL flag, which is zero, but results
      // in true anyway.
      if ($this->m_editflags==WW_ALL) return true;
      return (($this->m_editflags & $flag) == $flag);
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
      global $atkwwview, $PHP_SELF;
      
      //var_dump($record);
                                                            
      if (!ieExtensions())
      {
        return atkTextAttribute::edit($record, $fieldprefix);
      }
      else
      {    
        $wwview = $atkwwview[$fieldprefix.$this->fieldName()];
        if ($wwview=="")
	{
	  $wwview = atkconfig("ww_defaultview");
	}
        $viewselectbox = '<select name="atkwwview['.$fieldprefix.$this->fieldName().']">
                            <option value="visual">'.text("ww_visualmode").
                            '<option value="plain" '.($wwview=="plain"?"selected":"").'>'.text("ww_sourcemode").
                         '</select>';
      
        if ($wwview!="plain")
        {
          $g_layout->register_script($config_atkroot."atk/javascript/class.atkvisualhtmlattribute.js");      
          $nom_editor = $fieldprefix.$this->fieldname()."_editor";
          $g_layout->register_submitscript($nom_editor."_save();");
          $idi_edit = 'eng';
          $editor_height = 200;
          $editor_width= 550;
          //$funcio_save= 'guardaho(codi);';
          //$action_submit = "sample_submit.php";
          $document_complet = 0;
          $current_value = $record[$this->fieldname()];
          $result = '<input type="hidden" name="'.$fieldprefix.$this->fieldname().'" value="">';         
          include $config_atkroot."atk/attributes/wysiwyg/editor.php";                    
        }
        else
        {          
          $result.=$viewselectbox.'<br>';
          $result.=atkTextAttribute::edit($record, $fieldprefix);          
        }
        return $result;
      }
    }
    
    function display($record)
    {
      return $record[$this->fieldName()];
    }
        
  }
?>
