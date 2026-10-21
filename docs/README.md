# ATK Framework documentation

ATK is a PHP framework for business applications. You describe your data as **nodes**, which are made of **attributes** and **relations**. ATK then generates the admin list, search, add, edit, view and delete pages for you. It also handles validation, access control and the AdminLTE-based UI.

This documentation covers the `main` branch: ATK 11.x, PHP ≥ 8.3, Smarty 5, AdminLTE 3 and phpdotenv 5. See [changelog.md](changelog.md) for version history.

## Where to start

**If you build applications with ATK:**

1. [Installation](getting-started/installation.md)
2. [Configuration basics](getting-started/configuration-basics.md)
3. [Tutorial: your first app](getting-started/tutorial.md)
4. [Core concepts](guide/concepts.md)

**If you work on ATK itself:**

1. [Architecture](internals/architecture.md)
2. [Request lifecycle](internals/request-lifecycle.md)
3. [Contributing](internals/contributing.md)
4. [Known issues](internals/known-issues.md)

## Contents

### Getting started
- [Installation](getting-started/installation.md): requirements, Composer, the skeleton project, public assets.
- [Configuration basics](getting-started/configuration-basics.md): the `.env` file and `config/atk.php`.
- [Tutorial](getting-started/tutorial.md): build a small application step by step.

### Developer guide
- [Core concepts](guide/concepts.md): nodes, attributes, relations, modules, actions and handlers.
- [Modules and menu](guide/modules-and-menu.md)
- [Nodes](guide/nodes.md)
- [Node hooks and overrides](guide/node-hooks.md)
- [Attributes](guide/attributes.md)
- [Attribute reference](guide/attribute-reference.md)
- [Relations](guide/relations.md)
- [Data access](guide/data-access.md)
- [Security](guide/security.md)
- [Internationalisation](guide/i18n.md)
- [Lists, export and import](guide/lists-export-import.md)
- [UI and templates](guide/ui-and-templates.md)

### Reference
- [Configuration keys](reference/configuration.md)
- [Node flags (`NF_*`)](reference/node-flags.md)
- [Attribute flags (`AF_*`)](reference/attribute-flags.md)

### Internals
- [Architecture](internals/architecture.md)
- [Request lifecycle](internals/request-lifecycle.md)
- [Action handlers](internals/handlers.md)
- [Session and state](internals/session-and-state.md)
- [Database layer](internals/database-layer.md)
- [Known issues](internals/known-issues.md)
- [Contributing](internals/contributing.md)

## Conventions used in these docs

- Unless stated otherwise, class names are relative to the root namespace `Sintattica\Atk\`. For example, `Core\Node` means `Sintattica\Atk\Core\Node`.
- Source links point to files under [`src/`](../src). They do not include line numbers, so search the file for the method name.
- Code examples are illustrative. Adapt namespaces and table names to your application.
- Features with a known defect on `main` are linked to [Known issues](internals/known-issues.md).
