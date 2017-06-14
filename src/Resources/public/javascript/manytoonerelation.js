if (!window.ATK) {
    var ATK = {};
}

ATK.ManyToOneRelation = {
    parse: function (link, value) {
        var value_array = value.split('=');
        if (value_array[1] === '' || typeof value_array[1] === "undefined")
            return -1;
        var atkselector = value.replace("='", "_1253D_12527").replace("'", "_12527");
        return link.replace('REPLACEME', atkselector);
    },

    autocomplete: {
        ajax: {
            cache: true,
            delay: 300,
            data: function (params) {
                return {
                    value: params.term,
                    page: params.page || 1
                };
            },
            processResults: function (data) {
                var results = [];
                var $ = jQuery;

                var $dom = $.parseHTML(data, document, true);
                var more = false;

                $.each($dom, function (i, item) {
                    var $item = $(item);
                    if ($item.is('ul')) {
                        $.each($item.find('li'), function (j, li) {
                            var $li = $(li);
                            results.push({
                                'id': $li.attr('value'),
                                'text': $li.text()
                            });
                        });
                    }
                    else if ($item.is('script')) {
                        var html = $item.html();
                        if (html) {
                            $.globalEval(html);
                        }
                    } else if ($item.is('div')) {
                        if ($item.attr('id') === 'more') {
                            more = $item.html() === 'true';
                        }
                    }
                });

                return {
                    results: results,
                    pagination: {
                        more: more
                    }
                };
            }
        }
    }
};
