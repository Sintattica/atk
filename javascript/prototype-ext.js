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