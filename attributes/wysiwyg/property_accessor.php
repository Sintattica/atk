<?php
// Fem funció contextual que varia el codi del PropertyAccessor en funció de si es Explorer o Netscape: retorna string amb PropertyAccessor.Get(...) si és Netscape o bé directament Object.property si és Explorer:
function PropAcce_string($situ,$nomeditor,$property,$value){
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