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
 * @copyright (c) 2000-2008 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 5798 $
 * $Id$
 */

/**
 * A fieldset can be used to combine multiple attributes to a single
 * attribute in edit/view mode.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage attributes
 */
class atkFieldSet extends atkAttribute 
{
  private $m_template;  
  private $m_parser;
  
  /**
   * Constructor.
   *
   * @param string $name     fieldset name
   * @param string $template template string
   * @param int    $flags    flags
   */
  public function __construct($name, $template, $flags=0)
  {
    parent::__construct($name, $flags|AF_NO_SORT);
    $this->setTemplate($template);
    $this->setLoadType(NOLOAD);
    $this->setStorageType(NOSTORE);
  }
  
  /**
   * Returns the fieldset template.
   *
   * @return string template string
   */
  public function getTemplate()
  {
    return $this->m_template;
  }
  
  /**
   * Sets the fieldset template. To include an attribute label use 
   * [attribute.label] to include an attribute edit/display field
   * use [attribute.field].
   *
   * @param string $template template string
   */
  public function setTemplate($template)
  {
    $this->m_template = $template;
    $this->m_parser = null;
  }
  
  /**
   * Returns the string parser instance for the fieldset template.
   *
   * @return atkStringParser
   */
  protected function getParser()
  {
    if ($this->m_parser == null)
    {
      atkimport('atk.utils.atkstringparser');
      $this->m_parser = new atkStringParser($this->getTemplate());
    }
    
    return $this->m_parser;
  }
  
  /**
   * Make sure we disable the normal rendering for attributes that
   * are part of this fieldset.
   */
  public function postInit()
  {
    $fields = $this->getParser()->getFields();
    foreach ($fields as $field)
    {
      list($attrName) = explode('.', $field);
      $attr = $this->getOwnerInstance()->getAttribute($attrName);
      $attr->addDisabledMode(DISABLED_VIEW|DISABLED_EDIT);
      $attr->setTabs($this->getTabs());
      $attr->setSections($this->getSections());
    }
  }  
  
  /**
   * Renders the fieldset.
   *
   * @param string $type        edit or display
   * @param array $record       record
   * @param string $mode        mode
   * @param string $fieldprefix fieldprefix
   * 
   * @return string rendered HTML
   */
  protected function renderFieldSet($type, $record, $mode, $fieldprefix='')
  {
    $replacements = array();
    
    $fields = array_unique($this->getParser()->getFields());
    foreach ($fields as $field)
    {
      @list($attrName, $part) = explode('.', $field);
      $attr = $this->getOwnerInstance()->getAttribute($attrName);

      switch ($part)
      {
        case 'label':
          $replacements[$attrName][$part] = $attr->getLabel($record, $mode);
          break;
          
        case 'field':
          if ($type == 'edit')
            $replacements[$attrName][$part] = $attr->getEdit($mode, $record, $fieldprefix);
          else if ($type == 'display')
            $replacements[$attrName][$part] = $attr->display($record, $mode);
          break;
      }
    }
    
    return $this->getParser()->parse($replacements);    
  }  
  
  /**
   * Edit fieldset.
   *
   * @param array  $record
   * @param string $fieldprefix
   * @param string $mode
   * 
   * @return string
   */
  public function edit($record, $fieldprefix='', $mode='')
  {
    return $this->renderFieldSet('edit', $record, $mode, $fieldprefix);
  }
  
  /**
   * Display fieldset.
   *
   * @param string $record
   * @param string $fieldprefix
   * 
   * @return string
   */
  public function display($record, $mode='')
  {
    return $this->renderFieldSet('display', $record, $mode);
  }  
}