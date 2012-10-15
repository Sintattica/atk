{if count($stacktrace) || $lockstatus!="" || $helplink!=""}
<table border="0" width="100%">
  <tr>
     {if count($stacktrace)}
     <td align="left" class="stacktrace">
        {section name=i loop=$stacktrace}
           {if %i.last%}
             <span class="stacktrace_end">{$stacktrace[i].title}</span>
           {else}           
             <a href="{$stacktrace[i].url}" class="stacktrace">{$stacktrace[i].title}</a> &raquo;
           {/if}
        {/section}
     </td>
     {/if}
     {if $lockstatus!=""}<td align="right" class="lockstatus">{$lockstatus}</td>{/if}
     {if $helplink!=""}<td align="right" class="helplink">{$helplink}</td>{/if}
  </tr>
</table>
{/if}