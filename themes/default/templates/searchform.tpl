<table width="100%">
  <tr>
    <td class="fieldlabel">
      {$searchmode_title}
    </td>
    <td colspan="2" class="field">
      {$searchmode_and} &nbsp;&nbsp; {$searchmode_or}
    </td>
  </tr>    
  <tr>
    <td colspan="3"><hr></td>
  </tr>
  {foreach from=$fields item=field}        
    <tr>
      {if $field.line!=""}
        <td colspan="2" valign="top" class="field">{$field.line}</td>      
      {else}
        <td valign="top" class="fieldlabel">{if $field.label!=""}{$field.label}: {/if}</td>
        <td valign="top" class="field">{$field.full}</td>
      {/if}
      <td class="field">
        {$field.searchmode}
      </td>
    </tr>
  {/foreach}
</table>