<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * Utility class for the wysiwyg attribute. Based on code found on the
   * internet somewhere, origin unknown. 
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Method to access property
   * @access private
   */
  function PropAcce_string($situ,$nomeditor,$property,$value)
  {
  	global $navegador;
  	$pa_str = "";
  	if( !strcmp($situ,"Get") ) {
  		if( !strcmp($navegador,"IE") ){
  			$pa_str = $nomeditor . "." . $property ;
  		}
  		else
  		{
  			$pa_str = "PropertyAccessor.Get(".$nomeditor.",\"".$property."\");";
  		}
  	}
  	if( !strcmp($situ,"Set") ) {
  		if( !strcmp($navegador,"IE") ){
  			$pa_str = $nomeditor . "." . $property . " = " . $value ;
  		}
  		else
  		{
  			$pa_str = "PropertyAccessor.Set(".$nomeditor.",\"".$property."\",".$value.");";
  		}
  	}
  	return $pa_str;
}
?>