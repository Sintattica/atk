if (!window.ATK) {
    var ATK = {};
}

ATK.ProfileAttribute = {
    profile_getForm: function () {
        return document.entryform;
    },
    profile_checkAll: function (fieldname) {
        var $form = jQuery(ATK.ProfileAttribute.profile_getForm());

        $form.find(':checkbox').each(function(index, el){
           var $el = jQuery(el);
           if($el.attr('name').substr(0, fieldname.length) === fieldname){
               $el.prop('checked', true);
           }
        });
    },
    profile_checkNone: function (fieldname) {
        var $form = jQuery(ATK.ProfileAttribute.profile_getForm());

        $form.find(':checkbox').each(function(index, el){
            var $el = jQuery(el);
            if($el.attr('name').substr(0, fieldname.length) === fieldname){
                $el.prop('checked', false);
            }
        });
    },
    profile_checkInvert: function (fieldname) {
        var $form = jQuery(ATK.ProfileAttribute.profile_getForm());

        $form.find(':checkbox').each(function(index, el){
            var $el = jQuery(el);
            if($el.attr('name').substr(0, fieldname.length) === fieldname){
                $el.prop('checked', !$el.prop('checked'));
            }
        });
    },
    profile_checkAllByValue: function (fieldname, fieldvalue) {
        var $form = jQuery(ATK.ProfileAttribute.profile_getForm());

        $form.find(':checkbox').each(function(index, el){
            var $el = jQuery(el);
            if($el.attr('name').substr(0, fieldname.length) === fieldname && $el.val().substr(0, fieldvalue.length) === fieldvalue){
                $el.prop('checked', true);
            }
        });
    },
    profile_checkNoneByValue: function (fieldname, fieldvalue) {
        var $form = jQuery(ATK.ProfileAttribute.profile_getForm());

        $form.find(':checkbox').each(function(index, el){
            var $el = jQuery(el);
            if($el.attr('name').substr(0, fieldname.length) === fieldname && $el.val().substr(0, fieldvalue.length) === fieldvalue){
                $el.prop('checked', false);
            }
        });
    },
    profile_checkInvertByValue: function (fieldname, fieldvalue) {
        var $form = jQuery(ATK.ProfileAttribute.profile_getForm());

        $form.find(':checkbox').each(function(index, el){
            var $el = jQuery(el);
            if($el.attr('name').substr(0, fieldname.length) === fieldname && $el.val().substr(0, fieldvalue.length) === fieldvalue){
                $el.prop('checked', !$el.prop('checked'));
            }
        });
    }
};
