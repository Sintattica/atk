<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * This file is the skeleton index file, which you can copy to your
   * application dir and modify if necessary. By default, it checks
   * the setting of $config_fullscreen, and if set, launches the
   * app in a full screen window. If not set, the frameset is loaded.
   *
   * @package atk
   * @subpackage skel
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal includes
   */
  $config_atkroot = "./";
  include_once("atk.inc");
  atksession();
  atksecure();

  $theme = &atkinstance('atk.ui.atktheme');
  if ($theme->getAttribute('useframes',true))
  {
    if (atkconfig("fullscreen"))
    {
      // Fullscreen mode. Use index.php as launcher, and launch app.php fullscreen.
      atksession();
      atksecure();

      $page = &atknew("atk.ui.atkpage");
      $ui = &atkinstance("atk.ui.atkui");
      $theme = &atkTheme::getInstance();
      $output = &atkOutput::getInstance();

      $page->register_style($theme->stylePath("style.css"));
      $page->register_script(atkconfig("atkroot")."atk/javascript/launcher.js");

      $content = '<script language="javascript">atkLaunchApp(); </script>';
      $content.= '<br><br><a href="#" onClick="atkLaunchApp()">'.text('app_reopen').'</a> &nbsp; '.
      '<a href="#" onClick="window.close()">'.text('app_close').'</a><br><br>';

      $box = $ui->renderBox(array("title"=>text("app_launcher"),
      "content"=>$content));

      $page->addContent($box);
      $output->output($page->render(text('app_launcher'), true));

      $output->outputFlush();
    }
    else
    {
      // Regular mode. app.php can be included directly.
      include "app.php";
    }
  }
  else 
  {
    $indexpage = new atkIndexPage();
    $indexpage->generate();
  }

  class atkIndexPage
  {    
    /**
     * @var atkPage
     */
    var $m_page;
    
    /**
     * @var atkTheme
     */
    var $m_theme;
    
    /**
     * @var atkUi
     */
    var $m_ui;
    
    /**
     * @var atkOutput
     */
    var $m_output;
    
    function atkIndexPage()
    {
      $this->m_page =   &atkinstance("atk.ui.atkpage");
      $this->m_ui =     &atkinstance("atk.ui.atkui");
      $this->m_theme =  &atkinstance('atk.ui.atktheme');
      $this->m_output = &atkinstance('atk.ui.atkoutput');
    }
    
    function generate()
    {
      $this->atkGenerateTop();
      $this->atkGenerateMenu();
      $this->atkGenerateDispatcher();

      $this->m_output->output($this->m_page->render(atktext('app_title')), true);
      $this->m_output->outputFlush();
    }
    
    function atkGenerateMenu()
    {
      /* general menu stuff */
      /* load menu layout */
      atkimport("atk.menu.atkmenu");
      $menu = &atkMenu::getMenu();

      if (is_object($menu)) $this->m_page->addContent($menu->getMenu($this->m_page));
      else atkerror("no menu object created!");;
    }

    function atkGenerateTop()
    {
      $this->m_page->register_style($this->m_theme->stylePath("style.css"));
      $this->m_page->register_stylecode("form{display: inline;}");
      $this->m_page->register_style($this->m_theme->stylePath("top.css"));

      //Backwards compatible $content, that is what will render when the box.tpl is used instead of a top.tpl
      $loggedin = text("logged_in_as", "", "atk").": <b>".($g_user["name"]?$g_user['name']:'administrator')."</b>";
      $content = '<br />'.$loggedin.' &nbsp; <a href="index.php?atklogout=1">'.ucfirst(atktext("logout")).' </a>&nbsp;<br /><br />';

      $top = $this->m_ui->renderBox(array("content"=> $content,
      "logintext" => atktext("logged_in_as"),
      "logouttext" => ucfirst(text("logout", "", "atk")),
      "logoutlink" => "index.php?atklogout=1",
      "logouttarget"=>"_top",
      "centerpiece"=>"",
      "searchpiece"=>"",
      "title" => atktext("app_title"),
      "user"   => $g_user["name"]),
      "top");

      $this->m_page->addContent($top);
    }

    function atkGenerateDispatcher()
    {
      global $ATK_VARS;
      $session = &atkSessionManager::getSession();

      if($session["login"]!=1)
      {
        // no nodetype passed, or session expired
        $this->m_page->register_style($this->m_theme->stylePath("style.css"));

        $destination = "";
        if(isset($ATK_VARS["atknodetype"]) && isset($ATK_VARS["atkaction"]))
        {
          $destination = "&atknodetype=".$ATK_VARS["atknodetype"]."&atkaction=".$ATK_VARS["atkaction"];
          if (isset($ATK_VARS["atkselector"])) $destination.="&atkselector=".$ATK_VARS["atkselector"];
        }

        $box = $this->m_ui->renderBox(array("title"=>text("title_session_expired"),
        "content"=>'<br><br>'.text("explain_session_expired").'<br><br><br><br>
                                           <a href="index.php?atklogout=true'.$destination.'" target="_top">'.text("relogin").'<a/><br><br>'));

        $this->m_page->addContent($box);

        $this->m_output->output($this->m_page->render(text("title_session_expired"), true));
      }
      else
      {
        $lockType = atkconfig("lock_type");
        if (!empty($lockType)) atklock();

        // Create node
        if ($ATK_VARS['atknodetype'])
        {
          $obj = &getNode($ATK_VARS['atknodetype']);

          if (is_object($obj))
          {
            $obj->loadDispatch($ATK_VARS);
          }
          else
          {
            atkdebug("No object created!!?!");
          }
        }
        else
        {
          $this->m_page->register_style($this->m_theme->stylePath("style.css"));
          $box = $this->m_ui->renderBox(array("title"=>text("app_shorttitle"),
          "content"=>"<br /><br />".text("app_description")."<br /><br />"));

          $this->m_page->addContent($box);
        }
      }
    }
  }
  
?>
