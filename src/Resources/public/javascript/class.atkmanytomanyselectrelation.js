if (!window.ATK) {
    var ATK = {};
}

ATK.ManyToManySelectRelation = {
    deleteItem: function (el) {
        var li = $(el).up('li');
        li.parentNode.removeChild(li);
    },
    add: function (el, url, makeSortable) {
        var selector = $F(el);
        if (selector) {
            new Ajax.Request(
                url, {
                    parameters: {selector: selector},
                    onSuccess: function (response) {
                        var li = $(el).up('li');
                        var select = jQuery(li).find('select.ManyToOneRelation');
                        select.val(null).trigger('change');
                        li.insert({before: response.responseText});
                        response.responseText.evalScripts();
                        if (makeSortable) {
                            ATK.ManyToManySelectRelation.makeItemsSortable(li.up('ul'));
                        }
                    }
                });
        }
    },
    makeItemsSortable: function (id) {
        Sortable.create($(id), {
            constraint: 'vertical',
            only: 'atkmanytomanyselectrelation-selected'
        });
    }
};
