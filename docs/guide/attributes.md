# Attributes

[← Documentation index](../README.md)

An **attribute** is one field of a [node](nodes.md). It knows how to:

- load its value from the database and store it back;
- render the value in lists, view pages and edit forms;
- build a search field and turn the search input into SQL;
- validate user input.

All attributes extend `Attributes\Attribute` ([source](../../src/Attributes/Attribute.php)). [Relations](relations.md) are attributes too, because `Relations\Relation` extends `Attribute`. For the full list of attribute classes, see the [Attribute reference](attribute-reference.md).

## Adding an attribute to a node

```php
use Sintattica\Atk\Attributes\Attribute as A;
use Sintattica\Atk\Attributes\NumberAttribute;
use Sintattica\Atk\Attributes\DateAttribute;

// inside the node constructor
$this->add(new NumberAttribute('id', A::AF_AUTOKEY));
$this->add(new A('name', A::AF_OBLIGATORY | A::AF_SEARCHABLE));
$this->add(new DateAttribute('birthdate'), 'details');   // placed on the "details" tab
```

- The constructor of `Attribute` is `__construct($name, $flags = 0)`. Subclasses add their own parameters after `$flags`.
- `$name` is the database column name, unless the attribute is not stored (see [Storage and load types](#storage-and-load-types)).
- `Node::add()` returns the attribute, so you can chain fluent setters:

```php
$this->add(new A('email'))->setLabel('E-mail')->setHelp('Used for notifications');
```

How sections, tabs and ordering work in `Node::add()` is described in [Nodes](nodes.md).

## Flags

Attribute behaviour is mostly controlled with `AF_*` bit flags. Combine them with `|`, and change them later with `addFlag()`, `removeFlag()`, `setFlags()` and `hasFlag()`.

The flags you will use most often:

| Flag | Effect |
|---|---|
| `AF_OBLIGATORY` | Value is required; validation fails if it is empty. |
| `AF_UNIQUE` | Value must be unique in the table. |
| `AF_PRIMARY` | Part of the primary key (implies obligatory). |
| `AF_AUTO_INCREMENT` | Value comes from a sequence or auto-increment column. |
| `AF_AUTOKEY` | Shortcut for a hidden, auto-incremented primary key. |
| `AF_HIDE_LIST`, `AF_HIDE_ADD`, `AF_HIDE_EDIT`, `AF_HIDE_VIEW`, `AF_HIDE_SEARCH`, `AF_HIDE_SELECT` | Hide the attribute in one mode. `AF_HIDE` hides it everywhere. |
| `AF_READONLY_ADD`, `AF_READONLY_EDIT`, `AF_READONLY` | Show the value but do not allow editing. |
| `AF_SEARCHABLE` | Adds a search field in the list header. |
| `AF_FORCE_LOAD` | Always load the value, even if the attribute is not displayed. |
| `AF_NO_LABEL`, `AF_BLANK_LABEL` | Label handling in forms. |
| `AF_NO_SORT` | Column cannot be sorted. |
| `AF_TOTAL` | Show a totals row for this column in lists. |
| `AF_CASCADE_DELETE` | Call the attribute's `delete()` when the owner record is deleted (used by relations). |
| `AF_LARGE` | For relations with many records: use a select page instead of a dropdown. |

Flags from `33554432` (bit 25) upwards are **class-specific**. Their meaning depends on the attribute class, for example `DateAttribute::AF_DATE_DEFAULT_EMPTY` or `FileAttribute::AF_FILE_NO_UPLOAD`. Always reference them through the class that defines them.

The complete table, including class-specific flags, is in [Attribute flags reference](../reference/attribute-flags.md).

## Disabled modes

In addition to flags, an attribute can be disabled in view or edit mode with `addDisabledMode()`, `setDisabledModes()` and `removeDisabledMode()`. The values are:

- `Attribute::DISABLED_VIEW`
- `Attribute::DISABLED_EDIT`
- `Attribute::DISABLED_ALL`

## Storage and load types

By default an attribute is part of the main `SELECT` (`addToQuery()`) and of the `INSERT`/`UPDATE`. Change this with `setStorageType($type, $mode = null)` and `setLoadType($type, $mode = null)`.

| Constant | As storage type | As load type |
|---|---|---|
| `NOSTORE` / `NOLOAD` (0) | Never stored. | Never loaded. |
| `PRESTORE` / `PRELOAD` (1) | `store()` is called before the normal attributes. | `load()` is called before `select()`. |
| `POSTSTORE` / `POSTLOAD` (2) | `store()` is called after the record is saved (relations that need the new primary key). | `load()` is called after `select()`. |
| `ADDTOQUERY` (4) | Stored through the normal `INSERT`/`UPDATE`. | Loaded through `addToQuery()`. |

The optional `$mode` limits the type to one mode, e.g. `'add'` or `'update'`.

Use `NOSTORE`/`NOLOAD` for computed or display-only attributes that have no column. Dummy, button and expression attributes already do this.

## Lifecycle

These are the methods the framework calls on an attribute. Override them when you write your own attribute class.

| Phase | Method | Purpose |
|---|---|---|
| Init | `init()`, `postInit()` | Called when the node is initialised. |
| Initial value | `initialValue()` | Value for new records. Set it with `setInitialValue()`. |
| Form input | `fetchValue($postvars)` | Extract the value from the posted form. |
| Validation | `validate(&$record, $mode)` | Add errors with `Tools::triggerError(&$record, $attrib, $err, $msg)`. |
| To database | `value2db(array $record)` | Convert the internal value to the database value. |
| From database | `db2value($record)` | Convert the database value to the internal value. |
| Query | `addToQuery($query, ...)` | Add the field (and joins) to a `Db\Query`. |
| Edit | `edit($record, $fieldprefix, $mode)` | Return the HTML form element. |
| Display | `display(array $record, string $mode)` | Return the HTML for list and view (`$mode` is `'list'`, `'view'`, `'csv'`, …). |
| Search | `search($record, $extended, $fieldprefix, $grid)` | Return the search field HTML. |
| Search SQL | `getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname)` | Return the SQL condition for a search value. |
| Save decisions | `needsInsert($record)`, `needsUpdate($record)` | Whether the attribute takes part in the insert or update. |
| Delete | `delete($record)`, `postDelete($record)` | Called when the owner record is deleted. |

The framework does not call `edit()` and `display()` directly. It calls the "smart" wrappers `getEdit()` and `getView()`, which apply readonly flags and the overrides described in the next section.

## Customising rendering without subclassing

There are three ways to change how an attribute is rendered. They are checked in this order:

1. **Callbacks set on the attribute.**
   - `setViewCallback(callable $cb)` is called as `$cb($record, $mode, $attribute)` instead of `display()`.
   - `setEditCallback(callable $cb)` is called as `$cb($record, $fieldprefix, $mode, $attribute)` instead of `edit()`.
2. **Methods on the node named after the attribute:**
   - `<name>_display($record, $mode, $attribute)`;
   - `<name>_edit($record, $fieldprefix, $mode, $attribute)`. This does not work for an attribute named `action`, because `action_edit` would clash with the action method.
3. **The attribute's own `display()` / `edit()`.**

The node-method overrides are described with the other node hooks in [Node hooks and overrides](node-hooks.md). These include `<name>_hide`, `<name>_search`, `<name>_validate` and `<name>_searchcondition`.

```php
$this->add(new A('status'))->setViewCallback(function ($record, $mode, $attr) {
    return '<span class="badge badge-info">' . htmlspecialchars($record['status']) . '</span>';
});
```

## Labels, help and appearance

The common fluent setters are:

| Purpose | Setters |
|---|---|
| Labels | `setLabel()`, `setPostFixLabel()`, `setLabelPositionTop()` |
| Help and hints | `setHelp()`, `setPlaceholder()` |
| Size | `setAttribSize($size)`, `setMaxSize()`, `setSearchSize()`, `setMaxChars()` |
| Width and height | `setMinWidth()`, `setMaxWidth()`, `setMaxHeight()` |
| Text wrapping | `setTextWrap()`, `setTitleWrap()` |
| Display mode | `setDisplayMode()` with `Attribute::MODE_DEFAULT`, `MODE_SCROLL` or `MODE_INLINE`, for long values in lists |
| CSS | `addCSSClass()`, `addCSSClasses()`, `addRowCSSClass()`, `setCssStyle($type, $style, $value)` |
| Output escaping | `setHtmlSpecialChars()`, `setNl2br()` |
| Select2 widgets | `setSelect2Options($options, $types)` |
| Initial visibility | `setInitialHidden(true)` hides the field until a dependency shows it |

If no label is set, the label is looked up as a translation of the attribute name. See [Internationalisation](i18n.md).

## Dependencies and on-change handlers

**`addOnChangeHandler($jscode)`** attaches client-side JavaScript to the field's change event. Inside the code, the element is available as `el`.

**`addDependency(callable $cb)`** adds a server-side dependency.
- When the field changes, ATK sends a partial request (`partial_dependencies`). Each callback is called with a `Utils\EditFormModifier`.
- The callbacks also run once when the form is first rendered. On that first call `$modifier->isInitial()` returns `true`.
- `EditFormModifier` provides:
  - `getNode()`, `getRecord()` (returned by reference), `getMode()`, `getFieldPrefix()`;
  - `showAttribute()` / `hideAttribute()` (and the plural forms);
  - `showTab()` / `hideTab()`;
  - `refreshAttribute()` / `refreshAttributes()`;
  - `scriptCode()` / `scriptFile()`.

```php
$type = $this->add(new ListAttribute('type', 0, ['Person', 'Company'], ['P', 'C']));
$this->add(new A('vat_number'))->setInitialHidden(true);

$type->addDependency(function (EditFormModifier $modifier) {
    $record = $modifier->getRecord();
    if ($record['type'] === 'C') {
        $modifier->showAttribute('vat_number');
    } else {
        $modifier->hideAttribute('vat_number');
    }
});
```

## Search modes

`search()` renders the search field. The search mode decides how the value is matched. The base `Attribute::getSearchModes()` returns `substring`, `exact`, `wildcard` and `regexp`. Subclasses return their own list; for example, number and date attributes support range searches.

You can fix the mode for one attribute with `setAttributeSearchmode($mode)`. The global default is the config key `search_defaultmode`; see the [Configuration reference](../reference/configuration.md).

## Nested (JSON) attributes

The classes in `Attributes\NestedAttributes` store their value inside a JSON column instead of their own column. Their constructor takes an extra `$nestedAttributeField` argument, which is the name of the JSON column.

When you `add()` the first nested attribute for a given JSON column, `Node::add()` does the following:

- It creates a hidden `JsonAttribute` for that column, with `AF_HIDE | AF_FORCE_LOAD`, and forces it to update.
- It sets the nested attribute to `NOSTORE`/`NOLOAD`. Its value is read from and written to the JSON column.

Sorting and searching use JSON extraction in SQL (`NestedAttribute::buildJSONExtractValue()`).

```php
use Sintattica\Atk\Attributes\NestedAttributes\NestedBoolAttribute;
use Sintattica\Atk\Attributes\NestedAttributes\NestedNumberAttribute;

$this->add(new NestedBoolAttribute('newsletter', 0, 'preferences'));
$this->add(new NestedNumberAttribute('max_items', 0, 'preferences'));   // both stored in column "preferences"
```

See the [Attribute reference](attribute-reference.md#nested-json-attributes) for the available nested classes.

## Writing your own attribute

Extend the closest existing class and override only what you need:

- usually `edit()` and `display()`;
- `value2db()`/`db2value()` if the stored format differs;
- `validate()` for custom rules;
- `getSearchCondition()` for custom search;
- `dbFieldType()` so ATK knows how to quote the value.

Use `$this->getOwnerInstance()` to reach the node, and `$this->getHtmlId($fieldprefix)` / `$this->getHtmlName($fieldprefix)` for form element names.

## See also
- [Attribute reference](attribute-reference.md)
- [Relations](relations.md)
- [Node hooks and overrides](node-hooks.md)
- [Attribute flags reference](../reference/attribute-flags.md)
