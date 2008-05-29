<!-- groups -->
{foreach from=$groups item=group}
 <tr id="ar_{$group.name}" class="{$group.tab}" {if $group.initial_on_tab!='yes'} style="display: none"{/if}>

      <td colspan="2" valign="top" nowrap>
   {if $group.box}
  <fieldset><legend>{$group.label}</legend>
   {/if}
   <table border="0">
      <tr>
   {foreach from=$group.fields item=details}
       {if isset($details.line) && $details.line!=""}
      <td colspan="2" valign="top" nowrap>{$details.line}
        {else}
        {if $details.label!=="AF_NO_LABEL"}<td valign="top" class="{if isset($details.error)}errorlabel{else}fieldlabel{/if}">{if $details.label!=""}<b>{$details.label}</b>:  {if isset($details.obligatory)}{$details.obligatory}{/if}{/if}</td>{/if}
      <td valign="top" id="{$details.id}" {if $details.label==="AF_NO_LABEL"}colspan="2"{/if} class="field">{$details.full}</td>
        {/if}
     {/foreach}
    </tr>
 </table>
   {if $group.box}
 </fieldset>
   {/if}
 </td>
</tr>
{/foreach}
<!-- /groups -->
{foreach from=$fields item=field}
  {include file="theme:field.tpl" field=$field}
{/foreach}