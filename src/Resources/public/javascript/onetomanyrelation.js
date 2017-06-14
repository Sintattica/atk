if (!window.ATK) {
    var ATK = {};
}

ATK.OneToManyRelation = {
    toggleAddForm: function (formname, linkname) {
        ATK.Tools.toggleDisplay(formname, ATK.Tools.get_object(formname));
        ATK.Tools.toggleDisplay(linkname, ATK.Tools.get_object(linkname));
    }
};
