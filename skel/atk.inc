<?php
  /**
   * ATK Include file.. If you strip out stuff you don't need, you 
   * might have some performance improvement.
   * Some things can't be stripped out.
   */
   
  // This variable indicates where atk is located. Usually, this is
  // the current working directory.
  $config_atkroot = "./";      

  // The session manager is required. Don't remove it.
  include_once($config_atkroot."atk/session/class.atksessionmanager.inc");
  // Atknode is required. Don't remove it.
  include_once($config_atkroot."atk/class.atknode.inc");
 
 
  // The treenode. Only enable this if you use a treeview in your 
  // application
  // include_once($config_atkroot."atk/class.atktreenode.inc");
  
  
  /** 
   * The attributes. All attributes that you don't use in your application 
   * can be stripped out. Some attributes are used rarely and are 
   * commented out by default. You can enable them if you want to use them.
   */
  include_once($config_atkroot."atk/attributes/class.atkradioattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atkboolattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atkemailattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atknumberattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atktextattribute.inc");  
  // include_once($config_atkroot."atk/attributes/class.atkhtmlattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atklistattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atkparserattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atkdateattribute.inc");  
  // include_once($config_atkroot."atk/attributes/class.atkfileattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atktimeattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atkdurationattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atkmultiplefileattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atkpasswordattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atkdummyattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atkmlselectorattribute.inc");  
  // include_once($config_atkroot."atk/attributes/class.atkfilewriterattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atkprofileattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atkmlattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atkmltextattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atkmlnumberattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atkmlhtmlattribute.inc");
  // include_once($config_atkroot."atk/attributes/class.atktoolbar.inc");
  
  /** 
   * The relations. All relations that you don't use in your application 
   * can be stripped out.
   */
  include_once($config_atkroot."atk/relations/class.atkmanytoonerelation.inc");  
  // include_once($config_atkroot."atk/relations/class.atkmanyboolrelation.inc");
  include_once($config_atkroot."atk/relations/class.atkonetomanyrelation.inc");
  // include_once($config_atkroot."atk/relations/class.atkmatrixrelation.inc");
  // include_once($config_atkroot."atk/relations/class.atkonetoonerelation.inc");
  
?>
