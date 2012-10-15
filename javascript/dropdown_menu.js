/**
 * DHTML list menu.
 *
 * Makes the CSS based list menu work in Internet Explorer.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision: 4893 $
 */
DHTMLListMenu = function(element) {
  this.initialize(element);
}

DHTMLListMenu.prototype = {
  /**
   * Initialize the menu.
   */
  initialize: function(element) {
    this.element = document.getElementById(element);
   
    // only needed for Internet Explorer
    if (!this.isIE()) {
      return;
    } 

    this.timer = null;
    this.isOld = this.isOld();
    
    this.changeList = [];
    
    var listItems = this.element.getElementsByTagName('LI');  
    for (i = 0; i < listItems.length; i++) {      
      if (listItems[i].lastChild.tagName == 'UL') {
        if (this.isOld) {
          this.installIFrame(listItems[i]);      
        }
        
        this.registerListeners(listItems[i]);
      }
    }    
  },
  
  /**
   * Register mouse listeners for the
   * given listItem.
   */
  registerListeners: function(listItem) {
    var listMenu = this;
  
    listItem.onmouseover = function() {
      listMenu.handleMouseOver(listItem);
    }
    
    listItem.onmouseout = function() {
      listMenu.handleMouseOut(listItem);    
    }
  },

  /**
   * Handle mouse over event. We don't handle this event immediately 
   * the order of events we get from Internet Explorer is not how
   * we would like it to be.
   */
  handleMouseOver: function(listItem) {
    this.setTimer();
    listItem.active = true;
    this.changeList.push(listItem);
  },

  /**
   * Handle mouse out event. We don't handle this event immediately 
   * the order of events we get from Internet Explorer is not how
   * we would like it to be.
   */
  handleMouseOut: function(listItem) {
    this.setTimer();
    listItem.active = false;    
    this.changeList.push(listItem);    
  },
  
  /**
   * Re(-set) the timer that processes past mouse over/out events.
   */  
  setTimer: function() {
    if (this.timer) {
      window.clearTimeout(this.timer);
      this.timer = null;
    }
    
    var listMenu = this;
    this.timer = window.setTimeout(
      function() { 
        listMenu.handleTimeout(); 
      }, 
      100
    );
  },
  
  /**
   * Is the given list item active? A list item is active
   * it the item itself or one of it's submenu's is active.
   */
  isActive: function(listItem) {
    if (listItem.active) {
      return true;
    }
    
    var childListItems = listItem.getElementsByTagName('li');
    for (var i = 0; i < childListItems.length; i++) {
      if (childListItems[i].active) {
        return true;
      }
    }
    
    return false;
  },
  
  /**
   * Show the given submenu.
   */
  show: function(listItem) {
    // display the inner menu
    var ul = listItem.lastChild;
    ul.style.display = "block";    

    // add hover class to child elements
    for (var i = 0; i < listItem.childNodes.length; i++) {
      if (listItem.childNodes[i].tagName && !listItem.childNodes[i].className.match(' hover')) {
        listItem.childNodes[i].className += ' hover';
      }
    }

    // display the iframe (if needed)
    if (this.isOld) {
      var valueTop = 0;
      var valueLeft = 0;
      
      var element = ul;
      
      do {
        valueTop += element.offsetTop  || 0;
        valueLeft += element.offsetLeft || 0;
        element = element.offsetParent;
      } while (element);       	  
      
      listItem.iframe.style.left = valueLeft;
      listItem.iframe.style.top = valueTop;
      listItem.iframe.style.width = ul.offsetWidth + 'px';
      listItem.iframe.style.height = ul.offsetHeight + 'px';
      listItem.iframe.style.display = ''; 
    }  
  },
  
  /**
   * Hide the given submenu.
   */
  hide: function(listItem) {
    // hide the inner menu
    listItem.lastChild.style.display = 'none';
    
    // remove the hover class from the child elements
    for (var i = 0; i < listItem.childNodes.length; i++) {
      if (listItem.childNodes[i].tagName) {
        listItem.childNodes[i].className = 
          listItem.childNodes[i].className.replace(' hover', '');
      }
    }    
    
    // hide the iframe (if needed)
    if (this.isOld) {
      listItem.iframe.style.display = 'none';  
    }
  },
  
  /**
   * Hide/show submenu's depending on the past mouse over/out events.
   */
  handleTimeout: function() {
    for (var i = 0; i < this.changeList.length; i++) {
      var listItem = this.changeList[i];
      var active = this.isActive(listItem);
      
      if (active) {
        this.show(listItem);
      } else  {
        this.hide(listItem);
      }
    }
  
    this.changeList = [];
  },
  
  /**
   * Install iframe that fixes overlap issues in Internet Explorer < 7.0
   * with select boxes, flash, java etc.
   */
  installIFrame: function(listItem) {
    // create iframe
    var iframe = document.createElement("IFRAME");
    iframe.frameBorder = 0;
    iframe.src = "javascript:;";
    iframe.style.position = 'absolute';
    iframe.style.display = 'none';        
    iframe.style.filter = 'progid:DXImageTransform.Microsoft.Alpha(style=0,opacity=0)';
    
    // install iframe
    listItem.iframe = listItem.parentNode.insertBefore(iframe, listItem);      
  },
  
  /**
   * Does the user use Internet Explorer?
   */
  isIE: function() {
    return document.all && this.element.currentStyle;
  },
  
  /**
   * Does the user use an older version of Internet Explorer (older then version 7.0)?
   */
  isOld: function() {
    var version = navigator.appVersion
    return version.indexOf("MSIE 4") > -1 || version.indexOf("MSIE 5") > -1 || version.indexOf("MSIE 6") > -1;
  }
}

/**
 * Activate menu, please use new DHTMLListMenu(...) instead.
 */
activateMenu = function(element) {
  if (typeof(console) != 'undefined' && console.debug) {
    console.debug('Use of deprecated activateMenu method, please use new DHTMLListMenu(...) instead!');
  }
  
  new DHTMLListMenu(element);
}