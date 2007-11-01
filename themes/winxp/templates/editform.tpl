  {foreach from=$fields item=field}
    <tr{if $field.rowid != ""} id="{$field.rowid}"{/if}{if !$field.initial_on_tab} style="display: none"{/if} class="{$field.tab}">
      {if isset($field.line)}
        <td colspan="2" valign="top">{$field.line}</td>
      {else}
      {if $field.label!=="AF_NO_LABEL"}<td valign="top" class="{if isset($field.error)}errorlabel{else}fieldlabel{/if}">{if $field.label!=""}{$field.label} {if $field.obligatory!=""}{$field.obligatory}{/if} : {/if}</td>{/if}
        <td id="{$field.id}" valign="top" {if $field.label==="AF_NO_LABEL"}colspan="2"{/if} class="field">{$field.full}</td>
      {/if}
    </tr>
  {/foreach}