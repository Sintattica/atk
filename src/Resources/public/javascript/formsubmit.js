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
 * @version $Revision: 6176 $
 * $Id$
 */

if (!window.ATK) {
    var ATK = {};
}

ATK.FormSubmit = {
    atkSubmit: function (target, standardSubmit) {
        //if standardSubmit == true, the submit action doesn't come from the main form action buttons


        if (target === '-1')
            return;

        // Set ALL <input name="atkescape"> to target--for some reason
        // there are multiple atkescape inputs on some pages, as it's
        // possible to set the wrong one, which means atkSessionManager::atksession() in
        // gets a blank atkescape.
        jQuery('input[name="atkescape"]').each(function (index, n) {
            n.value = target;
        });


        if (typeof CKEDITOR === 'object') {
            for (var instanceName in CKEDITOR.instances) {
                CKEDITOR.instances[instanceName].updateElement();
            }
        }

        // call global submit function, which doesn't get called automatically
        // when we call entryform.submit manually.
        var entryForm = jQuery("#entryform");
        ATK.globalSubmit(entryForm, standardSubmit);
        entryForm.submit();
    },
    preGlobalSubmit: function (formEl, bag, standardSubmit) {

        //if standardSubmit == true, ignore
        if (standardSubmit) {
            return true;
        }

        var form = jQuery(formEl);
        var spinner = form.find('#action-buttons .spinner');
        var atksubmitaction;
        bag.spinnerVisibility = spinner.css('visibility');
        spinner.css('visibility', 'visible');

        var actionButton = form.find("#action-buttons button[clicked=true]").get(0);

        // No action button pressed, probably triggered by enter key on text input.
        // Consider first *in DOM*
        if (actionButton === undefined) {
            actionButton = form.find("#action-buttons button").get(0);
        }

        if (actionButton) {
            actionButton = jQuery(actionButton);
            atksubmitaction = form.find('input[type="hidden"].atksubmitaction');
            bag.actionButtonEl = actionButton;
            bag.actionButtonDisabled = actionButton.prop('disabled');
            bag.atksubmitaction = atksubmitaction;
            actionButton.prop('disabled', true);
            atksubmitaction.attr('name', actionButton.attr('name')).val(actionButton.val());
        }

        return true;
    },
    postGlobalSubmit: function (formEl, bag, retval, standardSubmit) {
        if (standardSubmit) {
            return retval;
        }

        if (!retval) {
            var spinner = jQuery(formEl).find('#action-buttons .spinner');
            spinner.css('visibility', bag.spinnerVisibility);

            if (bag.actionButton) {
                bag.actionButton.prop('disabled', bag.actionButtonDisabled);
            }
            if (bag.atksubmitaction) {
                bag.atksubmitaction.removeAttr('name').removeAttr('value');
            }
        }
        return retval;
    }
};
