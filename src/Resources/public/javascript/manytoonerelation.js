if (!window.ATK) {
    var ATK = {};
}

ATK.ManyToOneRelation = {
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
