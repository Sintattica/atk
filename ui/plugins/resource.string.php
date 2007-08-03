<?php

/**
 * This file is part of the Ibuildings E-business Platform.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be 
 * included in the distribution.
 *
 * @package atk
 * @subpackage ui
 *
 * @author Martin Roest <martin@ibuildings.nl>
 *
 * @copyright (c)2004 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */
 
  function smarty_resource_string_source($tpl_name, &$tpl_source, &$smarty)
  {    
    $tpl_source = $tpl_name;    
    return true;
  }
  
  /**
  * @todo make this useful
  */
  function smarty_resource_string_timestamp($tpl_name, &$tpl_timestamp, &$smarty)
  {
    $tpl_timestamp = 1000;
    return true;
  }

  /**
  * @todo make this useful
  */
  function smarty_resource_string_secure($tpl_name, &$smarty)
  {
    // assume all templates are secure
    return true;
  }
  
  /**
  * @todo make this useful
  */
  function smarty_resource_string_trusted($tpl_name, &$smarty)
  {
    // not used for templates
    return true;
  }
  
?>
