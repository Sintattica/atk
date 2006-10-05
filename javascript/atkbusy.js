Ajax.Responders.register({
  onCreate: function() {
    if ($('atkbusy') && Ajax.activeRequestCount > 0) {
      $('atkbusy').style.visibility = 'visible';
    }
  },
  
  onComplete: function() {
    if ($('atkbusy') && Ajax.activeRequestCount == 0) {
      $('atkbusy').style.visibility = 'hidden';
    }
  }
});