function showTab(tab)
{
	// Eerst de class namen van alle elementen verkrijgen
	for (i = 0; i < document.getElementsByTagName("td").length; i++)
	{
		var tabclass = document.getElementsByTagName("td").item(i).className;
		
		// Dan de namen van alle tabs verkrijgen
		for(j = 0; j < tabs.length; j++)
		{
			// De elementen die op alle tabs voorkomen altijd weergeven
			if (tabclass == "alltabs")
			{
				document.getElementsByTagName("td").item(i).style.display="block";
			}
			
			// De overige elementen...
			else
			{
				// Kijken welke van de elementen bij een of meerdere tabs horen
				if (tabclass.indexOf(tabs[j]) != -1)
				{
					// Van deze elementen kijken welke bij de tab horen die moet worden weergegeven en deze weergeven
					if(tabclass.indexOf(tab) != -1)
					{
						document.getElementsByTagName("td").item(i).style.display="block";
					}
					// Van deze elementen kijken welke bij de tab horen die niet moet worden weergegeven en deze verbergen					
					else
					{
						document.getElementsByTagName("td").item(i).style.display="none";				
					}
				}
			}
			
			// De kleuren van de tabs goed zetten
			// De actieve tab
			if(tabs[j] == tab)
			{
				if(window.tabLeftImage)
				{
					document.getElementById("imgLeft_"+tabs[j]).src = tabSelectedLeftImage;
					document.getElementById("imgRight_"+tabs[j]).src = tabSelectedRightImage;
					document.getElementById(tabs[j]).background = tabSelectedBackgroundImage;
					document.getElementById(tabs[j]).style.color = tabSelectedColor;		
				}
				else
				{
					document.getElementById(tabs[j]).style.backgroundColor = tabSelectedBackground;
					document.getElementById(tabs[j]).style.color = tabSelectedColor;		
				}
			}
			// De niet-actieve tabs
			else
			{
				if(window.tabLeftImage)
				{
					document.getElementById("imgLeft_"+tabs[j]).src = tabLeftImage;
					document.getElementById("imgRight_"+tabs[j]).src = tabRightImage;
					document.getElementById(tabs[j]).background = tabBackgroundImage;
					document.getElementById(tabs[j]).style.color = tabColor;	
				}
				else
				{
					document.getElementById(tabs[j]).style.backgroundColor = tabBackground;
					document.getElementById(tabs[j]).style.color = tabColor;	
				}				
			}
		}
	}
}