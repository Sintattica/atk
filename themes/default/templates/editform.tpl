<br>
<table id="editform" width="100%">
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
    {include file="theme:field.tpl" field=$field}
  {/foreach}
</table>
