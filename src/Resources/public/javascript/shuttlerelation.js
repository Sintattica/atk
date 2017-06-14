if (!window.ATK) {
    var ATK = {};
}

ATK.ShuttleRelation = {
    shuttle_selectAll: function (id) {
        var el = document.getElementById(id);
        var options = el.options;

        for (var i = 0; i < options.length; i++) {
            options[i].selected = true;
        }
        return true;
    },
    shuttle_change: function (name) {
        var changefunction = name + '_onChange';
        eval('if (window.' + changefunction + ') window.' + changefunction + '();');
    },
    shuttle_move: function (id1, id2, name) {
        var el1 = document.getElementById(id1);
        var el2 = document.getElementById(id2);

        var newel = el1.cloneNode(false);
        newel.options.length = 0;

        for (var i = 0; i < el1.options.length; i++) {
            if (el1.options[i].selected &&
                (window.getComputedStyle(el1.options[i])).display !== 'none') {
                // move options only if not hidden because of filters
                el2.options[el2.options.length] = new Option(el1.options[i].text, el1.options[i].value);
            } else {
                newel.options[newel.options.length] = new Option(el1.options[i].text, el1.options[i].value);
                // remember if option is hidden or not
                jQuery(newel.options[newel.options.length - 1]).css(
                    'display',
                    (window.getComputedStyle(el1.options[i])).display !== 'none' ? 'block' : 'none'
                );
            }
        }

        el1.options.length = 0;
        for (i = 0; i < newel.options.length; i++) {
            el1.options[el1.options.length] = new Option(newel.options[i].text, newel.options[i].value);
            jQuery(el1.options[el1.options.length - 1]).css(
                'display',
                jQuery(newel.options[i]).css('display')
            );
        }
        ATK.ShuttleRelation.shuttle_change(name);
    },
    shuttle_moveall: function (id1, id2, name) {
        var el1 = document.getElementById(id1);
        var el2 = document.getElementById(id2);

        for (var i = 0; i < el1.options.length; i++) {
            if ((window.getComputedStyle(el1.options[i])).display !== 'none') {
                // move options only if not hidden because of filters
                el2.options[el2.options.length] = new Option(el1.options[i].text, el1.options[i].value);
                el1.removeChild(el1.options[i]);
                i--;
            }
        }

        ATK.ShuttleRelation.shuttle_change(name);
    }
};
