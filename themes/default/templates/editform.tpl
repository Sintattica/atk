<br>
<table width="100%">
  {if (count($errors)>0)}
    <tr>
      <td colspan="2" class="error">
        {$errortitle}
        {foreach from=$errors item=error}
          <br>{$error.label}: {$error.msg} {if $error.tab!=""} ({$error.tab}){/if}          
        {/foreach}
      </td>
    </tr>
  {/if}  
  {foreach from=$fields item=field}
    <tr>
      {if $field.line!=""}
        <td {if $field.initial_on_tab!='yes'}style="display: none"{/if} colspan="2" valign="top" class="{$field.tab}">{$field.line}</td>      
      {else}
        <td {if $field.initial_on_tab!='yes'}style="display: none"{/if} valign="top" class="{$field.tab} {if $field.error!=""}error{else}fieldlabel{/if}">{if $field.label!=""}{$field.label}: {/if}</td>
        <td {if $field.initial_on_tab!='yes'}style="display: none"{/if} valign="top" class="{$field.tab} field">{$field.full}</td>
      {/if}
    </tr>
  {/foreach}
</table>