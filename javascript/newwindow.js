function NewWindow(mypage, myname, w, h, scroll) {
  var winl =  w;
  var wint =  h;
  winprops = "height="+h+",width="+w+",top="+wint+",left="+winl+",scrollbars="+scroll+",resizable"
  win = window.open(mypage, myname, winprops)
  if (parseInt(navigator.appVersion) >= 4) { win.window.focus(); }
}