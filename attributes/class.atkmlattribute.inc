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
   * The atkMlAttribute class represents an multilanguage attribute of an atkNode.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkMlAttribute extends atkAttribute
  {
    var $m_language = "";
    var $m_mlattribute = TRUE;
    var $m_edited = false;
    var $m_languages = array();

    /**
     * Constructor
     * @param string $name name of the attribute
     * @param int $flags flags for the attribute
     * @param int $size The size of the attribute
     */
    function atkMlAttribute($name, $flags = 0, $size = 0)
    {
      global $config_atkroot;
      /* base class constructor */
      $this->atkAttribute($name,$flags,$size);
      $this->m_language = strtoupper(atkconfig("defaultlanguage"));
    }

    /**
     * Get the value of this attribute for the given language
     *
     * @param array $rec The record
     * @param string $language The language
     * @return the value of this attribute for the given languager
     */
    function getValue($rec,$language="")
    {
      ($language=="")?$language=$this->m_language:$language=strtoupper($language);
      return $rec[$this->fieldName()][$language];
    }

    /**
     * Is the node an multilanguage node?
     *
     * @return boolean
     */
    function isMlNode()
    {
      return ($this->m_ownerInstance->hasFlag(NF_ML));
    }

    /**
     * Return the possible languages
     *
     * @return array with supported languages
     */
    function getLanguages()
    {
      return atkconfig("supported_languages");
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value (one box which can be used for all the languages).
     * @param array $record array with values
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return piece of html code
     */
    function edit($record="",$fieldprefix="",$mode="")
    {
      $this->m_edited = true;
      /* register javascript */
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkmultilanguage.js.php");

      $languages = $this->getLanguages();
      $this->m_languages = $languages; // cache.

      /* hidden values */
      for ($i = 1; $i < sizeof($languages); $i++)
      {
        $result .= '<input type="hidden" name="'.$fieldprefix.$this->formName().'['.$languages[$i].']"'.
                   ' value="'.htmlspecialchars($record[$this->fieldName()][$languages[$i]]).'" >';
      }

      /* default language */
      $result .= '<input type="text" name="'.$fieldprefix.$this->formName().'['.$languages[0].']"'.
                 ' value="'.htmlspecialchars($record[$this->fieldName()][$languages[0]]).'"'.
                 ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                 ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';

      return $result;
    }

    /**
     * Adds the attribute's edit / hide HTML code to the edit array.
     *
     * This method is called by the node if it wants the data needed to create
     * an edit form.
     *
     * @param String $mode     the edit mode ("add" or "edit")
     * @param array  $arr      pointer to the edit array
     * @param array  $defaults pointer to the default values array
     * @param array  $error    pointer to the error array
     * @param String $fieldprefix   the fieldprefix
     */
    function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
      parent::addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);

      if ($this->m_edited) // if we edited this attrib, we also need to add the
      {
        $key= array_search($this->m_ownerInstance->m_postvars['atkeditlng'],$this->m_languages);
        $curlng = $this->m_languages[($key!==false?$key:1)];
        $entry = array("name" => $this->m_name."_ml", "obligatory" => $this->hasFlag(AF_OBLIGATORY), "attribute" => &$this);

        /* label? */
        $entry["label"] = $this->label($defaults).' (<label id="'.$fieldprefix.$this->formName().'_label">'.atktext("language_".strtolower($curlng)).'</label>)';
        $entry["id"] = $this->getHtmlId($fieldprefix);
        $entry["tabs"] = $this->m_tabs;
        $entry["sections"] = $this->m_sections;

        /* first language of other languages */
        $entry["html"] = '<input type="text" name="'.$fieldprefix.$this->formName().'['.(count($this->m_languages)==2 ? $curlng : "multilanguage").']"'.
                   ' value="'.htmlspecialchars($defaults[$this->fieldName()][$curlng]).'"'.
                   ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                   ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';

        $arr["fields"][] = $entry;
        $this->m_edited = false;
      }
    }

    /**
     * Returns a piece of html code that can be used to search the attribute.
     * We will use the default language for this!
     * @param $record array with values
     * @param boolean $extended if set to false, a simple search input is
     *                          returned for use in the searchbar of the
     *                          recordlist. If set to true, a more extended
     *                          search may be returned for the 'extended'
     *                          search page. The atkAttribute does not
     *                          make a difference for $extended is true, but
     *                          derived attributes may reimplement this.
     * @param $fieldprefix The fieldprefix of this attribute's HTML element.
     * @return piece of htmlcode
     */
    function search($record="", $extended=false, $fieldprefix="")
    {
      $languages = $this->getLanguages();

      $id  =  $this->getSearchFieldName($prefix);
      $id .= !$this->isMlNode()?'_'.$languages[0]:'';

      $result= '<input type="text" name="'.$id.'"'.
               ' value="'.$record[$this->fieldName().(!$this->isMlNode()?'_'.$languages[0]:'')].'"'.
               ($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').
               ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';

      return $result;
    }

    /**
     * Check if the attribute is empty for all languages
     * @param array $record Current record
     * @return boolean true if it's empty
     */
    function isEmpty($record)
    {
      $languages = $this->getLanguages();
      for ($i=0,$_i=count($languages);$i<$_i;$i++)
      {
        if(isset($record[$this->fieldName()][$languages[$i]])) return 0;
      }

      return 1;
    }

    /**
     * Returns a displayable string for this value.
     * We always display the first language only.
     * @param $record array with values
     * @return string ready for displaying
     */
    function display($record)
    {
      if($this->isMlNode())
      {
        return nl2br(htmlspecialchars($record[$this->fieldName()]));
      }
      else
      {
        $languages = $this->getLanguages();
        return nl2br(htmlspecialchars($record[$this->fieldName()][$languages[0]]));
      }
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
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * @param array $rec The record that holds this attribute's value.
     * @return String The database compatible value
     */
    function value2db($rec)
    {
      if($this->isMlNode())
      {
        $rec[$this->fieldName()] = $rec[$this->fieldName()][$this->m_language];
      }
      return parent::value2db($rec);
    }

    /**
     * Removes slashes from the strings
     * @param $rec array with values
     * @return array with string without slashes
     */
    function db2value($rec)
    {
      $languages = $this->getLanguages();
      if($this->isMlNode())
      {
        if(is_array($rec[$this->fieldName()]))
        {
          foreach($rec[$this->fieldName()] as $lng=>$value)
          {
            $result[$lng]=stripslashes($value);
          }
        } else $result = stripslashes($rec[$this->fieldName()]);
      }
      else
      {
        for ($i=0,$_i=count($languages);$i<$_i;$i++)
        {
          $result[$languages[$i]] = stripslashes($rec[$this->fieldName().'_'.$languages[$i]]);
        }
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
      if($this->isMlNode())
      {
        $res = parent::fetchValue($rec);
        return $res;
      }
      else
      {
        $languages = $this->getLanguages();
        for ($i=0,$_i=count($languages);$i<$_i;$i++)
        {
          $result[$languages[$i]] = stripslashes($rec[$this->fieldName()][$languages[$i]]);
        }
        return $result;
      }
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
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec, $level, $mode)
    {
      $languages = $this->getLanguages();
      if(!$this->isMlNode())
      {
        if ($mode == "add" || $mode == "update")
        {
          for ($i=0,$_i=count($languages);$i<$_i;$i++)
          {
            $query->addField($this->fieldName().'_'.$languages[$i],$this->escapeSQL($rec[$this->fieldName()][$languages[$i]]),"","",!$this->hasFlag(AF_NO_QUOTES));
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
      else
      {
        // when this node has multilanguage support we save multiple records per occurance
        // we only add the default language to this query
        parent::addToQuery($query,$tablename,$fieldaliasprefix,$rec,$level,$mode);
      }
    }

    /**
     * Fetch the metadata about this attrib from the table metadata
     * 
     * @param array $metadata The table metadata from the table for this
     *                        attribute.
     */
    function fetchMeta($metadata)
    {
      $languages = $this->getLanguages();
      $attribname = $this->fieldName().(!$this->isMlNode()?'_'.$languages[0]:'');
      $this->m_size = min($metadata[$attribname]['len'], $this->maxInputSize());
      $this->m_searchsize = min($metadata[$attribname]['len'], 20);
      $this->m_maxsize = $metadata[$attribname]['len'];
    }

    /**
     * Creates a searchcondition for the field,
     * was once part of searchCondition, however,
     * searchcondition() also immediately adds the search condition.
     *
     * @param atkQuery $query     The query object where the search condition should be placed on
     * @param String $table       The name of the table in which this attribute
     *                              is stored
     * @param mixed $value        The value the user has entered in the searchbox
     * @param String $searchmode  The searchmode to use. This can be any one
     *                              of the supported modes, as returned by this
     *                              attribute's getSearchModes() method.
     * @return String The searchcondition to use.
     */
    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      $languages = $this->getLanguages();
      $func = $searchmode."Condition";
      if (method_exists($query,$func))
      {
        $searchcondition = $query->$func($table.".".$this->fieldName().(!$this->isMlNode()?'_'.$languages[0]:''),$this->escapeSQL($value));
      }
      else
      {
        atkdebug("Database doesn't support searchmode '$searchmode' for ".$this->fieldName().", ignoring condition.");
      }
      return $searchcondition;
    }
  }

?>
