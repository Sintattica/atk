var focussedRecordlist = null;

/**
 * The keylistener class for recordlists.
 */
function atkRLKeyListener(rlId, highlight, reccount)
{
  this.recordlistId = rlId;  
  this.currentrec = -1;
  this.prevcolor = '';
  this.highlight = highlight;
  this.reccount = reccount;
  
  var el = document.getElementById(this.recordlistId);
  el.listener = this; // Give the element a pointer to the listener, so we can always access it. 
}

atkRLKeyListener.prototype = new atkGKeyListener();
atkRLKeyListener.superclass = atkGKeyListener.prototype;

atkRLKeyListener.prototype.handleKey = function(key, ctrl, shift)
{  
  if (key==KEY_DOWN) this.down();
  else if (key==KEY_UP) this.up();
  else if (key==KEY_RIGHT) this.next();
  else if (key==KEY_LEFT) this.previous();
  else if (key==KEY_DEL) this.do_action('delete');
  else if (key==KEY_E) this.do_action('edit');
  else if (key==KEY_V) this.do_action('view');
  else
  {
    // Other key, which we ignore.
    // alert('key '+key+' pressed...');
  }
}

atkRLKeyListener.prototype.do_action = function(action)
{
  if (this.currentrec>-1)
  {
    rl_do(this.recordlistId, this.currentrec, action);
  }
}

atkRLKeyListener.prototype.next = function()
{
  return rl_next(this.recordlistId);  
}

atkRLKeyListener.prototype.previous = function()
{
  return rl_previous(this.recordlistId);
}

atkRLKeyListener.prototype.focus = function(direction)
{    
  focussedRecordlist = this.recordlistId;
  if (direction==DIR_UP)   // We come from below.
  {  
    this.last();
  }
  else if (direction==DIR_DOWN) // We come from above
  {
    this.first(); // move to first record.
  }
  else // Clicked in the middle
  {    
  }
}

atkRLKeyListener.prototype.blur = function()
{
  this.deselectRow();
  focussedRecordlist = null;  
}

atkRLKeyListener.prototype.first = function()
{
  // We can implement a jump to the first record by setting the pointer to -1 and 
  // moving one down.
  this.currentrec=-1;
  this.down();
}

atkRLKeyListener.prototype.setRow = function(rownum, oldcolor)
{
  if (this.currentrec!=rownum) // check if not already selected
  {
    kb_focus(this.id, DIR_NONE);
  
    this.deselectRow();
    this.currentrec=rownum;
    this.selectRow();
    this.prevcolor=oldcolor;
    return true;
  }
  return false;
}

atkRLKeyListener.prototype.deselectRow = function()
{
  if (this.currentrec>=0)
  {
    curRow = document.getElementById(this.recordlistId+'_'+this.currentrec);
    curRow.style.backgroundColor = this.prevcolor;  
  }
}

atkRLKeyListener.prototype.selectRow = function()
{
  if (this.currentrec>=0)
  {
    newRow = document.getElementById(this.recordlistId+'_'+this.currentrec);
    this.prevcolor = newRow.style.backgroundColor;
    newRow.style.backgroundColor = this.highlight;
  }
}

atkRLKeyListener.prototype.last = function()
{
  this.currentrec=-1; // put the pointer to nothing.
  this.up();
}

atkRLKeyListener.prototype.down = function()
{
  if (this.currentrec>=0) // a record was already selected  
  {
    this.deselectRow();
    this.currentrec++;
  }
  else  
  {
    this.currentrec=0;
  }
  
  if (this.currentrec>=this.reccount) // pointer has moved beyond last record
  {
    this.currentrec = -1; // reset pointer to nothing.
    if (!this.next())
    {
      // There's no next page
      kb_focusNext(); // pass onto next element.
    }
  }
  else
  {
    //alert('naam: '+this.recordlistId+'_'+this.currentrec);
    this.selectRow();
  }
}

atkRLKeyListener.prototype.up = function()
{
  if (this.currentrec>=0) // a record was already selected  
  {
    this.deselectRow();
    this.currentrec--;
  }  
  else
  {
    this.currentrec=this.reccount-1; 
  }
  
  if (this.currentrec<0) // pointer moved before first record
  {
    this.currentrec = -1; // reset pointer to nothing.
    if (!this.previous()) // there is no previous page
    {
      kb_focusPrevious(); // pass onto previous element.
    }
  }
  else
  {
    this.selectRow();
  }  
}

