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
 */


if (!ATK) {
    var ATK = {};
}


ATK.RL = {

    // actions
    a: {},

    /**
     * Try to perform one of the given actions on the given row
     * until one action can be succesfully performed.
     */
    rl_try: function (recordListId, clickEvent, rowNum, actions, confirmText) {
        // Ignore click events on the checkbox because they will be  forwarded already
        // by the toggleRecord method. We only have to do this for Firefox because
        // Internet Explorer will only call the onClick method on the checkbox and not both.
        var target = clickEvent === null ? null : (clickEvent.target || clickEvent.srcElement);

        if (target && ['INPUT', 'SELECT', 'OPTION', 'A'].indexOf(target.tagName) >= 0) {
            return;
        }

        jQuery(actions).each(function (index, action) {
            if (ATK.RL.rl_doAndReturn(recordListId, rowNum, action, confirmText)) {
                return false;
            }
        });
    },

    rl_doAndReturn: function (rlId, rownum, action, confirmtext) {
        var extra = "";
        if (confirmtext) {
            var confirmed = confirm(confirmtext);
            if (confirmed) {
                extra = "&confirm=1";
            }
        }

        if (ATK.RL.a[rlId][rownum][action] && (!confirmtext || confirmed)) {
            if (typeof (ATK.RL.a[rlId][rownum][action]) === 'function') {
                ATK.RL.a[rlId][rownum][action](rlId);
            } else if (!ATK.RL.a[rlId]['embed']) {
                document.location.href = ATK.RL.a[rlId][rownum][action] + '&' + ATK.RL.a[rlId]['base'] + extra;
            } else {
                ATK.FormSubmit.atkSubmit(ATK.RL.a[rlId][rownum][action] + '&' + ATK.RL.a[rlId]['base'] + extra, true);
            }

            return true;
        } else {
            return false;
        }
    },
    rl_do: function (rlId, rownum, action, confirmtext) {
        ATK.RL.rl_doAndReturn(rlId, rownum, action, confirmtext);
    },
    rl_next: function (rlId) {
        if (ATK.RL.a[rlId]['next']) {
            document.location.href = ATK.RL.a[rlId]['next'];
        }
        return false;
    },
    rl_previous: function (rlId) {
        if (ATK.RL.a[rlId]['previous']) {
            document.location.href = ATK.RL.a[rlId]['previous'];
            return true;
        }
        return false;
    },
    highlightRow: function (row, color) {
        if (typeof (row.style) !== 'undefined') {
            row.oldcolor = row.style.backgroundColor;
            row.style.backgroundColor = color;
        }
    },
    resetRow: function (row) {
        row.style.backgroundColor = row.oldcolor;
    },
    selectRow: function (row, rlId, rownum) {
        var table = document.getElementById(rlId);
        if (table.listener && table.listener.setRow(rownum, row.oldcolor)) {
            row.oldcolor = row.style.backgroundColor;
        }
    }
};
