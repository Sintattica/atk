# Relations

[← Documentation index](../README.md)

A relation links a node to another node, called the **destination**. In ATK, relations are attributes: they extend `Relations\Relation`, which extends `Attributes\Attribute`. You add them to a node with `$this->add(...)` like any other attribute. All classes are in the namespace `Sintattica\Atk\Relations` ([source](../../src/Relations)).

The destination is always a **node URI** in `module.node` notation, e.g. `'app.customer'`. The node must be registered by its module; see [Modules and menu](modules-and-menu.md).

| Relation | Database shape | UI |
|---|---|---|
| [`ManyToOneRelation`](#manytoonerelation) | Foreign key in the owner table | Dropdown (Select2), autocomplete or select page |
| [`ManyToOneTreeRelation`](#manytoonetreerelation) | Same, destination is a tree | Indented dropdown |
| [`OneToManyRelation`](#onetomanyrelation) | Foreign key in the destination table | Embedded list of child records |
| [`OneToOneRelation`](#onetoonerelation) | Foreign key on either side | Embedded or integrated form |
| [`PolymorphicOneToOneRelation`](#polymorphiconetoonerelation) | One-to-one where the destination node depends on a type | Embedded form |
| [`ManyToManyRelation`](#many-to-many-relations) family | Link (junction) table | Checkboxes, list, autocomplete or shuttle |

## Common relation API (`Relation`)

Constructor: `__construct($name, $flags, $destination)`.

**`getDestination()`** returns the destination node instance; `createDestination()` creates it if needed.

**Destination filters** restrict which destination records can be chosen:
- `setDestinationFilter($filter)` replaces the filter.
- `addDestinationFilter($filter)` adds one with `AND`.
- The filter is an SQL condition. It may contain `[field]` placeholders taken from the current record, which `parseFilter()` replaces. Example: `"customer.country_id = '[country_id]'"`.

**Descriptor overrides** change how destination records are described:
- `setDescriptorTemplate($template)`, e.g. `'[lastname], [firstname]'`;
- `setDescriptorHandler($handler)`.

By default the destination node's own descriptor is used; see [Nodes](nodes.md).

**`setDisplayListMode($mode)`** changes how multiple values are rendered in lists: `Relation::MODE_LIST_BADGE_PILL` (default) or `Relation::MODE_LIST_UL`.

**Flags.** Generic flags that matter for relations:
- `AF_LARGE`: select page instead of a dropdown.
- `AF_NO_FILTER`: ignore the destination's node filters.
- `AF_CASCADE_DELETE`: delete related records together with the owner.

Relation-specific flags are listed per class below and in the [Attribute flags reference](../reference/attribute-flags.md).

## ManyToOneRelation

N:1. The owner record stores the primary key of one destination record. For example, every project has one coordinator.

```php
new ManyToOneRelation($name, $flags, $destination)
```

- `$name` is the **foreign key column** in the owner table. For composite foreign keys, pass an array of columns in the same order as the destination's primary key attributes.
- The value in a record is an array with the destination's primary key (and descriptor fields), not a scalar.

```php
use Sintattica\Atk\Relations\ManyToOneRelation;
use Sintattica\Atk\Attributes\Attribute as A;

$this->add(new ManyToOneRelation('customer_id', A::AF_OBLIGATORY | A::AF_SEARCHABLE, 'app.customer'))
    ->setDestinationFilter("customer.active = 1");
```

**Flags** (the `AF_RELATION_*` and `AF_MANYTOONE_*` names are aliases):

| Flag | Effect |
|---|---|
| `AF_RELATION_AUTOLINK` / `AF_MANYTOONE_AUTOLINK` | Add view/edit/add links next to the dropdown. |
| `AF_RELATION_NO_NULL_ITEM` / `AF_MANYTOONE_NO_NULL_ITEM` | Never add the empty option. |
| `AF_MANYTOONE_OBLIGATORY_NULL_ITEM` | Add an empty option even when the relation is obligatory. |
| `AF_RELATION_AUTOCOMPLETE` / `AF_MANYTOONE_AUTOCOMPLETE` | Autocomplete field instead of a dropdown. |
| `AF_MANYTOONE_LAZY` | Load the destination record lazily instead of joining it in the main query. |

**Autocomplete settings.** The global defaults are the `manytoone_autocomplete_*` and `manytoone_search_autocomplete` config keys. With `manytoone_autocomplete_large`, `AF_LARGE` relations use autocomplete automatically. Per relation you can call:
- `setAutoCompleteSearchFields(array)`;
- `setAutoCompleteSearchMode()` with `ManyToOneRelation::SEARCH_MODE_EXACT`, `SEARCH_MODE_STARTSWITH` or `SEARCH_MODE_CONTAINS`;
- `setAutoCompleteMinChars()`, `setAutoCompleteMinRecords()`, `setAutoCompleteSize()`, `setPaginationLimit()`.

**Other useful setters:**

| Setter | Effect |
|---|---|
| `addListColumn($attr)`, `addListColumns(...)`, `setListColumns(...)`, `setAlwaysShowListColumns()` | Show destination attributes as extra columns in the owner's list. |
| `setNoneLabel($label)` | Label of the empty option. |
| `setSelectableRecordsCallback(callable)` | Compute the selectable records yourself. |
| `setJoinFilter($filter)` | Extra condition on the SQL join. |
| `setAutoLinkDestination()`, `setUseFilterForAddLink()` | Control the auto-links. |
| `setHideWhenEmpty()` | Hide the field when there is nothing to choose. |
| `setMultipleSearch()` | Allow selecting several values in the search field. |
| `setConcatDescriptorFunction()` | Customise descriptor concatenation in SQL. |

`Attributes\CreatedByAttribute` and `Attributes\UpdatedByAttribute` are `ManyToOneRelation`s to the user node; see the [Attribute reference](attribute-reference.md#security-and-users).

## ManyToOneTreeRelation

Same constructor as `ManyToOneRelation`. It renders the destination records as an indented tree in the dropdown. The destination node must have a parent attribute flagged with `AF_PARENT`.

## OneToManyRelation

1:N, the master-detail relationship. The foreign key lives in the **destination** table. For example, a department has many employees.

```php
new OneToManyRelation($name, $flags, $destination, $refKey = '')
```

- `$name` is a unique name in the node. It is **not** a database column.
- `$refKey` is the foreign key attribute in the destination node that points to the owner. Use an array for multi-column keys.
  - If it is empty, the owner node type name is used, e.g. `department`.
  - The `$refKey` columns are excluded from the embedded grid automatically.
- The constructor always adds `AF_NO_SORT | AF_HIDE_ADD`. The child list is not shown while the master record is being added, because it has no primary key yet.

```php
use Sintattica\Atk\Relations\OneToManyRelation;

$this->add(new OneToManyRelation('employees', A::AF_CASCADE_DELETE, 'app.employee', 'department_id'), 'employees');
```

The destination node usually has the matching `ManyToOneRelation('department_id', ..., 'app.department')`.

**Flags:**

| Flag | Effect |
|---|---|
| `AF_RESTRICTED_DELETE` | The master record cannot be deleted while child records exist. |
| `AF_ONETOMANY_SHOW_ADD` | Show the child grid in add mode too. Children are kept in the session (`Session\SessionStore`, `OneToManyRelationSessionGridHandler`) and saved together with the master. The source comments warn that this only works in simple cases, not with descriptors that use the foreign key, `AF_UNIQUE` children, or composite keys. |
| `AF_CASCADE_DELETE` | Delete the children when the master is deleted. |

**Other setters:**
- `setGridExcludes()`: hide further columns in the embedded grid.
- `setHeader()`, `setFooter()`: content around the grid.
- `setDescriptorListSep()`: separator for multiple descriptors.
- `setOwnerFields()`: owner fields to match against the `$refKey`, when it is not the primary key.
- `setUseFilterForAddLink()`, `setUseFilterForEditLink()`, `setUseRefKeyForFilter()`.

Config keys `onetomany_addlink_position` and `onetomany_label_position_top` control the layout.

If the destination node has `NF_EXPORT`, an export button is shown above the embedded grid.

## OneToOneRelation

1:1. The relation works in one of two modes, chosen by `$refKey`:

```php
new OneToOneRelation($name, $flags, $destination, $refKey = '')
```

- **Slave mode** (`$refKey` empty): the owner table contains the foreign key, and `$name` is that column.
- **Master mode** (`$refKey` set): the destination table contains a foreign key `$refKey` pointing back to the owner.

The destination form is embedded in the owner's form and saved with it.

| Flag | Effect |
|---|---|
| `AF_ONETOONE_ADD` | Override the default "no add" behaviour. |
| `AF_ONETOONE_ERROR` | Enable error notifications and triggers of the destination node. |
| `AF_ONETOONE_INTEGRATE` | Integrate the destination fields as if they belonged to the owner. No divider, no section heading. |
| `AF_ONETOONE_LAZY` | Lazy loading instead of adding to the main query. |
| `AF_ONETOONE_RESPECT_TABS` | Keep the tabs and sections assigned to the destination's attributes. |

## PolymorphicOneToOneRelation

A one-to-one relation whose destination node is chosen at runtime from a type table.

```php
new PolymorphicOneToOneRelation($name, $flags, $typefk, $discriminatorfield, $defaultdest, $modulename, $refKey)
```

| Parameter | Meaning |
|---|---|
| `$typefk` | Foreign key in the master node pointing to the type table. |
| `$discriminatorfield` | Field in the type table that holds the destination table name. A node with the same name must exist in `$modulename`. |
| `$defaultdest` | Default destination node (`module.node`). |
| `$refKey` | Foreign key in the destination that points to the master. |

> **Note:** the example in the class docblock passes `$flags` as the last argument, which does not match the constructor. Follow the signature above.

## Many-to-many relations

`ManyToManyRelation` is the base class. The docblock says it "should not be used directly". It needs a **link node** for the junction table.

```php
new ManyToManyRelation($name, $flags, $link, $destination, $local_key = null, $remote_key = null)
```

- `$link` is the node URI of the intermediate node.
- By default the link node must have one attribute named after each end of the relation. For example, `project_activity` has `project` and `activity`. If such an attribute does not exist, the same name with `_id` appended is tried.
- To use other names, pass `$local_key` / `$remote_key`, or call `setLocalKey()` / `setRemoteKey()`.
- Other setters:
  - `setLocalFilter()`;
  - `setOwnerFields()`;
  - `setStoreDeletionFilter()`, which limits which link rows are deleted when the selection is saved.
- Flag: `AF_MANYTOMANY_DETAILVIEW` shows the linked records in detail.

Use one of the concrete subclasses. They all share the constructor above.

| Class | UI | Specific API |
|---|---|---|
| `ManyBoolRelation` | One checkbox per destination record. | `AF_MANYBOOL_AUTOLINK` (add links), `AF_MANYBOOL_NO_TOOLBAR` (hide select all/none/invert); `setShowDetailsLink()`. |
| `ManyToManyListRelation` | Multi-select list box. | `setRows()`, `setAutoCalculateRows()`. |
| `ManyToManySelectRelation` | Add records one at a time with autocomplete or a select page; optional drag-sorting. | `AF_MANYTOMANYSELECT_DETAILEDIT`, `AF_MANYTOMANYSELECT_DETAILADD`, `AF_MANYTOMANYSELECT_NO_AUTOCOMPLETE`; `setPositionAttribute()` for ordering, `setAutoCompleteSearchFields()`, `setAutoCompleteSearchMode()`, `setAutoCompleteMinChars()`. |
| `ShuttleRelation` | Two lists, "available" and "selected". | `setMaxListWidth()`, `setInitialValue()`. |

```php
use Sintattica\Atk\Relations\ManyBoolRelation;

// table project_activity(project_id, activity_id), node app.project_activity
$this->add(new ManyBoolRelation('activities', 0, 'app.project_activity', 'app.activity', 'project_id', 'activity_id'));
```

## See also
- [Attributes](attributes.md)
- [Nodes](nodes.md)
- [Data access](data-access.md)
- [Attribute flags reference](../reference/attribute-flags.md)
