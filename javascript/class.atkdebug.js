if (!window.ATK) {
  var ATK = {};
}

ATK.Debug = {
  ELEMENT_ID: 'atk_debugging_div',
  
  EXPANDED: 'expanded',
  COLLAPSED: 'collapsed',
  
  BLOCK_CLASS: 'atkDebugBlock',
  EXPANDED_CLASS: 'atkDebugExpanded',
  COLLAPSED_CLASS: 'atkDebugCollapsed',
  
  /**
   * Register stylesheet.
   */
  registerStylesheet: function(stylesheet) {
    document.writeln('<link href="' + stylesheet + '" rel="stylesheet" type="text/css" media="all" />');
  },
  
  /**
   * Add content to the debugging div.
   */
  addContent: function(content) {
    var el = document.getElementById(ATK.Debug.ELEMENT_ID);
    if (el != null) {
      el.innerHTML += content;
    }
  },
  
  /**
   * Update cookie which stores whatever the first debug line was last 
   * expanded or collapsed. This cookie will expire in one year.
   */
  updateCookie: function(state) {
    var date = new Date();
    date.setTime(date.getTime() + 1000 * 60 * 60 * 24 * 365);
    document.cookie = 'atkdebugstate=' + state + '; expires=' + date.toGMTString() + '; path=/';
  },
  
  /**
   * Is first debug block?
   */
  isFirst: function(block) {
    var first = true;
    
    var previous = block.previousSibling;
    while (previous != null) {
      if (previous.className && previous.className.match(ATK.Debug.BLOCK_CLASS)) {
        first = false;
      }
      
      previous = previous.previousSibling;      
    }    
    
    return first;
  },
  
  /**
   * Toggle an ATK debug block.
   */
  toggle: function(toggle) {
    var block = toggle.parentNode;
    var first = ATK.Debug.isFirst(block);
    
    if (block.className.match(ATK.Debug.COLLAPSED_CLASS)) {    
      if (first) ATK.Debug.updateCookie(ATK.Debug.EXPANDED);
      block.className = block.className.replace(ATK.Debug.COLLAPSED_CLASS, ATK.Debug.EXPANDED_CLASS);    
    } else {
      if (first) ATK.Debug.updateCookie(ATK.Debug.COLLAPSED);
      block.className = block.className.replace(ATK.Debug.EXPANDED_CLASS, ATK.Debug.COLLAPSED_CLASS);    
    }
  }
};