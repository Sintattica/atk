if (!window.ATK) {
    var ATK = {};
}

ATK.ManyToManySelectRelation = {
    deleteItem: function (el) {
        jQuery(el).closest('li').remove();
    },

    add: function (el, url, makeSortable) {
        var $el = jQuery(el);

        if($el.val()) {
            var li = $el.closest('li');

            jQuery.post(url, {selector: $el.val()}, function (responseText) {
                li.before(responseText);
                li.find('select.ManyToOneRelation').val(null).trigger('change');
                jQuery('<div>').html(responseText).find('script').each(function () {
                    var text = jQuery(this).text();
                    if (text) {
                        jQuery.globalEval(text);
                    }
                });
                if (makeSortable) {
                    ATK.ManyToManySelectRelation.makeItemsSortable(li.closest('ul').attr('id'));
                }
            });
        }
    },

    makeItemsSortable: function (id) {
        // https://johnny.github.io/jquery-sortable/
        jQuery('#' + id).sortable();
    }
};
