<?php
  $config_atkroot = "./";  
  include_once("atk.inc");
  atksession();
  atksecure();

  $output='<html><head><title>'.text('app_title').'</title></head>';
  
  atkimport("atk.menu.atkmenu");
  atkimport("atk.utils.atkframeset");
  $menu = &atkMenu::getMenu();
  
  /* @var $menu atkmenuinterface */    
  
  $position = $menu->getPosition();
  $scrolling = ($menu->getScrollable()==MENU_SCROLLABLE?FRAME_SCROLL_AUTO:FRAME_SCROLL_NO);
  
  $topframe = &new atkFrame("70", "top", "top.php", FRAME_SCROLL_NO);
  $mainframe = &new atkFrame("*", "main", "welcome.php", FRAME_SCROLL_AUTO);
  $menuframe = &new atkFrame(($position==MENU_LEFT||$position==MENU_RIGHT?190:$menu->getHeight()), "menu", "menu.php", $scrolling);
  $noframes = '<body>
                 <p>Your browser doesnt support frames, but this is required to run '.text('app_title').'</p>
               </body>';
  
  $root = &new atkRootFrameset();
  if (atkconfig("top_frame"))
  {
    $outer = &new atkFrameSet("*", FRAMESET_VERTICAL, 0, $noframes);
    $outer->addChild($topframe);
    $root->addChild($outer);           
  }
  else
  {
    $outer = &$root;
    $outer->m_noframes = $noframes;
  }    
  
  $orientation = ($position==MENU_TOP||$position==MENU_BOTTOM?FRAMESET_VERTICAL:FRAMESET_HORIZONTAL);
  
  $wrapper = &new atkFrameSet("*", $orientation);  
  
  if($position==MENU_TOP||$position==MENU_LEFT)
  {    
    $wrapper->addChild($menuframe);
    $wrapper->addChild($mainframe);    
  }
  else
  {    
    $wrapper->addChild($mainframe);    
    $wrapper->addChild($menuframe);
  }      
    
  $outer->addChild($wrapper);

  echo $root->render();
?>
