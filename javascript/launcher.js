function atkLaunchApp()
{
  if (window.screen)
  {
    var hori=screen.availWidth;
    var verti=screen.availHeight;
    window.open('app.php','fullscreen', 'width='+hori+',height='+verti+',fullscreen=1, scrollbars=0,left='+(0)+',top='+(0));
  }
}
