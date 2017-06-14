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
    },
    profile_fixExpandImage: function (divName) {
        var icon = ATK.Tools.get_object("img_" + divName);
        if (ATK.Tools.get_object(divName).style.display === 'none')
            icon.className = ATK_PROFILE_ICON_OPEN;
        else
            icon.className = ATK_PROFILE_ICON_CLOSE;
    },
    profile_fixDivState: function (divName) {
        var divElement = ATK.Tools.get_object(divName);
        var inputElement = ATK.Tools.get_object("divstate['" + divName + "']");

        if (divElement.style.display === 'none') {
            inputElement.value = 'closed';
        } else {
            inputElement.value = 'opened';
        }
    },
    profile_swapProfileDiv: function (divName) {
        ATK.Tools.toggleDisplay(divName, ATK.Tools.get_object(divName));
        ATK.ProfileAttribute.profile_fixExpandImage(divName);
        ATK.ProfileAttribute.profile_fixDivState(divName);
    }
};
