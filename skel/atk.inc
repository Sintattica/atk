<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * This file is the skeleton main include file, which you can copy
   * to your application dir and modify if necessary. By default, it 
   * includes the atknode and sets the $config_atkroot. Any other
   * application global includes should be added to this file.
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
   * @internal This variable indicates where to get all the includes. Usually,
   *           this is the current pc.
   */
  if ($config_atkroot =="")  $config_atkroot = "./";      
  
  include_once($config_atkroot."atk/include/initial.inc");
  
?>
