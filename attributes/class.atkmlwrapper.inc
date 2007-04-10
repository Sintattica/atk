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
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Wrapper for turning any attribute into a multilanguage enabled 
   * attribute.
   *
   * This attribute implements the 'decorator' pattern. It takes an 
   * attribute as constructor parameter, and turns that attribute into
   * an internationalised attribute, compatible with the existing 
   * atkMl*Attributes.
   *
   * <i>Example usages</i>
   *
   * $this->add(new atkMlWrapper(new atkAttribute("name")));
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkMlWrapper extends atkAttribute
  {
    var $m_childList = array();
    
    function atkMlWrapper(&$attribute, $flags = 0)
    {
      // first highjack the containing attribute
      $this->atkAttribute($attribute->m_name, $flags|$attribute->m_flags);
      
      foreach($this->getLanguages() as $lng)
      {
        // Clone attrs with new name
        $attr = atkClone($attribute);
        
        $attr->m_name = $this->m_name."_".$lng;
        $this->m_childList[$lng] = $attr;
      }
    }

    /**
     * return possible languages
     */
    function getLanguages()
    {
      return atkconfig("supported_languages");
    }

    /**
     * Check if a record has an empty value for this attribute.
     * @param array $record The record that holds this attribute's value.
     * @return boolean
     */
    function isEmpty($record)
    {
      // @todo
      atkdebug("mlwrap isempty");
      return false;
    }

    function init()
    {
      // propagate ownerInstance to childs
      foreach (array_keys($this->m_childList) as $lng)
      {
        $attr = &$this->m_childList[$lng];
        $attr->m_ownerInstance = &$this->m_ownerInstance;
      }
    }

    /**
     * Converts a database value to an internal value.
     *
     * For the regular atkAttribute, this means stripping slashes.
     * Derived attributes may reimplement this for their own conversion.
     * (In which case, the return type might be 'mixed')
     *
     * This is the exact opposite of the value2db method.
     *
     * @param array $rec The database record that holds this attribute's value
     * @return mixed The internal value
     */
    function db2value($rec)
    {
      $value = array();
      foreach (array_keys($this->m_childList) as $lng)
      {
        $attr = &$this->m_childList[$lng];
        $value[$lng] = $attr->db2value($rec);
      }

      return $value;
    }

    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * For the regular atkAttribute, this means getting the field with the
     * same name as the attribute from the html posting.
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     * @return String The internal value
     */
    function fetchValue($postvars)
    {
      $value = array();
      foreach (array_keys($this->m_childList) as $lng)
      {
        $attr = &$this->m_childList[$lng];
        $value[$lng] = $attr->fetchValue($postvars);
      }

      return $value;
    }


    // Register a piece of script to execute upon language change
    function getMlSwitchCode()
    {
     // return "alert('hello world from ".$this->fieldName().":'+newlang)";
       return "hideAttrib('".$this->fieldName()."_'+oldlng); showAttrib('".$this->fieldName()."_'+newlng);";
    }


    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">)
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode with hidden form elements that post
     *                this attribute's value without showing it.
     */
    function hide($record="", $fieldprefix="")
    {
      $output = "";
      foreach (array_keys($this->m_childList) as $lng)
      {
        $attr = &$this->m_childList[$lng];
        $output.= $attr->hide($this->getRecordForLng($lng), $fieldprefix);
      }

      return $output;
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * The regular atkAttribute uses PHP's nl2br() and htmlspecialchars()
     * methods to prepare a value for display, unless $mode is "cvs".
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list"
     *                     for displaying in recordlists, "edit" for
     *                     displaying in editscreens, "add" for displaying in
     *                     add screens. "csv" for csv files. Applications can
     *                     use additional modes.
     * @return String HTML String
     */
    function display($record, $mode="")
    {
      // the next if-statement is a workaround for derived attributes which do
      $lng = $this->getDefaultLng();

      $rec = $this->getRecordForLng($record, $lng);
      return $this->m_childList[$lng]->display($rec);
    }

    function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
      $defaultlng = $this->getDefaultLng();
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/dhtml_formtools.js");
      $cnt = 0;
      foreach (array_keys($this->m_childList) as $lng)
      {
        $attr = &$this->m_childList[$lng];

        if ($lng==$defaultlng)
        {
          // default language
          $attr->setLabel($this->label());
        }
        else
        {
          // other languages
          $attr->setLabel($this->label()." (".atktext('language_'.strtolower($lng)).")");

          // All but the first other attrib need to be initially hidden.
          if ($cnt>1)
          {
            $page->register_loadscript("hideAttrib('".$attr->fieldName()."')");
          }
        }

        $rec = $this->getRecordForLng($defaults, $lng);
        $attr->addToEditArray($mode, $arr, $rec, $error, $fieldprefix);

        $cnt++;
      }
    }

    function getRecordForLng($masterrecord, $lng)
    {
      if (empty($masterrecord)) return array();
      return array($this->m_childList[$lng]->m_name=>$masterrecord[$this->fieldName()][$lng]);
    }

    function getDefaultLng()
    {
      $lngs = array_keys($this->m_childList);
      return $lngs[0]; // assume first is default
    }

    /**
     * Checks if a value is valid.
     *
     * The regular atkAttribute has no specific validation. Derived attributes
     * may override this method to perform custom validation.
     * Note that obligatory and unique fields are checked by the
     * atkNodeValidator, and not by the validate() method itself.
     *
     * @param array $record The record that holds the value for this
     *                      attribute. If an error occurs, the error will
     *                      be stored in the 'atkerror' field of the record.
     * @param String $mode The mode for which should be validated ("add" or
     *                     "update")
     */
    function validate(&$record, $mode)
    {
      atkdebug("mlwrap validate");
    }

    /**
     * Adds this attribute to database queries.
     *
     * Database queries (select, insert and update) are passed to this method
     * so the attribute can 'hook' itself into the query.
     *
     * Framework method. It should not be necessary to call this method
     * directly. Derived attributes that consist of more than a single simple
     * database field (like relations for example), may have to reimplement
     * this method.
     *
     * @param atkQuery $query The SQL query object
     * @param String $tablename The name of the table of this attribute
     * @param String $fieldaliasprefix Prefix to use in front of the alias
     *                                 in the query.
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
      foreach (array_keys($this->m_childList) as $lng)
      {
        $attr = &$this->m_childList[$lng];
        $newrec = $this->getRecordForLng($rec, $lng);
        $attr->addToQuery($query, $tablename, $fieldaliasprefix, $newrec, $level, $mode);
      }
    }


    /**
     * Fetch the metadata about this attrib from the table metadata, and
     * process it.
     *
     * Lengths for the edit and searchboxes, and maximum lengths are retrieved
     * from the table metadata by this method.
     *
     * @param array $metadata The table metadata from the table for this
     *                        attribute.
     */
    function fetchMeta($metadata)
    {
      foreach (array_keys($this->m_childList) as $lng)
      {
        $attr = &$this->m_childList[$lng];
        $attr->fetchMeta($metadata);
      }
    }



    /**
     * Return the database field type of the attribute.
     *
     * Note that the type returned is a 'generic' type. Each database
     * vendor might have his own types, therefor, the type should be
     * converted to a database specific type using $db->fieldType().
     *
     * If the type was read from the table metadata, that value will
     * be used. Else, the attribute will analyze its flags to guess
     * what type it should be. If AF_AUTO_INCREMENT is set, the field
     * is probaly "number". If not, it's probably "string".
     *
     * Note: Derived attributes should override this method if they
     *       use other field types than string or number. If the
     *       derived attribute is one that can not be stored in the
     *       database, an empty string should be returned.
     *
     * @return String The 'generic' type of the database field for this
     *                attribute.
     */
    function dbFieldType()
    {
      atkdebug("mlwrap dbfieldtype");
    }

    /**
     * Return the size of the field in the database.
     *
     * If 0 is returned, the size is unknown. In this case, the
     * return value should not be used to create table columns.
     *
     * Ofcourse, the size does not make sense for every field type.
     * So only interpret the result if a size has meaning for
     * the field type of this attribute. (For example, if the
     * database field is of type 'date', the size has no meaning)
     *
     * Note that derived attributes might set a dot separated size,
     * for example to store decimal numbers. The number after the dot
     * should be interpreted as the number of decimals.
     *
     * @return int The database field size
     */
    function dbFieldSize()
    {
      atkdebug("mlwrap dbfieldsize");
    }

    /*
     * Determine the storage type of this attribute.
     *
     * With this method, the attribute tells the framework whether it wants
     * to be stored in the main query (addToQuery) or whether the attribute
     * has its own store() implementation. The regular atkAttribute checks if
     * a store() method is present, and returns POSTSTORE in this case, or
     * ADDTOQUERY otherwise. Derived attributes may override this behavior.
     *
     * Framework method. It should not be necesary to call this method
     * directly.
     *
     * @param String $mode The type of storage ("add" or "update")
     *
     * @return int Bitmask containing information about storage requirements.
     *             Note that since it is a bitmask, multiple storage types
     *             could be returned at once.
     *             POSTSTORE  - store() method must be called, after the
     *                          master record is saved.
     *             PRESTORE   - store() must be called, before the master
     *                          record is saved.
     *             ADDTOQUERY - addtoquery() must be called, so the attribute
     *                          can nest itself in the master query.
     *             NOSTORE    - nor store(), nor addtoquery() should be
     *                          called (attribute can not be stored in the
     *                          database)
     */
    function storageType($mode)
    {
      return $this->m_childList[$this->getDefaultLng()]->storageType($mode);
    }

    /*
     * Determine the load type of this attribute.
     *
     * With this method, the attribute tells the framework whether it wants
     * to be loaded in the main query (addToQuery) or whether the attribute
     * has its own load() implementation. The regular atkAttribute checks if a
     * load() method is present, and returns POSTLOAD in this case, or
     * ADDTOQUERY otherwise. Derived attributes may override this behavior.
     *
     * Framework method. It should not be necesary to call this method
     * directly.
     *
     * @param String $mode The type of load (view,admin,edit etc)
     *
     * @return int Bitmask containing information about load requirements.
     *             Note that since it is a bitmask, multiple load types
     *             could be returned at once.
     *             POSTLOAD   - load() method must be called, after the
     *                          master record is loaded.
     *             PRELOAD    - load() must be called, before the master
     *                          record is loaded.
     *             ADDTOQUERY - addtoquery() must be called, so the attribute
     *                          can nest itself in the master query.
     *             NOLOAD     - nor load(), nor addtoquery() should be
     *                          called (attribute can not be loaded from the
     *                          database)
     */
    function loadType($mode, $searching=false)
    {
      return $this->m_childList[$this->getDefaultLng()]->loadType($mode, $searching);
    }

    function getOrderByStatement()
    {
      return $this->m_childList[$this->getDefaultLng()]->getOrderByStatement();
    }

    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      return $this->m_childList[$this->getDefaultLng()]->getSearchCondition($query, $table, $value, $searchmode);
    }


  }

?>