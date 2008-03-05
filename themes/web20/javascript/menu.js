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