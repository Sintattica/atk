<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * include
   */
  include_once(atkconfig("atkroot")."atk/atkbrowsertools.inc");
  atkimport("atk.attributes.atkattribute");

  /**
   * The atkTextAttribute class represents an attribute of a node
   * that is a big text field.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   * @todo autadjust needs to be modified as not every character == 1 column,
   *       perhaps forcing every textattribute to use a non-proportional font?
   */
  class atkTextAttribute extends atkAttribute
  {
    // number of rows of the edit box
    var $m_rows = 10;
    var $m_cols;
    var $m_autoadjust;
    
    private $m_wrapMode = 'soft';

    /**
     * Constructor
     * Note that for backwardscompatibility reasons, if you only pass 2
     * parameters and the second one is not an array, the attribute assumes
     * the second parameters is the $flags param, not the $size param.
     *
     * @param $name  Name of the attribute
     * @param $size  Can be an array with cols and rows key for size and
     *               an autoadjust value or just the rows size (in which case
     *               $flags is mandatory).
     * @param $flags Flags for this attribute
     */
    function atkTextAttribute($name,$size=0,$flags=0)
    {
      // compatiblity with old versions (old apps expect a 2 param call to be $name, $flags)
      if (func_num_args()==3 || is_array($size))
      {
         if(is_array($size))
         {
           if(isset($size['rows'])!='') $this->m_rows = $size['rows'];
           if(isset($size['cols'])!='') $this->m_cols = $size['cols'];
           if(isset($size['autoadjust'])) $this->m_autoadjust = $size['autoadjust'];
         }
         else $this->m_rows=$size;
      }
      else
      {
        $flags = $size;
      }

      $this->atkAttribute($name,$flags); // base class constructor
    }
    
    /**
     * Returns the current wrap mode.
     * 
     * @return string wrap mode
     */
    public function getWrapMode()
    {
      return $this->m_wrapMode;
    }
    
    /**
     * Sets the wrap mode for the text area.
     *
     * @param string $mode wrap mode ('soft', 'hard' or 'off')
     */
    public function setWrapMode($mode)
    {
      $this->m_wrapMode = $mode;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return piece of html code with a textarea
     */
    function edit($record="", $fieldprefix="", $mode='add')
    {
      // list mode, show a small textarea, until it get's focus
      // and is inflated to a big textarea
      if ($mode == 'list')
      {
        $id = $fieldprefix.$this->fieldName();

        $page = &$this->m_ownerInstance->getPage();

        // NOTE:
        // The onblur function uses a small timeout to make sure a click on a
        // new element is performed earlier then the swap of fields.
        // If we don't do this the click might end up wrong.
        $page->register_loadscript("
          \$('{$id}').onfocus = function() {
            \$('{$id}_textarea').value = this.value;
            Element.hide(this);
            Element.show('{$id}_textarea');
            \$('{$id}_textarea').focus();
          };

          \$('{$id}_textarea').onblur = function() {
            \$('{$id}').value = this.value;
            window.setTimeout(function() {
              Element.hide('{$id}_textarea');
              Element.show('{$id}'); }, 500
            );
          };
        ");

        $html =
          '<textarea id="'.$id.'" name="'.$id.'" wrap="soft" rows="1" cols="20">'."\n".htmlspecialchars($record[$this->fieldName()]).'</textarea>'.
          '<textarea id="'.$id.'_textarea" wrap="'.$this->getWrapMode().'" rows="5" cols="40" style="display: none"></textarea>';

        return $html;
      }


      (isset($record[$this->fieldName()]))?$text=$record[$this->fieldName()]:$text="";

      if ($this->m_cols != 0)
      {
        $cols = $this->m_cols;
      }
      else
      {
        $cols = $this->maxInputSize();
      }
      $rows = $this->m_rows;
      $id = $fieldprefix.$this->fieldName();
      $this->registerKeyListener($id, KB_CTRLCURSOR);
      if ($rows =="" || $rows == 0) $rows = 10;
      // watch out, $maxsize isn't supported for a textarea.

      if ($this->m_autoadjust)
      {
        $this->doAutoAdjust(htmlspecialchars($text), $rows, $cols);
      }

      $result= "<textarea id='$id' name='$id' wrap='".$this->getWrapMode()."' ";
      if ($rows) $result.= "rows='$rows' ";
      if ($cols) $result.= "cols='$cols' ";
      $result.=$this->getCSSClassAttribute();
      $result.=">\n".htmlspecialchars($text)."</textarea>";
      return $result;
    }

    /**
     * Adds this attribute to database queries.
     *
     * Database queries (select, insert and update) are passed to this method
     * so the attribute can 'hook' itself into the query.
     *
     * @param atkQuery $query The SQL query object
     * @param String $tablename The name of the table of this attribute
     * @param String $fieldaliasprefix Prefix to use in front of the alias
     *                                 in the query.
     * @param Array $rec The record that contains the value of this attribute.
     * @param int $level Recursion level if relations point to eachother, an
     *                   endless loop could occur if they keep loading
     *                   eachothers data. The $level is used to detect this
     *                   loop. If overriden in a derived class, any subcall to
     *                   an addToQuery method should pass the $level+1.
     * @param String $mode Indicates what kind of query is being processing:
     *                     This can be any action performed on a node (edit,
     *                     add, etc) Mind you that "add" and "update" are the
     *                     actions that store something in the database,
     *                     whereas the rest are probably select queries.
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode == "add" || $mode == "update")
      {
        $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES), $mode, $this->dbFieldType());
      }
      else
      {
        $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES), $mode, $this->dbFieldType());
      }
    }

    /**
     * Add's slashes to the string for the database
     * @param $rec Array with values
     * @return String with slashes
     */
    function value2db($rec)
    {
      $db = &$this->getDb();
      if ($db->getType() != "oci9" || $this->dbFieldType() != 'text')
        return $db->escapeSQL($rec[$this->fieldName()]);
      else
        return $rec[$this->fieldName()]; //CLOB in oci9 don't need quotes to be escaped EVIL HACK! THIS IS NOT ATKTEXTATTRIBUTE's PROBLEM!
    }

    /**
     * Removes slashes from the string
     * @param $rec Array with values
     * @return String without slashes
     */
    function db2value($rec)
    {
      if (isset($rec[$this->fieldName()]))
      {
        return $rec[$this->fieldName()];
      }
      return NULL;
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return String The 'generic' type of the database field for this
     *                attribute.
     */
    function dbFieldType()
    {
      // make sure our metadata is set
      if (is_object($this->m_ownerInstance)) $this->m_ownerInstance->setAttribSizes();

      if ($this->m_dbfieldtype=="")
      {
        return "text";
      }
      return $this->m_dbfieldtype;
    }

    /**
     * Fetch the metadata about this attrib from the table metadata, and
     * process it.
     *
     * @param array $metadata The table metadata from the table for this
     *                        attribute.
     */
    function fetchMeta($metadata)
    {
      $this->m_dbfieldtype = $metadata[$this->fieldName()]["gentype"];
    }

    /**
     * Parses the data that we are going to display in the textfield
     * and adjust rows to ensure that all the data is actually displayed
     * @param string $data	Data we want to display
     * @param int 	 $rows	Rows of the textarea
     * @param int		 $cols	Columns of the textarea
     */
    function doAutoAdjust($data, &$rows, &$cols)
    {
    	$browser = new browserInfo;
    	$maxlinechars = 0;
    	for ($counter = 0, $linecharacters=0, $rowsrequired=1;
    	     $counter<atk_strlen($data);
    	     $counter++, $linecharacters++)
    	{
    		// Current character we are parsing
    		$character = substr($data, $counter, 1);

    		// If we encounter a newline character or the number of characters
    		// equals the number of columns we have (with IE)...
    		if ($character == chr(13) || ($linecharacters==$cols && $browser->browser == "MSIE"))
    		{
    			if ($linecharacters>$maxlinechars) $maxlinechars = $linecharacters;
    			// We start another line
    			$linecharacters = 0;
    			// But need another row
    			$rowsrequired++;
    		}
    	}
    	// If we need more rows, we set them
  	  if ($rowsrequired>$rows) $rows= $rowsrequired;
  	  // IE wraps characters, other don't, so if we're not dealing with IE
  	  // we need more columns
  	  if ($maxlinechars>$cols && $browser->browser!=="MSIE") $cols = $maxlinechars;
    }
  }
?>
