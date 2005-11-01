<?php
  /**
   * atkDocumentWriter class file
   *
   * @package atk
   * @subpackage document
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * General DocumentWriter framework class. Should be extended to support specific file formats.
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk
   * @subpackage document
   */
  class atkDocumentWriter
  {

    /**
     * Template vars array
     *
     * @access protected
     * @var array
     */
    var $m_tpl_vars = array();

    /**
     * atkDocumentWriter Constructor.
     *
     * Dont use this, use &atkDocumentWriter::getInstance($format) instead to get a singleton instance for any format used
     */
    function atkDocumentWriter()
    {
    }

    /**
     * Assigns values to template variables
     *
     * @param string|array $tpl_var Template variable name or array of variable name/value pairs
     * @param mixed $value Value to assign (only used if $tpl_var is a string)
     */
    function assign($tpl_var, $value = null)
    {
      if (is_array($tpl_var))
      {
        foreach ($tpl_var as $key => $val)
        {
          if ($key != '')
          {
            $this->m_tpl_vars[$key] = $val;
          }
        }
      }
      else
      {
        if ($tpl_var != '')
          $this->m_tpl_vars[$tpl_var] = $value;
      }
    }

    /**
     * Get a singleton instance of the atkDocumentWriter class for any format used
     *
     * @param string $format Document format to be used (defaults to opendocument).
     * @return atkDocumentWriter Returns singleton instance of atkDocumentWriter descendant (depends on given format)
     */
    function &getInstance($format = "opendocument")
    {
      static $s_instance = NULL;

      if ($s_instance == NULL)
      {
        if ($format == "opendocument") {
          atkdebug("Creating a new atkOpenDocumentWriter instance");
          atkimport("atk.document.atkopendocumentwriter");
          $s_instance = new atkOpenDocumentWriter();
        }
        else {
          atkdebug(sprintf("Failed to create atkDocumentWriter instance (unknown format: %s)", $format));
        }
      }
      return $s_instance;
    }

  }

?>