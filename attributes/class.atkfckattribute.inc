<?php
// fv v. 0.1.1
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /** 
   * @internal include baseclass.
   */    
  include_once($config_atkroot."atk/attributes/class.atktextattribute.inc"); 

  /**
   * The atkFckAttribute is a 'wysiwyg' text editor.
   *
   * It is a wrapper for the FckEditor from http://www.fckeditor.net. It is
   * feature-rich and works in most popular browsers. The fckeditor is 
   * included in the ATK distribution, so this attribute should work out of
   * the box.
   *
   * @author Fabrizio Vanzani <fabrizio@vanzani.it>
   * @author Ivo Jansch       <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkFckAttribute extends atkTextAttribute
  {
    var $m_editorname = "";
    var $m_urlpath = "";
    var $fck_opt=array(
                 'ToolbarSet'=>'Default',
                 'Width'=>'100%',
                 'Height'=>'300px');        
	
    /** 
     * Constructor
     * @param String $name Name of the attribute
     * @param int $flags Flags for the attribute
     * @param array $opt Extra fckeditor configuration options.
     */
    function atkFckAttribute($name, $size='', $flags=0,$opt=array())
    {	
		  $config_fck=atkconfig('fck');
		  if(is_array($config_fck)) 
		  {
		    $this->fck_opt=array_merge($this->fck_opt,$config_fck);
		  }
		  if(is_array($opt)) 	  
		  {
			  $this->fck_opt=array_merge($this->fck_opt,$opt);
		  }
      
		  // $this->fck_opt['Language']=atkconfig("language"); <-- this doesn't work for some reason
      //$this->m_editflags = $editflags;
      $this->atkTextAttribute($name,$size,$flags); 
    }


    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a textarea
     */
    function edit($record="", $fieldprefix="")
    {
      global $config_atkroot;      
                        
      include_once($config_atkroot."atk/attributes/fck/fckeditor.php");
      return $this->editFck($record, $fieldprefix);
    }

    function editFck($record="", $fieldprefix="")
    {      		
		  $id = $fieldprefix.$this->fieldName();
		  $this->registerKeyListener($id, KB_CTRLCURSOR);  
		  if ($rows =="" || $rows == 0) $rows = 10;
	    // watch out, $maxsize isn't supported for a textarea.
	    	    		
		  $oFCKeditor = new FCKeditor($fieldprefix.$this->fieldName()) ;		
		  $oFCKeditor->BasePath	= atkconfig("atkroot")."atk/attributes/fck/";
		
      $oFCKeditor->Value = $record[$this->fieldName()];
      $oFCKeditor->ToolbarSet=$this->fck_opt['ToolbarSet'];
      $oFCKeditor->Width=$this->fck_opt['Width'];
      $oFCKeditor->Height=$this->fck_opt['Height'];
      $oFCKeditor->Language=$this->fck_opt['Langugae'];
		
		  return $oFCKeditor->CreateHtml();	
    }
        
  }

?>
