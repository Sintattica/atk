<?php
atkimport('atk.relations.controls.atkshuttlecontrol');

abstract class atkShuttleFilter extends atkShuttleControl
{
  public function render($record, $mode, $prefix)
  {
      $mode == "add" ? "add" : "edit";
      $url = addslashes(partial_url($this->m_shuttle->m_ownerInstance->atkNodeType(), $mode, "attribute.".$this->m_shuttle->getHtmlId($prefix).".filter", array("atkfieldprefix" => $prefix)));

      $page = $this->m_shuttle->m_ownerInstance->getPage();
      $page->register_scriptcode("function ".$prefix.$this->getName()."_onChange(el)
                                  {
                                    shuttle_refresh('".$url."', '".$this->m_shuttle->getHtmlId($prefix).'[cselected][]['.$this->m_shuttle->getRemoteKey().']'."', '".$prefix.$this->m_shuttle->fieldName()."[section]', '".$this->m_section."')
                                  }\n");
  }

  /**
   * Returns a filter clause for this filter's current value.
   * The current value can be retrieved from the record using
   * the getValue method.
   *
   * @param array $record full record
   */
  public abstract function getFilter(&$record);
}