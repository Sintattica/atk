function NewWindow(mypage, myname, w, h, scroll, resize) {
  var winl =  10;
  var wint =  10;
  winprops = "height="+h+",width="+w+",top="+wint+",left="+winl+",scrollbars="+scroll+","+resize
  win = window.open(mypage, myname, winprops)
  if (parseInt(navigator.appVersion) >= 4) { win.window.focus(); }
}