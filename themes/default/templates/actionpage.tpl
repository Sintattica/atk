{foreach from=$blocks item=block}
  {$block}
{/foreach}
{stacktrace}
{if count($stacktrace)}  
  <div align="right">
    {section name=i loop=$stacktrace}
       {if %i.last%}
         <span class="stacktrace_end">{$stacktrace[i].title}</span>
       {else}           
         <a href="{$stacktrace[i].url}" class="stacktrace">{$stacktrace[i].title}</a> &raquo;
       {/if}
    {/section}
    &nbsp;&nbsp;
    </div>    
{/if}