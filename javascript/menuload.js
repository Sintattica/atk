function menuload(menuurl, mainurl)
{
  parent.menu.location.href= menuurl;
  parent.main.location.href= mainurl;
}

function reloadProjects(el)
{
  var id = el.options[el.selectedIndex].value;
  window.location= "menu.php?atkmenutop=projectmanagement&selectedproject="+id;
}
