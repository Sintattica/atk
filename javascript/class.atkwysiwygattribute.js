 var attribute = new Array();
 var stylesheet = '';
 window.onload = set_html;

 function callFormatting(sFormatString)
 {
   obj_editor.focus();
   obj_editor.document.execCommand(sFormatString, false, null);
   obj_editor.focus();
 }

 function callFormatting2(sFormatString)
 {
   obj_editor.focus();
   obj_editor.document.execCommand(sFormatString);
   obj_editor.focus();
 }

 function load_image(name,imgfile)
 {
   document.images[name].src = imgfile;
   return true;
 }

 function get_html(nom_editor)
 {
   var obj_ed = eval(nom_editor);
   var cont = obj_ed.document.body.innerHTML;
   var texto = "" + cont
   texto = strip_body(texto);
   return texto
 }

 function set_html()
 {
   for (var i=0;i<attribute.length;i++)
   {
     obj_editor = eval(attribute[i].editor);
     obj_editor.document.designMode = 'on';
     obj_editor.document.open();
     obj_editor.document.write('<html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"'+stylesheet+'\"></head><body>'+attribute[i].content+'</body></html>');
     obj_editor.document.close();
   }
 }

 function strip_body(cont)
 {
   var  ini_cos = cont.search(/<BODY/i);
   if( ini_cos == -1 )
   {
     return cont;
   }
   var  lon = cont.length
   var  fi = false
   var prob = false
   var  i = ini_cos + 5
   while( !fi )
   {
     car = cont.charAt(i);
     if( car == '>' )
     {
       ini_cos = i + 1
       fi = true
     }
     if( car == '"' || car == "'" )
     {
       fi_com = false
       i++
       if( i >= lon )
       {
         fi = true;
         prob = true;
         fi_com = true;
       }
       while( !fi_com )
       {
         car_aux = cont.charAt(i);
         if( car_aux == car )
         {
           fi_com = true
         }
         else
         {
           i++;
         }
         if( i >= lon )
         {
           fi = true;
           prob = true;
           fi_com = true;
         }
       }
     }
     i++;
     if( i >= lon )
     {
       fi = true;
       prob = true;
     }
   }
   if( prob == true )
   {
     alert('Due to problems with the HTML code of the page it is not possible to execute this action.');
   }
   else
   {
     var fi_cos = cont.search(/<\/BODY/i);
     var aux = cont.substring(ini_cos,fi_cos)
     cont = aux
   }
   return cont;
 }

 function find_in_text()
 {
   var txt = new String();
   var obj_ed = eval(obj_editor);
   var txt = prompt('Enter text to find', '');
   var range = obj_ed.document.body.createTextRange();
   if(txt != null)
   {
     var found = range.findText(txt);
     if (found)
     {
       range.select();
     }
     else
     {
       alert('De tekst is niet gevonden');
     }
   }
 }

 function modifySelection(pre, post)
{
  if (!document.selection) return;

  var selection = obj_editor.document.selection.createRange();
  var current   = selection.text;

  if (current == '') return;

  selection.text = pre + current + post;
  selection.parentElement().focus();
}

function popupSelection(url,title)
{
  if (!document.selection)
  {
   alert("Selecteer eerst het stuk tekst waar u een link van wilt maken");
   return;
  }

  var selection = obj_editor.document.selection.createRange();
  var current   = selection.text;

  if (current == '')
  {
    alert("Selecteer eerst het stuk tekst waar u een link van wilt maken");
    return;
  }

  NewWindow(url,title,400,400,'yes');
}