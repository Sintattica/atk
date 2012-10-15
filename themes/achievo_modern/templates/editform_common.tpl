<table id="editform" border="0">
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
  {foreach from=$fieldspart item=part}
    {$part}
  {/foreach}
</table>