<div style="width: 80%">
  <div id="reset_criteria" style="text-align: left">
    <a href="{$reset_criteria}">{$label.reset_criteria}</a>
  </div>
  
  <br />
  
  {if $load_criteria}
    <div id="load_criteria" style="text-align: left">
      {$label.load_criteria}:<br />
      {$load_criteria}
      {if $forget_criteria}
        <a href="{$forget_criteria}" title="{$label.forget_criteria}">X</a>
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
    {$toggle_save_criteria} {$label.save_criteria} {$save_criteria}
  </div>
</div>
