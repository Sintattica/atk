# UI and templates

[← Documentation index](../README.md)

The ATK user interface is built on these pieces:

- **AdminLTE 3** with Bootstrap 4 and jQuery;
- **Smarty 5** templates;
- a small set of PHP classes in `Sintattica\Atk\Ui` ([source](../../src/Ui)).

This page explains how a page is put together and where you can hook in. For the full request flow, see [Request lifecycle](../internals/request-lifecycle.md).

## How a page is assembled

| Class | Role |
|---|---|
| `Ui\Page` | Singleton for the HTML document: registered scripts and styles, load and submit scripts, hidden vars, and the content. `Page::render()` produces the final HTML. |
| `Ui\Ui` | Singleton renderer for Smarty templates: `render($name, $vars)`, `renderBox()`, `renderAction()`, `renderList()`, `renderTabs()`. |
| `Ui\SmartyProvider` | Creates the shared `Smarty` instance, using config `template_dir` and `tplcompiledir`, with the plugins in `Ui/smarty-plugins`. |
| `Ui\PageBuilder` / `Ui\ActionBoxBuilder` | Fluent builder for standard action pages. |
| `Ui\IndexPage` | The outer layout: navbar, sidebar menu, breadcrumb, content. |
| `Ui\Output` | Sends headers and the body (optionally gzipped with `output_gzip`), and appends debug output. |
| `Core\AdminLTE` | Turns the AdminLTE config booleans into body, sidebar and navbar CSS classes. |

A handler renders its content into "boxes" (`box.tpl`) and wraps them in an action page (`actionpage.tpl`, which also shows queued messages). It then adds the result to the `Page`:

```php
public function action_report(ActionHandler $handler)
{
    $html = $this->getUi()->render('report.tpl', ['rows' => $this->select()->getAllRows()]);
    $box = $this->getUi()->renderBox(['title' => 'Report', 'content' => $html]);
    $this->getPage()->addContent($this->renderActionPage('report', $box));
}
```

`Node::createPageBuilder()` returns a `PageBuilder` that does the same thing fluently:

```php
$this->createPageBuilder()
    ->title('Report')
    ->beginActionBox()
        ->formStart('<form method="post">')
        ->content($html)
        ->buttons($buttons)
        ->formEnd('</form>')
    ->endActionBox()
    ->render();
```

## Templates

The built-in templates are in [`src/Resources/templates`](../../src/Resources/templates):

| Area | Templates |
|---|---|
| Layout | `layout.tpl`, `page.tpl`, `index_page.tpl`, `actionpage.tpl`, `box.tpl`, `dialog.tpl`, `footer.tpl`, `statusbar.tpl` |
| Actions | `action.tpl` (fallback), `action_add.tpl`, `action_edit.tpl`, `action_view.tpl`, `action_delete.tpl`, `action_save.tpl`, `action_update.tpl`, `action_search.tpl`, `action_export.tpl`, `action_import.tpl` |
| Forms | `editform.tpl`, `editform_common.tpl`, `editform_fields.tpl`, `tabbededitform.tpl`, `integratededitform.tpl`, `viewform.tpl`, `tabbedviewform.tpl`, `field.tpl`, `tabs.tpl`, `panetabs.tpl`, `lockinfo.tpl` |
| Lists | `list.tpl`, `recordlist.tpl`, `datagrid.tpl`, `dgindex.tpl`, `dglimit.tpl`, `dgpaginator.tpl` |
| Search | `searchform.tpl`, `smartsearchform.tpl`, `smartcriterium.tpl` |
| Login | `login.tpl`, `login_meta.tpl`, `u2f.tpl` |
| Menu | `menu/` (navbar, sidebar, breadcrumb and item partials) |
| Debug | `debugger.tpl` |

`Ui::renderAction($action, $vars)` looks for `action_<action>.tpl` and falls back to `action.tpl`. To give a custom action its own layout, provide a template named after it.

### Adding and overriding templates

Template lookup is plain Smarty: the directories in the template-dir list are searched **in order**, and the first match wins. The list starts with the config key `template_dir`, which defaults to ATK's own `src/Resources/templates/`.

- **New templates:** append your own directory, typically in `Module::register()`. This is what the skeleton does:

  ```php
  SmartyProvider::getInstance()->addTemplateDir(__DIR__ . '/templates');
  ```

  Because ATK's directory comes first, `addTemplateDir()` can add new templates (e.g. `action_demo.tpl`), but **cannot replace** a built-in one with the same name.

- **Replacing built-in templates:** put your directory before ATK's. Either set `template_dir` in your application config to an array with your directory first and ATK's directory after it, or call `SmartyProvider::getInstance()->setTemplateDir([...])` in the same order.

`Config::init()` merges your application config with `Tools::atk_array_merge_recursive()`. An array value therefore replaces the default string:

```php
// config/atk.php
'template_dir' => [__DIR__ . '/../templates/', __DIR__ . '/../vendor/sintattica/atk/src/Resources/templates/'],
```

Compiled templates go to `tplcompiledir` (default under `atktempdir`). Compilation is controlled with `tplcompilecheck` / `tplforcecompile`, which can be set through the `TPL_COMPILE_CHECK` / `TPL_FORCE_COMPILE` environment variables. See [Environment variables from `.env` are not visible to `Config::env()`](../internals/known-issues.md#environment-variables-from-env-are-not-visible-to-configenv).

### Smarty plugins

ATK registers these plugins ([`src/Ui/smarty-plugins`](../../src/Ui/smarty-plugins)):

| Plugin | Usage |
|---|---|
| `{atktext}` | Translation: `{atktext id="description" module="users" node="userinfo"}`. See [Internationalisation](i18n.md). |
| `{atkconfig}` | Read a config value into a template variable: `{atkconfig var="login_logo" smartyvar="login_logo"}`. |
| `{atkmessages}` | Assign the queued messages (`Utils\MessageQueue`) to `$atkmessages`. |
| `{atkdatefield}` | Embed a date control. |
| `{stacktrace}` | Render a stack trace (debug). |
| `atkvardump` modifier | Dump a variable (debug). |
| `string:` resource | Render a template from a string. |

## Messages

To show a message on the next page, queue it with `Utils\MessageQueue`:

```php
MessageQueue::getInstance()->addMessage('Invoice sent', MessageQueue::AMQ_SUCCESS);
```

The types are `AMQ_GENERAL`, `AMQ_SUCCESS`, `AMQ_WARNING` and `AMQ_FAILURE`. `actionpage.tpl` displays the queue.

## Scripts and styles

`Page` registers the standard assets in its constructor: jQuery, Bootstrap 4 bundle, Select2 (plus its locale), Moment, Tempus Dominus, bootstrap-switch, inputmask, overlayScrollbars, AdminLTE, Font Awesome, and ATK's `tools.js` / `atk.js`. Assets are served from the config key `assets_url` (default `/bundles/atk/`). Your web server must expose `src/Resources/public/` at that URL; see [Installation](../getting-started/installation.md).

From a node or attribute, use the `Page` API:

| Method | Purpose |
|---|---|
| `register_script($file)` / `unregister_script($name)` | External JavaScript file. |
| `register_scriptcode($code, $before = false)` | Inline JavaScript. |
| `register_loadscript($code)` | Code run on page load. |
| `register_submitscript($code)` | Code run on form submit. |
| `register_style($file, $media = 'all')` / `unregister_style($file)` | Stylesheet. |
| `register_stylecode($code)` | Inline CSS. |
| `register_hiddenvars(array $vars)` | Hidden form variables. |
| `register_metacode($code)` | Extra `<head>` meta tags. |
| `setTitle($title)`, `setFullscreenMode(bool)` | Page title, fullscreen layout. |

```php
$page = $this->getPage();
$page->register_script(Config::getGlobal('assets_url') . 'javascript/dateattribute.js');
$page->register_loadscript("console.log('form ready');");
```

**Application stylesheet and branding** (config keys):

| Key | Effect |
|---|---|
| `style_url` | Your own stylesheet, registered after the AdminLTE CSS. |
| `custom_font` | Registers `admin-lte/css/<name>-font.css`. |
| `brand_logo` | Logo in the sidebar or navbar. |
| `login_logo` | Logo on the login page. |
| `favicon_base_url`, `manifest_url` | Favicons and web manifest. |
| `icon_*`, `recordlist_icons` | Font Awesome classes for action icons. |

## AdminLTE layout options

The layout is controlled with boolean config keys, read by `Core\AdminLTE`:

| Area | Keys |
|---|---|
| Sidebar | `collapsedSidebar`, `expandSidebarOnHover`, `indentSidebarChildren`, `flatNavStyle`, `compactSidebarStyle`, `legacyNavStyle`, `menuAccordion`, `sidebarElevation` |
| Navbar | `fixedNavHeader`, `enableDevModeNavbar` (a distinct navbar colour scheme in development) |
| Text size | `bodySmallText`, `sidebarSmallText`, `navSmallText`, `footerSmallText`, `brandSmallText` |
| Behaviour | `holdTransition` |

`menu_hide_sidebar => true` switches to a navbar-only layout (`menu/navbar_without_sidebar.tpl`). Menu contents are covered in [Modules and menu](modules-and-menu.md).

All the keys are listed in the [Configuration reference](../reference/configuration.md).

## The JavaScript layer

ATK's own scripts are in [`src/Resources/public/javascript`](../../src/Resources/public/javascript). Each one defines an object under the global `ATK` namespace:

| File | Global | Loaded |
|---|---|---|
| `tools.js` | `ATK.Tools` | Always. DOM helpers and attribute show/hide. |
| `atk.js` | — | Always. jQuery bootstrapping, AJAX busy indicator, `visible()`/`invisible()` helpers. |
| `attribute.js` | `ATK.Attribute` | Attributes with dependencies or refresh (`ATK.Attribute.callDependencies()`). |
| `datagrid.js` | `ATK.DataGrid` | DataGrid AJAX update and save. |
| `recordlist.js` | `ATK.RL` | Record list row actions and highlighting. |
| `formsubmit.js` | `ATK.FormSubmit` | Global form-submit hooks. |
| `formselect.js` | `ATK.FormSelect` | Multi-record action submission. |
| `tabs.js`, `tabbedpane.js` | `ATK.Tabs`, `ATK.TabbedPane` | Tabs. |
| `dateattribute.js` | `ATK.DateAttribute` | Date attribute. |
| `manytoonerelation.js`, `manytomanyselectrelation.js`, `onetomanyrelation.js`, `shuttlerelation.js` | `ATK.ManyToOneRelation`, `ATK.ManyToManySelectRelation`, `ATK.OneToManyRelation`, `ATK.ShuttleRelation` | Relations. |
| `profileattribute.js`, `radiodetailsattribute.js` | `ATK.ProfileAttribute`, `ATK.RadioDetailsAttribute` | Those attributes. |
| `smartsearchhandler.js` | `ATK.SmartSearchHandler` | Smart search. |
| `debug.js` | `ATK.Debug` | Debug console (`Utils\Debugger`). |
| `u2f-api.js` | — | U2F login. |

Apart from `tools.js` and `atk.js`, each script is registered by the PHP class that needs it.

`libs.min.js` is a legacy bundle (jQuery 2 and Bootstrap 3) and is not loaded by anything.

### Partial (AJAX) requests

Attributes and handlers can serve fragments of a page:

1. Build the URL with `Tools::partial_url($nodeUri, $action, $partial, $params)`. It sets the `atkpartial` request parameter and the session status `SessionManager::SESSION_PARTIAL`.
2. ATK calls `partial_<name>()` on the handler, or on the attribute named in the partial (for example `Attribute::partial_dependencies()`).
3. The output is returned with `Page::renderPartial()`.

Details are in [Action handlers](../internals/handlers.md).

## See also
- [Request lifecycle](../internals/request-lifecycle.md)
- [Action handlers](../internals/handlers.md)
- [Modules and menu](modules-and-menu.md)
- [Configuration reference](../reference/configuration.md)
