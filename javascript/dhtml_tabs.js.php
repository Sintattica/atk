function showTab(tab)
{
	// Eerst de class namen van alle elementen verkrijgen
	
	var tags = document.getElementsByTagName("tr");
	
	for (i = 0; i < tags.length; i++)
	{
		var tabclass = tags.item(i).className;
		var id = tags.item(i).id;

		if (id.substring(0,3)=="ar_")
		{
		  if (tabclass==tab||tabclass=="alltabs")
		  {		  
  		  tags.item(i).style.display="";		  
		  }
		  else
		  {		  
  		  tags.item(i).style.display="none";		  
		  }
		}
		else
		{
		  // Don't touch any element that is not an attribute row
		}
	}

	// Dan de namen van alle tabs verkrijgen
	for(j = 0; j < tabs.length; j++)
	{
		// De kleuren van de tabs goed zetten
		// De actieve tab
		if(tabs[j] == tab)
		{
			document.getElementById('tab_'+tabs[j]).className = 'activetab';
		}
		// De niet-actieve tabs
		else
		{
		  document.getElementById('tab_'+tabs[j]).className = 'passivetab';
		}
	}
}