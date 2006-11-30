<?php
  /**
   * Convertes a atkwizardaction key array to string value. 
   *
   * @author maurice <maurice@ibuildings.nl>
   * @package atk
   *
   */
  class AtkWizardActionLoader
  {
    function getWizardAction($wizardAction)
    {  
      if(is_array($wizardAction))
        return key($wizardAction);
      else 
        return $wizardAction;
    }
  }
?>