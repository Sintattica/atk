{atkmessages}
{if count($atkmessages)}
<div class="atkmessages">
  {foreach from=$atkmessages item=message}
    <div class="atkmessages_{$message.type}">{$message.message}</div>
  {/foreach}
</div>
{/if}
<div class="actionpageWrapper">
{stacktrace}
{if count($stacktrace) > 1}  
  <div class="stacktrace">
    {section name=i loop=$stacktrace}
      {if %i.index%>=%i.loop%-4}
       {if %i.last%}
         <span class="stacktrace_end">{$stacktrace[i].title}</span>
       {else}           
         <a href="{$stacktrace[i].url}" class="stacktrace">{$stacktrace[i].title}</a> &raquo;
       {/if}
      {else}
        {if %i.index% == 0}... &raquo;{/if}
      {/if}
    {/section}
    &nbsp;&nbsp;
    </div>    
{/if}
{foreach from=$blocks item=block}
  {$block}
{/foreach}
</div>