/**
 * This file is part of the ATK distribution on GitHub.
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
 * $Id$
 */

if (!window.ATK) {
    var ATK = {};
}


ATK.FormSelect = {
    /**
     * Returns the selectors for the given form and
     * the given datagrid name.
     */
    getATKSelectors: function (name, form) {
        var list = [];
        var prefix = name + '_atkselector';

        jQuery(form).find('input:checkbox').each(function (index, el) {
            var $el = jQuery(el);
            var name = $el.attr('name');
            if (name !== null && name.substring(0, prefix.length) === prefix) {
                list.push(el);
            }
        });

        return list;
    },

    /**
     * Updates the selection of select boxes for the record list form.
     * @param name unique recordlist name
     * @param form reference to the form object
     * @param type "all", "none" or "invert"
     */
    updateSelection: function (name, form, type) {
        /* get selectors */
        var list = ATK.FormSelect.getATKSelectors(name, form);

        /* walk through list */
        for (var i = 0; i < list.length; i++) {
            if ("all" === type && !list[i].disabled)
                list[i].checked = true;
            else if ("none" === type && !list[i].disabled)
                list[i].checked = false;
            else if ("invert" === type && !list[i].disabled)
                list[i].checked = !list[i].checked;
        }
    },

    /**
     * Disables / enables checkboxes depending if the record supports
     * a certain action or not.
     * @param name unique recordlist name
     * @param form reference to the form object
     */
    updateSelectable: function (name, form) {
        /* get selectors */
        var list = ATK.FormSelect.getATKSelectors(name, form);

        /* some stuff we need to know */
        var index = form.elements[name + '_atkaction'].selectedIndex;
        var action = form.elements[name + '_atkaction'][index].value;

        /* walk through list */
        for (var i = 0; i < list.length; i++) {
            /* supported actions */
            var actions = eval(name + '["' + list[i].value + '"]');
            if (typeof (actions) === 'undefined') {
                actions = [];
            }

            /* contains action? */
            var disabled = true;
            for (var j = 0; disabled && j < actions.length; j++)
                if (actions[j] === action) {
                    disabled = false;
                }

            /* disable */
            list[i].disabled = disabled;
            if (disabled) {
                list[i].checked = false;
            }
        }
    },

    /**
     * Because we allow embedded recordLists for 1:n relations we need a way to somehow
     * distinguish between the submit of the edit form, and the submit of the multi-record action.
     * This method uses the atkescape option to redirect the multi-record action to a level higher
     * on the session stack, which makes it possible to return to the edit form (saving updated values!)
     * @param name unique recordlist name
     * @param form reference to the form object
     * @param target where do we escape to?
     * @param embedded
     * @param ignoreHandler
     */
    atkSubmitMRA: function (name, form, target, embedded, ignoreHandler) {

        // some stuff we need to know
        var atkaction, input;
        var index = form.elements[name + '_atkaction'].selectedIndex;
        if (typeof(index) === 'undefined') {
            atkaction = form.elements[name + '_atkaction'].value;
        } else {
            atkaction = form.elements[name + '_atkaction'][index].value;
        }

        // if no multi-record action is selected, bail out!
        if (atkaction === '') {
            return;
        }

        // if there exists a function with the name atkMRA_<action> we let
        // this function handle the MRA action instead of submitting the form
        if (!ignoreHandler && 'atkMRA_' + atkaction) {
            try {
                var handler = eval('atkMRA_' + atkaction);
                handler(name, form, target);
                return;
            }
            catch (ex) {
                // If the handler cannot be called, proceed as normal.
            }
        }

        /* get selectors */
        var list = ATK.FormSelect.getATKSelectors(name, form);

        /* count selected selectors */
        var selectorLength = 0;

        // Container for the hidden elements we are about to submit
        var hiddenInputContainer = document.createElement('span');
        hiddenInputContainer.style.display = 'none';
        form.appendChild(hiddenInputContainer);


        for (var i = 0; i < list.length; i++) {
            if (list[i].type === 'hidden' || (!list[i].disabled && list[i].checked)) {
                var key = list[i].name.substring(name.length + 1);
                var value = list[i].value;

                // For single selects, we need to add the record number
                // after the user is done selecting, otherwise the singleselect
                // will act like a multi-select
                if (list[i].type === 'radio')
                    key += '[' + i + ']';

                // For multi-selects, we index the selectors with the record number to
                // be able to link the selector to the record after submit
                if (list[i].type === 'checkbox')
                    key = key.replace('[]', '[' + i + ']');

                if (embedded) {
                    target += '&' + key + '=' + value;
                } else {
                    input = document.createElement('input');
                    input.setAttribute('type', 'hidden');
                    input.setAttribute('name', key);
                    input.setAttribute('value', value);
                    hiddenInputContainer.appendChild(input);
                }

                selectorLength++;
            }
        }

        /* change atkaction and atkrecordlist values and submit form */
        if (selectorLength > 0) {
            if (embedded) {
                target += '&atkaction=' + atkaction;
            } else if (typeof(form.atkaction) === 'undefined') {
                input = document.createElement('input');
                input.setAttribute('type', 'hidden');
                input.setAttribute('name', 'atkaction');
                input.setAttribute('value', atkaction);
                hiddenInputContainer.appendChild(input);
            } else {
                form.atkaction.value = atkaction;
            }

            if (embedded) {
                target += '&atkrecordlist=' + name;
            } else if (typeof(form.atkrecordlist) === 'undefined') {
                input = document.createElement('input');
                input.setAttribute('type', 'hidden');
                input.setAttribute('name', 'atkrecordlist');
                input.setAttribute('value', name);
                hiddenInputContainer.appendChild(input);
            } else {
                form.atkrecordlist.value = name;
            }

            // default the form is build using SESSION_DEFAULT,
            // but if we submit a multi-record-action we should
            // use SESSION_NESTED instead, the difference is that
            // SESSION_NESTED increases the session level by 1, so
            // let's do so manually
            if (typeof(form.atklevel) === 'undefined') {
                input = document.createElement('input');
                input.setAttribute('type', 'hidden');
                input.setAttribute('name', 'atklevel');
                input.setAttribute('value', 1);
                hiddenInputContainer.appendChild(input);
            } else {
                form.atklevel.value = parseInt(form.atklevel.value) + 1;
            }

            if (embedded) {
                form.atkescape.value = target;
            }

            ATK.globalSubmit(form, false);
            form.submit();

            // In some rare occasions we have to remove the hidden elements from the
            // form because otherwise they are resubmitted eventhough the selection could
            // be changed.
            form.removeChild(hiddenInputContainer);
        }
    },

    /**
     * Because we allow embedded recordLists for 1:n relations we need a way to somehow
     * distinguish between the submit of the edit form, and the submit of the multi-record action.
     * This method uses the atkescape option to redirect the multi-record-priority action to a level higher
     * on the session stack, which makes it possible to return to the edit form (saving updated values!)
     * @param name unique recordlist name
     * @param form reference to the form object
     * @param target where do we escape to?
     */
    atkSubmitMRPA: function (name, form, target) {
        // some stuff we need to know
        var index = form.elements[name + '_atkaction'].selectedIndex;
        var atkaction;

        if (typeof (index) === 'undefined') {
            atkaction = form.elements[name + '_atkaction'].value;
        } else {
            atkaction = form.elements[name + '_atkaction'][index].value;
        }

        if (atkaction === '') {
            return;
        }

        // initial target URL
        target += 'atkaction=' + atkaction;

        // get selectors
        var list = ATK.FormSelect.getATKSelectors(name, form);

        // add the selectors to the target URL
        var selectorLength = 0;

        for (var i = 0; i < list.length; i++) {
            if (list[i].selectedIndex !== 0) {
                var priority = list[i][list[i].selectedIndex].value;
                target += '&atkselector[' + list[i][0].value + ']=' + priority;
                selectorLength++;
            }
        }

        // change atkescape value and submit form
        if (selectorLength > 0) {
            form.atkescape.value = target;
            ATK.globalSubmit(form, true);
            form.submit();
        }
    }
};
