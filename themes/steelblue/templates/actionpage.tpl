<div class="actionpageWrapper">
{foreach from=$blocks item=block}
  {$block}
{/foreach}
{stacktrace}
{if count($stacktrace)}  
  <div align="right" class="stacktrace">
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
</div>