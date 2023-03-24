if (!window.ATK) {
    var ATK = {};
}

ATK.ManyToManySelectRelation = {
    deleteItem: function (el) {
        jQuery(el).closest('.atkmanytomanyselectrelation-selected').remove();
    },

    add: function (el, url, makeSortable) {
        const $el = jQuery(el);

        if ($el.val()) {
            const boxParent = $el.closest('.atkmanytomanyselectrelation-addition');

            jQuery.post(url, {selector: $el.val()}, function (responseText) {
                let newItem = null;
                jQuery(responseText).each(function () {
                    if (jQuery(this).hasClass('atkmanytomanyselectrelation-selected')) {
                        newItem = this;
                    }
                });

                // adds the new item to the list
                boxParent.parent().next('.atkmanytomanyselectrelation-selection').append(newItem);

                // empty the select
                boxParent.find('select.ManyToOneRelation').val(null).trigger('change');

                // loads js scripts, here it seems to be useless...
                // jQuery('<div>').html(responseText).find('script').each(function () {
                //     var text = jQuery(this).text();
                //     if (text) {
                //         jQuery.globalEval(text);
                //     }
                // });

                // order by id, it doesn't work...
                if (makeSortable) {
                    // ATK.ManyToManySelectRelation.makeItemsSortable(li.closest('ul').attr('id'));
                }
            });
        }
    },

    makeItemsSortable: function (id) {
        // https://johnny.github.io/jquery-sortable/
        jQuery('#' + id).sortable();
    }
};
