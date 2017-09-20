if (!window.ATK) {
    var ATK = {};
}

ATK.ManyToManySelectRelation = {
    deleteItem: function (el) {
        jQuery(el).closest('li').remove();
    },

    add: function (el, url, makeSortable) {
        var $el = jQuery(el);
        var params = {selector: $el.val()};
        var li = $el.closest('li');

        jQuery.post(url, params, function (responseText) {
            li.before(responseText);
            li.find('select.ManyToOneRelation').empty();
            jQuery('<div>').html(responseText).find("script").each(function () {
                var text = jQuery(this).text();
                if(text) {
                    jQuery.globalEval(text);
                }
            });
            if (makeSortable) {
                ATK.ManyToManySelectRelation.makeItemsSortable(li.closest('ul').attr('id'));
            }
        });


        if (el.type === 'select-one') {
            el.selectedIndex = 0;
        }
        else {
            jQuery('#' + $el.attr('name') + '_search').val('');
            $el.val('');
        }
    },

    makeItemsSortable: function (id) {
        // https://johnny.github.io/jquery-sortable/
        jQuery('#' + id).sortable();
    }
};
