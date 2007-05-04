<br>
<table id="editform" width="100%">
  {if (count($errors)>0)}
    <tr>
      <td colspan="2" class="error">
        {$errortitle}
        {foreach from=$errors item=error}
          <br>{$error.label}: {$error.msg} {if isset($error.tab)} ({$error.tab}){/if}
        {/foreach}
      </td>
    </tr>
  {/if}
  {foreach from=$fields item=field}
    {include file="theme:field.tpl" field=$field}
  {/foreach}
</table>
