<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage utils
 *
 * @copyright (c) 2000-2007 Ibuildings.nl BV
 * 
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 */

/**
 * The grid renderer is responsible for rendering the grid components and 
 * ofcourse the grid itself.
 * 
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage datagrid
 */
class atkDGRenderer extends atkDGComponent
{  
  /**
   * Surrounds the grid by a container if we are rendering the grid for the
   * first time (e.g. if this is not an update of the grid contents).
   *
   * @param string $result grid HTML
   * 
   * @return string grid HTML
   */
  protected function renderContainer($result)
  {
    if (!$this->getGrid()->isUpdate())
    {
      $result = 
        '<div id="'.$this->getGrid()->getName().'_container">'.
          $result.
        '</div>';
    }
    
    return $result;
  }
  
  /**
   * Surrounds the grid by a form if needed.
   *
   * @param string $result grid HTML
   * 
   * @return sting grid HTML 
   */
  protected function renderForm($result)
  {
    if (!$this->getGrid()->isEmbedded()) 
    {
      $result = 
        '<form id="'.$this->getGrid()->getFormName().'" name="'.$this->getGrid()->getFormName().'" method="post" action="'.atkSelf().'">'.
          session_form().
          $result.
        '</form>';
    }
    
    return $result;
  }  
  
  /**
   * Render the grid components and the grid itself.
   *
   * @return string grid HTML
   */
  protected function renderGrid()
  {
    $vars = array();
    
    foreach ($this->getGrid()->getComponents() as $name => $info)
    {
      $vars[$name] = atknew($info['class'], $this->getGrid(), $info['options'])->render();
    }

    return $this->getUi()->render($this->getGrid()->getTemplate(), $vars);
  }
  
  /**
   * Register JavaScript code for the grid.
   */
  protected function registerScript() 
  {
    atkimport('atk.utils.atkjson');
    
    $name = atkJSON::encode($this->getGrid()->getName());
    $baseUrl = atkJSON::encode($this->getGrid()->getBaseUrl());
    $embedded = $this->getGrid()->isEmbedded() ? 'true' : 'false';
    
    $this->getPage()->register_script(atkconfig('atkroot').'atk/javascript/class.atkdatagrid.js');
    $this->getPage()->register_loadscript("
      ATK.DataGrid.register($name, $baseUrl, $embedded);
    ");  
  }
  
  /**
   * Render the grid.
   *
   * @return string grid HTML
   */
  public function render()
  {
    $this->registerScript();
    $result = $this->renderGrid();
    $result = $this->renderContainer($result);       
    $result = $this->renderForm($result);
    return $result;
  }
}