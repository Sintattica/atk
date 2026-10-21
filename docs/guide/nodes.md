# Nodes

[← Documentation index](../README.md)

A node (`Core\Node`, [source](../../src/Core/Node.php)) is the central building block of an ATK application. It describes one entity: its table, its attributes and relations, and how its list and forms behave. This page covers what you put in a node's constructor and the most common configuration methods. For the methods you *override*, see [Node hooks and overrides](node-hooks.md).

## A complete example

```php
<?php

namespace App\Modules\Crm;

use Sintattica\Atk\Attributes\Attribute as A;
use Sintattica\Atk\Attributes\DateAttribute;
use Sintattica\Atk\Attributes\EmailAttribute;
use Sintattica\Atk\Attributes\ListAttribute;
use Sintattica\Atk\Attributes\NumberAttribute;
use Sintattica\Atk\Attributes\TextAttribute;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Relations\ManyToOneRelation;
use Sintattica\Atk\Relations\OneToManyRelation;

class Customers extends Node
{
    public function __construct($nodeUri)
    {
        parent::__construct($nodeUri, Node::NF_ADD_LINK | Node::NF_EXPORT | Node::NF_MRA);

        $this->setTable('crm_customer');
        $this->setOrder('[table].name');
        $this->setDescriptorTemplate('[name] ([city])');

        $this->add(new NumberAttribute('id', A::AF_AUTOKEY));
        $this->add(new A('name', A::AF_OBLIGATORY | A::AF_SEARCHABLE | A::AF_UNIQUE));
        $this->add(new A('city', A::AF_SEARCHABLE));
        $this->add(new EmailAttribute('email', A::AF_HIDE_LIST));
        $this->add(new ListAttribute('status', A::AF_SEARCHABLE, ['Prospect', 'Active', 'Lost'], ['prospect', 'active', 'lost']));
        $this->add(new ManyToOneRelation('account_manager_id', A::AF_SEARCHABLE, 'auth.users'));

        // Second tab
        $this->add(new DateAttribute('customer_since'), 'details');
        $this->add(new TextAttribute('notes', A::AF_HIDE_LIST), 'details');
        $this->add(new OneToManyRelation('contacts', A::AF_HIDE_LIST, 'crm.contacts', 'customer_id'), 'contacts');
    }
}
```

Register it from a module with `$this->registerNode('customers', Customers::class, ['admin', 'add', 'edit', 'delete', 'view', 'export'])`. See [Modules and menu](modules-and-menu.md).

## The constructor

```php
public function __construct($nodeUri, $flags = 0)
```

- ATK instantiates nodes with **one** argument, the node URI (see `Atk::newAtkNode()`). Your subclass constructor must therefore accept `$nodeUri` and pass it to the parent along with the node flags.
- The parent constructor splits the URI into module (`getModule()`) and type (`getType()`). It also adds a hidden internal attribute named `_row_color_attribute`, used for row colouring. Call `parent::__construct()` **before** adding attributes.
- After construction, ATK calls `Node::init()`. It finalises the attributes (owner instance, tabs, sorting, `postInit()`) and then calls these hooks:
  - `setFilters()`
  - `setRowColors()`
  - `setLegendItems()`
  - `setAdminHeaderFilter()`
  - `buildAdminPageNodeHelp()`
- Override those hooks rather than putting late set-up in the constructor. See [Node hooks](node-hooks.md#initialisation-hooks).

## Node flags

Flags are `NF_*` constants combined with `|` and passed to the constructor. They can also be changed later with `addFlag()`, `removeFlag()`, `setFlags()` and `hasFlag()`. The most common are:

| Flag | Effect |
|---|---|
| `NF_NO_ADD`, `NF_NO_EDIT`, `NF_NO_DELETE`, `NF_NO_VIEW`, `NF_NO_SEARCH` | Disable the corresponding action |
| `NF_READONLY` | Shorthand for `NF_NO_ADD | NF_NO_EDIT | NF_NO_DELETE` |
| `NF_ADD_LINK` | The admin page shows a "new" link instead of an inline add form |
| `NF_EDITAFTERADD`, `NF_ADDAFTERADD` | After saving, open the edit form of the new record, or another empty add form |
| `NF_COPY`, `NF_EDITAFTERCOPY` | Enable record copy, optionally opening the copy for editing |
| `NF_MRA` (`NF_MULTI_RECORD_ACTIONS`) | Checkboxes and multi-record actions in the list |
| `NF_EXPORT`, `NF_IMPORT` | CSV export / import actions |
| `NF_TRACK_CHANGES` | The original record is available as `$record['atkorgrec']` in `postUpdate()` |
| `NF_NO_SECURITY` | Skip privilege checks for every action on this node |
| `NF_NO_EXTENDED_SEARCH`, `NF_EXT_SORT`, `NF_NO_SORT`, `NF_NO_FILTER` | Search, sort and filter behaviour |

The full list, with values, is in the [node flags reference](../reference/node-flags.md).

## Table, database and ordering

| Method | Description |
|---|---|
| `setTable(string $table, string $seq = '', mixed $db = null)` | Table name. `$seq` is the sequence name for generated ids and defaults to the table name. `$db` is a `Db` instance or the name of a connection in the `db` config key. With no `$db`, the default connection is used. |
| `setDb($db)` / `getDb()` | Change or obtain the database connection of the node. |
| `setOrder(string $orderby)` | Default `ORDER BY` of the list. `[table]` is replaced with the table name. |
| `setIndex(string $attribute)` | Show an alphabetical index (A, B, C…) above the list, based on that attribute. |

## Adding attributes

```php
public function add(Attribute $attribute, string|array $sections = null, int $order = 0): Attribute
```

`add()` returns the attribute, so you can chain setters on it:

```php
$this->add(new A('code', A::AF_OBLIGATORY))->setLabel('Customer code')->setHelp('customer_code_help');
```

Related methods:
- `getAttribute($name)`, `hasAttribute($name)` and `getAttributeNames()` read the attributes;
- `remove($name)` removes one;
- `setAttributeOrder($name, $order)` moves one;
- `addAttributesFlag(array $names, int $flag)` and `removeAttributesFlag(...)` change flags on several attributes at once.

### Order

When `$order` is 0, attributes are shown in the order they are added, at steps of 100. Pass an explicit value to place an attribute between others. For example, `150` places it between the first and second attribute. Adding an attribute with an existing name replaces it and keeps its position, unless you give a new `$order`.

### Tabs and sections

The `$sections` argument places the attribute on tabs and in collapsible sections. The `tabs` config key must be `true`, which is the default; when it is false, everything goes on the default tab.

| `$sections` value | Meaning |
|---|---|
| `null` or `''` | The default tab (`'default'`, or whatever `setDefaultTab()` set) |
| `'details'` | Tab `details` |
| `['details', 'history']` | Shown on both tabs |
| `'*'` | Shown on every tab |
| `'details.address'` | Section `address` on tab `details` |
| `'.address'` | Section `address` on the default tab |

Tab-related methods:
- `setDefaultTab($tab)` changes the tab used for attributes added afterwards.
- `setTabIndex($tab, $index, $action = '')` reorders tabs.
- `addDefaultExpandedSections()` / `removeDefaultExpandedSections()` control whether sections start expanded.

Tab labels are translated with the keys `tab_<name>` or `<name>`. See [Internationalisation](i18n.md).

### Field sets

A field set groups several attributes into one form row, using a template:

```php
$this->add(new A('zip', A::AF_HIDE_LIST | A::AF_NO_LABEL));
$this->add(new A('town', A::AF_HIDE_LIST | A::AF_NO_LABEL));
$this->addFieldSet('address', '[zip] [town]');
```

`addFieldSet($name, $template, $flags = 0, $sections = null, $order = 0)` is shorthand for `add(new FieldSet(...))`. See [Attributes](attributes.md).

### Nested (JSON) attributes

Attributes created as nested attributes do not have their own column. `add()` automatically adds a hidden `JsonAttribute` for the JSON column that stores them, and loads and stores the nested values through it. The base `preAdd()` and `preUpdate()` serialise these values, so **call the parent method** when you override those triggers. See [Attributes](attributes.md).

## Primary key and descriptor

The primary key consists of the attributes flagged `AF_PRIMARY`. `AF_AUTOKEY` includes it, and also implies `AF_AUTO_INCREMENT` and hides the field. The related methods are:
- `primaryKeyField()` returns the first key field.
- `primaryKey($record)` returns the SQL condition identifying a record, e.g. `crm_customer.id='12'`.
- `fetchByPk(int $pk)` loads a record by numeric key.

The **descriptor** is the human-readable name of a record. It is used in dropdowns, page titles and delete confirmations. `Node::descriptor($record)` resolves it in this order:

1. A descriptor handler object set with `setDescriptorHandler($obj)`. Its `descriptor($record, $node)` method is called.
2. A template set with `setDescriptorTemplate('[lastname], [firstname]')`. Fields of relations can be referenced as `[relation.field]`.
3. A `descriptor_def()` method on the node, returning such a template.
4. The primary key value, or the first attribute if there is no primary key.

You can also override `descriptor()` itself. `descriptorFields()` returns the attributes used by the template, so relations know which fields to load.

## Filters

Filters restrict every query the node makes: list, select, relation dropdowns and so on.

```php
protected function setFilters(): void
{
    $this->addFilter('crm_customer.deleted', '0');                 // field = value
    $this->addFilter("crm_customer.status <> 'lost'");             // raw SQL condition
}
```

- **Ways to add them.** Call `addFilter()` in the constructor or in the `setFilters()` hook.
- **Removing them.** `removeFilter($filter, $value = '')` removes a filter.
- **Bypassing them.** `NF_NO_FILTER` makes the node ignore its filters. For a single query, use `->ignoreDefaultFilters()` on the `Selector` returned by `select()`. See [Data access](data-access.md).

## Admin page extras

These options change the list page (`admin` action). Most are set from the init hooks.

- **Row colours.** Call `addColorCondition(string $uiState, callable $callback)` in `setRowColors()`. The first condition that returns true colours the row. `$uiState` is a `AdminLte\UIStateColors` constant such as `STATE_SUCCESS` or `STATE_DANGER`.
  - Records with `disabled` (or `_disabled`) equal to `1` or `'t'` are always shown with the "disabled" state.
  - `setRowColorMode(Node::ROW_COLOR_MODE_CELL)` shows a coloured dot column instead of colouring the whole row.
  - The older override `rowColor(array $record)` still exists.
- **Legend.** Call `addLegendItem($text, $uiState)` in `setLegendItems()` to explain the colours above the list.
- **Header filter buttons.** `addAdminHeaderButtonFilter($label, array $values, $noFilterLabel)` renders one button per value. Clicking a button stores the value as page variable `$label`, which you read back, for example with `SessionManager::getInstance()->pageVar($label)`, to add a filter.
- **Header input filters.** The protected `addAdminHeaderInputFilter(Attribute $attr, $mode = 'edit')` and `getAdminHeaderInputFilterValue($name)` render attribute widgets in the header and read their posted values.
- **Node help.** Return HTML from `buildAdminPageNodeHelp()`, or call `setAdminPageNodeHelp()`, to show help in the page header.
- **Inline-editable columns.** `setEditableListAttributes([...])` makes those attributes editable directly in the list.
- **Multi-record selection.** `setMRASelectionMode(Node::MRA_MULTI_SELECT | MRA_SINGLE_SELECT | MRA_NO_SELECT)` sets how many records can be selected for multi-record actions.

See also [Lists, export and import](lists-export-import.md).

## Feedback after actions

By default, ATK returns to the previous page after `save`, `update` or `delete`. To show a message page first, call `setFeedback()`:

```php
use Sintattica\Atk\Handlers\ActionHandler;

$this->setFeedback(['save', 'update'], ActionHandler::ACTION_SUCCESS | ActionHandler::ACTION_FAILED);
```

## Using a node from code

```php
$node = Atk::getInstance()->atkGetNode('crm.customers');

$rows = $node->select("crm_customer.status = 'active'")
    ->orderBy('crm_customer.name')
    ->limit(10)
    ->getAllRows();

$record = $node->fetchByPk(12);
$record['city'] = 'Milano';
$node->updateDb($record);
$node->getDb()->commit();   // addDb/updateDb/deleteDb do not commit
```

See [Data access](data-access.md) for the full `Selector` API and the store methods.

## Special node classes

- **`Core\DataNode`** ([source](../../src/Core/DataNode.php)) is a node backed by an in-memory array instead of a table. Pass data with `setData()` or override `select`. It forces `NF_NO_ADD | NF_NO_EDIT`. See [Data access](data-access.md).
- **`Core\TreeNode`** ([source](../../src/Core/TreeNode.php)) shows records as a parent/child tree. One attribute must carry `AF_PARENT`. It is only partially ported; see [Known issues](../internals/known-issues.md#treenode-is-not-fully-ported).

## Related pages

- [Node hooks and overrides](node-hooks.md)
- [Attributes](attributes.md) and [Relations](relations.md)
- [Security](security.md): `allowed()`, security aliases and maps
- [Node flags reference](../reference/node-flags.md)
