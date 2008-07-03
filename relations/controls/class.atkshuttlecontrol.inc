<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage relations
   *
   * @copyright (c)2007 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Abstract baseclass for controls for the shuttle
   * @author Tjeerd Bijlsma <tjeerd@ibuildings.nl>
   * @package atk
   * @subpackage relations
   */
  abstract class atkShuttleControl
  {
    const AVAILABLE = 'available';
    const SELECTED = 'selected';

    protected $m_name;
    protected $m_shuttle;
    protected $m_section;

    public function __construct($name)
    {
      $this->m_name = $name;
    }
    
    public function getShuttle()
    {
      return $this->m_shuttle;
    }

    public function setShuttle($shuttle)
    {
      $this->m_shuttle = $shuttle;
    }

    public function setSection($section)
    {
      $this->m_section = $section;
    }

    public function init()
    {
    }

    public function getName()
    {
      return $this->m_name;
    }

    protected function getValue($record)
    {
      if (isset($record[$this->m_shuttle->fieldName()]['controls'][$this->getName()]))
      {
        return $record[$this->m_shuttle->fieldName()]['controls'][$this->getName()];
      }
      else
      {
        return null;
      }
    }

    public function getFormName($prefix)
    {
      return $prefix.$this->m_shuttle->fieldName().'[controls]['.$this->getName().']';
    }

    /**
     * Called if a filter or selection event has occured. And allows the control to
     * state if it needs to be refreshed based on the filter or selection changes.
     *
     * @param string $type  type of event ('filter' or 'selection'
     * @param array $record full record (see partial_filter, partial_selection for more information)
     *
     * @return boolean needs refresh?
     */
    public function needsRefresh($type, $record)
    {
      return false;
    }

    /**
     * Renders the control. Returns a piece of HTML that is used in the shuttle
     * to represent this control. If this control has input elements then the
     * getFormName method can be used to retrieve the base name for the input
     * elements. The getValue method can be used to retrieve this controls value(s)
     * for the given record.
     *
     * @param array $record full record
     * @param string $mode add/edit mode
     * @param string $prefix field prefix
     *
     * @return string HTML string
     */
    public abstract function render($record, $mode, $prefix);
    
    /**
     * Text proxy. Forwards translations to the shuttle attribute.
     *
     * @param string $string
     * @return string
     */
    public function text($string)
    {
      return $this->m_shuttle->text($string);
    }
  }