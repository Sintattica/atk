# Contributing

[← Documentation index](../README.md)

## Branches

| Branch | Content |
|---|---|
| `main` | Latest development. It started from `release/v9` and is now the 11.x line (see [`CHANGELOG.md`](../../CHANGELOG.md)). |
| `release/v9` | The 9.x rolling releases, with the AdminLTE-based layout. |
| `9.1` | Like 9.0, jQuery only. |
| `9.0` | The deep refactoring: Composer, PSR-4, PHP 7, Smarty 3, Select2, Bootstrap. Not backward compatible. |
| `8.2` | "Classic" ATK, closest to the original iBuildings version. |

The [README](../../README.md#branches) describes each branch in more detail.

## Pull requests

These rules come from the README. Sintattica uses these branches in production, so bug-fix pull requests are welcome. New features are not guaranteed to be accepted, so get in touch before starting one.

- Create a feature branch from `8.2`, `9.0` or `main`.
- Keep PSR-4 layout and formatting.
- Keep the change tight: one well-defined fix or feature, not dozens of classes.
- Avoid redundant comments.
- Add an entry to [`CHANGELOG.md`](../../CHANGELOG.md).

## Changelog and versions

`CHANGELOG.md` follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) and [Semantic Versioning](https://semver.org/spec/v2.0.0.html):

- each release is `## [x.y.z] - YYYY-MM-DD`;
- entries go under `Added`, `Changed` and `Fixed`;
- each entry starts with the area, e.g. `- Attribute: ...`.

Releases are git tags (`v11.8.0`) and are published to Packagist as `sintattica/atk`. For history before 9.0, see [`doc/CHANGES`](../../doc/CHANGES).

## Code style

- PSR-1/PSR-2 style, namespace `Sintattica\Atk\` mapped to `src/` (PSR-4).
- [`.php_cs`](../../.php_cs) is a legacy PHP-CS-Fixer 1.x config: PSR-2 level plus `concat_without_spaces`, with `src/Resources` excluded. PHP-CS-Fixer 3 does not read it as-is.
- [`doc/Project.xml`](../../doc/Project.xml) is the PhpStorm code-style scheme, with a 160-column right margin.
- Classic phpdoc blocks (`@param`, `@return`) are used throughout. Newer code also adds native PHP 8 parameter and return types. When you change a method signature on `Core\Node` or `Attributes\Attribute`, remember that applications override these methods and must match their return types.
- No file uses `declare(strict_types=1)`.

## Development environment

The repository ships a minimal container setup:

- [`compose.yaml`](../../compose.yaml) defines a single PHP-FPM container, `php-fpm-atk`.
  - It uses the image `registry.gitlab.com/sintattica-sinlog/docker/php-fpm:v5`.
  - It mounts the repository at `/app`.
  - It joins the external Docker network `sinlog-net`, which must already exist.
  - There is no web server or database service. Test ATK changes from an application that requires your working copy.
- [`docker/console.sh`](../../docker/console.sh) wraps Composer inside that container:

  ```bash
  docker compose up -d
  docker/console.sh composer-install
  docker/console.sh composer-require vendor/package
  docker/console.sh composer-update [vendor/package]
  docker/console.sh composer-remove vendor/package
  ```

To try a change in a real application, point the application's `composer.json` at your working copy:

```json
"repositories": [{ "type": "path", "url": "../atk", "options": { "symlink": true } }],
"require": { "sintattica/atk": "dev-main" }
```

> **To verify:** that `dev-main` resolves for a path repository on your branch. Otherwise use `"version": "11.99.0"` in the repository options, or require your branch name with a `dev-` prefix.

## Tests

There are no automated tests: no `tests/` directory, PHPUnit configuration or `require-dev` dependencies. Verify changes manually in an application, ideally with `debug` ≥ 1 so the debug console shows queries and notices. Adding a test suite would be a welcome contribution.

## Documentation

These docs live in `docs/`.

- Keep claims grounded in the code.
- Reference code by file path and class/method name, not by line number.
- When you fix a bug listed in [Known issues](known-issues.md), remove its entry in the same pull request and update the pages that link to it.
