<table class="shuttleTable">
<tr>
  <td>
    {foreach from=$ava_controls item=control}
      {$control}
    {/foreach}
  </td>
  <td>&nbsp;</td>
  <td>
    {foreach from=$sel_controls item=control}
      {$control}
    {/foreach}
  </td>
</tr>
<tr>
  <td>
    {atktext available}:<br/>
    <div id="{$htmlid}_available">
      <select class="shuttle_select" id="{$leftname}" name="{$leftname}" multiple size="10" onDblClick="shuttle_move('{$leftname}','{$rightname}','add','{$htmlid}[selected][][{$remotekey}]');{$htmlid}_onChange('selected');">
        {foreach from=$available_options key=key item=option}
          <option value="{$key}">{$option}</option>
        {/foreach}
      </select>
    </div>
  </td>
  <td valign="center" align="center">
    <input type="button" value="&gt;"     onClick="shuttle_move   ('{$leftname}', '{$rightname}', 'add', '{$name}'); {$htmlid}_onChange('selected');"><br/>
    <input type="button" value="&lt;"     onClick="shuttle_move   ('{$rightname}', '{$leftname}', 'del', '{$name}'); {$htmlid}_onChange('available');"><br/><br/>
    <input type="button" value="&gt;&gt;" onClick="shuttle_moveall('{$leftname}', '{$rightname}', 'add', '{$name}'); {$htmlid}_onChange('selected');"><br/>
    <input type="button" value="&lt;&lt;" onClick="shuttle_moveall('{$rightname}', '{$leftname}', 'del', '{$name}'); {$htmlid}_onChange('available');">
  </td>
  <td>
    {atktext selected}:<br/>
    <div id="{$htmlid}_selected">
      <select class="shuttle_select" id="{$rightname}" name="{$rightname}" multiple size="10" onDblClick="shuttle_move('{$rightname}','{$leftname}','del','{$htmlid}[selected][][{$remotekey}]');{$htmlid}_onChange('available');">
        {foreach from=$selected_options key=key item=option}
          <option value="{$key}">{$option}</option>
        {/foreach}
      </select>
    </div>
  </td>
</tr>
</table>

<input type="hidden" id="{$name}" name="{$name}" value={$value} />
<input type="hidden" id="{$htmlid}[section]" name="{$htmlid}[section]" />