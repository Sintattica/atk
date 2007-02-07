<script type="text/javascript" src="{$themedir}javascript/menu.js"></script>

<script language="javascript">
  var panename  = new Array();
  var panestate = new Array();
</script>

<div id="dhtmlgoodies_xpPane">
{foreach from=$menuitems item=menuitem}
  {if $menuitem.enable}
    <script language="javascript">
      {if $menuitem.url}
        panename[panename.length] = '<a href="{$menuitem.url}" target="main">{$menuitem.name}</a>';
      {else}
        panename[panename.length] = '{$menuitem.name}';
      {/if} 
      panestate[panestate.length] = false;
    </script>
  	<div class="dhtmlgoodies_panel">
  		<div>
  		  {section name=i loop=$menuitem.submenu}
  		    {if $menuitem.submenu[i].enable}
  		      <a href="{$menuitem.submenu[i].url}" target="main">{$menuitem.submenu[i].name}</a><br/>
  		    {/if}
  		  {/section}
      </div>
    </div>
  {/if}
{/foreach} 
</div>

<script type="text/javascript">
/*
Arguments to function
1) Array of titles
2) Array indicating initial state of panels(true = expanded, false = not expanded )
*/
initDhtmlgoodies_xpPane(panename,panestate,Array(), '{$themedir}');
</script>