      <tr>
        <td valign="top">
          <table>
  {foreach from=$fields item=field}
    {if $field.column != 1}{include file="theme:field.tpl" field=$field}{/if}
  {/foreach}
          </table>
        </td>
        <td valign="top">
          <table>
  {foreach from=$fields item=field}
    {if $field.column == 1}{include file="theme:field.tpl" field=$field}{/if}
  {/foreach}
          </table>
        </td>
      </tr>  