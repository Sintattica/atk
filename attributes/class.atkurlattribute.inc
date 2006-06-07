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
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Add one space after each "/", "?" and "&" to fit a (long) url into <td></td>.
   */
  define('AF_URL_ALLOWWRAP', AF_SPECIFIC_1);

  /**
   * Don't display "http://". Link remains intact.
   */
  define('AF_URL_STRIPHTTP', AF_SPECIFIC_2);


  /**
   * The atkUrlAttribute class represents a field containing URLs.
   *
   * @author Przemek Piotrowski <przemek.piotrowski@nic.com.pl>
   *
   * @package atk
   * @subpackage attributes
   */
  class atkUrlAttribute extends atkAttribute
  {
  	var $m_newWindow = false;
  	var $m_allowWrap = false;
  	var $m_stripHttp = false;

  	function atkUrlAttribute($name, $flags = 0, $size=0)
  	{
  		if (AF_POPUP === ($flags & AF_POPUP)) {
  			$this->m_newWindow = true;
  			$flags &= (~ AF_POPUP);
  		}
  		if (AF_URL_ALLOWWRAP === ($flags & AF_URL_ALLOWWRAP)) {
  			$this->m_allowWrap = true;
  			$flags &= (~ AF_URL_ALLOWWRAP);
  		}
  		if (AF_URL_STRIPHTTP === ($flags & AF_URL_STRIPHTTP)) {
  			$this->m_stripHttp = true;
  			$flags &= (~ AF_URL_STRIPHTTP);
  		}

  		$this->atkAttribute($name, $flags, $size);
  	}

  	function display($record)
  	{
  		$output = '';

  		if (!empty($record[$this->fieldName()])) {
  			$url = $record[$this->fieldName()];

  			$target = '';
  			if (true === $this->m_newWindow) {
  				$target = ' target="_new"';
  			}

  			$text = $record[$this->fieldName()];
  			if (true === $this->m_stripHttp) {
  				$text = preg_replace('/^http:\/\//', '', $text);
  			}
  			if (true === $this->m_allowWrap) {
  				$text = preg_replace('/([^\/?])\/([^\/?])/', '\1/ \2', $text);
  				$text = preg_replace('/([?&].)/', ' \1', $text);
  			}

  			$output = '<a href="' . $url . '"' . $target . '">' . $text . '</a>';
  		}

  		return $output;
  	}
  }

?>