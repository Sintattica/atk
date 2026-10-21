# Configuration reference

[← Documentation index](../README.md)

ATK's default configuration lives in [`src/Resources/config/atk.php`](../../src/Resources/config/atk.php). For how to set up a project, see [Configuration basics](../getting-started/configuration-basics.md).

## How configuration is loaded

`Core\Config::init()` runs inside the `Core\Atk` constructor and builds the configuration in three steps:

1. It loads the defaults from `src/Resources/config/atk.php`. A key that was already set with `Config::setGlobal()` before `new Atk(...)` is not overwritten.
2. It loads the application file named by `application_config`. By default this is `<app root>/config/atk.php`.
3. It merges the application values over the defaults with `Tools::atk_array_merge_recursive()`.

Read values with these methods:

| Call | Returns |
|---|---|
| `Config::getGlobal($key, $default = null)` | A key of the merged global configuration. |
| `Config::setGlobal($key, $value)` | Changes a key at runtime. |
| `Config::get($section, $tag, $default = '')` | A key from a separate file `<application_config_dir>/<section>.php`. If `<section>` is a module, it falls back to `<module dir>/config/<section>.php`. |
| `Config::env($key, $default = false)` | An environment variable. See [Known issues](../internals/known-issues.md#environment-variables-from-env-are-not-visible-to-configenv). |

> **Merge gotcha:** the merge is recursive and works key by key, numeric keys included. List-like arrays are therefore merged by position, not replaced. For example, setting `'recordsperpage_options' => [10, 20]` produces `[10, 20, 50, 100, 200]`. To replace a list completely, give at least as many elements as the default, or call `Config::setGlobal()` after construction.

> **App root:** the default paths assume ATK is installed as a Composer dependency in `vendor/sintattica/atk/`. The app root is computed as six directories above `src/Resources/config/`. If ATK lives elsewhere, call `Config::setGlobal('application_config', ...)` before creating `Atk`.

In the tables below:

- *Env* names the environment variable used for the default, read through `Config::env()`.
- *Not read* marks keys that are defined in the defaults but are not read anywhere in `src/` on `main`.

## Paths and files

| Key | Default | Meaning |
|---|---|---|
| `configdirname` | `'config'` | Name of the config directory, in the app and in modules. |
| `application_dir` | app root | Application root directory. |
| `application_config_dir` | `<root>/config/` | Directory searched by `Config::get()`. |
| `application_config` | `<root>/config/atk.php` | Application config file merged over the defaults. |
| `assets_url` | `'/bundles/atk/'` | Public URL of ATK's `src/Resources/public` assets. |
| `template_dir` | ATK `src/Resources/templates/` | Smarty template directory. |
| `public_dir` | `<root>/web/` | Public web directory. *Not read.* |
| `version` | `''` | Application version string. |
| `default_upload_dir` / `default_upload_url` | `'uploads'` | Default directory and URL for `FileAttribute`. |
| `favicon_base_url` | `'/'` | Base URL for favicons. |
| `manifest_url` | `''` | Web app manifest URL. |
| `atktempdir` | `<root>/var/atktmp/` | Writable temp directory (compiled templates, caches). |
| `use_atkerrorhandler` | `true` | Install ATK's error handler. |
| `meta_caching` | env `META_CACHING`, `1` | Cache table metadata. Disable in development. |
| `datagrid_class` | `DataGrid\DataGrid` | Class used to build data grids. |
| `datagrid_display_top_info` / `datagrid_display_bottom_info` | `false` / `true` | Show the summary above or below the grid. |
| `datagrid_total_paginate` | `false` | Paginate the totals row. |
| `dispatcher` | `'index.php'` | Front controller that every request goes through. |

## Database

| Key | Default | Meaning |
|---|---|---|
| `db` | `['default' => [...]]` | Connections by name. Each connection has `host` (env `DB_HOST`, `localhost`), `db` (env `DB_NAME`, `atk`), `user` (env `DB_USER`, `root`), `password` (env `DB_PASSWORD`), `charset` (env `DB_CHARSET`, `utf8`) and `driver` (env `DB_DRIVER`, `MySqli`). It also accepts `port`, `collate`, `force_ci` and `mode`. See [Database layer](../internals/database-layer.md). |
| `databasepersistent` | `true` | Use persistent connections. *Not read.* |
| `database_sequenceprefix` / `database_sequencesuffix` | `''` / `'_seq'` | Naming of sequences (PostgreSQL). |

## Security

| Key | Default | Meaning |
|---|---|---|
| `administratorpassword` | env `ADMIN_PASSWORD`, `''` | Bcrypt hash for the built-in `administrator` user. Empty disables it. |
| `guestpassword` | env `GUEST_PASSWORD`, `''` | Hash for the built-in `guest` user. Empty disables it. |
| `authentication` | `'none'` | Authentication backend(s): `none`, `db`, `config`, `imap`, `server`, `sspi`, or a class name. A comma-separated list is allowed. `ldap` is documented but has [no backend](../internals/known-issues.md#ldap-authentication-has-no-backend). |
| `authentication_session` | `true` | Keep authentication in the session. |
| `state_cookie_expire` | `10080` | Expiry of `State` cookies, in minutes (7 days). |
| `securityscheme` | `'none'` | Authorization scheme: `none`, `level` or `group`. |
| `restrictive` | `true` | Deny access when no rule exists for a privilege. |
| `security_attributes` | `false` | Enable attribute-level access rights. |
| `reverse_attributeaccess_logic` | `false` | Attribute rights: everything is allowed by default, and the access table lists what is denied. |
| `auth_grantall_privilege` | `''` | A `module.node.privilege` that grants every privilege. |
| `logging` / `logfile` | `0` / `<root>/atk-security.log` | Security log level (0 = off) and file. |
| `password_minsize`, `password_minupperchars`, `password_minlowerchars`, `password_minalphabeticchars`, `password_minnumbers`, `password_minspecialchars` | `0` | Password complexity rules. *Not read.* |
| `encryption_defaultmethod` | `'base64'` | Default `Security\Encryption` method. See [known issue](../internals/known-issues.md#encryptiongetencryption-always-returns-the-base-class). |
| `enable_ssl_encryption` | `false` | Keep the user password in the session for encryption. *Not read.* |

## Authentication: database backend

These keys configure `authentication = 'db'`. See [Security](../guide/security.md).

| Key | Default | Meaning |
|---|---|---|
| `auth_database` | `'default'` | Connection name. |
| `auth_usertable` | `'user'` | Users table. |
| `auth_userpk` | `'userid'` | Primary key of the users table. |
| `auth_userfield` | `'userid'` | Login name column. |
| `auth_passwordfield` | `'password'` | Password column. |
| `auth_languagefield` | `'lng'` | User language column. |
| `auth_accountdisablefield` | `''` | Column that disables an account. |
| `auth_accountenableexpression` | `''` | Extra SQL condition a user must satisfy to log in. |
| `auth_administratorfield` | `''` | Column that marks a user as administrator. |
| `auth_administratorusers` | `[]` | Usernames that are administrators. |
| `auth_leveltable` | `''` | Table that holds levels or group memberships (defaults to the user table). |
| `auth_levelfield` | `'entity'` | Level or group column. |
| `auth_accesstable` | `'access'` | Table of granted privileges. |
| `auth_accessfield` | `''` | Column of the access table that holds the level or group (defaults to `auth_levelfield`). |
| `auth_grouptable`, `auth_groupfield`, `auth_groupparentfield` | `''` | Group hierarchy table, its key and its parent column. |
| `auth_usecryptedpassword` | `true` | Verify passwords with `password_verify()`. |
| `auth_ignorepasswordmatch` | `false` | Accept any password. **Development only.** |
| `auth_userdescriptor` | `'[userid]'` | Template used to display the logged-in user. |
| `auth_loginform` | `true` | Use the HTML login form instead of HTTP basic auth. |
| `auth_changerealm` | `false` | HTTP auth: change the realm on each login so logout works. |
| `auth_mail_virtual` | `false` | IMAP: virtual mail domains. *Not read.* |

## Remember me, U2F and access log

| Key | Default | Meaning |
|---|---|---|
| `auth_enable_rememberme` | `false` | Enable "remember me". This is never available to the administrator and guest users. |
| `auth_rememberme_expireinterval` | `'+14 days'` | Token lifetime, as a `DateTime` modifier. |
| `auth_rememberme_cookiename` | `'rememberme'` | Cookie name. |
| `auth_rememberme_dbtable` | `'auth_rememberMe'` | Token table. |
| `auth_enable_u2f` | `false` | Enable U2F second factor. |
| `auth_u2f_dbtable` / `auth_u2f_enabledfield` | `'auth_u2f'` / `''` | U2F key table and the user column that enables it. |
| `auth_accesslog_enabled` | `false` | Log logins to `auth_accesslog_table`. |
| `auth_accesslog_table` | `'auth_accesslog'` | Access log table. Its required structure is described in the default config file. |
| `app_name` | `'atk'` | Application name written to the access log. |

## LDAP

`authentication_ldap_host`, `authentication_ldap_context` and `authentication_ldap_field` are defined but *not read*. There is [no LDAP backend](../internals/known-issues.md#ldap-authentication-has-no-backend).

## Debugging and errors

| Key | Default | Meaning |
|---|---|---|
| `debug` | env `DEBUG_LEVEL`, `0` | Debug level. `> 0` enables the debug console and debug messages; `>= 2` adds more detail; `> 2` also logs every query. |
| `debuglog` | `''` | File that receives debug output when `debug > 0`. |
| `display_errors` | `true` | Show errors to the user. |
| `halt_on_error` | `'critical'` | Error severity that stops execution. |
| `mailreport` | `''` | Address that receives error reports. |
| `throw_exception_on_error` | `false` | Make `atkerror` throw an exception instead of only logging. |
| `debug_translations` | `true` | Report missing translations. |

## Index page and menu

| Key | Default | Meaning |
|---|---|---|
| `indexPage` | `Ui\IndexPage` | Class that renders the main page. |
| `default_destination` | `null` | `['module.node', 'action']` to open after login. |
| `menu` | `Core\Menu\Menu` | Menu class. Applications normally set their own `MenuBase` subclass. See [Modules and menu](../guide/modules-and-menu.md). |
| `menu_show_logout_link` | `true` | Show the logout link. |
| `menu_show_user` | `true` | Show the logged-in user in the navbar. |
| `menu_enable_user_link` | `true` | Link the user name to its record. *Not read in PHP.* |
| `menu_default_item_position` | `MenuBase::MENU_SIDEBAR` | Default position of menu items. |
| `menu_hide_sidebar` | `false` | Hide the sidebar. |

## Layout and record lists

| Key | Default | Meaning |
|---|---|---|
| `tabs` | `true` | Enable tabs. |
| `dhtml_tabs_stateful` | `true` | Remember the active tab. |
| `recordsperpage` | `25` | Records per page. |
| `recordsperpage_options` | `[10, 25, 50, 100, 200]` | Options of the records-per-page selector. See the merge gotcha above. |
| `enable_showall` | `true` | Add a "show all" option. |
| `datagrid_iconize_links` | `true` | Use icons for record actions. |
| `pagelinks` | `3` | Number of page links. |
| `pagelinks_previous_next` / `pagelinks_first_last` | `true` | Show previous/next and first/last links. |
| `stacktrace` | `true` | Show the breadcrumb of the session stack. |
| `max_input_size` / `max_searchinput_size` | `70` / `20` | Maximum size of generated input and search fields. |
| `recordlist_onclick` | `false` | Clicking a row opens its view or edit page. |
| `recordlist_orientation` | `'left'` | Side of the action column (`left` or `right`). |
| `recordlist_top_scroller` | `false` | Extra horizontal scrollbar above the list. |
| `recordlist_icons` | `true` | Use icons in record lists. |
| `recordlist_javascript_delete` | `false` | Confirm deletes with JavaScript instead of a confirmation page. |
| `recordList_show_dropdown_labels` | `true` | Show action names in the actions dropdown. |
| `recordList_dropdown_start_index` | `3` | Actions shown before the rest collapse into a dropdown (`< 1` disables the dropdown). |
| `mra_position` | `'bottom'` | Position of multi-record actions (`top` or `bottom`). |
| `action_form_buttons_position` | `Ui::ACTION_FORM_BUTTON_POSITION_RIGHT` | Alignment of form buttons (left, center, right). |
| `style_url` | `''` | Extra stylesheet loaded on every page. |
| `login_logo` / `brand_logo` | `<assets_url>images/login_logo.jpg` / `''` | Logos on the login page and in the sidebar. |
| `custom_font` | `'sans'` | Font family class. |
| `icon_*` | Font Awesome classes | Icons for actions (`icon_add`, `icon_edit`, `icon_delete`, `icon_view`, `icon_copy`, `icon_export`, …). `DataGridList` also looks up `icon_<nodetype>_<action>` and `icon_<action>`, so you can add keys for custom actions. |

## Output, language and templates

| Key | Default | Meaning |
|---|---|---|
| `output_gzip` | `false` | Gzip pages, if the web server does not already do it. |
| `language` | `'en'` | Default language. |
| `supported_languages` | `['en']` | Languages users can choose. |
| `language_basedir` | `'languages/'` | Directory of application language overrides. See [Internationalisation](../guide/i18n.md). |
| `use_browser_language` | `false` | Detect the language from the browser. |
| `tplcompiledir` | `<atktempdir>tpl/` | Smarty compile directory. Created if missing. |
| `tplcompilecheck` | env `TPL_COMPILE_CHECK`, `1` | *Not read.* |
| `tplforcecompile` | env `TPL_FORCE_COMPILE`, `0` | Force template recompilation. |
| `smarty_file_perms` | `0775` | Permissions of compiled templates. |

## Session

| Key | Default | Meaning |
|---|---|---|
| `identifier` | `'default'` | Application identifier, used to namespace session data. |
| `session_name` | `''` | PHP session name (defaults to the identifier). |
| `cookie_path` | `'/'` | Session cookie path. |
| `session_max_stack_inactivity_period` | `3600` | Seconds before an inactive session stack is removed (`<= 0` disables this). |
| `session_autorefresh` | `false` | Keep the session alive with AJAX calls. |
| `session_autorefresh_time` | `300000` | Refresh interval in milliseconds. |
| `session_autorefresh_key` | `'_sessionautorefresh'` | Request key that identifies refresh calls. |
| `session_cache_expire` | `180` | `session_cache_expire()` in minutes. |
| `session_cache_limiter` | `'nocache'` | `session_cache_limiter()` value. |

See [Session and state](../internals/session-and-state.md).

## Search, relations and lists

| Key | Default | Meaning |
|---|---|---|
| `search_defaultmode` | `'substring'` | Default search mode. |
| `extended_search_action` | `'search'` | Action used by the extended search link. |
| `list_obligatory_null_item` | `false` | Add a "select none" item to obligatory lists. |
| `manytoone_autocomplete_default` | `false` | Use autocomplete on every many-to-one relation. |
| `manytoone_autocomplete_large` | `true` | Use autocomplete on `AF_LARGE` many-to-one relations. |
| `manytoone_search_autocomplete` | `true` | Use autocomplete in search mode too. |
| `manytoone_autocomplete_minchars` | `2` | Characters typed before autocomplete starts. |
| `manytoone_autocomplete_size` | `50` | Width of the autocomplete input. |
| `manytoone_autocomplete_searchmode` | `'contains'` | `startswith`, `exact` or `contains`. |
| `manytoone_autocomplete_search_case_sensitive` | `false` | Case-sensitive autocomplete. |
| `manytoone_autocomplete_minrecords` | `-1` | Minimum record count before autocomplete replaces the dropdown. |
| `manytoone_autocomplete_pagination_limit` | `50` | Results per autocomplete page. |
| `onetomany_addlink_position` | `'top'` | Position of the add link in one-to-many grids. |
| `onetomany_label_position_top` | `false` | Put the one-to-many label at the top instead of vertically centred. |
| `numberattribute_use_thousands_separator` | `false` | Thousands separator in `NumberAttribute`. |
| `ck_options` | `[]` | Options merged into the CKEditor configuration of `CkAttribute`. |

## Export

| Key | Default | Meaning |
|---|---|---|
| `export_delimiterexport_delimiter` | `','` | Misspelled, so it has no effect. Set `export_delimiter` instead. See the [known issue](../internals/known-issues.md#export-delimiter-config-key-is-misspelled). |
| `export_enclosure` | `'&quot;'` | Field enclosure. |
| `export_titlerow_checked` | `true` | Include the title row by default. |

## Cache

| Key | Default | Meaning |
|---|---|---|
| `cache_method` | `'var'` | Cache class or classes for `Cache\Cache::getInstance()`. See the [known issue](../internals/known-issues.md#the-cache_method-default-breaks-cachegetinstance). |
| `cache_namespace` | `'default'` | Cache namespace. Change it on shared hosting. |

## AdminLTE layout

These booleans are translated into AdminLTE CSS classes by `Core\AdminLTE`. See [UI and templates](../guide/ui-and-templates.md).

| Key | Default |
|---|---|
| `enableDevModeNavbar` | `true` |
| `expandSidebarOnHover` | `true` |
| `indentSidebarChildren` | `true` |
| `flatNavStyle` | `false` |
| `compactSidebarStyle` | `false` |
| `legacyNavStyle` | `false` |
| `bodySmallText` | `true` |
| `navSmallText` | `false` |
| `sidebarSmallText` | `false` |
| `footerSmallText` | `false` |
| `brandSmallText` | `false` |
| `collapsedSidebar` | `false` |
| `sidebarElevation` | `2` |
| `holdTransition` | `false` |
| `fixedNavHeader` | `true` |
| `menuAccordion` | `true` |

## Keys read by ATK but not in the defaults

ATK reads these keys with `Config::getGlobal()`, but the default file does not define them. Set them in your application config when you need them. Unless noted, the default is whatever the calling code passes.

| Key | Used by |
|---|---|
| `authorization` | `SecurityManager`. Authorization backend; defaults to the value of `authentication`. |
| `auth_usernode` | The node URI of your users node (e.g. `auth.users`). Required by `CreatedByAttribute`, `UpdatedByAttribute`, the navbar user link and DB auth. |
| `auth_userfk` | The user foreign key in the level table. |
| `auth_emailfield` | The user e-mail column. |
| `auth_accesslevelfield` | Level column in the access table. |
| `auth_loginpage` | URL of a custom login page. |
| `auth_mail_server`, `auth_mail_port`, `auth_mail_login_type`, `auth_mail_suffix` | IMAP backend. |
| `auth_reloadusers`, `auth_sspi_accountfield`, `auth_sspi_trusted_domains` | SSPI backend (`auth_reloadusers` reloads the user on every request). |
| `access`, `attribrestrict` | Filled by `Config::grant()` and `Config::attribRestrict()` (config auth backend). |
| `export_delimiter` | `ExportHandler` (default `;`). |
| `edit_fieldprefix` | Prefix of form field names. |
| `cache_active` | `Cache` (default `true`). |
| `cookiedomain` | Session cookie domain. |
| `default_charset` | Output charset. |
| `durationformat` | `DurationAttribute` display format. |
| `date_to_char` | Date format in `Db::func_datetochar()`. |
| `db_mysql_default_<type>_columntype` | `MySqliDdl` column type overrides. |
| `mysqlfetchmode` | Extra fetch-mode flag for `mysqli_fetch_array`. |
| `database` | Legacy driver name checked by `DateTimeAttribute`. |
| `error_handlers` | Additional error handlers (`Errors\*`). |
| `language_modulefallback` | `Language`. When `true`, missing translations are also looked up in every registered module (default `false`). |
| `mail_sender` | Sender address of error mails. |
| `showmaximporterrors` | `ImportHandler` error limit. |
| `use_mbstring` | Use multibyte string functions. |
| `ziplocation`, `unziplocation`, `zipmode` | `Utils\Zip`. |
