function NewWindow(mypage, myname, w, h, scroll, resize) {
  var winl =  w;
  var wint =  h;
  winprops = "height="+h+",width="+w+",top="+wint+",left="+winl+",scrollbar="+scroll+","+resize
  win = window.open(mypage, myname, winprops)
  if (parseInt(navigator.appVersion) >= 4) { win.window.focus(); }
}

function OpenChild(file,window) {
    childWindow=open(file,window,'scrolbars=no,toolbar=no,status=no,menubar=no,location=no,resizable=no,width=400,height=140');
    if (childWindow.opener == null) childWindow.opener = self;
    }