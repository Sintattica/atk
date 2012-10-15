<br>
<table id="editform" width="100%">
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
  {foreach from=$fields item=field}
    {include file="theme:field.tpl" field=$field}
  {/foreach}
</table>
