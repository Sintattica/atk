<?php

  /**
   * Handles errors by sending them to a specified email address
   *
   * Params used:
   * - mailto: The email address the errors will be mailed to
   *
   */
  class atkMailErrorHandler extends atkErrorHandlerBase
  {

    /**
     * Handle the error
     *
     * @param string $errorMessage
     * @param string $debugMessage
     */
    public function handle($errorMessage, $debugMessage)
    {
      global $g_sessionManager, $g_sessionData, $g_user, $g_modules;
      $txt_app_title = atktext("app_title");

      if ($this->params['mailto']!="") // only if enabled..
      {
        $subject = "[".$_SERVER["SERVER_NAME"]."] $txt_app_title error";

        $defaultfrom = sprintf("%s <%s@%s>", $txt_app_title, atkconfig("identifier", "atk"), $_SERVER["SERVER_NAME"]);
        $from = atkconfig("mail_sender", $defaultfrom);

        $body = "Hello,\n\nAn error seems to have occurred in the atk application named '$txt_app_title'.\n";
        $body.= "\nThe errormessage was:\n\n".implode("\n",is_array($errorMessage)?$errorMessage:array())."\n";
        $body.= "\nA detailed report follows:\n";
        $body.= "\nPHP Version: ".phpversion()."\n\n";

        $body.= "\nDEBUGMESSAGES\n".str_repeat("-",70)."\n";

        $lines = array();
        for($i=0, $_=count($debugMessage);$i<$_;$i++)
        {
          $lines[] = _wordwrap(atk_html_entity_decode(preg_replace('(\[<a.*</a>\])','',$debugMessage[$i])));
        }
        $body.= implode("\n",$lines);

        if (is_array($_GET))
        {
          $body.= "\n\n_GET\n".str_repeat("-",70)."\n";
          foreach ($_GET as $key=>$value)
          {
            $body.=_wordwrap($key.str_repeat(" ",max(1,20-strlen($key)))." = ".var_export($value,1))."\n";
          }
        }

        if (function_exists("getallheaders"))
        {
          $request = getallheaders();
          if (count($request)>0)
          {
            $body.= "\n\nREQUEST INFORMATION\n".str_repeat("-",70)."\n";
            foreach($request as $key=>$value)
            {
              $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".var_export($value,1))."\n";
            }
          }
        }

        if (is_array($_POST))
        {
          $body.= "\n\n_POST\n".str_repeat("-",70)."\n";
          foreach ($_POST as $key=>$value)
          {
            $body.=_wordwrap($key.str_repeat(" ",max(1,20-strlen($key)))." = ".var_export($value,1))."\n";
          }
        }

        if (is_array($_COOKIE))
        {
          $body.= "\n\n_COOKIE\n".str_repeat("-",70)."\n";
          foreach ($_COOKIE as $key=>$value)
          {
            $body.=_wordwrap($key.str_repeat(" ",max(1,20-strlen($key)))." = ".var_export($value,1))."\n";
          }
        }

        $body.= "\n\nATK CONFIGURATION\n".str_repeat("-",70)."\n";
        foreach ($GLOBALS as $key=>$value)
        {
          if (substr($key,0,7)=="config_")
          {
            $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".var_export($value,1))."\n";
          }
        }

        $body.= "\n\nMODULE CONFIGURATION\n".str_repeat("-",70)."\n";
        foreach ($g_modules as $modname => $modpath)
        {
          $modexists = file_exists($modpath) ? " (path exists)" : " (PATH DOES NOT EXIST!)";
          $body.=_wordwrap($modname.":".str_repeat(" ",max(1,20-strlen($modname))).var_export($modpath,1).$modexists)."\n";
        }

        $body.= "\n\nCurrent User:\n".str_repeat("-",70)."\n";
        if (is_array($g_user) && count($g_user))
        {
          foreach($g_user as $key => $value)
          {
            $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".var_export($value,1))."\n";
          }
        }
        else $body.= "Not known\n";

        if (is_object($g_sessionManager))
        {
          $body.= "\n\nATK SESSION\n".str_repeat("-",70);
          $body.= "\nNamespace: ".$g_sessionManager->getNameSpace()."\n";
          if (isset($g_sessionData[$g_sessionManager->getNameSpace()]["stack"]))
          {
            $stack = $g_sessionData[$g_sessionManager->getNameSpace()]["stack"];
            for($i=0;$i<count($stack);$i++)
            {
              $body.="\nStack level $i:\n";
              $item = isset($stack[$i]) ? $stack[$i] : null;
              if (is_array($item))
              {
                foreach ($item as $key=>$value)
                {
                  $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".var_export($value,1))."\n";
                }
              }
            }
          }
          if (isset($g_sessionData[$g_sessionManager->getNameSpace()]["globals"]))
          {
            $ns_globals = $g_sessionData[$g_sessionManager->getNameSpace()]["globals"];
            if(count($ns_globals)>0)
            {
              $body.="\nNamespace globals:\n";
              foreach ($ns_globals as $key=>$value)
              {
                $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".var_export($value,1))."\n";
              }
            }
          }
          if (isset($g_sessionData["globals"]))
          {
            $globals = $g_sessionData["globals"];
            if (count($globals)>0)
            {
              $body.="\nGlobals:\n";
              foreach ($globals as $key=>$value)
              {
                $body.=_wordwrap($key.str_repeat(" ",max(1,30-strlen($key)))." = ".var_export($value,1))."\n";
              }
            }
          }
        }

        $body.= "\n\nSERVER INFORMATION\n".str_repeat("-",70)."\n";

        foreach ($_SERVER as $key=>$value)
        {
          $body.=_wordwrap($key.str_repeat(" ",max(1,20-strlen($key)))." = ".var_export($value,1))."\n";
        }

        mail($this->params['mailto'],$subject,$body,"From: $from");
      }
    }
  }
