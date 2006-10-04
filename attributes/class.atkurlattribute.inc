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
   * Check if URL is a valid absolute URL
   */
  define('ABSOLUTE', 1);
  
  /**
   * Check if URL is a valid relative URL
   */  
  define('RELATIVE', 2);

  /**
   * Check if URL is a valid anchor
   */    
  define('ANCHOR',   4);

  
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
    var $m_accepts_url_flag = 0;
    
  	var $m_newWindow = false;
  	var $m_allowWrap = false;
  	var $m_stripHttp = false;

  	function atkUrlAttribute($name, $flags = 0, $size=0)
  	{
  		if (AF_POPUP === ($flags & AF_POPUP)) 
  		{
  			$this->m_newWindow = true;
  			$flags &= (~ AF_POPUP);
  		}
  		
  		if (AF_URL_ALLOWWRAP === ($flags & AF_URL_ALLOWWRAP)) 
  		{
  			$this->m_allowWrap = true;
  			$flags &= (~ AF_URL_ALLOWWRAP);
  		}
  		
  		if (AF_URL_STRIPHTTP === ($flags & AF_URL_STRIPHTTP)) 
  		{
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

  	function acceptUrls($accepts_flag)
  	{
      $this->m_accepts_url_flag = $accepts_flag;
  	}
  	
    /**
     * Validates absolute, relative and anchor URL through regular expression
  	 * 
     * @param array $record Record that contains value to be validated.
     *                      Errors are saved in this record, in the 'atkerror'
     *                      field.
     * @param String $mode Validation mode. Can be either "add" or "update"
     */
  	function validate(&$record, $mode)
  	{
  	  $result = true;
  	  
  	 /**
      * Validate URL, check if format is absolute (external URL's)
      * 
      * Example: http(s)://www2-dev.test_url.com
      */
  	  if (($this->m_accepts_url_flag & ABSOLUTE) == ABSOLUTE)
  	  {
  	    $local_result = preg_match("/^(ft|htt)ps?:\/\/[a-zA-Z0-9\.\-\_]+\.[a-zA-Z]{2,4}.*/Ui", $record[$this->fieldName()])?true:false;

    	  if ($local_result === false) triggerError($record, $this->fieldName(), 'invalid_absolute_url', atktext('invalid_absolute_url'));    	  
    	  $result = $result && $local_result;
  	  }  	 

  	 /**
      * Validate URL, check if format is relative
      * 
      * Example: /mysite/guestbook/index.html
      */  	  
  	  if (($this->m_accepts_url_flag & RELATIVE) == RELATIVE)
  	  {
  	    $local_result = preg_match("/^[a-zA-Z0-9\.\-\_\/?&=%]+$/Ui", $record[$this->fieldName()])?true:false;
  	    
  	    if ($local_result === false) triggerError($record, $this->fieldName(), 'invalid_relative_url', atktext('invalid_relative_url'));
  	    $result = $result && $local_result;
  	  }

  	 /**
      * Validate URL, check if format is a valid anchor
      * 
      * Example: #internal_bookmark
      */
  	  if (($this->m_accepts_url_flag & ANCHOR) == ANCHOR)
  	  {
  	    $local_result = preg_match("/^#[a-zA-Z0-9\.\-\_\/?&=%]+$/Ui", $record[$this->fieldName()])?true:false;
  	    
  	    if ($local_result === false) triggerError($record, $this->fieldName(), 'invalid_url_anchor', atktext('invalid_url_anchor'));
  	    $result = $result && $local_result;
  	  }
  	  
  	  return ($result||parent::validate($record, $mode));
  	}
  }

?>