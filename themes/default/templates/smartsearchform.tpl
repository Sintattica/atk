<div style="width: 80%">
  <div id="reset_criteria" style="text-align: left">
    <a href="{$reset_criteria}">{$label.reset_criteria}</a>
  </div>
  
  <br />
  
  {if $saved_criteria.load_criteria}
    <div id="load_criteria" style="text-align: left">
      {$saved_criteria.label_load_criteria}:<br />
      {$saved_criteria.load_criteria}
      {if $saved_criteria.forget_criteria}
        <a href="{$saved_criteria.forget_criteria}" title="{$saved_criteria.label_forget_criteria}"><img class="recordlist" border="0" title="Verwijder" alt="Verwijder" src="{atkthemeicon name='delete' type='recordlist'}" /></a>
      {/if}
    </div>
    
    <br />
  {/if}
  
  <div id="criteria" style="text-align: left">
    {foreach from=$criteria item=criterium}     
      {include file=$criterium.template} 
    {/foreach}
  </div>
  
  <div id="add_criterium">
    <a href="javascript:void(0)" onclick="{$action_add}" title="{$label.add_criterium}">{$label.add_criterium}</a>
  </div>
  
  <br />
  <br />
  
  <div id="save_criteria" style="text-align: left">
    {$saved_criteria.toggle_save_criteria} {$saved_criteria.label_save_criteria} {$saved_criteria.save_criteria}
  </div>
</div>
