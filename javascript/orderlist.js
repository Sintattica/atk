<!--
 function setOrder(direction)
 {
   // first get pointers to the elementlist and it's collections
   var lijst = document.getElementById("orderlist");
   var items = eval(lijst.options);
   var item  = lijst.options.selectedIndex;
   var order = "";   

   // do we have any selected pages ?
   if (item == -1)
   {
     alert("U heeft geen pagina geselecteerd")
   }
   else
   {   
     // Move up ?
     if (direction == "up")
     {
       if (item == 0)
       {
         alert("Deze pagina staat al bovenaan");
       }
       else
       {
         orgTitle = items[item-1].text;
         orgValue = items[item-1].value;
         myTitle  = items[item].text;     
         myValue  = items[item].value;     

         items[item-1].text  = myTitle
         items[item-1].value = myValue;
         items[item].text    = orgTitle;
         items[item].value   = orgValue;     
         items[item-1].selected = true;
       }
     }

     // Move down ?
     if (direction == "dn")
     {
       if (item == items.length-1)
       {
         alert("Deze pagina staat al onderaan");
       }
       else
       {   
         orgTitle = items[item+1].text;
         orgValue = items[item+1].value;
         myTitle  = items[item].text;     
         myValue  = items[item].value;    
                 
         items[item+1].text     = myTitle
         items[item+1].value    = myValue;
         items[item].text       = orgTitle;
         items[item].value      = orgValue; 
         items[item+1].selected = true;
       } 
     }  

     // save current page orders
     for (i=0;i<items.length;i++)
     {
       order += items[i].value+",";  
     }

     // and save it to our hidden field which
     // used to post to the PHP code.
     var orderField = document.getElementById("h_orderfield");
     orderField.value = order;
   }
 }
//-->