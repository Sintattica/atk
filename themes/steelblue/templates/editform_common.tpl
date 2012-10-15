<table id="editform" border="0">
  {if (count($errors)>0)}
    <tr>
      <td colspan="2" class="error">
        {$errortitle}<br/>
        {foreach from=$errors item=error}
          {$error.label}: {$error.message} {if $error.tablink} ({atktext "error_tab"} {$error.tablink}){/if}<br/>
        {/foreach}
        <br/>
      </td>
    </tr>
  {/if}
  {foreach from=$fieldspart item=part}
    {$part}
  {/foreach}
</table>
