function toggleHiding(name, linkhidden, linkunhidden)
{
  var integratedadd = get_object(name);
  toggleDisplay(name,integratedadd);
  
  var link = get_object(name+'_link');
  
  if (integratedadd.style.display!=="none")
  {  
    link.innerHTML = linkunhidden;
  }
  else
  {
    link.innerHTML = linkhidden;
  }
}