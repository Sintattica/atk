<table width="100%">
  {foreach from=$fields item=field}
    <tr>
      {if $field.line!=""}
        <td {if $field.initial_on_tab!='yes'}style="display: none"{/if} colspan="2" valign="top" class="{$field.tab} field">{$field.line}</td>      
      {else}
        <td {if $field.initial_on_tab!='yes'}style="display: none"{/if} valign="top" class="{$field.tab} fieldlabel">{if $field.label!=""}{$field.label}: {/if}</td>
        <td {if $field.initial_on_tab!='yes'}style="display: none"{/if} valign="top" class="{$field.tab} field">{$field.full}</td>
      {/if}
    </tr>
  {/foreach}
</table>