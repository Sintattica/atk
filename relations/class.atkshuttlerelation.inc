<?php

  userelation("atkmanytomanyrelation");

  class atkShuttleRelation extends atkManyToManyRelation
  {
    function edit($record, $fieldprefix, $mode)
    {
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

      $filter = "";
      if ($this->m_destinationFilter!="")
      {
        $parser = new atkStringParser($this->m_destinationFilter);
        $filter = $parser->parse($record);
      }
      // now select all records
      $recordset = $this->m_destInstance->selectDb($filter);

      $left = array();
      $right = array();
      for ($i=0;$i<count($recordset);$i++)
      {
        if (in_array($this->m_destInstance->primaryKey($recordset[$i]),$selectedPk))
        {
          $right[] = $recordset[$i];
        }
        else
        {
          $left[] = $recordset[$i];
        }
      }

      $result = '<table border="0"><tr><td>';

      $leftname = $fieldprefix.$this->fieldName()."_sel";
      $rightname = $fieldprefix.$this->fieldName().'_AMDAE_'.$this->getRemoteKey();
      $result.= $this->renderSelect($leftname, $left, $fieldprefix);

      $result.= '</td><td valign="center" align="center">';

      $result.= '<input type="button" value="&gt;" onClick="shuttle_move(\''.$leftname.'\', \''.$rightname.'\');"><br/>';
      $result.= '<input type="button" value="&lt;" onClick="shuttle_move(\''.$rightname.'\', \''.$leftname.'\');"><br/><br/>';
      $result.= '<input type="button" value="&gt;&gt;" onClick="shuttle_moveall(\''.$leftname.'\', \''.$rightname.'\');"><br/>';
      $result.= '<input type="button" value="&lt;&lt;" onClick="shuttle_moveall(\''.$rightname.'\', \''.$leftname.'\');">';

      $result.= '</td><td>';

      $result.= $this->renderSelect($rightname, $right, $fieldprefix);

      // on submit, we must select all items in the right selector, as unselected items
      // will not be posted.
      $page = &$this->m_ownerInstance->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkshuttlerelation.js");
      $page->register_submitscript("shuttle_selectAll('".$rightname."');");

      $result.= '</table>';

      return $result;
    }

    function renderSelect($name, $recordset, $prefix)
    {
      $result = '<select id="'.$name.'" name="'.$name.'[]" multiple size="10">';
      for($i=0, $_i=count($recordset); $i<$_i; $i++)
      {
        $result.= '<option value="'.$recordset[$i][$this->m_destInstance->primaryKeyField()].'">'.$this->m_destInstance->descriptor($recordset[$i]);
      }
      $result.= '</select>';
      return $result;
    }

  }

?>