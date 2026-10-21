# Session and state

[← Documentation index](../README.md)

ATK keeps its navigation state on the server. Opening an edit form from a list, a select popup from an edit form, and so on, each push a level on a **session stack**. Going back pops a level and restores the previous page with its filters, sorting, pagination and half-filled forms. This page describes that mechanism and the smaller state helpers.

Source: [`src/Session/SessionManager.php`](../../src/Session/SessionManager.php), [`State.php`](../../src/Session/State.php), [`SessionStore.php`](../../src/Session/SessionStore.php).

## Session layout

All ATK data lives under `$_SESSION[<identifier>]`. Change the `identifier` config key (default `'default'`) when several ATK apps share a domain. `SessionManager::getSession()` returns a reference to that array.

```
$_SESSION[identifier]
├── login                          # 1 once SecurityManager::run() has authenticated the user
└── <namespace>                    # SessionManager namespace, "default"
    ├── globals                    # globalVar() values; globals['#STACK#'][stackId] = globalStackVar() values
    ├── stack[stackId][level]      # one array per stack level (session vars, page vars, defined_stackvars, atkformdata, atkreject…)
    └── stack_stamp[stackId]       # last-access time, used for garbage collection
```

The PHP session name is `session_name`, or `identifier` if that is empty. The cookie path comes from `cookie_path`, and the cookie domain from `cookiedomain` if set. `session_cache_expire` and `session_cache_limiter` are applied before `session_start()`. A malformed session id (`SessionManager::isValidSessionId()`) aborts the session start with a warning.

## Stacks and levels

Each browser tab or flow has a **stack id** (`atkstackid`, generated with `uniqid()`). Within it, `atklevel` selects the current level. Three request variables drive navigation:
- `atklevel`: target level;
- `atkprevlevel`: level the request came from;
- `atkstackid`: which stack.

Code does not set them by hand. It uses the session status constants:

| Constant | `atklevel` sent | Effect |
|---|---|---|
| `SESSION_DEFAULT` (0) | current level | Stay on the current level. |
| `SESSION_NEW` (1) | `-1` | Start a new stack (new stack id) at level 0. Used by menu links. |
| `SESSION_NESTED` (2) | current + 1 | Push a level, e.g. list → edit. |
| `SESSION_BACK` (3) | current − `levelskip` | Pop back. |
| `SESSION_REPLACE` (4) | `-2` | Replace the top level, e.g. save → feedback. |
| `SESSION_PARTIAL` (5) | `-3` | Reuse the top level for an AJAX partial. Locked vars (`atknodeuri`, `atkaction`, `atkselector`, `atkmenu`) are not overwritten. |

The URL and form helpers on `SessionManager` add these variables:
- `sessionUrl($url, $status, $levelskip)` returns the URL with the session variables appended;
- `sessionVars($status, $levelskip)` returns just the query-string part;
- `formState($status, $returnbehaviour, $fieldprefix)` returns the hidden inputs for forms, including `atkescape` and the session id.

`Tools::dispatch_url()` and `Tools::partial_url()` build ATK URLs on top of these.

### What `start()` does to the request

`SessionManager::start()` → `session_read($ATK_VARS)` applies two scopes to the request variables.

1. **Global scope** (`_globalscope`). Values stored with `globalVar()` are posted back into `$ATK_VARS`. A non-empty posted value replaces the stored one.
2. **Stack scope** (`_stackscope`).
   - Resolves the stack id and level. `-1`, `-2` and `-3` are translated as shown in the table.
   - Touches the stack timestamp, and removes stacks that have been idle longer than `session_max_stack_inactivity_period` seconds (default 3600; `0` disables this).
   - Caps the level at the top of the stack + 1.
   - **New level:**
     - copies the *session vars* and any `defined_stackvars` from the level below;
     - overlays the posted *page vars* and *session vars*. The value `clear` empties a var. Names ending in `*` are merged recursively.
   - **Existing level:**
     - pops every level above it;
     - merges the posted page vars and session vars. In partial mode, locked vars are skipped.
   - Restores `atkformdata`, the form values saved when the user "escaped" to another page, into `$ATK_VARS` without overriding URL values.
   - Copies every remaining stored value into `$ATK_VARS` that the request did not pass explicitly.

The variable lists are hard-coded in `_stackscope()`:
- **Session vars** live on every level and are inherited upward: `atknodeuri`, `atkfilter`, `atkaction`, `atkpkret`, `atkstore`, `atkstore_key` and `atkmenu`.
- **Page vars** live only on their own level: `atkdg*`, `atkdgsession`, `atksearch`, `atkselector`, `atksearchmode`, `atkorderby`, `atkstartat`, `atklimit`, `atktarget`, `atkformdata`, `atktree`, `atksuppress`, `atktab`, `atksmartsearch` and `atkindex`.

### Escape, nested and back

After the scopes are applied, `start()` handles three redirect requests:
- **`atkescape=<url>`:** the current form data is saved into `atkformdata` on the top level, then the browser is redirected to the URL. Escape mode is used for "add new related record" style buttons that leave a half-filled form. In this mode `stackVar()`, `globalStackVar()` and `pageVar()` are no-ops.
- **`atknested=<url>`:** redirects to `sessionUrl($url, SESSION_NESTED)`.
- **`atkback`:** redirects to the dispatcher at `atkprevlevel - 1`.

## Variable scopes API

All of these read the request first. Called without a value, they pick up `$_REQUEST[$var]` if it is present, store it, and return the current value.

| Method | Lifetime | Typical use |
|---|---|---|
| `globalVar($var, $value = '', $noNamespace = false)` | Whole session. Read back with `getValue($var)`. | User preferences. |
| `stackVar($var, $value = '', $level = null)` | The current stack level and every level pushed above it. The name is recorded in `defined_stackvars`. | Context that nested pages need, e.g. `atkstore`. |
| `globalStackVar($var, $value = '')` | The whole stack, all levels. | The CSRF token (`ATK_CSRF_TOKEN`), `atkstore_key`. |
| `pageVar($var, $value = '')` | The current level only. The name is recorded in `defined_pagevars`. | Per-page UI state. |

Reject info (`atkreject`) is a stack var. `ActionHandler::setRejectInfo()` writes it when validation fails, and `getRejectInfo()` reads it. It lets a redirected form show the posted values and errors again. `handle()` clears it at the end of a request it did not set it in.

`stackTrace()` and `descriptorTrace()` build the breadcrumb data from the stack's node, action and descriptor entries.

## Session auto-refresh

With `session_autorefresh` set to `true`, `IndexPage` injects `SessionManager::getSessionAutoRefreshJs()`. This is a jQuery interval that calls `<dispatcher>?<session_autorefresh_key>` every `session_autorefresh_time` milliseconds (default 300000). `Atk::run()` answers such a request with `die(session_id())` straight after `SessionManager::start()`. This happens before authentication, so the request only keeps the PHP session alive.

## `State`: small persistent UI state

`Session\State` is a static key/value helper used for things such as collapsed sections and tab state.
- `State::get($key, $default)` reads the cookie, then `$_SESSION`, then falls back to the default.
- `State::set($key, $value, $type = 'cookie')` writes either a cookie with a lifetime of `state_cookie_expire` minutes (default 10080, one week) or a `$_SESSION` entry.

Keys can be any value. They are hashed with `md5(print_r($key, true))`, so cookie names are opaque.

## `SessionStore`: records that are not in the database yet

When a one-to-many relation is edited inside a record that is still being **added**, the child rows cannot be saved yet. `Relations\OneToManyRelation` and `OneToManyRelationSessionGridHandler` therefore pass `atkstore=session` and an `atkstore_key` as URL parameters. Both are session vars, so they stay on the stack for the nested pages. The add, update and delete handlers check `stackVar('atkstore')` and store the rows with `SessionStore` instead of `addDb()`.

- `SessionStore::getInstance($key = false, $reset = false)` returns one instance per key. The default key is `globalStackVar('atkstore_key')`.
- The data methods are `addDataRow($row, $pkField)`, `getDataRowForSelector($selector)`, `updateDataRowForSelector()`, `deleteDataRowForSelector()`, `getData()` and `setData()`.
- When the parent record is finally saved, the relation reads the stored rows and writes them to the database.

## Related pages

- [Request lifecycle](request-lifecycle.md)
- [Action handlers](handlers.md)
- [Relations](../guide/relations.md)
