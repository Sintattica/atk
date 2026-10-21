# Known issues

[← Documentation index](../README.md)

This page lists defects on `main` that change how documented features behave. Each entry was verified by reading the source. Other pages link to the anchors below, so keep the headings stable. When an issue is fixed, remove its entry and update the pages that link to it.

### Environment variables from `.env` are not visible to `Config::env()`

`Core\Atk::__construct()` loads `.env` with `Dotenv::createImmutable($basedir)`. In phpdotenv 5, `createImmutable` writes only to `$_SERVER` and `$_ENV`. It does not call `putenv()`.

`Core\Config::env()` reads values with `getenv()`. As a result, keys defined only in `.env` fall back to their defaults: `DB_HOST`, `DB_NAME`, `DB_USER`, `DB_PASSWORD`, `DB_CHARSET`, `DB_DRIVER`, `ADMIN_PASSWORD`, `GUEST_PASSWORD`, `DEBUG_LEVEL`, `META_CACHING`, `TPL_COMPILE_CHECK` and `TPL_FORCE_COMPILE`.

Real process environment variables, for example those set by Docker `environment:` or PHP-FPM `env[...]`, are still visible.

`Config::env()` also returns the raw `false` when the variable is missing and the default is falsy. For example, `Config::env('DEBUG_LEVEL', 0)` returns `false`, not `0`.

**Workaround:** in your application `config/atk.php`, read `$_ENV['DB_HOST']` (or `$_SERVER[...]`) and set the config keys explicitly. See [Configuration basics](../getting-started/configuration-basics.md).

### Action and trigger listeners are never registered

`Core\Node::addListener()` checks the listener type with `is_a($listener, 'ActionListener')` and `is_a($listener, 'TriggerListener')`. These are unqualified class names. They never match `Utils\ActionListener` or `Utils\TriggerListener`, so the listener is dropped and only a debug message is logged.

The parameter is also typed `ActionListener`, so passing a `TriggerListener` (such as `Listeners\RecycleBin`) throws a `TypeError`.

**Effect:** `Utils\EventLog`, `Listeners\RecycleBin` and custom listeners do nothing. Use the node trigger methods instead (`postAdd`, `postUpdate`, …), described in [Node hooks](../guide/node-hooks.md).

### Registered action handlers are ignored by internal `getHandler()` lookups

A handler registered with `Atk::atkRegisterNodeHandler()` **is** used for the request's main action: `Core\Node::callHandler()` checks it with `instanceof ActionHandler`.

However, `Core\Node::getHandler()` validates registered handlers with `is_subclass_of($handler, 'ActionHandler')`, an unqualified name that never matches. It therefore always falls back to `Handlers\ActionHandler::getDefaultHandler($action)`. Internal lookups go through `getHandler()`, so they use the default handler instead of yours. Examples:

- the add form embedded in the admin page (`AdminHandler` → `getHandler('add')`);
- `AddHandler` → `getHandler('edit')`;
- action confirmation in `Node`.

Other problems in the same code:
- `getHandler()` re-registers the default handler under `$this->m_type` (the node type without its module), but lookups use the full node URI. The cache therefore never hits.
- `Core\Atk::atkGetNodeHandler()` is typed `?ActionHandler` and instantiates any registered string with `new $handler()`. The "handler function" branch in `callHandler()` can never be reached, and registering a function name fails.

**Workaround:** customise actions by defining methods on the node. `ActionHandler::invoke()` calls `$node-><method>($handler, ...)` before the handler's own method. See [Action handlers](handlers.md).

### `IndexPage` passes `true` instead of the page title, body props and headers

`Ui\IndexPage::generate()` uses `$x = $this->m_title != '' ?: null;`, and the same for `m_extrabodyprops` and `m_extraheaders`. The short ternary returns the *comparison result* (`true`), not the value. Titles, body properties and extra headers set on the index page are therefore lost. The sidebar title has the same pattern.

### `Tools::dispatch_url()` ignores an explicit `$phpfile`

`Core\Tools::dispatch_url($nodeUri, $action, $params, $phpfile)` starts with `$phpfile = ($phpfile != '') ?: Config::getGlobal('dispatcher');`. Passing a non-empty `$phpfile` produces URLs that start with `1?atknodeuri=…`. Leave the argument empty and set the `dispatcher` config key instead.

### `Module::addNodeToMenu()` ignores `$position`

`Core\Module::addNodeToMenu()` forwards its arguments to `MenuBase::addMenuItem()` one slot off: `$position` ends up in the `$raw` parameter. Items added this way are always placed at the default position (`MENU_SIDEBAR`). To place an item in the navbar, call `$this->getMenu()->addMenuItem(...)` with named arguments, or add an `Item` from your menu class.

### Tab privileges are never enforced

`Core\Node::checkTabRights()` looks up `$atk->g_nodes[$module][$type]`. `Core\Atk::registerNode()`, however, stores privileges as `g_nodes[$section][$module][$type]`. The lookup never matches, so `tab_*` privileges registered with `registerNode()` have no effect.

### Returning `false` from `preUpdate()` does not stop an update

`Handlers\UpdateHandler` and the main branch of `Core\Node::updateDb()` call `executeTrigger('preUpdate', $record)` and ignore its result. Only the secondary path in `updateDb()` throws on `false`. To block an update, add a validation error in `validate()` or `<attr>_validate()` instead.

### A list of authentication backends breaks the default authorization

`Security\SecurityManager::getInstance()` defaults `authorization` to the raw `authentication` value. With `'authentication' => 'db,imap'`, the authorization class name becomes `Db,imapAuth`, which does not exist. When you list several backends, set `authorization` explicitly (e.g. `'db'`).

### Menu item order is ignored

`MenuBase::addMenuItem()` and `Item::setOrder()` store an `order` value, but nothing sorts by it. Menu items render in insertion order.

### Password complexity config keys are not read

The `password_min*` keys in the default config are not read anywhere. Pass restrictions to the `PasswordAttribute` constructor (`$restrictions`) instead.

### Legacy `implode()` argument order

Two calls use the pre-PHP-8 argument order `implode($array, $glue)`, which throws a `TypeError` on PHP 8 when reached:

- `Handlers\DeleteHandler::_checkAllowed()`, when `atkselector` is an array (deleting several records at once);
- `DataGrid\DataGridList`, in the script that builds the multi-record-action checkboxes.

> **To verify:** which UI paths reach these lines in practice. The `DataGridList` call sits in a selection-mode branch that may not be the default.

### `DataNode` cannot back an admin page

`Core\DataNode::select()` returns a plain array. `DataGrid`, `ViewHandler`, `DeleteHandler` and `ExportHandler` call `Selector` methods on the result (e.g. `select()->excludes(...)`), so those calls fail. It also reads an undefined `$limit`, so offset and limit are never applied. See [Data access](../guide/data-access.md).

### `IndexPage` requires `APP_ENV`

`Ui\IndexPage::generate()` reads `$_SERVER['APP_ENV']` without checking that it is set, and enables the development navbar unless it equals `'prod'`. Always define `APP_ENV` in `.env` or in the server environment.

### `MySqliDdl` maps `boolean` to an Oracle type

`Db\MySqliDdl::getType('boolean')` returns `NUMBER(1,0)`, which MySQL does not accept. Tables generated with `Ddl` and a boolean field fail to create.

### `Encryption::getEncryption()` always returns the base class

`Security\Encryption\Encryption::getEncryption()` builds the class name as `$type.'Encryption'`, without a namespace. The concrete classes (`Base64Encryption`, `BajusEncryption`, `OpenSSLEncryption`) are never found. Nothing in `src/` calls this factory.

### The `cache_method` default breaks `Cache::getInstance()`

`Cache\Cache::getInstance()` runs `new $type()` with the configured `cache_method`, whose default is `'var'`. Class `var` does not exist. The resulting `Error` is not caught, because the code catches only `Exception`. No ATK code calls the cache, so this only affects applications that use it directly. To use it, pass the fully qualified class, e.g. `Sintattica\Atk\Cache\VarCache`.

### Export delimiter config key is misspelled

The default config defines `export_delimiterexport_delimiter` instead of `export_delimiter`. `Handlers\ExportHandler` reads `export_delimiter`, so its own fallback `;` is used unless your application sets `export_delimiter` explicitly.

### LDAP authentication has no backend

The default config documents `"ldap"` as an `authentication` value and defines the `authentication_ldap_*` keys. There is no `Security\Auth\LdapAuth` class, so configuring `ldap` fails.

### `TreeNode` is not fully ported

The class docblock of `Core\TreeNode` states it "has not been ported" and "may not work". Treat it as experimental.
