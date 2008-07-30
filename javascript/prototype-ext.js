/**
 * This file contains overrides for the basic Prototype methods
 * to fix some minor issues or add some small functionality.
 */

// Override evalScripts to make sure scripts get executed in 
// the context of the current document. This makes it possible
// to add functions etc. in the document's context.
String.prototype.evalScripts = function() {
  return this.extractScripts().map(function(script) { 
    atkEval(script);
  });
};

function atkEval(script) {
	if (window.execScript) {
	  return window.execScript(script);
	} else if (navigator.userAgent.indexOf('Safari') != -1) {
	  window.setTimeout(script, 0);
	} else {
	  return window.eval(script);
	}      
};

if (Prototype.Browser.IE) {
  $w('positionedOffset viewportOffset').each(function(method) {
    Element.Methods[method] = Element.Methods[method].wrap(
      function(proceed, element) {
        element = $(element);
        var position = element.getStyle('position');
        if (position !== 'static') return proceed(element);
        // Trigger hasLayout on the offset parent so that IE6 reports
        // accurate offsetTop and offsetLeft values for position: fixed.
        var offsetParent = element.getOffsetParent();
        if (offsetParent && offsetParent.getStyle && offsetParent.getStyle('position') === 'fixed')
          offsetParent.setStyle({ zoom: 1 });
        element.setStyle({ position: 'relative' });
        var value = proceed(element);
        element.setStyle({ position: position });
        return value;
      }
    );
  });

  Element.Methods.getStyle = function(element, style) {
    element = $(element);
    style = (style == 'float' || style == 'cssFloat') ? 'styleFloat' : style.camelize();
    var value = ( element.style ? element.style[style] : '');
    if (!value && element.currentStyle) value = element.currentStyle[style];

    if (style == 'opacity') {
      if (value = (element.getStyle('filter') || '').match(/alpha\(opacity=(.*)\)/))
        if (value[1]) return parseFloat(value[1]) / 100;
      return 1.0;
    }

    if (value == 'auto') {
      if ((style == 'width' || style == 'height') && (element.getStyle('display') != 'none'))
        return element['offset' + style.capitalize()] + 'px';
      return null;
    }
    return value;
  };
}
