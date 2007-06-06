if (!window.ATK) {
  var ATK = {};
}

ATK.Debug = {
  ELEMENT_ID: 'atk_debugging_div',
  
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
   * Toggle an ATK debug block.
   */
  toggle: function(toggle) {
    var block = toggle.parentNode;

    if (block.className.match(ATK.Debug.COLLAPSED_CLASS)) {    
      block.className = block.className.replace(ATK.Debug.COLLAPSED_CLASS, ATK.Debug.EXPANDED_CLASS);    
    } else {
      block.className = block.className.replace(ATK.Debug.EXPANDED_CLASS, ATK.Debug.COLLAPSED_CLASS);    
    }
  }
};