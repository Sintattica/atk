function loginonload()
{
  function roundCorners(arg1,arg2)
  {
    if ($(arg1)) Rico.Corner.round(arg1,arg2);
  }
  roundCorners('loginform');
  roundCorners('loginform-title', {compact:true});
  roundCorners('loginform-content', {compact:true});
  roundCorners('loginform-submit', {color:"transparent", bgColor:Rico.Corner._background($('loginform-content'))});
  verticalAlignBox('loginform');
  showLoginButton();
}
window.onresize=function(){verticalAlignBox('loginform');}

function verticalAlignBox(id)
{
  var vertalign = "0px";
  obj = $(id);
  if (obj.offsetWidth < document.documentElement.clientHeight)
  {
    vertalign = parseInt((document.documentElement.clientHeight - obj.offsetWidth) / 2)-20 + "px";
  }
  obj.style.marginTop = vertalign;
}

function showLoginButton()
{
  if (document.getElementById('auth_user').value && document.getElementById('password').value)
  {
    document.getElementById('loginform-submit').style.display="";
  }
  else document.getElementById('loginform-submit').style.display="none";
  window.setTimeout('showLoginButton()', 50);
}