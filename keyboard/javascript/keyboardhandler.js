// Key definitions.
KEY_DOWN  = 40;
KEY_RIGHT = 39;
KEY_UP    = 38;
KEY_LEFT  = 37;
KEY_DEL   = 46;
KEY_E     = 69;
KEY_V     = 86;

// Directions
DIR_DOWN = 1;
DIR_UP   = 2;
DIR_NONE = 3;

// Detect if we are dealing with netscape/mozilla.
var ver = navigator.appVersion; 
var len = ver.length;
for (var iln = 0; iln < len; iln++) 
{
  if (ver.charAt(iln) == "(") break;
}

var isNetscape = (ver.charAt(iln+1).toUpperCase() != "C");

var keyListeners = new Array();
if (!document.getElementById('<%=getFocusElement%>').focus())
{
  var focussedListener = -1;
}

/**
 * The generic keylistener base class.
 */
function atkGKeyListener()
{  
}

atkGKeyListener.prototype.handleKey = function(key, ctrl, shift)
{
  alert("you pressed "+key+" ctrl: "+ctrl+" shift: "+shift);
}

atkGKeyListener.prototype.focus = function(direction)
{
  // Default listener does not know how to receive focus.
}

atkGKeyListener.prototype.blur = function()
{
  // Default listener does not know how to lose focus.
}

/**
 * The keylistener class for form elements.
 */
function atkFEKeyListener(elementId, onUp, onDown, onLeft, onRight, onCtrl)
{
  this.elementId = elementId;
  this.onUp = onUp;
  this.onDown = onDown;
  this.onLeft = onLeft;
  this.onRight = onRight;
  this.onCtrl = onCtrl;
  this.id = -1;
  
  // Hook ourselves an onfocus event to keep track of focus, when using the mouse.
  var el = document.getElementById(this.elementId);
  el.listener = this; // Give the element a pointer to the listener, so we can always access it. 
  el.onfocus = function() 
  { 
    if (this.listener.id != focussedListener)
    {
      kb_defocus(); // Another element was focussed before.
    }
    focussedListener = this.listener.id; 
  }
}

atkFEKeyListener.prototype = new atkGKeyListener();
atkFEKeyListener.superclass = atkGKeyListener.prototype;

atkFEKeyListener.prototype.handleKey = function(key, ctrl, shift)
{  
  if (!ctrl)
  {   
    if(key==KEY_DOWN && this.onDown) kb_focusNext();
    if(key==KEY_UP && this.onUp) kb_focusPrevious();
    if(key==KEY_LEFT && this.onLeft) kb_focusPrevious();
    if(key==KEY_RIGHT && this.onRight) kb_focusNext();
  }
  else
  {  
    if ((key==KEY_DOWN||key==KEY_RIGHT) && this.onCtrl) kb_focusNext();
    if ((key==KEY_UP  ||key==KEY_LEFT)  && this.onCtrl) kb_focusPrevious();
  }
}

atkFEKeyListener.prototype.blur = function()
{
  var el = document.getElementById(this.elementId);
  if (el) el.blur();
}

atkFEKeyListener.prototype.focus = function(direction)
{
  var el = document.getElementById(this.elementId);
  if (el)
  {
    el.focus();
  }
  else
  {
    alert('listener not attached to an element!');
  }
}

/**
 The keyboard handler
 **/
function kb_addListener(listener)
{
  listener.id = keyListeners.length; // Let the listener know it's position in the array.
  keyListeners[keyListeners.length] = listener;  
}

function kb_init()
{
  document.onkeydown = kb_handleKey;
}

function kb_handleKey(e)
{ // handles a keypress
  var k = 0;
  var ctrl = false;
  var shift = false;  
  
  if (focussedListener<0)
  {
    kb_focusFirst();
  }
  
  if (focussedListener>=0)    // check if it's valid after the focusFirst check.
  {
    if (isNetscape)
    {
      k = e.which;
      var mod = parseInt(e.modifiers)
      ctrl = (mod & 2)==2;
      shift = (mod & 4)==4;  
    }
    else
    {
      k = window.event.keyCode;
      ctrl = window.event.ctrlKey;
      shift = window.event.shiftKey;
    }
    
    keyListeners[focussedListener].handleKey(k, ctrl, shift);  
  }
}

function kb_focusFirst()
{
  kb_defocus();
  if (keyListeners.length>0) focussedListener = 0;
  keyListeners[focussedListener].focus(DIR_DOWN);
}

function kb_focusLast()
{
  kb_defocus();
  if (keyListeners.length>0) focussedListener = keyListeners.length-1;
  keyListeners[focussedListener].focus(DIR_UP);
}

function kb_focusPrevious()
{
  if (focussedListener>0) 
  {
    kb_defocus(); // blur the previous one (we must do this, because we cannot
                  // assume that it is done automatically by the browser, 
                  // since we focus things that the browser cannot focus.
    focussedListener--;
    keyListeners[focussedListener].focus(DIR_UP);
  }
  else kb_focusLast();  
}

function kb_focusNext()
{
  if (focussedListener<keyListeners.length-1) 
  {
    kb_defocus();
    focussedListener++;
    keyListeners[focussedListener].focus(DIR_DOWN);
  }
  else kb_focusFirst();  
}

function kb_focus(num, direction)
{  
  if (num<keyListeners.length)
  {
    kb_defocus();
    focussedListener=num;
    keyListeners[focussedListener].focus(direction);
  }  
}

function kb_defocus()
{
  if (focussedListener>-1 && focussedListener<keyListeners.length)
  {
    keyListeners[focussedListener].blur(); // blur the currently focussed element.    
  }
}
