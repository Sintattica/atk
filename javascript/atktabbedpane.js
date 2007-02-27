function showpanetab(classname,parentid,defaulttab)
{
	var id= 'tabbedpane_' + parentid;
	var h = $(id).getElementsBySelector('tr');

	var id= 'tabbed_' + parentid;
	var t = $(id).getElementsBySelector('div');

	for (i=0;el=h[i];i++)
	{
		var id=$(el).id
		if(id==null) {continue}
		if(id.substring(0,3) == 'ar_')
		{
			if($(el).hasClassName(classname))
			{
				$(el).show();
				$(el).addClassName('section_'+defaulttab);
			}
			else
			{
				$(el).hide();
				$(el).removeClassName('section_'+defaulttab);
			}
		}
	}

	for (i=0;el=t[i];i++)
	{
		var id = $(el).id
		if(id==null) {continue}
		if(id.substring(0,8) == 'panetab_')
		{
			if(id == ('panetab_' + classname))
			{
				$(el).addClassName('activetab');
				$(el).removeClassName('passivetab');
			}
			else
			{
				$(el).addClassName('passivetab');
				$(el).removeClassName('activetab');
			}
		}
	}
}