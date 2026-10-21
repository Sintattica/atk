# Installation

[← Documentation index](../README.md)

ATK is a Composer library (`sintattica/atk`). It is not a standalone application. You install it into your own project, which provides:

- a front controller (`index.php`);
- a `.env` file;
- a `config/atk.php` file;
- your modules.

## Requirements

From [`composer.json`](../../composer.json):

- **PHP ≥ 8.3**, with the `zlib`, `json`, `simplexml`, `zip` and `mysqli` extensions.
- **A database:** MySQL/MariaDB (driver `MySqli`, the default) or PostgreSQL (driver `PgSql`, which also needs the `pgsql` extension).
- **Composer.**
- **A web server** that sends every request to your front controller: nginx with PHP-FPM, Apache, or `php -S` for development.

Composer also installs Smarty 5, phpdotenv 5 and moment.php.

## Project layout

ATK's default paths assume this layout. The app root is the directory that contains `vendor/`.

```
my-app/
├── .env                      # environment values, loaded by Core\Atk
├── composer.json
├── config/
│   └── atk.php               # your configuration, merged over ATK's defaults
├── languages/                # optional application translation overrides
├── src/
│   ├── Menu.php              # your MenuBase subclass
│   └── Modules/
│       └── <Name>/Module.php # one directory per module
├── var/atktmp/               # writable: compiled templates, temp files
├── vendor/                   # Composer
└── web/                      # document root
    ├── index.php
    └── bundles/atk -> ../../vendor/sintattica/atk/src/Resources/public
```

The defaults (see [Configuration reference](../reference/configuration.md#paths-and-files)) expect:

- `config/atk.php` in the app root;
- a writable `var/atktmp/`;
- ATK's public assets served at `/bundles/atk/` (`assets_url`).

Change these keys if your layout is different. For example, the [atk-skeleton](https://github.com/Sintattica/atk-skeleton) project serves `public/` and sets `assets_url` to `/assets/`.

## Step by step

1. **Create the project and require ATK.**

   ```bash
   mkdir my-app && cd my-app
   composer init --name=acme/my-app --type=project -n
   composer require sintattica/atk:^11.8
   ```

   Add your application namespace to `composer.json`, then run `composer dump-autoload`:

   ```json
   "autoload": { "psr-4": { "App\\": "src/" } }
   ```

2. **Publish the assets.** ATK's CSS, JavaScript and images live in `vendor/sintattica/atk/src/Resources/public`. Symlink them under the document root, matching `assets_url`:

   ```bash
   mkdir -p web/bundles var/atktmp
   ln -s ../../vendor/sintattica/atk/src/Resources/public web/bundles/atk
   ```

3. **Write the front controller** `web/index.php`:

   ```php
   <?php

   use Sintattica\Atk\Core\Atk;

   require __DIR__ . '/../vendor/autoload.php';

   // The second argument is the app root, with a trailing slash. ATK loads "<root>.env" from it.
   $atk = new Atk($_SERVER['APP_ENV'] ?? 'prod', __DIR__ . '/../');
   $atk->run();
   ```

   `new Atk()` stops with `cannot locate .env file` if the `.env` file is missing. Only one `Atk` instance may exist per process.

4. **Create `.env` and `config/atk.php`.** See [Configuration basics](configuration-basics.md).

5. **Create the database.** Create an empty database and a user. If you use database authentication, also create the user and access tables (see [Security](../guide/security.md)).

6. **Serve it.**

   ```bash
   php -S 0.0.0.0:8000 -t web/
   ```

   Open `http://localhost:8000`. You should see the ATK login page.

## Using the skeleton project

[Sintattica/atk-skeleton](https://github.com/Sintattica/atk-skeleton) is a ready-made project. It includes:

- Docker containers for nginx, PHP-FPM and MySQL;
- an `Auth` module with users, groups and access rights;
- a demo module and a SQL dump.

The skeleton currently targets ATK **9.9** and PHP 7.4. To use it with `main`:

- raise `php` to `>=8.3` and `sintattica/atk` to `^11.8` in its `composer.json`;
- update its Dockerfile to a PHP 8.3 image;
- check its code against the [Known issues](../internals/known-issues.md) and the changelog.

## Next steps

- [Configuration basics](configuration-basics.md)
- [Tutorial](tutorial.md)
