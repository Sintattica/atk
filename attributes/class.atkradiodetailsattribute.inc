<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage relations
 *
 * @copyright (c) 2000-2007 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 5798 $
 * $Id: class.atkmanytomanylistrelation.inc 5798 2008-03-05 13:41:26Z guido $
 */

/**
 * Displays radio buttons with options to choose from just like the 
 * atkRadioAttribute but allows you to use other attributes for detail
 * selections once certain options are selected.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage relations
 */
class atkRadioDetailsAttribute extends atkAttribute 
{
  /**
   * Options
   *
   * @var array
   */
  protected $m_options;
  
  /**
   * Details.
   * 
   * @var array
   */
  protected $m_details;
  
  /**
   * Constructor.
   * 
   * Options can be an array using the following format:
   * array('option_a', 'option_b' => 'option_b_details')
   * 
   * The options can either be specified as value (by not specifying a key)
   * or as key in which case you need to specify an attribute name as
   * value of another attribute which renders the detail selection for the 
   * given option.
   *
   * @param string $name    attribute name
   * @param string $options can either be an array of values or a key/value 
   *                        array in which case the key is used for the 
   *                        translation and value is the value which is saved
   *                        in the database
   * @param array $details  allows you to specify attributes that should be
   *                        used for the detail selection for certain options
   *                        the key should be the option value and the value
   *                        should be the attribute name
   * @param int $flags
   */
  public function __construct($name, $options, $details, $flags=0)
  {
    parent::__construct($name, $flags);

    $this->m_options = isset($options[0]) ? array_combine($options, $options) : $options;
    $this->m_details = $details;
  }
  
  /**
   * Hide attributes that are used for the details because we are 
   * going to render them inline.
   */
  public function postInit()
  {
    foreach (array_values($this->m_details) as $attrName)
    {
      if ($attrName != null)
      {
        $attr = $this->getOwnerInstance()->getAttribute($attrName);
        $attr->addDisabledMode(DISABLED_VIEW|DISABLED_EDIT);
      }
    }
  }
  
  /**
   * Edit.
   * 
   * @param array  $record
   * @param string $fieldprefix
   * @param string $mode
   */
  public function edit($record, $fieldprefix, $mode='add')
  {
    $this->getOwnerInstance()->getPage()->register_script(atkconfig('atkroot').'atk/javascript/class.'.strtolower(__CLASS__).'.js');
    
    $name = $fieldprefix.$this->fieldName();
    
    $result = '<div class="atkradiodetailsattribute-selection">';
    
    foreach ($this->m_options as $label => $value)
    {
      $isSelected = $record[$this->fieldName()] == $value;
      $checked = $isSelected ? ' checked="checked"' : '';
      $attrName = @$this->m_details[$value];

      if ($attrName != null) 
      {
        $url = partial_url($this->getOwnerInstance()->atkNodeType(), $mode, 'attribute.'.$this->fieldName().'.details', array('value' => $value, 'fieldprefix' => $fieldprefix));
        $onChange = "ATK.RadioDetailsAttribute.select(this, '{$url}');";
      }
      else 
      {
        $onChange = "ATK.RadioDetailsAttribute.select(this);";
      }
      
      $result .= '
        <input type="radio" class="atkradiodetailsattribute-option" name="'.$name.'" id="'.$name.'_'.$value.'" value="'.$value.'" onchange="'.$onChange.'"'.$checked.'/>
        <label for="'.$name.'_'.$value.'">'.$this->text($label).'</label><br/>
      ';
      
      if ($attrName != null)
      {
        $result .= 
          '<div id="'.$name.'_'.$value.'_details" class="atkradiodetailsattribute-details">'.
            ($isSelected ? '<blockquote>'.$this->getOwnerInstance()->getAttribute($attrName)->edit($record, $fieldprefix, $mode).'</blockquote>' : '').
          '</div>';
      }
    }
    
    $result .= '</div>';
    
    return $result;
  }
  
  /**
   * Partial details.
   */
  public function partial_details($mode)
  {
    $fieldprefix = $this->getOwnerInstance()->m_postvars['fieldprefix'];
    $value = $this->getOwnerInstance()->m_postvars['value'];
    
    $attrName = $this->m_details[$value];
    if ($attrName == null) return '';
    
    $attr = $this->getOwnerInstance()->getAttribute($attrName);
    if ($attr == null) return '';
    
    return '<blockquote>'.$attr->edit(array(), $fieldprefix, $mode).'</blockquote>';
  }
}