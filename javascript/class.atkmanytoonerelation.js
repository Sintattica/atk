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
   * Auto-complete search field using Ajax.
   */
  completeSearch: function(searchField, update, url, minimumChars) {
    new ATK.ManyToOneRelation.Autocompleter(searchField, update, url, { paramName: 'value', minChars: minimumChars, frequency: 0.5 });
  },

  /**
   * Auto-complete edit field using Ajax.
   */
  completeEdit: function(searchField, resultField, valueField, spinnerElement, url, afterUpdate, minimumChars) {
    new ATK.ManyToOneRelation.AdvancedAutocompleter(searchField, resultField, valueField, url, { paramName: 'value', minChars: minimumChars, frequency: 0.5, afterUpdateElement: afterUpdate, serializeForm: true, indicator: spinnerElement});
  },

  refreshEditDialogUrl: function(url) {
    var params = Form.serialize('dialogform');
    new Ajax.Request(url, { method: 'post', parameters: params, onComplete: function(transport) { transport.responseText.evalScripts(); }});
  },

  refreshEditDialogAttribute: function(url) {
    var params = Form.serialize('dialogform');
    new Ajax.Request(url, { method: 'post', parameters: params, onComplete: function(transport) { transport.responseText.evalScripts(); }});
  }
};


ATK.ManyToOneRelation.Autocompleter = Class.create();
Object.extend(Object.extend(ATK.ManyToOneRelation.Autocompleter.prototype, Ajax.Autocompleter.prototype), {
  initialize: function(element, update, url, options) {
    Ajax.Autocompleter.prototype.initialize.apply(this, new Array(element, update, url, options));
  },

  // Fix for resetting the scollbar position.
  // See: http://dev.rubyonrails.org/ticket/4782
  hide: function() {
    this.stopIndicator();
    if(Element.getStyle(this.update, 'display')!='none')
    {
      this.options.onHide(this.element, this.update);
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
  },

  onComplete: function(request) {
    this.updateChoices(request.responseText.stripScripts());
    request.responseText.evalScripts();
  }
});

ATK.ManyToOneRelation.AdvancedAutocompleter = Class.create();
Object.extend(Object.extend(ATK.ManyToOneRelation.AdvancedAutocompleter.prototype, ATK.ManyToOneRelation.Autocompleter.prototype), {
  initialize: function(element, update, valueElement, url, options) {
    ATK.ManyToOneRelation.Autocompleter.prototype.initialize.apply(this, new Array(element, update, url, options));

    this.valueElement = $(valueElement);

    if (this.options.serializeForm) {
      this.options.callback = this.parametersCallback.bind(this);
    }
  },

  parametersCallback: function(element, entry) {
    var elements = Form.getElements(element.form).findAll(function(el) {
      return el.name && el.name.substring(0, 3) != 'atk'
    });

    var queryComponents = elements.collect(function(el) {
      return Form.Element.serialize(el);
    }).concat([entry]);

    return queryComponents.join('&');
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

  // Fix for resetting the scollbar position.
  // See: http://dev.rubyonrails.org/ticket/4782
  hide: function() {
    this.stopIndicator();
    if(Element.getStyle(this.update, 'display')!='none')
    {
      this.options.onHide(this.element, this.update);

      // needed to clear the value if the user has entered a search
      // query but doesn't select anything from the list
      if (this.clearValue) {
        this.valueElement.value = '';
        this.element.value = ''; // clear value
      }
    }
    if(this.iefix) Element.hide(this.iefix);
    this.update.scrollTop = 0;
  },

  updateElement: function(selectedElement) {
    var valueEl = this.findFirstNodeByClass(selectedElement, 'value');
    var labelEl = this.findFirstNodeByClass(selectedElement, 'selection');

    var value = valueEl != null ? valueEl.innerHTML : '';
    var label = labelEl != null ? labelEl.innerHTML : '';

    this.valueElement.value = value;
    this.element.value = label;
    this.element.focus();
    this.element.select();

    this.clearValue = false;

    if (this.options.afterUpdateElement)
      this.options.afterUpdateElement(this.element, selectedElement);
  },

  onKeyPress: function(event) {
    if (!this.active && (event.keyCode==Event.KEY_TAB || event.keyCode==Event.KEY_RETURN ||
       (navigator.appVersion.indexOf('AppleWebKit') > 0 && event.keyCode == 0))) return;

    this.clearValue = true;

    ATK.ManyToOneRelation.Autocompleter.prototype.onKeyPress.apply(this, [event]);
  },

  clear: function() {
    this.element.value = '';
    this.valueElement.value = '';
    this.clearValue = false;

    if (this.options.afterUpdateElement)
      this.options.afterUpdateElement(this.element, null);
  },

  onObserverEvent: function() {
    if (this.getToken().length < this.options.minChars) {
      this.clear();
    }

    if (!this.hasFocus)
    {
      if (this.clearValue)
        this.clear();
      return;
    }

    ATK.ManyToOneRelation.Autocompleter.prototype.onObserverEvent.apply(this);
  }
});