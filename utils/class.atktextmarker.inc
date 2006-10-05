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
   * @version $Revision$
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

    function atkTextMarker($text="")
    {
      $this->setText($text);
    }

    function setText($text)
    {
      $this->m_editedtext = $this->m_text = $text;

    }

    function hide($position, $length)
    {
      $this->m_editedtext = substr($this->m_editedtext, 0, $position).substr($this->m_editedtext, $position+$length);
      $orgpos = $this->getOriginalPosition($position);
      $this->m_cutpoints[$orgpos] = $length;
    }

    function getText()
    {
      return $this->m_editedtext;
    }

    function getOriginalText()
    {
      return $this->m_text;
    }

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