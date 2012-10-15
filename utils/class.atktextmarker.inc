<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage utils
   *
   * @copyright (c)2006 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6320 $
   * $Id$
   */

  /**
   * The atkTextMarker class
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   */
  class atkTextMarker
  {
    var $m_text = "";
    var $m_editedtext = "";
    var $m_cutpoints = array();

    /**
     * Constructor
     *
     * @param string $text
     * @return atkTextMarker
     */
    function atkTextMarker($text="")
    {
      $this->setText($text);
    }

    /**
     * Set the text
     *
     * @param string $text
     */
    function setText($text)
    {
      $this->m_editedtext = $this->m_text = $text;
    }

    /**
     * HIde a piece of the text
     *
     * @param int $position The position from where to start hiding text
     * @param int $length The number of characters to hide
     */
    function hide($position, $length)
    {
      $this->m_editedtext = substr($this->m_editedtext, 0, $position).substr($this->m_editedtext, $position+$length);
      $orgpos = $this->getOriginalPosition($position);
      $this->m_cutpoints[$orgpos] = $length;
    }

    /**
     * Get the text
     *
     * @return string The (edited) text
     */
    function getText()
    {
      return $this->m_editedtext;
    }

    /**
     * Get the original text
     *
     * @return string The original text
     */
    function getOriginalText()
    {
      return $this->m_text;
    }

    /**
     * Get original position
     *
     * @param int $position
     * @return int The original position
     */
    function getOriginalPosition($position)
    {
      $newval = $position;
      foreach($this->m_cutpoints as $pos=>$len)
      {
        if ($pos<=$position) $newval+=$len;
      }
      return $newval;
    }
  }

?>