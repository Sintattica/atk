if (!window.ATK) {
  var ATK = {};
}

ATK.RadioDetailsAttribute = {
  select: function(option, detailsUrl) {
    var options = option.up('.atkradiodetailsattribute-selection').getElementsBySelector('.atkradiodetailsattribute-option');
    for (var i = 0; i < options.length; i++) {
      var detailsEl = $(options[i].id + '_details');
      if (detailsEl != null) {
        detailsEl.update('');
      }
    }
    
    if (detailsUrl != null) {
      var detailsEl = $(option.id + '_details');
      new Ajax.Updater(detailsEl, detailsUrl, { evalScripts: true });
    }
  }
}