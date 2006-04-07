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
      $width=100;
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

        // fancy autowidth detection
        $width = max(strlen($this->m_destInstance->descriptor($recordset[$i]))*10, $width);
      }

      $result = '<table border="0"><tr><td>'.atktext('available', 'atk').':<br/>';

      $leftname = $fieldprefix.$this->fieldName()."_sel";
      $rightname = $fieldprefix.$this->fieldName().'_AMDAE_'.$this->getRemoteKey();
      $result.= $this->renderSelect($leftname, $left, $fieldprefix, $width, $rightname);

      $result.= '</td><td valign="center" align="center">';

      $result.= '<input type="button" value="&gt;" onClick="shuttle_move(\''.$leftname.'\', \''.$rightname.'\');"><br/>';
      $result.= '<input type="button" value="&lt;" onClick="shuttle_move(\''.$rightname.'\', \''.$leftname.'\');"><br/><br/>';
      $result.= '<input type="button" value="&gt;&gt;" onClick="shuttle_moveall(\''.$leftname.'\', \''.$rightname.'\');"><br/>';
      $result.= '<input type="button" value="&lt;&lt;" onClick="shuttle_moveall(\''.$rightname.'\', \''.$leftname.'\');">';

      $result.= '</td><td>'.atktext('selected', 'atk').':<br/>';

      $result.= $this->renderSelect($rightname, $right, $fieldprefix, $width, $leftname);

      // on submit, we must select all items in the right selector, as unselected items
      // will not be posted.
      $page = &$this->m_ownerInstance->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkshuttlerelation.js");
      $page->register_submitscript("shuttle_selectAll('".$rightname."');");

      $result.= '</table>';

      return $result;
    }

    function renderSelect($name, $recordset, $prefix, $width, $opposite)
    {
      $result = '<select id="'.$name.'" name="'.$name.'[]" multiple size="10" style="width: '.$width.'px;" onDblClick="shuttle_move(\''.$name.'\', \''.$opposite.'\')">';
      for($i=0, $_i=count($recordset); $i<$_i; $i++)
      {
        $result.= '<option value="'.$recordset[$i][$this->m_destInstance->primaryKeyField()].'">'.$this->m_destInstance->descriptor($recordset[$i]);
      }
      $result.= '</select>';
      return $result;
    }

  }

?>