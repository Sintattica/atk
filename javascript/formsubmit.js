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

function atkSubmit(target)
{
    if (target == '-1')
        return;

    // Set ALL <input name="atkescape"> to target--for some reason
    // there are multiple atkescape inputs on some pages, as it's
    // possible to set the wrong one, which means atksession() in
    // class.atksessionmanager.inc gets a blank atkescape.
    $$('input[name="atkescape"]').each(function(n) {
        n.value = target;
    });

    // call global submit function, which doesn't get called automatically
    // when we call entryform.submit manually.
    globalSubmit(document.entryform);
    document.entryform.submit();
}


function preGlobalSubmit(formEl, bag) {
    var form = jQuery(formEl);
    var spinner = form.find('#action-buttons .spinner');
    var atksubmitaction;
    bag.spinnerVisibility = spinner.css('visibility');
    spinner.css('visibility', 'visible');

    //ho premuto un bottone?
    var actionButton = form.find("#action-buttons button:focus").get(0);

    //nessun bottone premuto, considero il primo a sinistra *nel DOM*
    if(actionButton === undefined) {
        actionButton = form.find("#action-buttons button").get(0);
    }

    if(actionButton) {
        actionButton = jQuery(actionButton);
        atksubmitaction = form.find('input[type="hidden"].atksubmitaction');
        bag.actionButtonEl = actionButton;
        bag.actionButtonDisabled = actionButton.prop('disabled');
        actionButton.prop('disabled', true);
        atksubmitaction.attr('name', actionButton.attr('name')).val(actionButton.val());
    }

    return true;
}

function postGlobalSubmit(formEl, bag, retval) {
    if(!retval) {
        var spinner = jQuery(formEl).find('#action-buttons .spinner');
        spinner.css('visibility', bag.spinnerVisibility);

        if(bag.actionButton) {
            bag.actionButton.prop('disabled', bag.actionButtonDisabled);
        }
    }
    return retval;
}