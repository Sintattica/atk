activateMenu = function(nav) {

  // currentStyle restricts the Javascript to IE only 
  if (document.all && document.getElementById(nav).currentStyle) 
  {  
    var navroot = document.getElementById(nav);
        
    // Get all the list items within the menu 
    var lis=navroot.getElementsByTagName("LI");  
    for (i=0; i<lis.length; i++) 
    {
      // If the LI has another menu level 
      if(lis[i].lastChild.tagName=="UL") 
      {
        var iframeEl = document.createElement("IFRAME");
        iframeEl.frameBorder = 0;
        iframeEl.src = "javascript:;";
        iframeEl.style.position = "absolute";
        iframeEl.style.display = "none";        
        iframeEl.style.filter = "progid:DXImageTransform.Microsoft.Alpha(style=0,opacity=0)";
        lis[i].iframeEl = lis[i].parentNode.insertBefore(iframeEl, lis[i]);
        
        // assign the function to the LI 
       	lis[i].onmouseover = function() {	
          // display the inner menu
          this.lastChild.style.display = "block";                    

          // display the iframe           
       	  var element = this.lastChild;
       	  var valueTop = 0;
       	  var valueLeft = 0;
       	  
          do {
            valueTop += element.offsetTop  || 0;
            valueLeft += element.offsetLeft || 0;
            element = element.offsetParent;
          } while (element);       	  
          
          this.iframeEl.style.left     = valueLeft;
          this.iframeEl.style.top      = valueTop;
          this.iframeEl.style.width    = this.lastChild.offsetWidth + "px";
          this.iframeEl.style.height   = this.lastChild.offsetHeight + "px";
          this.iframeEl.style.display  = "";
        }             	  
        
       	lis[i].onmouseout = function() {	        
           this.lastChild.style.display = "none";
           this.iframeEl.style.display  = "none";
        }
      }
    }
  }
}