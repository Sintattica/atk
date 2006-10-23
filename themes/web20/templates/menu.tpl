<script type="text/javascript" src="{$themedir}javascript/menu.js"></script>
<div id="box-menu">
<div id="box-menu-title">{$title}</div>
<div id="box-menu-content">
{foreach from=$menuitems item=menuitem}
{if !$firstmenuitem}{assign var='firstmenuitem' value=$menuitem.name}{/if}
  {if $menuitem.name!=='-' && $menuitem.enable}
    <a href="#" onclick="showSubMenu('{$menuitem.name}'); {if $atkmenutop!==$menuitem.id && $menuitem.url}document.location.href= '{$menuitem.url}';{/if}" onmouseover="this.style.cursor = 'pointer'" class="menuitem_link">
      <div id="mi_{$menuitem.name}" class="menuItemLevel1">
        <span class="menu-menuitem">{$menuitem.name}</span>
      </div>
    </a>
  {/if}
        
  {if (count($menuitem.submenu)>0)}
    <div id="smi_{$menuitem.name}" style="display: none; color: #333;">
      {$menuitem.header}
      {foreach from=$menuitem.submenu item=submenuitem}
         {if $submenuitem.enable && $submenuitem.name!=='-'}
           <a class="menuItemLevel2" onclick="document.location.href='{$submenuitem.url}'" onmouseover="this.style.cursor = 'pointer'; this.style.fontWeight = 'bold';" onmouseout="this.style.fontWeight = '';">
             &nbsp;&nbsp;&nbsp;{$submenuitem.name}
           </a><br />
         {elseif $submenuitem.name=='-'}
         <br />
         {/if}
      {/foreach}
      <br />
    </div>
  {/if}
{/foreach}
</div>
</div>
<script type="text/javascript">
showSubMenu('{if $atkmenutop!=="main"}{$atkmenutopname}{else}{$firstmenuitem}{/if}');
</script>