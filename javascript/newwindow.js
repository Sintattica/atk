 
function NewWindow(mypage, myname, w, h, scroll, resize, statusbar, menubar, toolbar, personalbar, titlebar) {
  var winl =  10;
  var wint =  10;
  scroll = scroll || 'no';
  statusbar = statusbar || 'no';
  menubar=menubar || 'no';
  toolbar= toolbar || 'no';
  personalbar= personalbar || 'no';
  titlebar= titlebar || 'no';
  resizable= resize || 'no';
  winprops = "height="+h+",width="+w+",top="+wint+",left="+winl+",scrollbars="+scroll+", resizable="+resizable+",status="+statusbar+",menubar="+menubar+",toolbar="+toolbar+",personalbar="+personalbar+",titlebar="+titlebar;
  win = window.open(mypage, myname, winprops)
  if (parseInt(navigator.appVersion) >= 4) { win.window.focus(); }
}