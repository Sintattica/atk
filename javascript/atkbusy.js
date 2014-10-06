Ajax.Responders.register({
    count: 0,
    onCreate: function() {
        this.count++;

        var as = $$('.atkbusy');
        if (as.length == 0) { // shows default spinner (on top) only if there are not attributes spinners
            if ($('atkbusy')) {
                $('atkbusy').style.visibility = 'visible';
            }
        }
    },
    onComplete: function() {
        this.count--;

        if (this.count == 0) {

            if ($('atkbusy')) {
                $('atkbusy').style.visibility = 'hidden';
            }

            // hides attributes spinners
            var as = $$('.atkbusy');
            for (var i = 0; i < as.length; i++) {
                as[i].style.display = 'none';
            }
        }
    },
    onException: function(request, ex) {
        this.onComplete();

        if (typeof (console) == 'object' && console.debug) {
            console.debug('Exception in Ajax.Request:', ex, ex.stack);
        }
    }
});