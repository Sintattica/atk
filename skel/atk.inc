<?php
  /**
   * ATK Include file.. If you strip out stuff you don't need, you 
   * might have some performance improvement.
   * Somethings can't be stripped out.
   */
   
   // This variable indicates where to get all the includes. Usually, this is 
   // the current pc.
   if ($config_atkroot =="")  $config_atkroot = "./";      
  
  // Atknode is required. Don't remove it.
  include_once($config_atkroot."atk/class.atknode.inc");
 
  
  /** 
   * The attributes. All attributes that you don't use in your application 
   * can be stripped out. Some attributes are used rarely and are 
   * commented out by default. You can enable them if you want to use them.
   */
  include_once($config_atkroot."atk/attributes/class.atkboolattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atknumberattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atkemailattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atktextattribute.inc");  
  include_once($config_atkroot."atk/attributes/class.atklistattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atkparserattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atkdateattribute.inc");  
  include_once($config_atkroot."atk/attributes/class.atkdurationattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atktimeattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atkpasswordattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atkdummyattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atkprofileattribute.inc");

  include_once($config_atkroot."atk/attributes/class.atkcolorpickerattribute.inc"); 
  /** 
   * The relations. All relations that you don't use in your application 
   * can be stripped out.
   */
  include_once($config_atkroot."atk/relations/class.atkmanytoonerelation.inc");
  include_once($config_atkroot."atk/relations/class.atkmanytomanyrelation.inc");
  include_once($config_atkroot."atk/relations/class.atkmanyboolrelation.inc");
  include_once($config_atkroot."atk/relations/class.atkonetomanyrelation.inc");
  
?>