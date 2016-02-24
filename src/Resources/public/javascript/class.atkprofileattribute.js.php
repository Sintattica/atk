function profile_getForm() {
    if (document.dialogform)
        return document.dialogform;
    else
        return document.entryform;
}

function profile_checkAll(fieldname) {
    with (profile_getForm()) {
        for (i = 0; i < elements.length; i++) {
            if (elements[i].name.substr(0, fieldname.length) == fieldname) {
                elements[i].checked = true;
            }
        }
    }
}

function profile_checkNone(fieldname) {
    with (profile_getForm()) {
        for (i = 0; i < elements.length; i++) {
            if (elements[i].name.substr(0, fieldname.length) == fieldname) {
                elements[i].checked = false;
            }
        }
    }
}

function profile_checkInvert(fieldname) {
    with (profile_getForm()) {
        for (i = 0; i < elements.length; i++) {
            if (elements[i].name.substr(0, fieldname.length) == fieldname) {
                elements[i].checked = !elements[i].checked;
            }
        }
    }
}


function profile_checkAllByValue(fieldname, fieldvalue) {
    with (profile_getForm()) {
        for (i = 0; i < elements.length; i++) {
            if (elements[i].name.substr(0, fieldname.length) == fieldname && elements[i].value.substr(0, fieldvalue.length) == fieldvalue) {
                elements[i].checked = true;
            }
        }
    }
}

function profile_checkNoneByValue(fieldname, fieldvalue) {
    with (profile_getForm()) {
        for (i = 0; i < elements.length; i++) {
            if (elements[i].name.substr(0, fieldname.length) == fieldname && elements[i].value.substr(0, fieldvalue.length) == fieldvalue) {
                elements[i].checked = false;
            }
        }
    }
}

function profile_checkInvertByValue(fieldname, fieldvalue) {
    with (profile_getForm()) {
        for (i = 0; i < elements.length; i++) {
            if (elements[i].name.substr(0, fieldname.length) == fieldname && elements[i].value.substr(0, fieldvalue.length) == fieldvalue) {
                elements[i].checked = !elements[i].checked;
            }
        }
    }
}

function profile_fixExpandImage(divName, atkRoot) {
    var icon = get_object("img_" + divName);
    if (get_object(divName).style.display == 'none')
        icon.className = ATK_PROFILE_ICON_OPEN;
    else
        icon.className = ATK_PROFILE_ICON_CLOSE;
}

function profile_fixDivState(divName) {
    var divElement = get_object(divName);
    var inputElement = get_object("divstate['" + divName + "']");

    if (divElement.style.display == 'none')
        inputElement.value = 'closed';
    else
        inputElement.value = 'opened';
}

function profile_swapProfileDiv(divName, atkRoot) {
    toggleDisplay(divName, get_object(divName));
    profile_fixExpandImage(divName, atkRoot);
    profile_fixDivState(divName);
}