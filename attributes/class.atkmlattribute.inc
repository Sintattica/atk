<?php
  /**
   * The atkMlAttribute class represents an multilanguage attribute of an atkNode.
   *
   * @author Peter Verhage (peter@ibuildings.nl)
   *
   * $Id$
   * $Log$
   * Revision 4.1  2001/04/25 08:46:08  ivo
   * Big optimisations: using references for attribute manipulation.
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:09  ivo
   * Added atk4 to repository.
   *
   * Revision 1.2  2001/03/14 12:23:23  peter
   * added hide method, to reflect Ivo's changes
   *
   * Revision 1.1  2001/02/22 22:43:14  peter
   * initial release of the multilanguage attributes
   *
   */
  class atkMlAttribute extends atkAttribute
  {
    /**
     * Constructor
     * @param $name name of the attribute
     * @param $flags flags for the attribute
     */
    function atkMlAttribute($name, $flags = 0)
    {
      /* base class constructor */
      $this->atkAttribute($name, ($flags|AF_MULTILANGUAGE|(AF_PRIMARY^AF_OBLIGATORY))^(AF_PRIMARY^AF_OBLIGATORY));

      /* register javascript */
      $GLOBALS['g_layout']->register_script("atk/javascript/class.atkmultilanguage.js.php");
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value (one box which can be used for all the languages).
     * @param $record array with values
     * @return piece of html code
     */
    function edit($record="")
    {
      global $config_language, $g_layout;

      $this->m_size = min($g_layout->maxInputSize(), $this->m_tableMeta['len']);
      $this->m_maxsize = $this->m_tableMeta['len'];    

      /* hidden values */
      for ($i = 1; $i < sizeof($config_language); $i++)
      {
        $result .= '<input type="hidden" name="'.$this->formName().'['.$config_language[$i].']"'.
                   ' value="'.$record[$this->fieldName()][$config_language[$i]].'">';
      }

      /* default language */
      $result .= text('language_'.$config_language[0]).':<br>'.
                 '<input type="text" name="'.$this->formName().'['.$config_language[0].']"'.
                 ' value="'.$record[$this->fieldName()][$config_language[0]].'"'.
                 ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                 ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'><br><br>';

      /* first language of other languages */
      $result .= text('multilanguage_choice').':<br>'.
                 '<input type="text" name="'.$this->formName().'[multilanguage]"'.
                 ' value="'.$record[$this->fieldName()][$config_language[1]].'"'.
                 ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                 ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'><br><br>';

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
      global $config_language, $g_layout;
      
      $this->m_searchsize = min($g_layout->searchSize(), $this->m_tableMeta['len']);
      $this->m_maxsize = $this->m_tableMeta['len'];      

      $result= '<input type="text" name="atksearch['.$this->formName().'_'.$config_language[0].']"'.
               ' value="'.$record[$this->fieldName().'_'.$config_language[0]].'"'.
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
      global $config_language;

      reset($config_language);
      while (list($i, $language) = each($config_language))
        if (strlen($postvars[$this->fieldName()][$language]) == 0) return 1;

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
      global $config_language;
      return nl2br(htmlentities($record[$this->fieldName()][$config_language[0]]));
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="")
    {
      global $config_language;    
    
      /* hidden values */
      for ($i = 0; $i < sizeof($config_language); $i++)
      {
        $result .= '<input type="hidden" name="'.$this->formName().'['.$config_language[$i].']"'.
                   ' value="'.$record[$this->fieldName()][$config_language[$i]].'">';
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
      global $config_language;

      reset($config_language);
      while (list($i, $language) = each($config_language))
        $result[$language] = stripslashes($rec[$this->fieldName().'_'.$language]);

      return $result;
    }

    /**
     * Fetch values
     * @param $rec array with values
     * @return array with values
     */
    function fetchValue($rec)
    {
      global $config_language;

      reset($config_language);
      while (list($i, $language) = each($config_language))
        $result[$language] = stripslashes($rec[$this->fieldName()][$language]);

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
      global $config_language;

      if ($mode == "select")
      {
        reset($config_language);
        while (list($i, $language) = each($config_language))
          $query->addField($this->fieldName().'_'.$language,"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
      }

      else if ($mode == "add" || $mode == "edit")
      {
        reset($config_language);
        while (list($i, $language) = each($config_language))
          $query->addField($this->fieldName().'_'.$language,addslashes($rec[$this->fieldName()][$language]),"","",!$this->hasFlag(AF_NO_QUOTES));
      }
    }

    /**
     * Fetch the metadata about this attrib from the table metadata
     */    
    function fetchMeta($metadata)    
    {
      $this->m_tableMeta = $metadata[$this->fieldName().'_'.$config_language[0]];
    }
  }
  
  include "atk/attributes/class.atkmltextattribute.inc";
  include "atk/attributes/class.atkmlnumberattribute.inc";
  include "atk/attributes/class.atkmlhtmlattribute.inc";
?>
