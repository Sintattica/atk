<?php
  /**
   * The atkMlAttribute class represents an multilanguage attribute of an atkNode.
   *
   * @author Peter Verhage (peter@ibuildings.nl)
   * @version $Revision$
   *
   * $Id$
   *
   */
  class atkMlAttribute extends atkAttribute
  {
    /**
     * Constructor
     * @param $name name of the attribute
     * @param $flags flags for the attribute
     */
    function atkMlAttribute($name, $flags = 0, $size = 0)
    {
      global $config_atkroot;
      /* base class constructor */
      $this->atkAttribute($name, ($flags|AF_MULTILANGUAGE|(AF_PRIMARY^AF_OBLIGATORY))^(AF_PRIMARY^AF_OBLIGATORY), $size);
    }

    /**
    * return possible languages
    */
    function getLanguages()
    {
      global $config_language;
      atkdebug("atkmlattribute::getlanguages() ".(is_array($config_language)?implode(",",$config_language):""));
      return $config_language;
    }
    
    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value (one box which can be used for all the languages).
     * @param $record array with values
     * @return piece of html code
     */
    function edit($record="")
    {
      /* register javascript */
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkmultilanguage.js.php");
      $page->register_submitscript('submitSave(form);');

      $languages = $this->getLanguages();
      
      /* hidden values */
      for ($i = 1; $i < sizeof($languages); $i++)
      {
        $result .= '<input type="hidden" name="'.$this->formName().'['.$languages[$i].']"'.
                   ' value="'.htmlspecialchars($record[$this->fieldName()][$languages[$i]]).'" >';
      }

      /* default language */
      $result .= text('language_'.$languages[0]).': '.
                 '<input type="text" name="'.$this->formName().'['.$languages[0].']"'.
                 ' value="'.htmlspecialchars($record[$this->fieldName()][$languages[0]]).'"'.
                 ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                 ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';

      
      /* first language of other languages */
      $result .= text((count($languages)==2 ? $languages[1] : "multilanguage_choice").'').': '.
                 '<input type="text" name="'.$this->formName().'['.(count($languages)==2 ? $languages[1] : "multilanguage").']"'.
                 ' value="'.htmlspecialchars($record[$this->fieldName()][$languages[1]]).'"'.
                 ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                 ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';

      return $result;
    }

    /**
     * Returns a piece of html code that can be used to search the attribute.
     * We will use the default language for this!
     * @param $record array with values
     * @return piece of htmlcode
     */
    function search($record="")
    {
      $languages = $this->getLanguages();
      $result= '<input type="text" name="atksearch['.$this->formName().'_'.$languages[0].']"'.
               ' value="'.$record[$this->fieldName().'_'.$languages[0]].'"'.
               ($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').
               ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';

      return $result;
    }

    /**
     * Check if the attribute is empty
     * @return true if it's empty
     */
    function isEmpty($postvars)
    {
      $languages = $this->getLanguages();
      for ($i=0,$_i=count($languages);$i<$_i;$i++)
      {
        if (strlen($postvars[$this->fieldName()][$languages[$i]]) == 0) return 1;
      }

      return 0;
    }

    /**
     * Returns a displayable string for this value.
     * We always display the first language only.
     * @param $record array with values
     * @return string ready for displaying
     */
    function display($record)
    {
      $languages = $this->getLanguages();
      return nl2br(htmlspecialchars($record[$this->fieldName()][$languages[0]]));
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="")
    {
      $languages = $this->getLanguages();
      /* hidden values */
      for ($i = 0; $i < sizeof($languages); $i++)
      {
        $result .= '<input type="hidden" name="'.$this->formName().'['.$languages[$i].']"'.
                   ' value="'.htmlspecialchars($record[$this->fieldName()][$languages[$i]]).'">';
      }
    
      return $result;             
    }

    /**
     * Removes slashes from the strings
     * @param $rec array with values
     * @return array with string without slashes
     */
    function db2value($rec)
    {
      $languages = $this->getLanguages();
      for ($i=0,$_i=count($languages);$i<$_i;$i++)
      {      
        $result[$languages[$i]] = stripslashes($rec[$this->fieldName().'_'.$languages[$i]]);
      }
      return $result;
    }

    /**
     * Fetch values
     * @param $rec array with values
     * @return array with values
     */
    function fetchValue($rec)
    {
      $languages = $this->getLanguages();
      for ($i=0,$_i=count($languages);$i<$_i;$i++)
      {
        $result[$languages[$i]] = stripslashes($rec[$this->fieldName()][$languages[$i]]);
      }
      return $result;
    }

    /**
     * Add's fields to query
     * @param $query the SQL query
     * @param $tablename tablename
     * @param $fieldaliasprefix field alias prefix
     * @param $level ?
     * @param $mode select/add/edit mode
     * @return new SQL query
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec, $level, $mode)
    {
      if(!$this->m_ownerInstance->hasFlag(NF_ML))
      {
        $languages = $this->getLanguages();
        if ($mode == "add" || $mode == "update")
        {
          for ($i=0,$_i=count($languages);$i<$_i;$i++)
          {
            $query->addField($this->fieldName().'_'.$languages[$i],escapeSQL($rec[$this->fieldName()][$languages[$i]]),"","",!$this->hasFlag(AF_NO_QUOTES));
          }   
        }
        else
        {
          for ($i=0,$_i=count($languages);$i<$_i;$i++)
          {
            $query->addField($this->fieldName().'_'.$languages[$i],"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
          }
        }
      }
    }

    /**
     * Fetch the metadata about this attrib from the table metadata
     */    
    function fetchMeta($metadata)    
    {
      $languages = $this->getLanguages();
      $attribname = $this->fieldName().'_'.$languages[0];
      $this->m_size = min($metadata[$attribname]['len'], $this->maxInputSize());
      $this->m_searchsize = min($metadata[$attribname]['len'], 20);
      $this->m_maxsize = $metadata[$attribname]['len'];                           
    }
  }
  
?>