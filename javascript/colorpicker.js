function picker(a,color)
{
var formname = "document.entryform."+a+".value='"+color+"'";
eval(formname);
}

function remotePicker(a,color)
{
var formname = "opener.document.entryform."+a+".value='"+color+"'";
eval(formname);
window.close();
}