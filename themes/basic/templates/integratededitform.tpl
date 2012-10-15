<table class="integratededitform" width="100%">
  {if (count($errors)>0)}
    <tr>
      <td colspan="2" class="error">
        {$errortitle}
        {foreach from=$errors item=error}
          <br>{$error.label}: {$error.message} {if isset($error.tablink)} ({atktext "error_tab"} {$error.tablink}){/if}
        {/foreach}
      </td>
    </tr>
  {/if}
  {foreach from=$fields item=field}
    <tr>
      {if $field.line!=""}
        <td colspan="2" valign="top">{$field.line}</td>
      {else}
      {if $field.label!=="AF_NO_LABEL"}<td>{if $field.label!=""}{$field.label}: {/if}</td>{/if}
        <td>{$field.full}</td>
      {/if}
    </tr>
  {/foreach}
</table>