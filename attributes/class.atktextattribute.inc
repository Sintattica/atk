<?php
  /**
   * The atkTextAttribute class represents an attribute of a node
   * that is a big text field.
   *
   * @author Ivo Jansch (ivo@achievo.com)
   * @version 1.1
   *
   * <b>Changes</b>:
   * 
   * 1,1    - value of the textarea is now htmltag safe (htmlentities) (Ivo)
   *
   * $Id$
   * $Log$
   * Revision 4.1  2001/05/10 08:31:00  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:10  ivo
   * Added atk4 to repository.
   *
   * Revision 1.3  2001/02/28 15:34:32  sandy
   * - fixed a bug in the text attributes (length)
   *
   * Revision 1.2  2001/02/22 22:46:18  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:57:58  sandy
   * Achievo Tool Kit
   *
   */
  class atkTextAttribute extends atkAttribute
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function atkTextAttribute($name, $flags=0)
    {
      $this->atkAttribute($name,$flags); // base class constructor
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a textarea
     */
    function edit($record="", $fieldprefix="")
    {
 	    // todo: configurable rows
	    // watch out, $maxsize isn't supported for a textarea.
      $result= '<textarea name="'.$fieldprefix.$this->formName().'" rows="10" wrap="virtual"'.
               ($this->m_size > 0 ? ' cols="'.$this->m_size.'"' : '').
               '>'.htmlentities($record[$this->fieldName()]).'</textarea>';
      return $result;
    }
  }
?>