<table width="100%">
  {foreach from=$fields item=field}
    <tr>
      {if $field.line!=""}
        <td class="{$field.tab}" {if $field.initial_on_tab!='yes'}style="display: none"{/if} colspan="2" valign="top" class="field">{$field.line}</td>      
      {else}
        <td class="{$field.tab}" {if $field.initial_on_tab!='yes'}style="display: none"{/if} valign="top" class="fieldlabel">{if $field.label!=""}{$field.label}: {/if}</td>
        <td class="{$field.tab}" {if $field.initial_on_tab!='yes'}style="display: none"{/if} valign="top" class="field">{$field.full}</td>
      {/if}
    </tr>
  {/foreach}
</table>