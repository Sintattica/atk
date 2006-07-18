<!--<h2 class="menuTitle">Hoofdmenu</h2>-->


<div id="mainMenu">
{foreach from=$menuitems item=menuitem}
{if !$firstmenuitem}{assign var='firstmenuitem' value=$menuitem.name}{/if}
  {if $menuitem.name!=='-'}
    <a href="#" onclick="showSubMenu('{$menuitem.name}'); window.open('{$menuitem.url}','main','');" onmouseover="this.style.cursor = 'pointer'" class="menuitem_link">
      <div id="mi_{$menuitem.name}" class="menuItemLevel1">
        <span class="menu-menuitem">{$menuitem.name}</span>
      </div>
    </a>
  {/if}
        
  {if (count($menuitem.submenu)>0)}
    <div id="smi_{$menuitem.name}" style="display: none; background: url({$themedir}images/menuLevel2Bg.jpg) repeat-y top left; color: #333;">
      {foreach from=$menuitem.submenu item=submenuitem}
         {if $submenuitem.enable && $submenuitem.name!=='-'}
           <a class="menuItemLevel2" onclick="window.open('{$submenuitem.url}','main','')" onmouseover="this.style.cursor = 'pointer'; this.style.fontWeight = 'bold';" onmouseout="this.style.fontWeight = '';">
             {$submenuitem.name}
           </a>
         {/if}
      {/foreach}
    </div>
  {/if}
{/foreach}
</div>

<script type="text/javascript">
{literal}
var prevSelectedMenu = '';
var curSelectedMenu=''; 

function showSubMenu(menuitemname)
{
  prevSelectedMenu = curSelectedMenu;
  hideAllSubMenus();
  
  if (menuitemname!==prevSelectedMenu) 
  {
    curSelectedMenu = menuitemname;
    displaySubMenu(menuitemname);
  }
  else
  {
    curSelectedMenu = '';
  }
}

function displaySubMenu(menuitemname)
{
  var tags = document.getElementsByTagName("div");
  
  for (i = 0; i < tags.length; i++)
	{
		var id = tags.item(i).id;

		if (id=='mi_'+menuitemname)
		{
 		  tags.item(i).className='menuItemLevel2Head';
		}
	}
  
  submenu = document.getElementById('smi_'+menuitemname);
  if (submenu)
  {
    if (submenu.style.display =='')
      submenu.style.display = 'none';
    else 
      submenu.style.display = '';
  }
}

function hideAllSubMenus()
{
  var tags = document.getElementsByTagName("div");
  
  for (i = 0; i < tags.length; i++)
	{
		var id = tags.item(i).id;

		if (id.substring(0,4)=="smi_")
		{
 		  tags.item(i).style.display="none";
		}
		else if (id.substring(0,3)=="mi_")
		{
 		  tags.item(i).className="menuItemLevel1";
		}		
	}
}
{/literal}
showSubMenu('{if $atkmenutop!=="main"}{$atkmenutopname}{else}{$firstmenuitem}{/if}');
</script>
