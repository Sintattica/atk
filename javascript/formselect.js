/**
 * Updates the selection of select boxes for the record list form.
 * @param name unique recordlist name
 * @param form reference to the form object
 * @param type "all", "none" or "invert"
 */
function updateSelection(name, form, type)
{
  /* get selectors */
  var list = form.elements[name + '_atkselector[]'];

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
 * @param form reference to the form object
 */
function updateSelectable(name, form)
{
  /* get selectors */
  var list = form.elements[name + '_atkselector[]'];

  /* no selectors?! impossible situation, bail out! */
  if (typeof(list) == 'undefined') return;

  /* some stuff we need to know */
  var index  = form.elements[name + '_atkaction'].selectedIndex;
  var action = form.elements[name + '_atkaction'][index].value;

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
 * @param form reference to the form object
 * @param target where do we escape to?
 */
function atkSubmitMRA(name, form, target)
{
  /* some stuff we need to know */
  var index  = form.elements[name + '_atkaction'].selectedIndex;
  if (typeof(index) == 'undefined') var atkaction = form.elements[name + '_atkaction'].value;
  else var atkaction = form.elements[name + '_atkaction'][index].value;

  /* initial target URL */
  target += '&atkaction=' + atkaction;

  /* get selectors */
  var list = form.elements[name + '_atkselector[]'];

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
    form.atkescape.value = target;
    globalSubmit(form);
    form.submit();
  }
}

/**
 * Because we allow embedded recordLists for 1:n relations we need a way to somehow
 * distinguish between the submit of the edit form, and the submit of the multi-record action.
 * This method uses the atkescape option to redirect the multi-record-priority action to a level higher
 * on the session stack, which makes it possible to return to the edit form (saving updated values!)
 * @param name unique recordlist name
 * @param form reference to the form object
 * @param target where do we escape to?
 */
function atkSubmitMRPA(name, form, target)
{
  /* some stuff we need to know */
  var index  = form.elements[name + '_atkaction'].selectedIndex;
  if (typeof(index) == 'undefined') var atkaction = form.elements[name + '_atkaction'].value;
  else var atkaction = form.elements[name + '_atkaction'][index].value;

  /* initial target URL */
  target += 'atkaction=' + atkaction;

  /* get selectors */
  var list = form.elements[name + '_atkselector[]'];

  /* no selectors?! impossible situation, bail out! */
  if (index == 0 || typeof(list) == 'undefined') return;

  /* add the selectors to the target URL */
  var selectorLength = 0;
  if (typeof(list.selectedIndex) != 'undefined') list = new Array(list);
  for (var i = 0; i < list.length; i++)
    if (list[i].selectedIndex != 0)
    {
      var priority = list[i][list[i].selectedIndex].value;
      target += '&atkselector[' + list[i][0].value + ']=' + priority;
      selectorLength++;
    }

  /* change atkescape value and submit form */
  if (selectorLength > 0)
  {
    form.atkescape.value = target;
    globalSubmit(form);
    form.submit();
  }
}