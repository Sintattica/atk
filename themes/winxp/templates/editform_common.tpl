<br>
<table id="editform">
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
  {foreach from=$fieldspart item=part}
    {$part}
  {/foreach}
</table>