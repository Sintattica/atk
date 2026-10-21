# Modules and menu

[← Documentation index](../README.md)

A **module** groups the nodes of one functional area and tells ATK which nodes exist and which actions they support. The **menu** is a separate class that builds the sidebar and navbar entries. This page covers both.

## Writing a module

Subclass `Core\Module` ([source](../../src/Core/Module.php)). Declare a public static `$module` name and implement `register()`:

```php
<?php

namespace App\Modules\Crm;

use Sintattica\Atk\Core\Module as AtkModule;

class Module extends AtkModule
{
    public static string $module = 'crm';

    public function register()
    {
        $this->registerNode('customers', Customers::class, ['admin', 'add', 'edit', 'delete', 'view', 'export']);
        $this->registerNode('contacts', Contacts::class, ['admin', 'add', 'edit', 'delete']);

        // A node used only as a relation target, with no own privileges
        $this->registerNode('customer_tags', CustomerTags::class);
    }
}
```

> **Note:** ATK 11 declares `$module` as `public static string $module`. Older skeleton code uses `static $module = '...'` without a type. PHP refuses to redeclare an inherited typed property without the type, so add `string` when you port old modules.

Then list the module class in the application configuration (`config/atk.php`):

```php
'modules' => [
    App\Modules\Crm\Module::class,
    App\Modules\Auth\Module::class,
],
```

### What the module API provides

| Method | Purpose |
|---|---|
| `register()` | **Required.** Called once per request from the `Atk` constructor, before the session is started and before authentication. Register nodes here and do cheap set-up only. |
| `boot()` | Optional. Called by `Atk::bootModules()` **after** successful authentication and after the menu class has run `appendMenuItems()`. You can use the current user here (`SecurityManager::atkGetUser()`). Not called on the login page. |
| `registerNode($name, $class, $actions = null)` | Maps `module.name` to a node class. If `$actions` is given, the node is also added to the privilege registry (`Atk::$g_nodes`) with those actions. |
| `unregisterNode($name)` | Removes a node registered earlier, for example to replace a node of another module. |
| `addNodeToMenu($menuName, $nodeName, $action, $parent = 'main', $enable = null, $order = 0, $position = MenuBase::MENU_SIDEBAR)` | Adds a menu entry pointing to `module.nodeName` / `$action`. By default it is enabled only if the user has that privilege. `$position` is currently ignored; see [Known issues](../internals/known-issues.md#moduleaddnodetomenu-ignores-position). |
| `addMenuItem($name = '', $url = '', $parent = 'main', $enable = 1)` | Adds a raw menu entry. |
| `getMenu()`, `getAtk()` | Protected accessors for the menu singleton and the `Atk` instance. |

### The actions list matters

The `$actions` array of `registerNode()` is not a permission check by itself. It is the list of privileges that *can* be granted on the node:
- `Attributes\ProfileAttribute` reads this registry to build the "access rights" editor of a group or level.
- `Node::hasAction()` reads it.
- `Node::checkTabRights()` is meant to use it too. It looks for `tab_<name>` entries to decide whether a tab needs a privilege. The skeleton registers `'tab_profilo'` this way.

A node registered **without** actions can still be instantiated and used as a relation destination. It does not appear in the access-rights editor. Its actions are checked by the authorization backend like any other node. This usually means they are denied when `restrictive` is `true`, unless the node uses `NF_NO_SECURITY` or `setSecurityAlias()`. See [Security](security.md).

> **Warning:** `Node::checkTabRights()` looks up `Atk::$g_nodes[$module][$type]`, but `Atk::registerNode()` stores entries as `$g_nodes[$section][$module][$type]`. With the default section (the module name), the tab lookup does not find the registered `tab_*` privileges, so tab-level security is not enforced. See [Known issues](../internals/known-issues.md#tab-privileges-are-never-enforced).

### Module resources

`Atk::moduleDir($module)` returns the directory that contains the module class file. ATK resolves module-relative resources from that directory:
- `languages/<lng>.php` holds the module translations. See [Internationalisation](i18n.md).
- `config/<section>.php` is read by `Config::get($section, $key)` when the application has no `config/<section>.php` of its own.
- Templates are not resolved automatically. Add the directory to Smarty yourself:

```php
use Sintattica\Atk\Ui\SmartyProvider;

public function register()
{
    SmartyProvider::getInstance()->addTemplateDir(__DIR__ . '/templates');
    // ...
}
```

## Building the menu

The menu is a singleton of the class named in the `menu` config key. The default is `Core\Menu\Menu`, a `final` class whose `appendMenuItems()` is empty. Applications therefore provide their own subclass of `Core\Menu\MenuBase` ([source](../../src/Core/Menu/MenuBase.php)) and register it:

```php
// config/atk.php
'menu' => App\Menu::class,
```

`MenuBase::appendMenuItems()` is abstract. ATK calls it once per request, from `Atk::initMenu()`, right before the modules' `boot()` methods run.

### Object API (recommended)

Build `Core\Menu\Item` objects and pass them to `MenuBase::add()`:

```php
<?php

namespace App;

use Sintattica\Atk\Core\Menu\ActionItem;
use Sintattica\Atk\Core\Menu\HeaderItem;
use Sintattica\Atk\Core\Menu\MenuBase;
use Sintattica\Atk\Core\Menu\SeparatorItem;
use Sintattica\Atk\Core\Menu\UrlItem;

class Menu extends MenuBase
{
    public function appendMenuItems()
    {
        $this->add((new UrlItem('home', '/'))->setIcon('fas fa-home'));

        $this->add(new ActionItem('customers', 'crm.customers', 'admin'));
        $this->add(new SeparatorItem());
        $this->add(new HeaderItem('system'));

        // A parent item: no node URI and no action
        $auth = new ActionItem('auth');
        $this->add($auth);
        $this->add((new ActionItem('users', 'auth.users', 'admin'))->setParent($auth->getName()));
        $this->add((new ActionItem('groups', 'auth.groups', 'admin'))->setParent($auth->getName()));
    }
}
```

**Item types** (all in `Core\Menu`):

| Class | Constructor | Notes |
|---|---|---|
| `ActionItem` | `(string $name, string $nodeUri = '', string $action = '')` | Links to a node action. Enabled only if the user is allowed `$nodeUri`/`$action`. With no node and no action it is a parent (submenu) item, shown when at least one child is enabled. If `$name` is empty, the translated node name is used. Supports `setUrlParams()`/`addUrlParam()` and badges (`setBadgeText()`, `setBadgeStatus()`). |
| `UrlItem` | `(string $name, string $url, string $target = UrlItem::TARGET_SELF)` | Plain link, always enabled unless `setEnable(false)` is called. |
| `HeaderItem` | `(string $name)` | Section title in the sidebar. |
| `SeparatorItem` | `()` | Divider line. |

**Common setters** (from `Item`), all fluent:
- placement: `setParent()`, `setPosition(MenuBase::MENU_SIDEBAR | MENU_NAV_LEFT | MENU_NAV_RIGHT)`, `setOrder()`;
- state: `setEnable()`, `setActive()`;
- icon: `setIcon($icon, Item::ICON_FA | ICON_IMAGE | ICON_CHARS)`, `hideIcon()`;
- label: `hideName()`, `setRaw()`;
- tooltip: `setTooltip()`, `setTooltipPlacement()`.

The default position comes from the `menu_default_item_position` config key.

Sidebar `ActionItem`s without an icon get a default one:
- `admin` actions get a list icon;
- `add` actions get a plus icon;
- other actions get the item name's initials.

> **Warning:** `setOrder()` and the `$order` argument of `addMenuItem()` are stored on the menu item, but no code in `MenuBase` sorts by them. Items appear in the order they are added. See [Known issues](../internals/known-issues.md#menu-item-order-is-ignored).

### Array API (legacy)

`MenuBase::addMenuItem($name, $url, $parent, $enable, $order, $module, $target, $raw, $position, ...)` is the older signature that the object API calls internally. `Module::addNodeToMenu()` and `Module::addMenuItem()` also use it.
- **Name:** `"-"` makes a separator. An empty `$url` makes a submenu named `$name`.
- **`$enable`** accepts:
  - `1` or `0`;
  - a callable;
  - an array of `node, action, node, action, ...` pairs. The item is enabled if the user holds **any** of those privileges.

### Labels

Menu labels are translated by `MenuBase::getMenuTranslation()`:
- It first looks up `menu_<name>`, then `<name>`, in the item's module.
- Items created with the object API have an empty module unless you call `setModule()`. Their keys are therefore looked up in the application language files (`language_basedir`) and in ATK's own.

See [Internationalisation](i18n.md).

### Other menu options

These configuration keys affect the menu: `menu_show_logout_link`, `menu_show_user`, `menu_enable_user_link` (which needs `auth_usernode`), `menu_hide_sidebar` and the AdminLTE sidebar flags. See the [configuration reference](../reference/configuration.md) and [UI and templates](ui-and-templates.md).

## Related pages

- [Core concepts](concepts.md)
- [Nodes](nodes.md)
- [Security](security.md)
- [Request lifecycle](../internals/request-lifecycle.md)
