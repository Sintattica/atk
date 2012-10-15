// Fix for showing the shadow at first when the dialog is really shown.
UI.Window.addMethods({
  methodsAdded: function(base) {
    base.aliasMethodChain('create', 'fix');
  },

  showShadow: function() {
    if (!this.shadow && this.options.shadow) {
      this.shadow = new UI.Shadow(this.element, {theme: this.getShadowTheme()});
    }
  
    if (this.shadow) {
      this.shadow.hide();
      this.effect('show', this.shadow.shadow);
    }
  },

  // Private Functions
  createWithFix: function() {
    this.createWithoutShadow();

    this.observe('showing', this.showShadow)
        .observe('hiding',  this.hideShadow)
        .observe('hidden',  this.removeShadow)
        .observe('focused', this.focusShadow)
        .observe('blurred', this.blurShadow);
  }
});