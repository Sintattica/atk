/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be 
 * included in the distribution.
 *
 * @package atk
 * @subpackage javascript
 *
 * @copyright (c)2000-2004 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

function mto_parse(link, value)
{
 var value_array = value.split('=');
 if(value_array[1]=='' || typeof value_array[1] == "undefined" ) return -1;
 var atkselector = value.replace("='", "_1253D_12527").replace("'", "_12527");
 return link.replace('REPLACEME', atkselector);
}

 
  
if (!window.ATK) {
  var ATK = {};
}

ATK.ManyToOneRelation = {
  /**
   * Clear many-to-one auto-complete selection.
   */
  clear: function(id, afterUpdate) {
    $(id).value = '';
    $(id + '_selectionbox').style.display ='none';
    $(id + '_selection').innerHTML = '';
    if (afterUpdate != null) afterUpdate();
  },  
  
  /**
   * Auto-update the attribute input form using Ajax.
   */
  autoupdate: function(searchField, resultField, selectionField, selectionBoxField, field, url, afterUpdate, minimumChars) {
    var elements = Form.getElements('entryform');
    var queryComponents = new Array();

    for (var i = 0; i < elements.length; i++) {
      if (elements[i].name && elements[i].name.substring(0, 3) != 'atk') {
        var queryComponent = Form.Element.serialize(elements[i]);
        if (queryComponent)
          queryComponents.push(queryComponent);
      }
    }

    var params = queryComponents.join('&');

    if(afterUpdate != null)
    {
      new ATK.ManyToOneRelation.Autocompleter(searchField, resultField, selectionField, selectionBoxField, field, url, { paramName: 'value', parameters: params, minChars: minimumChars, frequency: 0.5, afterUpdateElement: afterUpdate});
    }
    else
    {
      new ATK.ManyToOneRelation.Autocompleter(searchField, resultField, selectionField, selectionBoxField, field, url, { paramName: 'value', parameters: params, minChars: minimumChars, frequency: 0.5});
    }
  }
};

ATK.ManyToOneRelation.Autocompleter = Class.create();
Object.extend(Object.extend(ATK.ManyToOneRelation.Autocompleter.prototype, Ajax.Autocompleter.prototype), {
  initialize: function(element, update, selection, selectionBox, value, url, options) {
    Ajax.Autocompleter.prototype.initialize.apply(this, new Array(element, update, url, options));
    this.selection = $(selection);
    this.selectionBox = $(selectionBox)
    this.value = $(value);
    if (this.options.serializeForm)
      this.options.callback = function(el, entry) { return Form.serialize(el.form) + '&' + entry; }
  },
  
  findFirstNodeByClass: function(element, className) {
    var nodes = $(element).childNodes;
    for (var i = 0; i < nodes.length; i++)
    {
      if (nodes[i].nodeType != 3 && Element.hasClassName(nodes[i], className)) return nodes[i];
      else if (nodes[i].nodeType != 3)
      {
        node = this.findFirstNodeByClass(nodes[i], className)
        if (node != null) return node;
      }
    }
    return null;
  },  

  updateElement: function(selectedElement) {
    valueEl = this.findFirstNodeByClass(selectedElement, 'value');    
    selectionEl = this.findFirstNodeByClass(selectedElement, 'selection');
    
    this.element.value = '';
    this.element.focus();
    
    if (valueEl != null && selectionEl != null) {
      var value = valueEl.innerHTML;
      var selection = selectionEl.innerHTML;

      this.value.value = value;
    
      this.selection.innerHTML = selection;    
    
      this.selectionBox.style.display = '';    
      new Effect.Highlight(this.selectionBox);
    
      if (this.options.afterUpdateElement)
        this.options.afterUpdateElement(this.element, selectedElement);       
    }
  },
  
  // Fix for resetting the scollbar position.
  // See: http://dev.rubyonrails.org/ticket/4782  
  hide: function() {
    this.stopIndicator();
    if(Element.getStyle(this.update, 'display')!='none') 
    {
      this.options.onHide(this.element, this.update);
      this.element.value = ''; // clear value      
    }
    if(this.iefix) Element.hide(this.iefix);
    this.update.scrollTop = 0; 
  },

  // Fix autocompleter scrollbar support on IE.
  // See: http://dev.rubyonrails.org/ticket/4782
  onBlur: function(event) {
    if (navigator.appVersion.match(/\bMSIE\b/) &&
        Element.getStyle(this.update, 'display') != 'none' &&
        (this.update.offsetWidth - this.update.clientLeft * 2) > this.update.clientWidth) {
      // We have IE, the autocomplete box visible, and it has a vertical scrollbar. The last
      // check above and some calculations below assume equal left/right & top/bottom borders.
      var scrollbarLeft, scrollbarRight, scrollbarTop, scrollbarBottom,
        x = event.clientX, y = event.clientY;
      with (this.update) {
        scrollbarLeft = document.body.clientLeft + offsetLeft + clientLeft + clientWidth;
        scrollbarRight = document.body.clientLeft + offsetLeft + offsetWidth - clientLeft;
        scrollbarTop = document.body.clientTop + offsetTop + clientTop;
        scrollbarBottom = scrollbarTop + clientHeight;
      }
      if (x >= scrollbarLeft && x <= scrollbarRight && y >= scrollbarTop && y <= scrollbarBottom) {
        this.element.focus();
        return;
      }
    }
    
    // needed to make click events working
    setTimeout(this.hide.bind(this), 250);
    this.hasFocus = false;
    this.active = false; 
  }
});

