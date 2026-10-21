# Changelog

[← Documentation index](../README.md)

The version history is kept outside `docs/`:

- [`CHANGELOG.md`](../CHANGELOG.md) covers ATK 9.1 (2018) to the current 11.x releases, in [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) format.
- [`doc/CHANGES`](../doc/CHANGES) is the historical log from ATK 3.0 to 9.0 (2016).

Git tags (`v11.8.0`, …) mark each release, which is also published on Packagist as `sintattica/atk`.

Notable changes for the current line:

| Version | Change |
|---|---|
| 11.5.0 | phpdotenv upgraded from 3.6 to 5.6. This is behind the [`.env` known issue](internals/known-issues.md#environment-variables-from-env-are-not-visible-to-configenv). |
| 11.7.0 | New `JsonMultiListAttribute`. |
| 11.8.0 | Smarty 5.8. |

See [Contributing](internals/contributing.md#changelog-and-versions) for how to add entries.
