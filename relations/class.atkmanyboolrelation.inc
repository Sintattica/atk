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
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /** @internal include baseclass. */
  userelation("atkmanytomanyrelation");


  /**
   * Attribute flag. When used the atkManyBoolRelation shows add links to add records for the related table
   */
  define("AF_MANYBOOL_AUTOLINK", AF_SPECIFIC_1);

  /**
   * Many-to-many relation.
   *
   * The relation shows a list of available records, and a set of checkboxes
   * to link the records with the current record on the source side.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage relations
   *
   */
  class atkManyBoolRelation extends atkManyToManyRelation
  {
    var $m_cols=3;

    /**
     * Return a piece of html code to edit the attribute
     * @param $record Current record
     * @return String piece of html code
     */
    function edit($record="", $fieldprefix="")
    {
      $cols = $this->m_cols;
      $modcols = $cols-1;
      $this->createDestination();
      $this->createLink();

      $selectedPk = array();

      // first the selected records..
      for ($i=0;$i<count($record[$this->m_name]);$i++)
      {

        if(is_array($record[$this->fieldName()][$i][$this->getRemoteKey()]))
          $newselected = $this->m_destInstance->primaryKey($record[$this->m_name][$i][$this->getRemoteKey()]);
        else{
          $newselected = $this->m_destInstance->primaryKey(array($this->m_destInstance->primaryKeyField()=>$record[$this->m_name][$i][$this->getRemoteKey()]));
        }
        $selectedPk[] = $newselected;
       }

      $filter = $this->parseFilter($this->m_destinationFilter,$record);
      // now select all records
      $recordset = $this->m_destInstance->selectDb($filter);

      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkprofileattribute.js.php");

      $result ='<div align="left"><font size="-2">
                [<a href="javascript:profile_checkAll(\''.$this->fieldName().'\')">'.
                     atktext("check_all", "atk").
                '</a> <a href="javascript:profile_checkNone(\''.$this->fieldName().'\')">'.
                     atktext("check_none", "atk").
                '</a> <a href="javascript:profile_checkInvert(\''.$this->fieldName().'\')">'.
                     atktext("invert_selection", "atk").'</a>]</font></div>';

      $result.= '<table border="0"><tr>';
      for ($i=0;$i<count($recordset);$i++)
      {
        $detaillink = "&nbsp;";
        $selector="";
        if (in_array($this->m_destInstance->primaryKey($recordset[$i]),$selectedPk))
        {
          $sel = "checked";
          if (!$this->m_linkInstance->hasFlag(NF_NO_EDIT)&&$this->m_linkInstance->allowed("edit"))
          {
            $selector = $this->m_linkInstance->m_table.'.'.$this->getLocalKey()."='".$record[$this->m_ownerInstance->primaryKeyField()]."'".
              ' AND '.$this->m_linkInstance->m_table.'.'.$this->getRemoteKey().
              "='".$recordset[$i][$this->m_destInstance->primaryKeyField()]."'";

            // Create link to details.
            $detaillink = href(dispatch_url($this->m_link, "edit", array("atkselector"=>$selector)),
              "[".atktext("details", "atk")."]",
              SESSION_NESTED,
              true);
          }
        }
        else
        {
          $sel = "";
        }
        $result.= '<td class="table"><input type="checkbox" name="'.
                       $fieldprefix.$this->fieldName().
                       '_AMDAE_'.$this->getRemoteKey().
                       '[]" value="'.
                       $recordset[$i][$this->m_destInstance->primaryKeyField()].
                       '" '.$sel.'></td><td class="table">'.
                       $this->m_destInstance->descriptor($recordset[$i]).
                       '</td><td class="table">'.$detaillink.'</td>';
        if ($i%$cols == $modcols) $result .="</tr><tr>\n";
      }
      $result.="</tr></table>\n";

      // Add the add link if AF_MANYBOOL_AUTOLINK used
      if (($this->hasFlag(AF_MANYBOOL_AUTOLINK)) && ($this->m_destInstance->allowed("add")))
        $result.= href(dispatch_url($this->m_destination, "add"), $this->getAddLabel(), SESSION_NESTED) . "\n";

      return $result;
    }

    function setCols($cols)
    {
      $this->m_cols = $cols;
    }

  }
?>
