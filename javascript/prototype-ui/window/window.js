/*  Prototype-UI, version trunk
 *
 *  Prototype-UI is freely distributable under the terms of an MIT-style license.
 *  For details, see the PrototypeUI web site: http://www.prototype-ui.com/
 *
 *--------------------------------------------------------------------------*/

if(typeof Prototype == 'undefined' || !Prototype.Version.match("1.6"))
  throw("Prototype-UI library require Prototype library >= 1.6.0");

if (Prototype.Browser.WebKit) {
  Prototype.Browser.WebKitVersion = parseFloat(navigator.userAgent.match(/AppleWebKit\/([\d\.\+]*)/)[1]);
  Prototype.Browser.Safari2 = (Prototype.Browser.WebKitVersion < 420);
}

if (Prototype.Browser.IE) {
  Prototype.Browser.IEVersion = parseFloat(navigator.appVersion.split(';')[1].strip().split(' ')[1]);
  Prototype.Browser.IE6 =  Prototype.Browser.IEVersion == 6;
  Prototype.Browser.IE7 =  Prototype.Browser.IEVersion == 7;
}

Prototype.falseFunction = function() { return false };
Prototype.trueFunction  = function() { return true  };

/*
Namespace: UI

  Introduction:
    Prototype-UI is a library of user interface components based on the Prototype framework.
    Its aim is to easilly improve user experience in web applications.

    It also provides utilities to help developers.

  Guideline:
    - Prototype conventions are followed
    - Everything should be unobstrusive
    - All components are themable with CSS stylesheets, various themes are provided

  Warning:
    Prototype-UI is still under deep development, this release is targeted to developers only.
    All interfaces are subjects to changes, suggestions are welcome.

    DO NOT use it in production for now.

  Authors:
    - Sébastien Gruhier, <http://www.xilinus.com>
    - Samuel Lebeau, <http://gotfresh.info>
*/

var UI = {
  Abstract: { },
  Ajax: { }
};
Object.extend(Class.Methods, {
  extend: Object.extend.methodize(),

  addMethods: Class.Methods.addMethods.wrap(function(proceed, source) {
    // ensure we are not trying to add null or undefined
    if (!source) return this;

    // no callback, vanilla way
    if (!source.hasOwnProperty('methodsAdded'))
      return proceed(source);

    var callback = source.methodsAdded;
    delete source.methodsAdded;
    proceed(source);
    callback.call(source, this);
    source.methodsAdded = callback;

    return this;
  }),

  addMethod: function(name, lambda) {
    var methods = {};
    methods[name] = lambda;
    return this.addMethods(methods);
  },

  method: function(name) {
    return this.prototype[name].valueOf();
  },

  classMethod: function() {
    $A(arguments).flatten().each(function(method) {
      this[method] = (function() {
        return this[method].apply(this, arguments);
      }).bind(this.prototype);
    }, this);
    return this;
  },

  // prevent any call to this method
  undefMethod: function(name) {
    this.prototype[name] = undefined;
    return this;
  },

  // remove the class' own implementation of this method
  removeMethod: function(name) {
    delete this.prototype[name];
    return this;
  },

  aliasMethod: function(newName, name) {
    this.prototype[newName] = this.prototype[name];
    return this;
  },

  aliasMethodChain: function(target, feature) {
    feature = feature.camelcase();

    this.aliasMethod(target+"Without"+feature, target);
    this.aliasMethod(target, target+"With"+feature);

    return this;
  }
});
Object.extend(Number.prototype, {
  // Snap a number to a grid
  snap: function(round) {
    return parseInt(round == 1 ? this : (this / round).floor() * round);
  }
});
/*
Interface: String

*/

Object.extend(String.prototype, {
  camelcase: function() {
    var string = this.dasherize().camelize();
    return string.charAt(0).toUpperCase() + string.slice(1);
  },

  /*
    Method: makeElement
      toElement is unfortunately already taken :/

      Transforms html string into an extended element or null (when failed)

      > '<li><a href="#">some text</a></li>'.makeElement(); // => LI href#
      > '<img src="foo" id="bar" /><img src="bar" id="bar" />'.makeElement(); // => IMG#foo (first one)

    Returns:
      Extended element

  */
  makeElement: function() {
    var wrapper = new Element('div'); wrapper.innerHTML = this;
    return wrapper.down();
  }
});
Object.extend(Array.prototype, {
  empty: function() {
    return !this.length;
  },

  extractOptions: function() {
    return this.last().constructor === Object ? this.pop() : { };
  },

  removeAt: function(index) {
    var object = this[index];
    this.splice(index, 1);
    return object;
  },

  remove: function(object) {
    var index;
    while ((index = this.indexOf(object)) != -1)
      this.removeAt(index);
    return object;
  },

  insert: function(index) {
    var args = $A(arguments);
    args.shift();
    this.splice.apply(this, [ index, 0 ].concat(args));
    return this;
  }
});
Element.addMethods({
  getScrollDimensions: function(element) {
    return {
      width:  element.scrollWidth,
      height: element.scrollHeight
    }
  },

  getScrollOffset: function(element) {
    return Element._returnOffset(element.scrollLeft, element.scrollTop);
  },

  setScrollOffset: function(element, offset) {
    element = $(element);
    if (arguments.length == 3)
      offset = { left: offset, top: arguments[2] };
    element.scrollLeft = offset.left;
    element.scrollTop  = offset.top;
    return element;
  },

  // returns "clean" numerical style (without "px") or null if style can not be resolved
  // or is not numeric
  getNumStyle: function(element, style) {
    var value = parseFloat($(element).getStyle(style));
    return isNaN(value) ? null : value;
  },

  // by Tobie Langel (http://tobielangel.com/2007/5/22/prototype-quick-tip)
  appendText: function(element, text) {
    element = $(element);
    text = String.interpret(text);
    element.appendChild(document.createTextNode(text));
    return element;
  }
});

document.whenReady = function(callback) {
  if (document.loaded)
    callback.call(document);
  else
    document.observe('dom:loaded', callback);
};

Object.extend(document.viewport, {
  // Alias this method for consistency
  getScrollOffset: document.viewport.getScrollOffsets,

  setScrollOffset: function(offset) {
    Element.setScrollOffset(Prototype.Browser.WebKit ? document.body : document.documentElement, offset);
  },

  getScrollDimensions: function() {
    return Element.getScrollDimensions(Prototype.Browser.WebKit ? document.body : document.documentElement);
  }
});
/*
Interface: UI.Options
  Mixin to handle *options* argument in initializer pattern.

  TODO: find a better example than Circle that use an imaginary Point function,
        this example should be used in tests too.

  It assumes class defines a property called *options*, containing
  default options values.

  Instances hold their own *options* property after a first call to <setOptions>.

  Example:
    > var Circle = Class.create(UI.Options, {
    >
    >   // default options
    >   options: {
    >     radius: 1,
    >     origin: Point(0, 0)
    >   },
    >
    >   // common usage is to call setOptions in initializer
    >   initialize: function(options) {
    >     this.setOptions(options);
    >   }
    > });
    >
    > var circle = new Circle({ origin: Point(1, 4) });
    >
    > circle.options
    > // => { radius: 1, origin: Point(1,4) }

  Accessors:
    There are builtin methods to automatically write options accessors. All those
    methods can take either an array of option names nor option names as arguments.
    Notice that those methods won't override an accessor method if already present.

     * <optionsGetter> creates getters
     * <optionsSetter> creates setters
     * <optionsAccessor> creates both getters and setters

    Common usage is to invoke them on a class to create accessors for all instances
    of this class.
    Invoking those methods on a class has the same effect as invoking them on the class prototype.
    See <classMethod> for more details.

    Example:
    > // Creates getter and setter for the "radius" options of circles
    > Circle.optionsAccessor('radius');
    >
    > circle.setRadius(4);
    > // 4
    >
    > circle.getRadius();
    > // => 4 (circle.options.radius)

  Inheritance support:
    Subclasses can refine default *options* values, after a first instance call on setOptions,
    *options* attribute will hold all default options values coming from the inheritance hierarchy.
*/

(function() {
  UI.Options = {
    methodsAdded: function(klass) {
      klass.classMethod($w(' setOptions allOptions optionsGetter optionsSetter optionsAccessor '));
    },

    // Group: Methods

    /*
      Method: setOptions
        Extends object's *options* property with the given object
    */
    setOptions: function(options) {
      if (!this.hasOwnProperty('options'))
        this.options = this.allOptions();

      this.options = Object.extend(this.options, options || {});
    },

    /*
      Method: allOptions
        Computes the complete default options hash made by reverse extending all superclasses
        default options.

        > Widget.prototype.allOptions();
    */
    allOptions: function() {
      var superclass = this.constructor.superclass, ancestor = superclass && superclass.prototype;
      return (ancestor && ancestor.allOptions) ?
          Object.extend(ancestor.allOptions(), this.options) :
          Object.clone(this.options);
    },

    /*
      Method: optionsGetter
        Creates default getters for option names given as arguments.
        With no argument, creates getters for all option names.
    */
    optionsGetter: function() {
      addOptionsAccessors(this, arguments, false);
    },

    /*
      Method: optionsSetter
        Creates default setters for option names given as arguments.
        With no argument, creates setters for all option names.
    */
    optionsSetter: function() {
      addOptionsAccessors(this, arguments, true);
    },

    /*
      Method: optionsAccessor
        Creates default getters/setters for option names given as arguments.
        With no argument, creates accessors for all option names.
    */
    optionsAccessor: function() {
      this.optionsGetter.apply(this, arguments);
      this.optionsSetter.apply(this, arguments);
    }
  };

  // Internal
  function addOptionsAccessors(receiver, names, areSetters) {
    names = $A(names).flatten();

    if (names.empty())
      names = Object.keys(receiver.allOptions());

    names.each(function(name) {
      var accessorName = (areSetters ? 'set' : 'get') + name.camelcase();

      receiver[accessorName] = receiver[accessorName] || (areSetters ?
        // Setter
        function(value) { return this.options[name] = value } :
        // Getter
        function()      { return this.options[name]         });
    });
  }
})();
/*
Namespace: CSS

  Utility functions for CSS/StyleSheet files access

  Authors:
    - Sébastien Gruhier, <http://www.xilinus.com>
    - Samuel Lebeau, <http://gotfresh.info>
*/

var CSS = (function() {
  // Code based on:
  //   - IE5.5+ PNG Alpha Fix v1.0RC4 (c) 2004-2005 Angus Turnbull http://www.twinhelix.com
  //   - Whatever:hover - V2.02.060206 - hover, active & focus (c) 2005 - Peter Nederlof * Peterned - http://www.xs4all.nl/~peterned/
  function fixPNG() {
   parseStylesheet.apply(this, $A(arguments).concat(fixRule));
  };

  function parseStylesheet() {
    var patterns = $A(arguments);
    var method = patterns.pop();

    // To avoid flicking background
    //document.execCommand("BackgroundImageCache", false, true);
    // Parse all document stylesheets
    var styleSheets = $A(document.styleSheets);
    if (patterns.length > 1) {
      styleSheets = styleSheets.select(function(css) {
        return patterns.any(function(pattern) {
          return css.href && css.href.match(pattern)
          });
      });
    }
    styleSheets.each(function(styleSheet) {fixStylesheet.call(this, styleSheet, method)});
  };

  // Fixes a stylesheet
  function fixStylesheet(stylesheet, method) {
    // Parse import files
    if (stylesheet.imports)
      $A(stylesheet.imports).each(fixStylesheet);

    var href = stylesheet.href || document.location.href;
    var docPath = href.substr(0, href.lastIndexOf('/'));
	  // Parse all CSS Rules
    $A(stylesheet.rules || stylesheet.cssRules).each(function(rule) { method.call(this, rule, docPath) });
  };

  var filterPattern = 'progid:DXImageTransform.Microsoft.AlphaImageLoader(src="#{src}",sizingMethod="#{method}")';

  // Fixes a rule if it has a PNG background
  function fixRule(rule, docPath) {
    var bgImg = rule.style.backgroundImage;
    // Rule with PNG background image
    if (bgImg && bgImg != 'none' && bgImg.match(/^url[("']+(.*\.png)[)"']+$/i)) {
      var src = RegExp.$1;
      var bgRepeat = rule.style.backgroundRepeat;
      // Relative path
      if (src[0] != '/')
        src = docPath + "/" + src;
      // Apply filter
      rule.style.filter = filterPattern.interpolate({
        src:    src,
        method: bgRepeat == "no-repeat" ? "crop" : "scale" });
      rule.style.backgroundImage = "none";
    }
  };

  var preloadedImages = new Hash();

  function preloadRule(rule, docPath) {
    var bgImg = rule.style.backgroundImage;
    if (bgImg && bgImg != 'none'  && bgImg != 'initial' ) {
      if (!preloadedImages.get(bgImg)) {
        bgImg.match(/^url[("']+(.*)[)"']+$/i);
        var src = RegExp.$1;
        // Relative path
        if (!(src[0] == '/' || src.match(/^file:/) || src.match(/^https?:/)))
          src = docPath + "/" + src;
        preloadedImages.set(bgImg, true);
        var image = new Image();
        image.src = src;
      }
    }
  }

  return {
    /*
       Method: fixPNG
         Fix transparency of PNG background of document stylesheets.
         (only on IE version<7, otherwise does nothing)

         Warning: All png background will not work as IE filter use for handling transparency in PNG
         is not compatible with all background. It does not support top/left position (so no CSS sprite)

         I recommend to create a special CSS file with png that needs to be fixed and call CSS.fixPNG on this CSS

         Examples:
          > CSS.fixPNG() // To fix all css
          >
          > CSS.fixPNG("mac_shadow.css") // to fix all css files with mac_shadow.css so mainly only on file
          >
          > CSS.fixPNG("shadow", "vista"); // To fix all css files with shadow or vista in their names

       Parameters
         patterns: (optional) list of pattern to filter css files
    */
    fixPNG: (Prototype.Browser.IE && Prototype.Browser.IEVersion < 7) ? fixPNG : Prototype.emptyFunction,

    // By Tobie Langel (http://tobielangel.com)
    //   inspired by http://yuiblog.com/blog/2007/06/07/style/
    addRule: function(css, backwardCompatibility) {
      if (backwardCompatibility) css = css + '{' + backwardCompatibility + '}';
      var style = new Element('style', { type: 'text/css', media: 'screen' });
      $(document.getElementsByTagName('head')[0]).insert(style);
      if (style.styleSheet) style.styleSheet.cssText = css;
      else style.appendText(css);
      return style;
    },

    preloadImages: function() {
      parseStylesheet.apply(this, $A(arguments).concat(preloadRule));
    }
  };
})();
UI.Benchmark = {
  benchmark: function(lambda, iterations) {
    var date = new Date();
    (iterations || 1).times(lambda);
    return (new Date() - date) / 1000;
  }
};
/*
  Group: Drag
    UI provides Element#enableDrag method that allow elements to fire drag-related events.

    Events fired:
      - drag:started : fired when a drag is started (mousedown then mousemove)
      - drag:updated : fired when a drag is updated (mousemove)
      - drag:ended   : fired when a drag is ended (mouseup)

    Notice it doesn't actually move anything, drag behavior has to be implemented
    by attaching handlers to drag events.

    Drag-related informations:
      event.memo contains useful information about the drag occuring:
        - dx         : difference between pointer x position when drag started
                       and actual x position
        - dy         : difference between pointer y position when drag started
                       and actual y position
        - mouseEvent : the original mouse event, useful to know pointer absolute position,
                       or if key were pressed.

    Example, with event handling for a specific element:

    > // Now "resizable" will fire drag-related events
    > $('resizable').enableDrag();
    >
    > // Let's observe them
    > $('resizable').observe('drag:started', function(event) {
    >   this._dimensions = this.getDimensions();
    > }).observe('drag:updated', function(event) {
    >   var drag = event.memo;
    >
    >   this.setStyle({
    >     width:  this._dimensions.width  + drag.dx + 'px',
    >     height: this._dimensions.height + drag.dy + 'px'
    >   });
    > });

    Example, with event delegating on the whole document:

    > // All elements in the having the "draggable" class name will fire drag events.
    > $$('.draggable').invoke('enableDrag');
    >
    > document.observe('drag:started', function(event) {
    >   UI.logger.info('trying to drag ' + event.element().id);
    > }):
*/
(function() {
  var initPointer, currentDraggable, dragging;

  document.observe('mousedown', onMousedown);

  function onMousedown(event) {
    var draggable = event.findElement('[ui:draggable="true"]');

    if (draggable) {
      // prevent default browser action
      event.stop();
      currentDraggable = draggable;
      initPointer = event.pointer();

      document.observe("mousemove", onMousemove)
              .observe("mouseup",   onMouseup);
    }
  };

  function onMousemove(event) {
    event.stop();

    if (dragging)
      fire('drag:updated', event);
    else {
      dragging = true;
      fire('drag:started', event);
    }
  };

  function onMouseup(event) {
    document.stopObserving('mousemove', onMousemove)
            .stopObserving('mouseup',   onMouseup);

    if (dragging) {
      dragging = false;
      fire('drag:ended', event);
    }
  };

  function fire(eventName, mouseEvent) {
    var pointer = mouseEvent.pointer();

    currentDraggable.fire(eventName, {
      dx: pointer.x - initPointer.x,
      dy: pointer.y - initPointer.y,
      mouseEvent: mouseEvent
    })
  };

  Element.addMethods({
    enableDrag: function(element) {
      element = $(element);
      element.writeAttribute('ui:draggable', 'true');
      return element;
    },

    disableDrag: function(element){
      element = $(element);
      element.writeAttribute('ui:draggable', null);
      return element;
    },

    isDraggable: function(element) {
      return $(element).readAttribute('ui:draggable') == 'true';
    }
  });
})();
/*
  Class: UI.IframeShim
    Handles IE6 bug when <select> elements overlap other elements with higher z-index

  Example:
    > // creates iframe and positions it under "contextMenu" element
    > this.iefix = new UI.IframeShim().positionUnder('contextMenu');
    > ...
    > document.observe('click', function(e) {
    >   if (e.isLeftClick()) {
    >     this.contextMenu.hide();
    >
    >     // hides iframe when left click is fired on a document
    >     this.iefix.hide();
    >   }
    > }.bind(this))
    > ...
*/

// TODO:
//
// Maybe it makes sense to bind iframe to an element
// so that it automatically calls positionUnder method
// when the element it's binded to is moved or resized
// Not sure how this might affect overall perfomance...

UI.IframeShim = Class.create(UI.Options, {

  /*
    Method: initialize
    Constructor

      Creates iframe shim and appends it to the body.
      Note that this method does not perform proper positioning and resizing of an iframe.
      To do that use positionUnder method

    Returns:
      this
  */
  initialize: function() {
    this.element = new Element('iframe', {
      style: 'position:absolute;filter:progid:DXImageTransform.Microsoft.Alpha(opacity=0);display:none',
      src: 'javascript:false;',
      frameborder: 0
    });
    $(document.body).insert(this.element);
  },

  /*
    Method: hide
      Hides iframe shim leaving its position and dimensions intact

    Returns:
      this
  */
  hide: function() {
    this.element.hide();
    return this;
  },

  /*
    Method: show
      Show iframe shim leaving its position and dimensions intact

    Returns:
      this
  */
  show: function() {
    this.element.show();
    return this;
  },

  /*
    Method: positionUnder
      Positions iframe shim under the specified element
      Sets proper dimensions, offset, zIndex and shows it
      Note that the element should have explicitly specified zIndex

    Returns:
      this
  */
  positionUnder: function(element) {
    var element = $(element),
        offset = element.cumulativeOffset(),
        dimensions = element.getDimensions(),
        style = {
          left: offset[0] + 'px',
          top: offset[1] + 'px',
          width: dimensions.width + 'px',
          height: dimensions.height + 'px',
          zIndex: element.getStyle('zIndex') - 1
        };
    this.element.setStyle(style).show();

    return this;
  },

  /*
    Method: setBounds
      Sets element's width, height, top and left css properties using 'px' as units

    Returns:
      this
  */
  setBounds: function(bounds) {
    for (prop in bounds) {
      bounds[prop] += 'px';
    }
    this.element.setStyle(bounds);
    return this;
  },

  /*
    Method: destroy
      Completely removes the iframe shim from the document

    Returns:
      this
  */
  destroy: function() {
    if (this.element)
      this.element.remove();

    return this;
  }
});
/*
Class: UI.Logger
*/

/*
  Group: Logging Facilities
    Prototype UI provides a facility to log message with levels.
    Levels are in order "debug", "info", "warn" and "error".

    As soon as the DOM is loaded, a default logger is present in UI.logger.

    This logger is :
    * an <ElementLogger> if $('log') is present
    * a <ConsoleLogger> if window.console is defined
    * a <MemLogger> otherwise

    See <AbstractLogger> to learn how to use it.

    Example:

    > UI.logger.warn('something bad happenned !');
*/

// Class: AbstractLogger

UI.Abstract.Logger = Class.create({
  /*
    Property: level
      The log level, default value is debug  <br/>
  */
  level: 'debug'
});

(function() {
  /*
    Method: debug
      Logs with "debug" level

    Method: info
      Logs with "info" level

    Method: warn
      Logs with "warn" level

    Method: error
      Logs with "error" level
  */
  var levels = $w(" debug info warn error ");

  levels.each(function(level, index) {
    UI.Abstract.Logger.addMethod(level, function(message) {
      // filter lower level messages
      if (index >= levels.indexOf(this.level))
        this._log({ level: level, message: message, date: new Date() });
    });
  });
})();

/*
  Class: NullLogger
    Does nothing
*/
UI.NullLogger = Class.create(UI.Abstract.Logger, {
  _log: Prototype.emptyFunction
});

/*
  Class: MemLogger
    Logs in memory

    Property: logs
      An array of logs, objects with "date", "level", and "message" properties
*/
UI.MemLogger = Class.create(UI.Abstract.Logger, {
  initialize: function() {
    this.logs = [ ];
  },

  _log: function(log) {
    this.logs.push(log);
  }
});

/*
  Class: ConsoleLogger
    Logs using window.console
*/
UI.ConsoleLogger = Class.create(UI.Abstract.Logger, {
  _log: function(log) {
    console[log.level || 'log'](log.message);
  }
});

/*
  Class: ElementLogger
    Logs in a DOM element
*/
UI.ElementLogger = Class.create(UI.Abstract.Logger, {
  /*
    Method: initialize
      Constructor, takes a DOM element to log into as argument
  */
  initialize: function(element) {
    this.element = $(element);
  },

  /*
    Property: format
      A format string, will be interpolated with "date", "level" and "message"

      Example:
        > "<p>(#{date}) #{level}: #{message}</p>"
  */
  format: '<p>(<span class="date">#{date}</span>) ' +
              '<span class="level">#{level}</span> : ' +
              '<span class="message">#{message}</span></p>',

  _log: function(log) {
    var entry = this.format.interpolate({
      level:   log.level.toUpperCase(),
      message: log.message.escapeHTML(),
      date:    log.date.toLocaleTimeString()
    });
    this.element.insert({ top: entry });
  }
});

document.observe('dom:loaded', function() {
  if ($('log'))             UI.logger = new UI.ElementLogger('log');
  else if (window.console)  UI.logger = new UI.ConsoleLogger();
  else                      UI.logger = new UI.MemLogger();
});
/*
Class: UI.Shadow
  Add shadow around a DOM element. The element MUST BE in ABSOLUTE position.

  Shadow can be skinned by CSS (see mac_shadow.css or drop_shadow.css).
  CSS must be included to see shadow.

  A shadow can have two states: focused and blur.
  Shadow shifts are set in CSS file as margin and padding of shadow_container to add visual information.

  Example:
    > new UI.Shadow("element_id");
*/
UI.Shadow = Class.create(UI.Options, {
  options: {
    theme: "mac_shadow",
    focus: false,
    zIndex: 100
  },

  /*
    Method: initialize
      Constructor, adds shadow elements to the DOM if element is in the DOM.
      Element MUST BE in ABSOLUTE position.

    Parameters:
      element - DOM element
      options - Hashmap of options
        - theme (default: mac_shadow)
        - focus (default: true)
        - zIndex (default: 100)

    Returns:
      this
  */
  initialize: function(element, options) {
    this.setOptions(options);

    this.element = $(element);
    this.create();
    if (Object.isElement(this.element.parentNode))
      this.render();
  },

  /*
    Method: destroy
      Destructor, removes elements from the DOM
  */
  destroy: function() {
    if (this.shadow.parentNode)
      this.remove();
  },

  // Group: Size and Position
  /*
    Method: setPosition
      Sets top/left shadow position in pixels

    Parameters:
      top -  top position in pixel
      left - left position in pixel

    Returns:
      this
  */
  setPosition: function(top, left) {
    if (this.shadowSize) {
      var shadowStyle = this.shadow.style;

      shadowStyle.top  = parseInt(top)  - this.shadowSize.top  + this.shadowShift.top + 'px';
      shadowStyle.left = parseInt(left) - this.shadowSize.left + this.shadowShift.left+ 'px';
    }
    return this;
  },

  /*
    Method: setSize
      Sets width/height shadow in pixels

    Parameters:
      width  - width in pixel
      height - height in pixel

    Returns:
      this
  */
  setSize: function(width, height) {
    if (this.shadowSize) {
      var w = parseInt(width) + this.shadowSize.width - this.shadowShift.width + "px";
      this.shadow.style.width = w;
      var h =  parseInt(height) - this.shadowShift.height + "px";

      // this.shadowContents[1].style.height = h;
      this.shadowContents[1].childElements().each(function(e) {e.style.height = h});
      this.shadowContents.each(function(item){ item.style.width = w});
    }
    return this;
  },

  /*
    Method: setBounds
      Sets shadow bounds in pixels

    Parameters:
      bounds - an Hash {top:, left:, width:, height:}

    Returns:
      this
  */
  setBounds: function(bounds) {
    return this.setPosition(bounds.top, bounds.left).setSize(bounds.width, bounds.height);
  },

  /*
    Method: setZIndex
      Sets shadow z-index

    Parameters:
      zIndex - zIndex value

    Returns:
      this
  */
  setZIndex: function(zIndex) {
    this.shadow.style.zIndex = zIndex;
    return this;
  },

   // Group: Render
  /*
    Method: show
      Displays shadow

    Returns:
      this
  */
  show: function() {
   this.shadow.show();
   return this;
  },

  /*
    Method: hide
      Hides shadow

    Returns:
      this
  */
  hide: function() {
    this.shadow.hide();
    return this;
  },

  /*
    Method: remove
      Removes shadow from the DOM

    Returns:
      this
  */
  remove: function() {
    this.shadow.remove();
    return this;
  },

  // Group: Status
  /*
    Method: focus
      Focus shadow.

      Change shadow shift. Shift values are set in CSS file as margin and padding of shadow_container
      to add visual information of shadow status.

    Returns:
      this
  */
  focus: function() {
    this.options.focus = true;
    this.updateShadow();
    return this;
  },

  /*
    Method: blur
      Blurs shadow.

      Change shadow shift. Shift values are set in CSS file as margin and padding of shadow_container
      to add visual information of shadow status.

    Returns:
      this
  */
  blur: function() {
    this.options.focus = false;
    this.updateShadow();
    return this;
  },

  // Private Functions
  // Adds shadow elements to DOM, computes shadow size and displays it
  render: function() {
    if (this.element.parentNode && !Object.isElement(this.shadow.parentNode)) {
      this.element.parentNode.appendChild(this.shadow);
      this.computeSize();
      this.setBounds(Object.extend(this.element.getDimensions(), this.getElementPosition()));
      this.shadow.show();
    }
    return this;
  },

  // Creates HTML elements without inserting them into the DOM
  create: function() {
    var zIndex = this.element.getStyle('zIndex');
    if (!zIndex)
      this.element.setStyle({zIndex: this.options.zIndex});
    zIndex = (zIndex || this.options.zIndex) - 1;

    this.shadowContents = new Array(3);
    this.shadowContents[0] = new Element("div")
      .insert(new Element("div", {className: "shadow_center_wrapper"}).insert(new Element("div", {className: "n_shadow"})))
      .insert(new Element("div", {className: "shadow_right ne_shadow"}))
      .insert(new Element("div", {className: "shadow_left nw_shadow"}));

    this.shadowContents[1] = new Element("div")
      .insert(new Element("div", {className: "shadow_center_wrapper c_shadow"}))
      .insert(new Element("div", {className: "shadow_right e_shadow"}))
      .insert(new Element("div", {className: "shadow_left w_shadow"}));
    this.centerElements = this.shadowContents[1].childElements();

    this.shadowContents[2] = new Element("div")
      .insert(new Element("div", {className: "shadow_center_wrapper"}).insert(new Element("div", {className: "s_shadow"})))
      .insert(new Element("div", {className: "shadow_right se_shadow"}))
      .insert(new Element("div", {className: "shadow_left sw_shadow"}));

    this.shadow = new Element("div", {className: "shadow_container " + this.options.theme,
                                      style: "position:absolute; top:-10000px; left:-10000px; display:none; z-index:" + zIndex })
      .insert(this.shadowContents[0])
      .insert(this.shadowContents[1])
      .insert(this.shadowContents[2]);
  },

  // Compute shadow size
  computeSize: function() {
    if (this.focusedShadowShift)
      return;
    this.shadow.show();

    // Trick to get shadow shift designed in CSS as padding
    var content = this.shadowContents[1].select("div.c_shadow").first();
    this.unfocusedShadowShift = {};
    this.focusedShadowShift = {};

    $w("top left bottom right").each(function(pos) {this.unfocusedShadowShift[pos] = content.getNumStyle("padding-" + pos) || 0}.bind(this));
    this.unfocusedShadowShift.width  = this.unfocusedShadowShift.left + this.unfocusedShadowShift.right;
    this.unfocusedShadowShift.height = this.unfocusedShadowShift.top + this.unfocusedShadowShift.bottom;

    $w("top left bottom right").each(function(pos) {this.focusedShadowShift[pos] = content.getNumStyle("margin-" + pos) || 0}.bind(this));
    this.focusedShadowShift.width  = this.focusedShadowShift.left + this.focusedShadowShift.right;
    this.focusedShadowShift.height = this.focusedShadowShift.top + this.focusedShadowShift.bottom;

    this.shadowShift = this.options.focus ? this.focusedShadowShift : this.unfocusedShadowShift;

    // Get shadow size
    this.shadowSize  = {top:    this.shadowContents[0].childElements()[1].getNumStyle("height"),
                        left:   this.shadowContents[0].childElements()[1].getNumStyle("width"),
                        bottom: this.shadowContents[2].childElements()[1].getNumStyle("height"),
                        right:  this.shadowContents[0].childElements()[2].getNumStyle("width")};

    this.shadowSize.width  = this.shadowSize.left + this.shadowSize.right;
    this.shadowSize.height = this.shadowSize.top + this.shadowSize.bottom;

    // Remove padding
    content.setStyle("padding:0; margin:0");
    this.shadow.hide();
  },

  // Update shadow size (called when it changes from focused to blur and vice-versa)
  updateShadow: function() {
    this.shadowShift = this.options.focus ? this.focusedShadowShift : this.unfocusedShadowShift;
    var shadowStyle = this.shadow.style, pos  = this.getElementPosition(), size = this.element.getDimensions();

    shadowStyle.top  =  pos.top    - this.shadowSize.top   + this.shadowShift.top   + 'px';
    shadowStyle.left  = pos.left   - this.shadowSize.left  + this.shadowShift.left  + 'px';
    shadowStyle.width = size.width + this.shadowSize.width - this.shadowShift.width + "px";
    var h = size.height - this.shadowShift.height + "px";
    this.centerElements.each(function(e) {e.style.height = h});

    var w = size.width + this.shadowSize.width - this.shadowShift.width+ "px";
    this.shadowContents.each(function(item) { item.style.width = w });
  },

  // Get element position in integer values
  getElementPosition: function() {
    return {top: this.element.getNumStyle("top"), left: this.element.getNumStyle("left")}
  }
});

// Set theme and focus as read/write accessor
document.whenReady(function() { CSS.fixPNG("shadow") });
/*
Class: UI.Window
  Main class to handle windows inside a web page.

  Example:
    > new UI.Window({ theme: 'bluglighting' }).show()
*/


/*
<div class="STitle">Options</div>
*/

UI.Window = Class.create(UI.Options, {
  // Group: Options
  options: {

    // Property: theme
    //   window theme, uses the window manager theme as default
    theme:         null,

    // Property: shadowTheme
    //   window shadow theme, uses the window manager one as default
    //   Only useful if <shadow> options is true, see <UI.Shadow> for details
    shadowTheme:   null,

    // Property: id
    //   id ot the window, generated by default
    id:            null,

    // Property: windowManager
    //   window manager that manages this window,
    //   uses UI.defaultWM as default
    windowManager: null,

    top:           null,
    left:          null,
    width:         200,
    height:        300,
    minHeight:     100,
    minWidth:      200,
    maxHeight:     null,
    maxWidth:      null,
    altitude:      "front",

    // Property: resizable
    //   true by default
    resizable:     true,

    // Property: draggable
    //   true by default
    draggable:     true,

    // Property: wired
    //   draw wires around window when dragged, false by default
    wired:         false,

    // Property: show
    //   Function used to show the window, default is Element.show
    show: Element.show,

    // Property: hide
    //   Function used to hide the window, default is Element.hide.
    hide: Element.hide,

    // Property: superflousEffects
    //   uses superflous effects when resizing or moving window.
    //   it's true if Scriptaculous' Effect is defined, false otherwise
    superflousEffects: !Object.isUndefined(window.Effect),

    // Property: shadow
    //   draw shadow around the window, default is false
    shadow:            false,

    // Property: activeOnClick
    //   When set to true, a click on an blurred window content activates it,
    //   default is true
    activeOnClick:     true,

    // Grid
    gridX:  1,
    gridY:  1,

    // Buttons and actions (false to disable)

    // Property: close
    //   Window method name as string, or false to disable close button
    //   Default is 'destroy'
    close:    'destroy',

    // Property: minimize
    //   Window method name as string, or false to disable minimize button
    //   Default is 'toggleFold'
    minimize: 'toggleFold',

    // Property: maximize
    //   Window method name as string, or false to disable maximize button
    //   Default is 'toggleMaximize'
    maximize: 'toggleMaximize'
  },

  // Group: Attributes

  /*
    Property: id
      DOM id of the window's element

    Property: element
      DOM element containing the window

    Property: windowManager
      Window manager that manages the window

    Property: content
      Window content element

    Property: header
      Window header element

    Property: footer
      Window footer element

    Property: visible
      true if window is visible

    Property: focused
      true if window is focused

    Property: folded
      true if window is folded

    Property: maximized
      true if window is maximized
  */

  /*
    Group: Events
    List of events fired by a window
  */

  /*
    Property: created
      Fired after creating the window

    Property: destroyed
      Fired after destroying the window

    Property: showing
      Fired when showing a window

    Property: shown
      Fired after showing effect

    Property: hiding
      Fired when hiding a window

    Property: hidden
      Fired after hiding effect

    Property: focused
      Fired after focusing the window

    Property: blurred
      Fired after bluring the window

    Property: maximized
      Fired after maximizing the window

    Property: restored
      Fired after restoring the window from its maximized state

    Property: fold
      Fired after unfolding the window

    Property: unfold
      Fired after folding the window

    Property: altitude:changed
      Fired when window altitude has changed (z-index)

    Property: size:changed
      Fired when window size has changed

    Property: position:changed
      Fired when window position has changed

    Property: move:started
      Fired when user has started a moving a window, position:changed are then fired continously

    Property: move:ended
      Fired when user has finished moving a window

    Property: resize:started
      Fired when user has started resizing window, size:changed are then fired continuously

    Property: resize:ended
      Fired when user has finished resizing window

  */

  // Group: Contructor

  /*
    Method: initialize
      Constructor, should not be called directly, it's called by new operator (new Window())
      The window is not open and nothing has been added to the DOM yet

    Parameters:
      options - (Hash) list of optional parameters

    Returns:
      this
  */
  initialize: function(options) {
    this.setOptions(options);
    this.windowManager = this.options.windowManager || UI.defaultWM;
    this.create();
    this.id = this.element.id;
    this.windowManager.register(this);
    this.render();
    if (this.options.activeOnClick)
      this.overlay.setStyle({ zIndex: this.lastZIndex + 1 }).show();
  },

  /*
    Method: destroy
      Destructor, closes window, cleans up DOM and memory
  */
  destroy: function($super) {
    this.hide();
    if (this.centerOptions)
      Event.stopObserving(this.windowManager.scrollContainer, "scroll", this.centerOptions.handler);
    this.windowManager.unregister(this);
    this.fire('destroyed');
  },

  // Group: Event handling

  /*
    Method: fire
      Fires a window custom event automatically namespaced in "window:" (see Prototype custom events).
      The memo object contains a "window" property referring to the window.

    Example:
      > UI.Window.addMethods({
      >   iconify: function() {
      >     // ... your iconifying code here ...
      >     this.fire('iconified');
      >     // chain friendly
      >     return this;
      >   }
      > });
      >
      > document.observe('window:iconified', function(event) {
      >   alert("Window with id " + event.memo.window.id + " has just been iconified");
      > });

    Parameters:
      eventName - an event name
      memo - a memo object

    Returns:
      fired event
  */
  fire: function(eventName, memo) {
    memo = memo || { };
    memo.window = this;
    return this.element.fire('window:' + eventName, memo);
  },

   /*
     Method: observe
       Observe a window event with a handler function automatically bound to the window

     Parameters:
       eventName - an event name
       handler - a handler function

     Returns:
       this
  */
  observe: function(eventName, handler) {
    this.element.observe('window:' + eventName, handler.bind(this));
    return this;
  },


  // Group: Actions

  /*
    Method: show
      Opens the window (appends it to the DOM)

    Parameters:
      modal - open the window in a modal mode (default false)

    Returns:
      this
 */
  show: function(modal) {
    if (this.visible) return this;

    this.fire('showing');
    this.effect('show');

    if (modal) {
      this.windowManager.startModalSession(this);
      this.modalSession = true;
    }

    this.addElements();
    this.visible = true;

    new PeriodicalExecuter(function(executer) {
      if (!this.element.visible()) return;
      this.fire('shown');
      executer.stop();
    }.bind(this), 0.1);

    return this;
  },

  /*
    Method: hide
       Hides the window, (removes it from the DOM)

     Returns:
       this
  */
  hide: function() {
    if (!this.visible) return this;

    this.fire('hiding');
    this.effect('hide');

    if (this.modalSession) {
      this.windowManager.endModalSession(this);
      this.modalSession = false;
    }

    this.windowManager.hide(this);

    new PeriodicalExecuter(function(executer) {
      if (this.element.visible()) return;
      this.visible = false;
      this.element.remove();
      this.fire('hidden');
      executer.stop();
    }.bind(this), 0.1);

    return this;
  },

  close: function() {
    return this.action('close');
  },

  /*
    Method: activate
      Brings window to the front and sets focus on it

     Returns:
       this
  */
  activate: function() {
    return this.bringToFront().focus();
  },

  /*
    Method: bringToFront
      Brings window to the front (but does not set focus on it)

     Returns:
       this
  */
  bringToFront: function() {
    return this.setAltitude('front');
  },

  /*
    Method: sendToBack
      Sends window to the back (without changing its focus)

     Returns:
       this
  */
  sendToBack: function() {
    return this.setAltitude('back');
  },

  /*
    Method: focus
      Focuses the window (without bringing window to the front)

     Returns:
       this
  */
  focus: function() {
    if (this.focused) return this;

    this.windowManager.focus(this);
    // Hide the overlay that catch events
    this.overlay.hide();
    // Add focused class name
    this.element.addClassName(this.options.theme + '_focused');

    this.focused = true;
    this.fire('focused');
    return this;
  },

  /*
    Method: blur
      Blurs the window (without changing windows order)

     Returns:
       this
  */
  blur: function() {
    if (!this.focused) return this;

    this.windowManager.blur(this);
    this.element.removeClassName(this.options.theme + '_focused');

    // Show the overlay to catch events
    if (this.options.activeOnClick)
      this.overlay.setStyle({ zIndex: this.lastZIndex + 1 }).show();

    this.focused = false;
    this.fire('blurred');
    return this;
  },

  /*
    Method: maximize
      Maximizes window inside its viewport (managed by WindowManager)
      Makes window take full size of its viewport

     Returns:
       this
  */
  maximize: function() {
    if (this.maximized) return this;

    // Get bounds has to be before  this.windowManager.maximize for IE!! this.windowManager.maximize remove overflow
    // and it breaks this.getBounds()
    var bounds = this.getBounds();
    if (this.windowManager.maximize(this)) {
      this.disableButton('minimize').setResizable(false).setDraggable(false);

      this.activate();
      this.maximized = true;
      this.savedArea = bounds;
      var newBounds = Object.extend(this.windowManager.viewport.getDimensions(), { top: 0, left: 0 });
      this[this.options.superflousEffects && !Prototype.Browser.IE ? "morph" : "setBounds"](newBounds);
      this.fire('maximized');
      return this;
    }
  },

  /*
    Function: restore
      Restores a maximized window to its initial size

     Returns:
       this
  */
  restore: function() {
    if (!this.maximized) return this;

    if (this.windowManager.restore(this)) {
      this[this.options.superflousEffects  && !Prototype.Browser.IE ? "morph" : "setBounds"](this.savedArea);
      this.enableButton("minimize").setResizable(true).setDraggable(true);

      this.maximized = false;
      this.fire('restored');
      return this;
    }
  },

  /*
    Function: toggleMaximize
      Maximizes/Restores window inside it's viewport (managed by WindowManager)

     Returns:
       this
  */
  toggleMaximize: function() {
    return this.maximized ? this.restore() : this.maximize();
  },

  /*
    Function: adapt
      Adapts window size to fit its content

     Returns:
       this
  */
  adapt: function() {
    var dimensions = this.content.getScrollDimensions();
    if (this.options.superflousEffects)
      this.morph(dimensions, true);
    else
      this.setSize(dimensions.width, dimensions.height, true);
    return this;
  },

  /*
    Method: fold
      Folds window content

     Returns:
       this
  */
  fold: function() {
    if (!this.folded) {
      var size = this.getSize(true);
      this.folded = true;
      this.savedInnerHeight = size.height;

      if (this.options.superflousEffects)
        this.morph({ width: size.width, height: 0 }, true);
      else
        this.setSize(size.width, 0, true);

      this.setResizable(false);
      this.fire("fold");
    }
    return this;
  },

  /*
    Method: unfold
      Unfolds window content

     Returns:
       this
  */
  unfold: function() {
    if (this.folded) {
      var size = this.getSize(true);
      this.folded = false;

      if (this.options.superflousEffects)
        this.morph({ width: size.width, height: this.savedInnerHeight }, true);
      else
        this.setSize(size.width, this.savedInnerHeight, true);

      this.setResizable(true);
      this.fire("unfold");
    }
    return this;
  },

  /*
    Method: toggleFold
      Folds/Unfolds window content

     Returns:
       this
  */
  toggleFold: function() {
    return this.folded ? this.unfold() : this.fold();
  },

  /*
    Method: setHeader
      Sets window header, equivalent to this.header.update(...) but allows chaining

     Returns:
       this
  */
  setHeader: function(header) {
    this.header.update(header);
    return this;
  },

  /*
    Method: setContent
      Sets window content, equivalent to this.content.update(...) but allows chaining

     Returns:
       this
  */
  setContent: function(content) {
    this.content.update(content);
    return this;
  },

  /*
    Method: setFooter
      Sets window footer, equivalent to this.footer.update(...) but allows chaining

     Returns:
       this
  */
  setFooter: function(footer) {
    this.footer.update(footer);
    return this;
  },

  /*
    Method: setAjaxContent
      Sets window content using Ajax request

     Parameters:
        url - Ajax URL
        options - Ajax Updater options (see http://prototypejs.org/api/ajax/options and
          http://prototypejs.org/api/ajax/updater)

     Returns:
       this
  */
  setAjaxContent: function(url, options) {
    // bind all callbacks to the window
    Object.keys(options || { }).each(function(name) {
      if (Object.isFunction(options[name]))
        options[name] = options[name].bind(this);
    }, this);

    new Ajax.Updater(this.content, url, options);
    return this;
  },

  // Group: Size and Position

  /*
    Method: getPosition
      Returns top/left position of a window (in pixels)

     Returns:
       an Hash {top:, left:}
  */
  getPosition: function() {
    return { left: this.options.left, top: this.options.top };
  },

  /*
    Method: setPosition
      Sets top/left position of a window (in pixels)

    Parameters
      top:  top position in pixel
      left: left position in pixel

    Returns:
      this
  */
  setPosition: function(top, left) {
    var pos = this.computePosition(top, left);
    this.options.top  = pos.top;
    this.options.left = pos.left;

    var elementStyle  = this.element.style;
    elementStyle.top  = pos.top + 'px';
    elementStyle.left = pos.left + 'px';

    this.fire('position:changed');
    return this;
  },

  /*
    Method: center
      Centers the window within its viewport

    Returns:
      this
  */
  center: function(options) {
    var size          = this.getSize(),
        windowManager = this.windowManager,
        viewport      = windowManager.viewport;
        viewportArea  = viewport.getDimensions(),
        offset        = viewport.getScrollOffset();

    if (options && options.auto) {
      this.centerOptions = Object.extend({ handler: this.recenter.bind(this) }, options);
      Event.observe(this.windowManager.scrollContainer,"scroll", this.centerOptions.handler);
      Event.observe(window,"resize", this.centerOptions.handler);
    }

    options = Object.extend({
      top:  (viewportArea.height - size.height) / 2,
      left: (viewportArea.width  - size.width)  / 2
    }, options || {});

    return this.setPosition(options.top + offset.top, options.left + offset.left);
  },

  /*
    Method: getSize
      Returns window width/height dimensions (in pixels)

    Parameters
      innerSize: returns content size if true, window size if false (defaults to false)

    Returns:
      Hash {width:, height:}
  */
  getSize: function(innerSize) {
    if (innerSize)
      return { width:  this.options.width  - this.borderSize.width,
               height: this.options.height - this.borderSize.height };
    else
      return { width: this.options.width, height: this.options.height };
  },

  /*
    Method: setSize
      Sets window width/height dimensions (in pixels), fires size:changed

    Parameters
      width:  width (in pixels)
      height: height (in pixels)
      innerSize: if true change set content size, else set window size (defaults to false)

    Returns:
      this
  */
  setSize: function(width, height, innerSize) {
    var size = this.computeSize(width, height, innerSize);
    var elementStyle = this.element.style, contentStyle = this.content.style;

    this.options.width  = size.outerWidth;
    this.options.height = size.outerHeight;

    elementStyle.width = size.outerWidth + "px", elementStyle.height = size.outerHeight + "px";
    contentStyle.width = size.innerWidth + "px", contentStyle.height = size.innerHeight + "px";
    this.overlay.style.height = size.innerHeight + "px";

    this.fire('size:changed');
 	  return this;
  },

  /*
    Method: getBounds
      Returns window bounds (in pixels)

    Parameters
      innerSize: returns content size if true, window size otherwise

    Returns:
      an Hash {top:, left:, width:, height:}
  */
  getBounds: function(innerSize) {
    return Object.extend(this.getPosition(), this.getSize(innerSize));
  },

  /*
    Method: setBounds
      Sets window bounds (in pixels), fires position:changed and size:changed

    Parameters
      bounds: Hash {top:, left:, width:, height:} where all values are optional
      innerSize: sets content size if true, window size otherwise

    Returns:
      Hash {top:, left:, width:, height:}
  */
  setBounds: function(bounds, innerSize) {
    return this.setPosition(bounds.top, bounds.left)
               .setSize(bounds.width, bounds.height, innerSize);
  },

  morph: function(bounds, innerSize) {
    bounds = Object.extend(this.getBounds(innerSize), bounds || {});

    if (this.centerOptions && this.centerOptions.auto)
       bounds = Object.extend(bounds, this.computeRecenter(bounds));

    if (innerSize) {
      bounds.width  += this.borderSize.width;
      bounds.height += this.borderSize.height;
    }

    this.animating = true;

    new UI.Window.Effects.Morph(this, bounds, {
      duration: 0.5,
      afterFinish: function() { this.animating = false }.bind(this)
    });

    Object.extend(this.options, bounds);

    return this;
  },

  /*
    Method: getAltitude
      Returns window altitude, an integer between 0 and the number of windows,
      the higher the altitude number - the higher the window position.
  */
  getAltitude: function() {
    return this.windowManager.getAltitude(this);
  },

  /*
    Method: setAltitude
      Sets window altitude, fires 'altitude:changed' if altitude was changed
  */
  setAltitude: function(altitude) {
    if (this.windowManager.setAltitude(this, altitude))
      this.fire('altitude:changed');
    return this;
  },

  /*
    Method: setResizable
      TODO
  */
  setResizable: function(resizable) {
    this.options.resizable = resizable;

    var toggleClassName = (resizable ? 'add' : 'remove') + 'ClassName';

    this.element[toggleClassName]('resizable')
      .select('div:[class*=_sizer]').invoke(resizable ? 'show' : 'hide');
    if (resizable)
      this.createResizeHandles();

    this.element.select('div.se').first()[toggleClassName]('se_resize_handle');

    return this;
  },

  /*
    Method: setDraggable
      TODO
  */
  setDraggable: function(draggable) {
    this.options.draggable = draggable;
    this.element[(draggable ? 'add' : 'remove') + 'ClassName']('draggable');
    return this;
  },

  // Group: Theme
  /*
    Method: getTheme
      Returns window theme name
  */
  getTheme: function() {
    return this.options.theme || this.windowManager.getTheme();
  },

  /*
    Method: setTheme
      Sets window theme
  */
  setTheme: function(theme, windowManagerTheme) {
    this.element.removeClassName(this.getTheme()).addClassName(theme);
    // window has it's own theme
    if (!windowManagerTheme)
      this.options.theme = theme;

    return this;
  },

  /*
    Method: getShadowTheme
      Returns shadow theme name
  */
  getShadowTheme: function() {
    return this.options.shadowTheme || this.windowManager.getShadowTheme();
  }
});

UI.Window.addMethods(UI.Window.Buttons);
UI.Window.addMethods(UI.Window.Shadow);
UI.Window.optionsAccessor($w(" minWidth minHeight maxWidth maxHeight gridX gridY altitude "));
// Private functions for window.js
UI.Window.addMethods({
  style: "position: absolute; top: 0; left: 0; width: 100%; height: 100%; border: 0;",

  action: function(name) {
    var action = this.options[name];
    if (action)
      Object.isString(action) ? this[action]() : action.call(this, this);
  },

  create: function() {
    function createDiv(className, options) {
      return new Element('div', Object.extend({ className: className }, options));
    };

    // Main div
    this.element = createDiv("ui-window " + this.getTheme(), {
      id: this.options.id,
      style: "top:-10000px; left:-10000px"
    });

    // Create HTML window code
    this.header  = createDiv('n move_handle').enableDrag();
    this.content = createDiv('content').appendText(' ');
    this.footer  = createDiv('s move_handle').enableDrag();

    var header   = createDiv('nw').insert(createDiv('ne').insert(this.header));
    var content  = createDiv('w').insert(createDiv('e', {style: "position:relative"}).insert(this.content));
    var footer   = createDiv('sw').insert(createDiv('se' + (this.options.resizable ?  " se_resize_handle" : "")).insert(this.footer));

    this.element.insert(header).insert(content).insert(footer).identify('ui-window');
    this.header.observe('mousedown', this.activate.bind(this));

    this.setDraggable(this.options.draggable);
    this.setResizable(this.options.resizable);

    this.overlay = new Element('div', { style: this.style + "display: none" })
        .observe('mousedown', this.activate.bind(this));

    if (this.options.activeOnClick)
      this.content.insert({ before: this.overlay });
  },

  createWiredElement: function() {
    this.wiredElement = this.wiredElement || new Element("div", {
      className: this.getTheme() + "_wired",
      style:    "display: none; position: absolute; top: 0; left: 0"
    });
  },

  createResizeHandles: function() {
    $w(" n  w  e  s  nw  ne  sw  se ").each(function(id) {
      this.insert(new Element("div", {
        className:   id + "_sizer resize_handle",
        drag_prefix: id }).enableDrag());
    }, this.element);
    this.createResizeHandles = Prototype.emptyFunction;
  },

  // First rendering, pre-compute window border size
  render: function() {
    this.addElements();

    this.computeBorderSize();
    this.updateButtonsOrder();
    this.element.hide().remove();

    // this.options contains top, left, width and height keys
    return this.setBounds(this.options);
  },

  // Adds window elements to the DOM
  addElements: function() {
    this.windowManager.container.appendChild(this.element);
  },

  // Set z-index to all window elements
  setZIndex: function(zIndex) {
    if (this.zIndex != zIndex) {
      this.zIndex = zIndex;
      [ this.element ].concat(this.element.childElements()).each(function(element) {
        element.style.zIndex = zIndex++;
      });
      this.lastZIndex = zIndex;
    }
    return this;
  },

  effect: function(name, element, options) {
    var effect = this.options[name] || Prototype.emptyFunction;
    effect(element || this.element, options || {});
  },

  // re-compute window border size
  computeBorderSize: function() {
    if (this.element) {
      if (Prototype.Browser.IEVersion >= 7)
        this.content.style.width = "100%";
      var dim = this.element.getDimensions(), pos = this.content.positionedOffset();
      this.borderSize = {  top:    pos[1],
                           bottom: dim.height - pos[1] - this.content.getHeight(),
                           left:   pos[0],
                           right:  dim.width - pos[0] - this.content.getWidth() };
      this.borderSize.width  = this.borderSize.left + this.borderSize.right;
      this.borderSize.height = this.borderSize.top  + this.borderSize.bottom;
      if (Prototype.Browser.IEVersion >= 7)
        this.content.style.width = "auto";
    }
  },

  computeSize: function(width, height, innerSize) {
    var innerWidth, innerHeight, outerWidth, outerHeight;
	  if (innerSize) {
	    outerWidth  =  width  + this.borderSize.width;
	    outerHeight =  height + this.borderSize.height;
    } else {
	    outerWidth  =  width;
	    outerHeight =  height;
    }
    // Check grid value
    if (!this.animating) {
      outerWidth = outerWidth.snap(this.options.gridX);
      outerHeight = outerHeight.snap(this.options.gridY);

      // Check min size
      if (!this.folded) {
        if (outerWidth < this.options.minWidth)
          outerWidth = this.options.minWidth;

        if (outerHeight < this.options.minHeight)
          outerHeight = this.options.minHeight;
      }

      // Check max size
      if (this.options.maxWidth && outerWidth > this.options.maxWidth)
        outerWidth = this.options.maxWidth;

      if (this.options.maxHeight && outerHeight > this.options.maxHeight)
        outerHeight = this.options.maxHeight;
    }

    if (this.centerOptions && this.centerOptions.auto)
      this.recenter();

    innerWidth  = outerWidth - this.borderSize.width;
    innerHeight = outerHeight - this.borderSize.height;
    return {
      innerWidth: innerWidth, innerHeight: innerHeight,
      outerWidth: outerWidth, outerHeight: outerHeight
    };
  },

  computePosition: function(top, left) {
    if (this.centerOptions && this.centerOptions.auto)
      return this.computeRecenter(this.getSize());                                                                                                            ;

    return {
      top:  this.animating ? top  : top.snap(this.options.gridY),
      left: this.animating ? left : left.snap(this.options.gridX)
    };
  },

  computeRecenter: function(size) {
    var viewport   = this.windowManager.viewport,
        area       = viewport.getDimensions(),
        offset     = viewport.getScrollOffset(),
        center     = {
          top:  Object.isUndefined(this.centerOptions.top)  ? (area.height - size.height) / 2 : this.centerOptions.top,
          left: Object.isUndefined(this.centerOptions.left) ? (area.width  - size.width)  / 2 : this.centerOptions.left
        };

    return {
      top:  parseInt(center.top + offset.top),
      left: parseInt(center.left + offset.left)
    };
  },

  recenter: function(event) {
    var pos = this.computeRecenter(this.getSize());
    this.setPosition(pos.top, pos.left);
  }
});
UI.URLWindow = Class.create(UI.Window, {
  options: {
    url: 'about:blank'
  },

  afterClassCreate: function() {
    this.undefMethod('setAjaxContent');
  },

  initialize: function($super, options) {
    $super(options);
    this.setUrl(this.options.url);
  },

  destroy: function($super){
    this.iframe.src = null;
    $super();
  },

  getUrl: function() {
    return this.iframe.src;
  },

  setUrl: function(url, options) {
    this.iframe.src = url;
    return this;
  },

  create: function($super) {
    $super();

    this.iframe = new Element('iframe', {
      style: this.style,
      frameborder: 0,
      src: this.options.url,
      name: this.element.id + "_frame",
      id:  this.element.id + "_frame"
    });

    this.content.insert(this.iframe);
  }
});
if (!Object.isUndefined(window.Effect)) {
  UI.Window.Effects = UI.Window.Effects || {};
  UI.Window.Effects.Morph = Class.create(Effect.Base, {
    initialize: function(window, bounds) {
      this.window = window;
      var options = Object.extend({
        fromBounds: this.window.getBounds(),
        toBounds:   bounds,
        from:       0,
        to:         1
      }, arguments[2] || { });
      this.start(options);
    },

    update: function(position) {
      var t = this.options.fromBounds.top + (this.options.toBounds.top   - this.options.fromBounds.top) * position;
      var l = this.options.fromBounds.left + (this.options.toBounds.left - this.options.fromBounds.left) * position;

      var ow = this.options.fromBounds.width + (this.options.toBounds.width - this.options.fromBounds.width) * position;
      var oh = this.options.fromBounds.height + (this.options.toBounds.height - this.options.fromBounds.height) * position;

      this.window.setBounds({top: t,  left: l, width: ow, height: oh})
    }
  });
}
UI.Window.addMethods({
  startDrag: function(handle) {
    this.initBounds = this.getBounds();
    this.activate();

    if (this.options.wired) {
      this.createWiredElement();
      this.wiredElement.style.cssText = this.element.style.cssText;
      this.element.hide();
      this.saveElement = this.element;
      this.windowManager.container.appendChild(this.wiredElement);
      this.element = this.wiredElement;
    }

    handle.hasClassName('resize_handle') ? this.startResize(handle) : this.startMove();
  },

  endDrag: function() {
    this.element.hasClassName('resized') ? this.endResize() : this.endMove();

    if (this.options.wired) {
      this.saveElement.style.cssText = this.wiredElement.style.cssText;
      this.wiredElement.remove();
      this.element = this.saveElement;
    }
  },

  startMove: function() {
    // method used to drag
    this.drag = this.moveDrag;
    this.element.addClassName('moved');
    this.fire('move:started');
  },

  endMove: function() {
    this.element.removeClassName('moved');
    this.fire('move:ended');
  },

  startResize: function(handle) {
    this.drag = this[handle.readAttribute('drag_prefix')+'Drag'];
    this.element.addClassName('resized');
    this.fire('resize:started');
  },

  endResize: function() {
    this.element.removeClassName('resized');
    this.fire('resize:ended');
  },

  moveDrag: function(dx, dy) {
    this.setPosition(this.initBounds.top + dy, this.initBounds.left + dx);
  },

  swDrag: function(dx, dy) {
    var initBounds = this.initBounds;
    this.setSize(initBounds.width - dx, initBounds.height + dy)
        .setPosition(initBounds.top,
                     initBounds.left + (initBounds.width - this.getSize().width));
  },

  seDrag: function(dx, dy) {
    this.setSize(this.initBounds.width + dx, this.initBounds.height + dy);
  },

  nwDrag: function(dx, dy) {
    var initBounds = this.initBounds;
    this.setSize(initBounds.width - dx, initBounds.height - dy)
        .setPosition(initBounds.top + (initBounds.height - this.getSize().height),
                     initBounds.left + (initBounds.width - this.getSize().width));
  },

  neDrag: function(dx, dy) {
    var initBounds = this.initBounds;
    this.setSize(initBounds.width + dx, initBounds.height - dy)
        .setPosition(initBounds.top + (initBounds.height - this.getSize().height),
                     initBounds.left);
  },

  wDrag: function(dx, dy) {
    var initBounds = this.initBounds;
    this.setSize(initBounds.width - dx, initBounds.height)
        .setPosition(initBounds.top,
                     initBounds.left + (initBounds.width - this.getSize().width));
  },

  eDrag: function(dx, dy) {
    this.setSize(this.initBounds.width + dx, this.initBounds.height);
  },

  nDrag: function(dx, dy) {
    var initBounds = this.initBounds;
    this.setSize(initBounds.width, initBounds.height - dy)
        .setPosition(initBounds.top + (initBounds.height - this.getSize().height),
                     initBounds.left);
  },

  sDrag: function(dx, dy) {
    this.setSize(this.initBounds.width, this.initBounds.height + dy);
  }
});
UI.Window.addMethods({
  methodsAdded: function(base) {
    base.aliasMethodChain('create',  'buttons');
    base.aliasMethodChain('destroy', 'buttons');
  },

  createWithButtons: function() {
    this.createWithoutButtons();

    if (!this.options.resizable) {
      this.options.minimize = false;
      this.options.maximize = false;
    }

    this.buttons = new Element("div", { className: "buttons" })
      .observe('click',     this.onButtonsClick.bind(this))
      .observe('mouseover', this.onButtonsHover.bind(this))
      .observe('mouseout',  this.onButtonsOut.bind(this));

    this.element.insert(this.buttons);

    this.defaultButtons.each(function(button) {
      if (this.options[button] !== false)
        this.addButton(button);
    }, this);
  },

  destroyWithButtons: function() {
    this.buttons.stopObserving();
    this.destroyWithoutButtons();
  },

  defaultButtons: $w(' minimize maximize close '),

  getButtonElement: function(buttonName) {
    return this.buttons.down("." + buttonName);
  },

  // Controls close, minimize, maximize, etc.
  // action can be either a string or a function
  // if action is a string, it is the method name that will be called
  // else the function will take the window as first parameter.
  // if not given action will be taken in window's options
  addButton: function(buttonName, action) {
    this.buttons.insert(new Element("a", { className: buttonName, href: "#"}));

    if (action)
      this.options[buttonName] = action;

    return this;
  },

  removeButton: function(buttonName) {
    this.getButtonElement(buttonName).remove();
    return this;
  },

  disableButton: function(buttonName) {
    this.getButtonElement(buttonName).addClassName("disabled");
    return this;
  },

  enableButton: function(buttonName) {
    this.getButtonElement(buttonName).removeClassName("disabled");
    return this;
  },

  onButtonsClick: function(event) {
    var element = event.findElement('a:not(.disabled)');

    if (element) this.action(element.className);
    event.stop();
  },

  onButtonsHover: function(event) {
    this.buttons.addClassName("over");
  },

  onButtonsOut: function(event) {
    this.buttons.removeClassName("over");
  },

  updateButtonsOrder: function() {
    var buttons = this.buttons.childElements();

    buttons.inject(new Array(buttons.length), function(array, button) {
      array[parseInt(button.getStyle("padding-top"))] = button.setStyle("padding: 0");
      return array;
    }).each(function(button) { this.buttons.appendChild(button) }, this);
  }
});
UI.Window.addMethods({
  methodsAdded: function(base) {
    (function(methods) {
      $w(methods).each(function(m) { base.aliasMethodChain(m, 'shadow') });
    })(' create addElements setZIndex setPosition setSize setBounds ');
  },

  showShadow: function() {
    if (this.shadow) {
      this.shadow.hide();
      this.effect('show', this.shadow.shadow);
    }
  },

  hideShadow: function() {
    if (this.shadow)
      this.effect('hide', this.shadow.shadow);
  },

  removeShadow: function() {
    if (this.shadow)
      this.shadow.remove();
  },

  focusShadow: function() {
    if (this.shadow)
      this.shadow.focus();
  },

  blurShadow: function() {
    if (this.shadow)
      this.shadow.blur();
  },

  // Private Functions
  createWithShadow: function() {
    this.createWithoutShadow();

    this.observe('showing', this.showShadow)
        .observe('hiding',  this.hideShadow)
        .observe('hidden',  this.removeShadow)
        .observe('focused', this.focusShadow)
        .observe('blurred', this.blurShadow);

    if (this.options.shadow)
      this.shadow = new UI.Shadow(this.element, {theme: this.getShadowTheme()});
  },

  addElementsWithShadow: function() {
    this.addElementsWithoutShadow();
    if (this.shadow) {
      this.shadow.setBounds(this.options).render();
    }
  },

  setZIndexWithShadow: function(zIndex) {
    if (this.zIndex != zIndex) {
      if (this.shadow)
        this.shadow.setZIndex(zIndex - 1);
      this.setZIndexWithoutShadow(zIndex);
      this.zIndex = zIndex;
    }
    return this;
  },

  setPositionWithShadow: function(top, left) {
    this.setPositionWithoutShadow(top, left);
    if (this.shadow) {
      var pos = this.getPosition();
      this.shadow.setPosition(pos.top, pos.left);
    }
    return this;
  },

  setSizeWithShadow: function(width, height, innerSize) {
    this.setSizeWithoutShadow(width, height, innerSize);
    if (this.shadow) {
      var size = this.getSize();
      this.shadow.setSize(size.width, size.height);
    }
    return this;
  },

  setBoundsWithShadow: function(bounds, innerSize) {
    this.setBoundsWithoutShadow(bounds, innerSize);
    if (this.shadow)
      this.shadow.setBounds(this.getBounds());
  }
});

/*
Class: UI.WindowManager
  Window Manager.
  A default instance of this class is created in UI.defaultWM.

  Example:
    > new UI.WindowManger({
    >   container: 'desktop',
    >   theme: 'mac_os_x'
    > });
*/

UI.WindowManager = Class.create(UI.Options, {
  options: {
    container:   null, // will default to document.body
    zIndex:      0,
    theme:       "alphacube",
    shadowTheme: "mac_shadow",
    showOverlay: Element.show,
    hideOverlay: Element.hide,
    positionningStrategy: function(win, area) {
      UI.WindowManager.DumbPositionningStrategy(win, area);
    }
  },

  initialize: function(options) {
    this.setOptions(options);

    this.container = $(this.options.container || document.body);

    if (this.container === $(document.body)) {
      this.viewport = document.viewport;
      this.scrollContainer = window;
    } else {
      this.viewport = this.scrollContainer = this.container;
    }

    this.container.observe('drag:started', this.onStartDrag.bind(this))
                  .observe('drag:updated', this.onDrag.bind(this))
                  .observe('drag:ended',   this.onEndDrag.bind(this));

    this.stack = new UI.WindowManager.Stack();
    this.modalSessions = 0;

    this.createOverlays();
    this.resizeEvent = this.resize.bind(this);

    Event.observe(window, "resize", this.resizeEvent);
  },

  destroy: function() {
    this.windows().invoke('destroy');
    this.stack.destroy();
    Event.stopObserving(window, "resize", this.resizeEvent);
  },

  /*
    Method: setTheme
      Changes window manager's theme, all windows that don't have a own theme
      will have this new theme.

    Parameters:
      theme - theme name

    Example:
      > UI.defaultWM.setTheme('bluelighting');
  */
  setTheme: function(theme) {
    this.stack.windows.select(function(w) {
      return !w.options.theme;
    }).invoke('setTheme', theme, true);
    this.options.theme = theme;
    return this;
  },

  register: function(win) {
    if (this.getWindow(win.id)) return;

    this.handlePosition(win);
    this.stack.add(win);
    this.restartZIndexes();
  },

  unregister: function(win) {
    this.stack.remove(win);

    if (win == this.focusedWindow)
      this.focusedWindow = null;
  },

  /*
    Method: getWindow
      Find the window containing a given element.

    Example:
      > $$('.ui-window a.close').invoke('observe', 'click', function() {
      >   UI.defaultWM.getWindow(this).close();
      > });

    Parameters:
      element - element or element identifier

    Returns:
      containing window or null
  */
  getWindow: function(element) {
    element = $(element);

    if (!element) return;

    if (!element.hasClassName('ui-window'))
      element = element.up('.ui-window');

    var id = element.id;
    return this.stack.windows.find(function(win) { return win.id == id });
  },

  /*
    Method: windows
      Returns an array of all windows handled by this window manager.
      First one is the back window, last one is the front window.

    Example:
      > UI.defaultWM.windows().invoke('destroy');
  */
  windows: function() {
    return this.stack.windows.clone();
  },

  /*
    Method: getFocusedWindow
      Returns the focused window
  */
  getFocusedWindow: function() {
    return this.focusedWindow;
  },

  // INTERNAL

  // Modal mode
  startModalSession: function(win) {
    if (!this.modalSessions) {
      this.removeOverflow();
      this.modalOverlay.className = win.getTheme() + "_overlay";
      this.container.appendChild(this.modalOverlay);

      if (!this.modalOverlay.opacity)
        this.modalOverlay.opacity = this.modalOverlay.getOpacity();
      this.modalOverlay.setStyle("height: " + this.viewport.getHeight() + "px");

      this.options.showOverlay(this.modalOverlay, {from: 0, to: this.modalOverlay.opacity});
    }
    this.modalOverlay.setStyle({ zIndex: win.zIndex - 1 });
    this.modalSessions++;
  },

  endModalSession: function(win) {
    this.modalSessions--;
    if (this.modalSessions) {
      this.modalOverlay.setStyle({ zIndex: this.stack.getPreviousWindow(win).zIndex - 1 });
    } else {
      this.resetOverflow();
      this.options.hideOverlay(this.modalOverlay, { from: this.modalOverlay.opacity, to: 0 });
    }
  },

  moveHandleSelector:   '.ui-window.draggable .move_handle',
  resizeHandleSelector: '.ui-window.resizable .resize_handle',

  onStartDrag: function(event) {
    var handle = event.element(),
        isMoveHandle   = handle.match(this.moveHandleSelector),
        isResizeHandle = handle.match(this.resizeHandleSelector);

    // ensure dragged element is a window handle !
    if (isResizeHandle || isMoveHandle) {
      event.stop();

      // find the corresponding window
      var win = this.getWindow(event.findElement('.ui-window'));

      // render drag overlay
      this.container.insert(this.dragOverlay.setStyle({ zIndex: this.getLastZIndex() }));

      win.startDrag(handle);
      this.draggedWindow = win;
    }
  },

  onDrag: function(event) {
    if (this.draggedWindow) {
      event.stop();
      this.draggedWindow.drag(event.memo.dx, event.memo.dy);
    }
  },

  onEndDrag: function(event) {
    if (this.draggedWindow) {
      event.stop();
      this.dragOverlay.remove();
      this.draggedWindow.endDrag();
      this.draggedWindow = null;
    }
  },

  maximize: function(win) {
    this.removeOverflow();
    this.maximizedWindow = win;
    return true;
  },

  restore: function(win) {
    if (this.maximizedWindow) {
      this.resetOverflow();
      this.maximizedWindow = false;
    }
    return true;
  },

  removeOverflow: function() {
    var container = this.container;
    // Remove overflow, save overflow and scrolloffset values to restore them when restore window
    container.savedOverflow = container.style.overflow || "auto";
    container.savedOffset = this.viewport.getScrollOffset();
    container.style.overflow = "hidden";

    this.viewport.setScrollOffset({ top:0, left:0 });

    if (this.container == document.body && Prototype.Browser.IE)
      this.cssRule = CSS.addRule("html { overflow: hidden }");
  },

  resetOverflow: function() {
    var container = this.container;
    // Restore overflow ans scrolloffset
    if (container.savedOverflow) {
      if (this.container == document.body && Prototype.Browser.IE)
        this.cssRule.remove();

      container.style.overflow = container.savedOverflow;
      this.viewport.setScrollOffset(container.savedOffset);

      container.savedOffset = container.savedOverflow = null;
    }
  },

  hide: function(win) {
    var previous = this.stack.getPreviousWindow(win);
    if (previous) previous.focus();
  },

  restartZIndexes: function(){
    // Reset zIndex
    var zIndex = this.getZIndex() + 1; // keep a zIndex free for overlay divs
    this.stack.windows.each(function(w) {
      w.setZIndex(zIndex);
      zIndex = w.lastZIndex + 1;
    });
  },

  getLastZIndex: function() {
    return this.stack.getFrontWindow().lastZIndex + 1;
  },

  overlayStyle: "position: absolute; top: 0; left: 0; display: none; width: 100%;",

  createOverlays: function() {
    this.modalOverlay = new Element("div", { style: this.overlayStyle });
    this.dragOverlay  = new Element("div", { style: this.overlayStyle+"height: 100%" });
  },

  focus: function(win) {
    // Blur the previous focused window
    if (this.focusedWindow)
      this.focusedWindow.blur();
    this.focusedWindow = win;
  },

  blur: function(win) {
    if (win == this.focusedWindow)
      this.focusedWindow = null;
  },

  setAltitude: function(win, altitude) {
    var stack = this.stack;

    if (altitude === "front") {
      if (stack.getFrontWindow() === win) return;
      stack.bringToFront(win);
    } else if (altitude === "back") {
      if (stack.getBackWindow() === win) return;
      stack.sendToBack(win);
    } else {
      if (stack.getPosition(win) == altitude) return;
      stack.setPosition(win, altitude);
    }

    this.restartZIndexes();
    return true;
  },

  getAltitude: function(win) {
    return this.stack.getPosition(win);
  },

  resize: function(event) {
    var area = this.viewport.getDimensions();

    if (this.maximizedWindow)
      this.maximizedWindow.setSize(area.width, area.height);

    if (this.modalOverlay.visible())
      this.modalOverlay.setStyle("height:" + area.height + "px");
  },

  handlePosition: function(win) {
    // window has its own position, nothing needs to be done
    if (Object.isNumber(win.options.top) && Object.isNumber(win.options.left))
      return;

    var strategy = this.options.positionningStrategy,
        area     = this.viewport.getDimensions();

    Object.isFunction(strategy) ? strategy(win, area) : strategy.position(win, area);
  }
});

UI.WindowManager.DumbPositionningStrategy = function(win, area) {
  size = win.getSize();

  var top  = area.height - size.height,
      left = area.width  - size.width;

  top  = top  < 0 ? 0 : Math.random() * top;
  left = left < 0 ? 0 : Math.random() * left;

  win.setPosition(top, left);
};

UI.WindowManager.optionsAccessor('zIndex', 'theme', 'shadowTheme');

UI.WindowManager.Stack = Class.create(Enumerable, {
  initialize: function() {
    this.windows = [ ];
  },

  each: function(iterator) {
    this.windows.each(iterator);
  },

  add: function(win, position) {
    this.windows.splice(position || this.windows.length, 0, win);
  },

  remove: function(win) {
    this.windows = this.windows.without(win);
  },

  sendToBack: function(win) {
    this.remove(win);
    this.windows.unshift(win);
  },

  bringToFront: function(win) {
    this.remove(win);
    this.windows.push(win);
  },

  getPosition: function(win) {
    return this.windows.indexOf(win);
  },

  setPosition: function(win, position) {
    this.remove(win);
    this.windows.splice(position, 0, win);
  },

  getFrontWindow: function() {
    return this.windows.last();
  },

  getBackWindow: function() {
    return this.windows.first();
  },

  getPreviousWindow: function(win) {
    return (win == this.windows.first()) ? null : this.windows[this.windows.indexOf(win) - 1];
  }
});

document.whenReady(function() {
  UI.defaultWM = new UI.WindowManager();
});
