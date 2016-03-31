function mto_parse(link, value) {
    var value_array = value.split('=');
    if (value_array[1] == '' || typeof value_array[1] == "undefined")
        return -1;
    var atkselector = value.replace("='", "_1253D_12527").replace("'", "_12527");
    return link.replace('REPLACEME', atkselector);
}

if (!window.ATK) {
    var ATK = {};
}

ATK.ManyToOneRelation = {

    select: function (field, options) {
        var $ = jQuery;
        var $field = $('#' + field);
        var defaultOptions = {
            'dropdownAutoWidth': true,
            'width': 'resolve'

        };
        var opts = $.extend(true, {}, defaultOptions, options);
        $field.select2(opts);
        return $field;
    },
    
    autocomplete: function (field, options) {
        var $ = jQuery;

        var $spinner = $('#' + field + '__spinner');
        var $field = $('#' + field);

        var defaultOptions = {
            width: '100%',
            ajax: {
                url: '',
                delay: 300,

                beforeSend: function () {
                    if ($spinner) {
                        $spinner.visible();
                    }
                },
                complete: function () {
                    if ($spinner) {
                        $spinner.invisible();
                    }
                },
                data: function (params) {
                    return {
                        value: params.term,
                        page: params.page || 1
                    };
                },
                processResults: function (data) {
                    var results = [];

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
                            if ($item.attr('id') == 'more') {
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
                },
                cache: true
            },
            minimumInputLength: 1
        };

        var opts = $.extend(true, {}, defaultOptions, options);
        $field.select2(opts);
        return $field;
    }
};