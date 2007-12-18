{foreach from=$links item='link' key='i'}
  {if $i > 0}
    |
  {/if}
  
  {if $link.current}
    <b>{$link.title}</b>
  {else}
    <a href="javascript:void(0)" onclick="{$link.call|escape}" title="{$link.title|escape}">{$link.title|escape}</a>
  {/if}
{/foreach}