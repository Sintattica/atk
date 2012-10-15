<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk.utils
 *
 * @copyright (c) 2010 Peter C. Verhage
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * Allows to make some modifications to the add/edit. Depending on which
 * time these methods are called the modifications are made in PHP or outputted
 * as JavaScript.
 * 
 * This class is used by the attribute dependency mechanism and should *not* be 
 * used stand-alone.
 * 
 * @author Peter C. Verhage <peter@achievo.org>
 *
 * @package atk.utils
 */
class atkEditFormModifier
{
  /**
   * Node.
   * 
   * @var atkNode
   */
  private $m_node;
  
  /**
   * Record.
   * 
   * @var array
   */
  private $m_record;
  
  /**
   * Add/edit mode.
   * 
   * @var string
   */
  private $mode;
  
  /**
   * Field prefix.
   * 
   * @var string
   */
  private $fieldPrefix;
  
  /**
   * Initial setup/modification of the edit form, e.g. when the form is
   * rendered for the first time.
   * 
   * @var boolean
   */
  private $m_initial;
  
  /**
   * Constructor.
   * 
   * @param atkNode $node         node instance
   * @param array   $record       record
   * @param string  $fieldPrefix  field prefix
   * @param string  $mode         add/edit mode
   * @param string  $initial      initial form setup?
   */
  public function __construct(atkNode $node, &$record, $fieldPrefix, $mode, $initial)
  {
    $this->m_node = $node;
    $this->m_record = &$record;
    $this->m_fieldPrefix = $fieldPrefix;
    $this->m_mode = $mode;
    $this->m_initial = $initial;
  }
  
  /**
   * Returns the node instance.
   * 
   * @return atkNode node instance
   */
  public function getNode()
  {
    return $this->m_node;
  }
  
  /**
   * Returns a reference to the record. This means the record can be modified
   * which can be used to modify the record before refreshAttribute calls.
   * 
   * @return array record reference
   */
  public function &getRecord()
  {
    return $this->m_record;
  }    
  
  /**
   * Returns the form's field prefix.
   * 
   * @return string field prefix
   */
  public function getFieldPrefix()
  {
    return $this->m_fieldPrefix;
  }
  
  /**
   * Returns the mode (add or edit).
   * 
   * @return string mode (add or edit)
   */
  public function getMode()
  {
    return $this->m_mode;
  }
  
  /**
   * Is this the initial setup of the form (or are we updating the form from
   * an Ajax request)?
   * 
   * @return boolean initial form setup?
   */
  public function isInitial()
  {
    return $this->m_initial;
  }

  /**
   * Show the attribute row for the attribute with the given name.
   * 
   * @param string $name attribute name
   */
  public function showAttribute($name)
  {
    if ($this->isInitial())
    {
      $this->getNode()->getAttribute($name)->setInitialHidden(false);
    }
    else
    {
      $this->scriptCode("$('ar_".$this->getFieldPrefix().$name."').removeClassName('atkAttrRowHidden');");
    }
  }
  
  /**
   * Hide the attribute row for the attribute with the given name.
   * 
   * @param string $name attribute name
   */
  public function hideAttribute($name)
  {
    if ($this->isInitial())
    {
      $this->getNode()->getAttribute($name)->setInitialHidden(true);
    }
    else
    {
      $this->scriptCode("$('ar_".$this->getFieldPrefix().$name."').addClassName('atkAttrRowHidden');");
    }
  }
  
  /**
   * Re-render / refresh the attribute with the given name.
   * 
   * @param string $name attribute name
   */
  public function refreshAttribute($name)
  {
    if ($this->isInitial())
    {
      return;
    }

    atkimport('atk.utils.atkjson');
    
    $offset = count($this->getNode()->getPage()->getLoadScripts());
    
    $error = array();
    $editArray = array('fields' => array());
    $this->m_node->getAttribute($name)->addToEditArray($this->getMode(), $editArray, $this->getRecord(), $error, $this->getFieldPrefix());

    $scriptCode = '';
    foreach ($editArray['fields'] as $field)
    {
      $element = str_replace('.', '_', $this->getNode()->atkNodeType().'_'.$field['id']);
      $value = atkJSON::encode(atk_iconv(atkGetCharset(), "UTF-8", $field['html'])); // atkJSON::encode excepts string in UTF-8
      $scriptCode .= "\$('{$element}').update({$value});\n";
    }    
    
    $this->getNode()->getPage()->register_loadscript($scriptCode, $offset);
  }
  
  /**
   * Output JavaScript code.
   * 
   * Script is executed in the on-load.
   * 
   * @param string $code JavaScript code
   */
  public function scriptCode($code)
  {
    $this->getNode()->getPage()->register_loadscript($code);
  }
  
  /**
   * Register JavaScript file.
   * 
   * @param string $file JavaScript file
   */
  public function scriptFile($file)
  {
    $this->getNode()->getPage()->register_script($file);
  }  
}