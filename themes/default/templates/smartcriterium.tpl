<div id="{$criterium.element.box}" style="background-color: #fafafa; margin-top: 8px; margin-bottom: 8px">
<table border="0" width="100%">
  <tr>
    <td valign="top" class="fieldlabel" width="10" nowrap>
      {$label.criterium_field}:
    </td>
    <td id="{$criterium.element.field}" valign="top" class="field">
      {$criterium.field}
    </td>
    <td rowspan="3" valign="top" align="right">
       <a href="javascript:void(0)" onclick="{$criterium.remove_action}" title="{$label.remove_criterium}"><img class="recordlist" border="0" src="{atkthemeicon name='delete' type='recordlist'}" /></a>        
    </td>
  </tr>
  <tr>
    <td valign="top" class="fieldlabel" width="10" nowrap>
      {$label.criterium_value}:
    </td>
    <td id="{$criterium.element.value}" valign="top" class="field" align="left">
      {$criterium.value}
    </td>
  </tr>
  <tr>
    <td valign="top" class="fieldlabel" width="10" nowrap>
      {$label.criterium_mode}:
    </td>
    <td id="{$criterium.element.mode}" valign="top" class="field" align="left">
      {$criterium.mode}
    </td>
  </tr>
  
</table>
{$criterium.script}
</div>