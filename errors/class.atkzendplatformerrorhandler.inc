<?php

  /**
   * Handles errors by sending them to Zend Platform
   * 
   * Params used: 
   * - none
   *
   * @author Harrie Verveer
   * @author Mark Wittens
   */
  class atkZendPlatformErrorHandler extends atkErrorHandlerBase
  {   
    /**
     * Handle the error
     *
     * @param string $errorMessage
     * @param string $debugMessage
     */
    public function handle($errorMessage, $debugMessage)
    {
      if ($this->zendPlatformAvailable())
      {
        // log in zend platform
        $errMsg = implode(' | ', is_array($errorMessage) ? $errorMessage : array());
        if ($errMsg == '')
          $errMsg = 'Something went terribly wrong, but there is no errormessage set...';
        else
          $errMsg = preg_replace('/\[\+.*s\]/','', $errMsg); // get rid of timestamps because they will prevent ZP from finding duplicate errors
        
        monitor_custom_event(atktext("app_title"), $errMsg, true);
      }
    }

    /**
     * Check if Zend Platform is available and good to go
     *
     * @return boolean
     */
    protected function zendPlatformAvailable()
    {
      if (!function_exists('accelerator_license_info'))
      {
        atkdebug('Zend Platform was not detected');
        return false;
      }
     
      if (!function_exists('accelerator_get_configuration'))
      {
        $licenseInfo = accelerator_license_info();
        atkdebug('The Zend Platform extension is not loaded correctly: '.$licenseInfo['failure_reason']);
        return false;
      }
      
      if(!function_exists('monitor_custom_event'))
      {
        atkdebug('Zend Platform seems to be there, but the function \'monitor_custom_event\' could not be found');
        return false;
      }
      
      return true;
    }
  }
