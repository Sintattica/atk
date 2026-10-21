# Security

[← Documentation index](../README.md)

ATK separates three concerns:

- **Authentication** checks who the user is (username and password). Backends live in `Security\Auth\*Auth`.
- **Authorization** decides what the user may do. The same backend classes implement it, through `AuthInterface::allowed()`.
- **The security scheme** sets how privileges are compared: `none`, `level` or `group`.

Everything is coordinated by the `Security\SecurityManager` singleton ([source](../../src/Security/SecurityManager.php)).

## Configuration at a glance

```php
// config/atk.php (excerpt, modelled on atk-skeleton)
'authentication'          => 'db',
'securityscheme'          => 'group',
'restrictive'             => true,
'administratorpassword'   => $_ENV['ADMIN_PASSWORD'] ?? '',   // bcrypt hash, see below

'auth_usernode'           => 'auth.users',
'auth_usertable'          => 'auth_user',
'auth_userpk'             => 'id',
'auth_userfield'          => 'username',
'auth_passwordfield'      => 'passwd',
'auth_accountdisablefield'=> 'disabled',
'auth_leveltable'         => 'auth_user_group',   // user ↔ group link table
'auth_userfk'             => 'user_id',
'auth_levelfield'         => 'group_id',
'auth_accesstable'        => 'auth_access_rights', // node, action, group_id
'auth_languagefield'      => 'language',
'auth_administratorfield' => 'is_admin',
```

All keys and their defaults are listed in the [configuration reference](../reference/configuration.md). The defaults are open: `authentication` is `none` and `securityscheme` is `none`.

## How a request is authenticated

`Atk::run()` calls `SecurityManager::run()` before any module is booted or any page is built. It tries the following in order:

1. **Logout.** If `atklogout` is in the request, it logs out and redirects to `dispatcher`.
2. **Session login.** If the session is already marked as logged in, it uses that session.
3. **Remember me.** This step runs only when `auth_enable_rememberme` is true. It reads a cookie named `<identifier>-<auth_rememberme_cookiename>` and checks it against `auth_rememberme_dbtable`.
4. **U2F.** This step runs only when `auth_enable_u2f` is true and a U2F response was posted.
5. **Username and password.**
   - Credentials come from the posted login form (`auth_user` / `auth_pw`) when `auth_loginform` is true, and from HTTP basic auth (`PHP_AUTH_USER` / `PHP_AUTH_PW`) otherwise.
   - System users (see below) are checked first. After that, each configured authentication backend is tried in turn.

If nobody is logged in after these steps, `run()` shows one of the following and stops the request:
- the redirect target in `auth_loginpage`, if set;
- otherwise the login template, if `auth_loginform` is true;
- otherwise a `401` HTTP basic-auth challenge.

On success, the user array is stored in the session (`authentication` global), and `postLogin` listeners are notified.

### Backends

The `authentication` key takes a backend name, or a comma-separated list tried in order (e.g. `'db,imap'`). Names are resolved to `Security\Auth\<Name>Auth`, or you can give a fully-qualified class name.

| Name | Class | Credentials checked against |
|---|---|---|
| `none` | `NoneAuth` | Nothing. Any username is accepted, and the user becomes the `administrator` system user. |
| `db` | `DbAuth` | A user table, configured through the `auth_*` keys. Group membership comes from `auth_leveltable`, and privileges from `auth_accesstable`. |
| `config` | `ConfigAuth` | The `user` config array: `['name' => ['password' => ..., 'level' => ...]]`. Privileges come from the `access` array. |
| `imap` | `ImapAuth` | An IMAP server (`auth_mail_server`, `auth_mail_port`, …). Needs the `imap` extension. |
| `server` | `ServerAuth` | The web server's own authentication (`PHP_AUTH_USER`) |
| `sspi` | `SspiAuth` | Windows SSPI (`auth_sspi_*`) |

> **Note:** with `none`, `SecurityManager::run()` still requires a username. Login is attempted only when `auth_user` (or `PHP_AUTH_USER`) is non-empty, so the login form is shown and any username is accepted. On the command line (`Atk::runCli()`), authentication and authorization are forced to `none` and no username is needed.

The default config also documents `ldap`, but no LDAP backend exists; see [Known issues](../internals/known-issues.md#ldap-authentication-has-no-backend).

The **authorization** backend is set by the `authorization` key. It is not in the default config; when absent, it takes the value of `authentication`.

> **Warning:** when `authentication` is a comma-separated list and `authorization` is not set, `SecurityManager` resolves the whole list (e.g. `'db,imap'`) as a single class name, which does not exist. Always set `authorization` explicitly, e.g. `'authorization' => 'db'`, when you use more than one authentication backend. See [Known issues](../internals/known-issues.md#a-list-of-authentication-backends-breaks-the-default-authorization).

### Passwords

- With `auth_usecryptedpassword` set to true (the default), passwords are verified with `password_verify()`. Store them as `password_hash()` / bcrypt hashes. ATK ships a small helper, [`src/Utils/generatehash.php`](../../src/Utils/generatehash.php), to generate one from the command line.
- `auth_ignorepasswordmatch` makes **every password valid**. The skeleton enables it in development (`APP_ENV=dev`). Never enable it in production.
- Password complexity rules are passed to `Attributes\PasswordAttribute` as its fourth constructor argument, `$restrictions`. It is an array with the keys `minsize`, `minupperchars`, `minlowerchars`, `minalphabeticchars`, `minnumbers` and `minspecialchars`. The `password_min*` keys of the default config are not read by any code in `src/`.

### System users

`SecurityManager` knows two built-in users that need no database row:

| User | Enabled when | Level |
|---|---|---|
| `administrator` | `administratorpassword` is non-empty. Its value is the password hash. | `-1`: superuser, every check passes |
| `guest` | `guestpassword` is non-empty | `-2`: denied everything |

The default config reads these from the environment variables `ADMIN_PASSWORD` and `GUEST_PASSWORD` through `Config::env()`. Values written only in `.env` are currently not visible there; see [Known issues](../internals/known-issues.md#environment-variables-from-env-are-not-visible-to-configenv). Set them explicitly in `config/atk.php`.

## Authorization

### Privileges

A privilege is the pair (node URI, action), e.g. `crm.customers` / `edit`. `Node::allowed(string $action, array $record = []): bool` is checked before every action. It returns true when any of the following holds:

1. The current user is an administrator (`SecurityManager::isUserAdmin()`, see below).
2. The node has `NF_NO_SECURITY`.
3. The action was whitelisted for this request with `addAllowedAction($action)`.
4. The authorization backend grants the privilege (`SecurityManager::allowed($nodeUri, $privilege)`, cached per request).
5. The privilege is *implied* by another one the user has. By default `view` is implied by `admin` (`Node::$m_securityImplied`).

Before checking, the action goes through the node's **security map** (`Node::$m_securityMap`). It maps actions onto the privilege that actually protects them:

| Action | Privilege checked |
|---|---|
| `save`, `copy`, `import`, `editcopy` | `add` |
| `update`, `multiupdate` | `edit` |
| `search`, `smartsearch` | `admin` |

Use these methods to adapt the checks:
- `addSecurityMap('archive', 'edit')` maps your own actions.
- `setSecurityAlias('crm.customers')` makes a node use another node's privileges. This is typical for detail nodes edited only from their master.

You can also override `allowed()` for record-level rules. The record is passed only in some calls, so always handle an empty `$record`:

```php
public function allowed(string $action, array $record = []): bool
{
    if ($action === 'delete' && ($record['locked'] ?? false)) {
        return false;
    }
    return parent::allowed($action, $record);
}
```

### Security schemes

`AuthInterface::allowed()` implements the scheme:

- **`none`**: everything is allowed.
- **Superuser** (level `-1`) or administrator: allowed. **Guest** (level `-2`): denied.
- **No rule for the privilege** in the access table: allowed only if `restrictive` is `false`. The default is `true`, so unknown privileges are denied.
- **`level`**: the user has one numeric level. The privilege is granted if the user's level is greater than or equal to the required level.
- **`group`**: the user's level is a list of group ids (from `auth_leveltable`). The privilege is granted if any of the user's groups appears among the groups allowed for the privilege.

With the `db` backend, the rules are rows of `auth_accesstable` with the columns `node`, `action` and the group or level column. The group or level column is `auth_accessfield`, falling back to `auth_levelfield`. The skeleton creates it as:

```sql
CREATE TABLE auth_access_rights (
  node     varchar(255) NOT NULL,
  action   varchar(255) NOT NULL,
  group_id char(5)      NOT NULL,
  PRIMARY KEY (node, action, group_id)
);
```

These rows are usually maintained through `Attributes\ProfileAttribute`. Add it to your groups node and it renders a checkbox matrix of every node and action registered by the modules. See [Modules and menu](modules-and-menu.md#the-actions-list-matters).

```php
$this->add(new ProfileAttribute('accessrights', A::AF_BLANK_LABEL | A::AF_HIDE_ADD));
```

With the `config` backend, privileges come from the `access` config array (`[$nodeUri => [[$action => $levelOrGroup], ...]]`). `Config::grant($node, $action, $entity)` appends to that array at runtime.

### Administrators

`SecurityManager::isUserAdmin()` returns true for:
- the `administrator` system user;
- a user whose `auth_administratorfield` column is truthy (`1`, `y`, `yes`, `true`, `t`, `on`, `j`);
- a user listed in the `auth_administratorusers` config array.

For administrators, `Node::allowed()` skips all checks.

### Attribute-level security

When `security_attributes` is true, attributes can be hidden or made read-only per group or level:
- the `config` backend reads the `attribrestrict` config array (`Config::attribRestrict()`);
- the `db` backend reads a table named `attribaccess` (columns `node`, `attribute`, `mode`, plus the level field).

`reverse_attributeaccess_logic` inverts the result for the `group` scheme.

> **To verify:** the table name `attribaccess` is hard-coded in `DbAuth::getAttribEntity()`, and there is no configuration key for it. This part of ATK is rarely used, so test it before relying on it.

## Working with the current user

```php
use Sintattica\Atk\Security\SecurityManager;

$user = SecurityManager::atkGetUser();          // array|null: the user record plus 'name', 'level', 'access_level', 'AUTH'
$id   = SecurityManager::atkGetUserId();        // value of auth_userpk, or 0 for system users
$lang = SecurityManager::atkGetUserKey('language');
$isAdmin = SecurityManager::isUserAdmin();

$sm = SecurityManager::getInstance();
if ($sm->allowed('crm.customers', 'export')) { /* ... */ }
if ($sm->hasLevel(3)) { /* ... */ }
```

`Attributes\CreatedByAttribute` and `Attributes\UpdatedByAttribute` store the current user automatically. They are many-to-one relations to the node in `auth_usernode`, which you must set.

## Auditing

- **Access logger.** Set `auth_accesslog_enabled` to log login, failed-login and logout events to `auth_accesslog_table`. `Security\AccessLogger` is registered as a security listener by `AccessLoggerServiceProvider::register()`, which the `Atk` constructor calls. `SecurityManager::addListener()` accepts your own `SecurityListener` subclasses too.
- **Log file.** `logging` (a level greater than 0) and `logfile` enable a plain-text log written by `SecurityManager::log()`.

## Related pages

- [Nodes](nodes.md) and [Node hooks](node-hooks.md)
- [Modules and menu](modules-and-menu.md)
- [Configuration reference](../reference/configuration.md)
- [Known issues](../internals/known-issues.md)
