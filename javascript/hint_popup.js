<!--
 function showLayer(layer_id)
 {
  IE4  = (document.all) ? 1 : 0;
  NS4  = (document.layers) ? 1 : 0;
  ver4 = (IE4 || NS4) ? 1 : 0;
  NS6  = (navigator.userAgent.indexOf("Gecko")!=-1) ? 1:0

  if (!ver4 && !NS6) { return }

  if (layer_id == 'pagelist')
  {
   off = 'imagelist';
  }
  else
  {
   off = 'pagelist';
  } 

  if (IE4) 
  {
   dummyOn  = 'document.all.'+layer_id+'.style';
   On       = eval(dummyOn);
   dummyOff = 'document.all.'+off+'.style';
   Off      = eval(dummyOff);   
  }
  else 
  {
   if (NS6)
   {
    On  = document.getElementById(layer_id);
    Off = document.getElementById(off);
   }
   else
   {
    dummyOn  = 'document.'+layer_id;
    On       = eval(dummyOn);
    dummyOff = 'document.'+off; 
    Off      = eval(dummyOff);
   }
  }

  if (ver4)
  {
   On.visibility  = "visible";
   Off.visibility = "hidden";
  }
  
  if (NS6)
  {
   On.style.visibility  = "visible";
   Off.style.visibility = "hidden";
  }
 }
//-->
