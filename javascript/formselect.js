/**
 * Updates the selection of select boxes for the record list form.
 * @param name unique recordlist name
 * @param type "all", "none" or "invert"
 */
function updateSelection(name, type)
{
  /* get selectors */
  var list = document.entryform.elements[name + '_atkselector[]'];

  /* no selectors?! impossible situation, bail out! */
  if (typeof(list) == 'undefined') return;

  /* walk through list */
  if (typeof(list.length) == 'undefined') list = new Array(list);
  for (var i = 0; i < list.length; i++)
  {
			if      ("all"    == type && !list[i].disabled)	list[i].checked = true;
      else if ("none"   == type && !list[i].disabled)	list[i].checked = false;
      else if ("invert" == type && !list[i].disabled)	list[i].checked = !list[i].checked;
  }
}

/**
 * Disables / enables checkboxes depending if the record supports
 * a certain action or not.
 * @param name unique recordlist name
 */
function updateSelectable(name)
{
  /* get selectors */
  var list = document.entryform.elements[name + '_atkselector[]'];

  /* no selectors?! impossible situation, bail out! */
  if (typeof(list) == 'undefined') return;

  /* some stuff we need to know */
  var index  = document.entryform.elements[name + '_atkaction'].selectedIndex;
  var action = document.entryform.elements[name + '_atkaction'][index].value;

  /* walk through list */
  if (typeof(list.length) == 'undefined') list = new Array(list);
  for (var i = 0; i < list.length; i++)
  {
    /* supported actions */
    var actions = eval(name + '["' + list[i].value + '"]');
    if (typeof(actions) == 'undefined') actions = new Array();

    /* contains action? */
    disabled = true;
    for (var j = 0; disabled && j < actions.length; j++)
      if (actions[j] == action) disabled = false

    /* disable */
    list[i].disabled = disabled;
    if (disabled) list[i].checked = false;
  }
}

/**
 * Because we allow embedded recordLists for 1:n relations we need a way to somehow
 * distinguish between the submit of the edit form, and the submit of the multi-record action.
 * This method uses the atkescape option to redirect the multi-record action to a level higher
 * on the session stack, which makes it possible to return to the edit form (saving updated values!)
 * @param name unique recordlist name
 * @param target where do we escape to?
 */
function atkSubmitMRA(name, target)
{
  /* some stuff we need to know */
  var atknodetype = document.entryform.elements[name + '_atknodetype'].value;
  var index  = document.entryform.elements[name + '_atkaction'].selectedIndex;
  var atkaction = document.entryform.elements[name + '_atkaction'][index].value;

  /* initial target URL */
  target += 'atknodetype=' + atknodetype + '&atkaction=' + atkaction;

  /* get selectors */
  var list = document.entryform.elements[name + '_atkselector[]'];

  /* no selectors?! impossible situation, bail out! */
  if (typeof(list) == 'undefined') return;

  /* add the selectors to the target URL */
  var selectorLength = 0;
  if (typeof(list.length) == 'undefined') list = new Array(list);
  for (var i = 0; i < list.length; i++)
    if (!list[i].disabled && list[i].checked)
    {
      target += '&atkselector[]=' + list[i].value;
      selectorLength++;
    }

  /* change atkescape value and submit form */
  if (selectorLength > 0)
  {
    document.entryform.atkescape.value = target;
    globalSubmit(document.entryform);
    document.entryform.submit();
  }
}