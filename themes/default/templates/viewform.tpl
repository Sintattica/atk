<table width="100%">
  {foreach from=$fields item=field}
    <tr{if $field.rowid != ""} id="{$field.rowid}"{/if}{if $field.initial_on_tab!='yes'} style="display: none"{/if} class="{$field.tab}">
      {if $field.line!=""}
        <td colspan="2" valign="top" class="field">{$field.line}</td>      
      {else}
        <td valign="top" class="fieldlabel">{if $field.label!=""}{$field.label}: {/if}</td>
        <td valign="top" class="field">{$field.full}</td>
      {/if}
    </tr>
  {/foreach}
</table>