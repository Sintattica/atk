// Scriptaculous fixes against version 1.8.1
if (window.Draggable) {
	Draggable.prototype.startDrag = function(event) {
	   this.dragging = true;
	   if(!this.delta)
	     this.delta = this.currentDelta();
	   
	   if(this.options.zindex) {
	     this.originalZ = parseInt(Element.getStyle(this.element,'z-index') || 0);
	     this.element.style.zIndex = this.options.zindex;
	   }
	   
	   if(this.options.ghosting) {
	     this._clone = this.element.cloneNode(true);
	     this._originallyAbsolute = (this.element.getStyle('position') == 'absolute'); // CHANGED
	     if (!this._originallyAbsolute) // CHANGED
	       Position.absolutize(this.element);
	     this.element.parentNode.insertBefore(this._clone, this.element);
	   }
	   
	   if(this.options.scroll) {
	     if (this.options.scroll == window) {
	       var where = this._getWindowScroll(this.options.scroll);
	       this.originalScrollLeft = where.left;
	       this.originalScrollTop = where.top;
	     } else {
	       this.originalScrollLeft = this.options.scroll.scrollLeft;
	       this.originalScrollTop = this.options.scroll.scrollTop;
	     }
	   }
	   
	   Draggables.notify('onStart', this, event);
	       
	   if(this.options.starteffect) this.options.starteffect(this.element);
	}
	
	Draggable.prototype.finishDrag = function(event, success) {
	   this.dragging = false;
	   
	   if(this.options.quiet){
	     Position.prepare();
	     var pointer = [Event.pointerX(event), Event.pointerY(event)];
	     Droppables.show(pointer, this.element);
	   }
	
	   if(this.options.ghosting) {
	     if (!this._originallyAbsolute)
	       Position.relativize(this.element);
	     delete this._originallyAbsolute;
	     Element.remove(this._clone);
	     this._clone = null;
	   }
	
	   var dropped = false; 
	   if(success) { 
	     dropped = Droppables.fire(event, this.element); 
	     if (!dropped) dropped = false; 
	   }
	   if(dropped && this.options.onDropped) this.options.onDropped(this.element);
	   Draggables.notify('onEnd', this, event);
	
	   var revert = this.options.revert;
	   if(revert && Object.isFunction(revert)) revert = revert(this.element);
	   
	   var d = this.currentDelta();
	   if(revert && this.options.reverteffect) {
	     if (dropped == 0 || revert != 'failure')
	       this.options.reverteffect(this.element,
	         d[1]-this.delta[1], d[0]-this.delta[0]);
	   } else {
	     this.delta = d;
	   }
	
	   if(this.options.zindex)
	     this.element.style.zIndex = this.originalZ;
	
	   if(this.options.endeffect) 
	     this.options.endeffect(this.element);
	     
	   Draggables.deactivate(this);
	   Droppables.reset();
	}
}