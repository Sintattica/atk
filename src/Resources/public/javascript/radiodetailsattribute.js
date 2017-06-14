if (!window.ATK) {
    var ATK = {};
}

ATK.RadioDetailsAttribute = {
    select: function (option, detailsUrl) {
        var options = jQuery(option).closest('.atkradiodetailsattribute-selection').getElementsBySelector('.atkradiodetailsattribute-option');
        var detailsEl;
        for (var i = 0; i < options.length; i++) {
            detailsEl = jQuery('#' + options[i].id + '_details');
            if (detailsEl !== null) {
                detailsEl.html('');
            }
        }

        if (detailsUrl !== null) {
            jQuery('#' + option.id + '_details').load(detailsUrl);
        }
    }
};
