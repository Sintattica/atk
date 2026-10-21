# Configuration basics

[← Documentation index](../README.md)

An ATK application is configured in two places:

- **`.env`** in the app root holds per-environment values and secrets. `Core\Atk` loads it with phpdotenv.
- **`config/atk.php`** returns an array that is merged over ATK's defaults in [`src/Resources/config/atk.php`](../../src/Resources/config/atk.php).

For every key, its default and how merging works, see the [Configuration reference](../reference/configuration.md).

## `.env`

```dotenv
APP_ENV=dev
DB_HOST=127.0.0.1
DB_NAME=conference
DB_USER=conference
DB_PASSWORD=conference
DEBUG_LEVEL=1
# bcrypt hash of the administrator password (see below)
ADMIN_PASSWORD='$2y$10$HURwCzn3JJmSV.8UZEVW/eaO/RSlYKELKFacIwTyKSPssxp101XDC'
```

Put the hash in single quotes so `$` is not interpolated.

### Read `.env` values explicitly

ATK's defaults try to read `DB_*`, `ADMIN_PASSWORD`, `DEBUG_LEVEL` and similar keys through `Config::env()`, which uses `getenv()`. With phpdotenv 5, `.env` values reach only `$_ENV` and `$_SERVER`, so these defaults do **not** see them. Details are in [Known issues](../internals/known-issues.md#environment-variables-from-env-are-not-visible-to-configenv).

Until that is fixed, map the values yourself in `config/atk.php`. That file is included after `.env` is loaded, so `$_ENV` is already populated.

Always set `APP_ENV` in `.env`. `Ui\IndexPage` reads `$_SERVER['APP_ENV']` without checking that it exists, and it shows the development navbar unless the value is `prod`. See [Known issues](../internals/known-issues.md#indexpage-requires-app_env).

## `config/atk.php`

```php
<?php

$env = fn(string $key, $default = null) => $_ENV[$key] ?? $default;
$isDev = $env('APP_ENV') === 'dev';

return [
    'identifier' => 'conference',

    // Database (the "default" connection)
    'db' => [
        'default' => [
            'host' => $env('DB_HOST', 'localhost'),
            'db' => $env('DB_NAME'),
            'user' => $env('DB_USER'),
            'password' => $env('DB_PASSWORD', ''),
            'charset' => 'utf8mb4',
            'driver' => 'MySqli',
        ],
    ],

    // Modules to register, and the menu class
    'modules' => [
        App\Modules\Conference\Module::class,
    ],
    'menu' => App\Menu::class,

    // Security
    'authentication' => 'config',
    'securityscheme' => 'none',
    'administratorpassword' => $env('ADMIN_PASSWORD', ''),

    // Language
    'language' => 'en',
    'supported_languages' => ['en', 'it'],

    // Development helpers
    'debug' => (int)$env('DEBUG_LEVEL', 0),
    'meta_caching' => !$isDev,
];
```

The keys you will set in almost every project:

| Key | Why |
|---|---|
| `db` | The connections. `default` is used unless a node calls `setTable(..., $db)` with another name. |
| `modules` | The `Core\Module` subclasses to register. See [Modules and menu](../guide/modules-and-menu.md). |
| `menu` | Your `Core\Menu\MenuBase` subclass. The built-in `Core\Menu\Menu` adds no items of its own. |
| `authentication`, `securityscheme`, `restrictive`, `auth_*` | Who can log in and what they can do. See [Security](../guide/security.md). |
| `administratorpassword` | Bcrypt hash for the built-in `administrator` user, who bypasses all security checks. An empty value disables it. |
| `language`, `supported_languages`, `language_basedir` | Translations. See [Internationalisation](../guide/i18n.md). |
| `debug` | `1` or higher shows the debug console under each page. |
| `meta_caching` | Caches table metadata. Disable it in development. |

### Generating password hashes

ATK ships a small CLI helper that prints a `password_hash()` hash:

```bash
php vendor/sintattica/atk/src/Utils/generatehash.php demo
# clean: demo
# hash: $2y$10$...
```

### Your own configuration sections

Any `config/<section>.php` file that returns an array can be read with `Config::get('<section>', '<key>', $default)`. If `<section>` is a module name, the module's own `config/<section>.php` provides the fallback values.

### Merging pitfall

The application array is merged into the defaults key by key, numeric keys included. A short list such as `recordsperpage_options => [10, 20]` therefore keeps the remaining default entries. See the [Configuration reference](../reference/configuration.md#how-configuration-is-loaded).

## Next steps

- [Tutorial](tutorial.md)
- [Core concepts](../guide/concepts.md)
