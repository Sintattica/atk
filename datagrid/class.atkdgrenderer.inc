<?php
atkimport('atk.datagrid.atkdgcomponent');

class atkDGRenderer extends atkDGComponent
{  
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
  
  protected function renderForm($result)
  {
    if (!$this->getGrid()->isEmbedded()) 
    {
      $result = 
        '<form id="'.$this->getGrid()->getFormName().'" name="'.$this->getGrid()->getFormName().'">'.
          $result.
        '</form>';
    }
    
    return $result;
  }  
  
  protected function renderGrid()
  {
    $vars = array();
    
    foreach ($this->getGrid()->getComponents() as $name => $component)
    {
      $vars[$name] = $component->render();
    }

    return $this->getUi()->render($this->getGrid()->getTemplate(), $vars);
  }
  
  protected function registerScript() 
  {
    atkimport('atk.utils.atkjson');
    
    $name = atkJSON::encode($this->getGrid()->getName());
    $formName = atkJSON::encode($this->getGrid()->getFormName());
    $baseUrl = atkJSON::encode($this->getGrid()->getBaseUrl());
    
    $this->getPage()->register_script(atkconfig('atkroot').'atk/javascript/class.atkdatagrid.js');
    $this->getPage()->register_loadscript("
      ATK.DataGrid.register($name, $formName, $baseUrl);
    ");  
  }
  
  public function render()
  {
    $this->registerScript();
    $result = $this->renderGrid();
    $result = $this->renderForm($result);
    $result = $this->renderContainer($result);   
    return $result;
  }
}
?>