{if $saved_criteria.load_criteria}
<div class="load_criteria">
{$saved_criteria.label_load_criteria}: 
{$saved_criteria.load_criteria}
{if $saved_criteria.forget_criteria}
  <a href="{$saved_criteria.forget_criteria}" title="{$saved_criteria.label_forget_criteria}"><img class="recordlist" border="0" title="Verwijder" alt="Verwijder" src="{atkthemeicon name='delete' type='recordlist'}" /></a>
{/if}
</div>
{/if}

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
  {if $saved_criteria.toggle_save_criteria }
  <tr>
    <td colspan="3"><hr></td>
  </tr>
  {/if}
</table>

{if $saved_criteria.toggle_save_criteria }
<div class="save_criteria">
{$saved_criteria.toggle_save_criteria} {$saved_criteria.label_save_criteria} {$saved_criteria.save_criteria}
</div>
{/if}