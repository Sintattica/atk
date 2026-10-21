# Attribute flags reference

[← Documentation index](../README.md)

Attribute flags are bit flags. You pass them as the second constructor argument of every attribute and relation, and change them later with `addFlag()`, `removeFlag()` and `setFlags()`. Combine them with `|`.

```php
use Sintattica\Atk\Attributes\Attribute as A;
use Sintattica\Atk\Attributes\DateAttribute;

$node->add(new A('id', A::AF_AUTOKEY));
$node->add(new DateAttribute('birthdate', A::AF_OBLIGATORY | DateAttribute::AF_DATE_NO_CALENDAR));
```

The flag space is split in two:

- **Generic flags** are bits up to `8388608`. They are defined on [`Attributes\Attribute`](../../src/Attributes/Attribute.php) and mean the same thing on every attribute.
- **Class-specific flags** start at `33554432`. Different classes reuse the same bits, so a class-specific flag only means something on the class that defines it and its subclasses.

For how to use flags, see [Attributes](../guide/attributes.md) and [Relations](../guide/relations.md).

## Generic flags (`Attributes\Attribute`)

### Validation and keys

| Constant | Value | Effect |
|---|---|---|
| `AF_OBLIGATORY` | 1 | A value must be entered. |
| `AF_UNIQUE` | 2 | The value must be unique. |
| `AF_PRIMARY` | 5 | Part of the primary key; implies `AF_OBLIGATORY`. |
| `AF_AUTO_INCREMENT` / `AF_AUTOINCREMENT` | 8 | Auto-increment field. |
| `AF_AUTOKEY` | 1021 | Hidden auto-increment primary key: `AF_PRIMARY \| AF_HIDE \| AF_AUTO_INCREMENT`. |

### Visibility (hide flags)

| Constant | Value | Effect |
|---|---|---|
| `AF_HIDE_LIST` | 16 | Not shown in record lists. |
| `AF_HIDE_ADD` | 32 | Not shown on add pages. |
| `AF_HIDE_EDIT` | 64 | Not shown on edit pages. |
| `AF_HIDE_SELECT` | 128 | Not shown on select pages. |
| `AF_HIDE_VIEW` | 256 | Not shown on view pages. |
| `AF_HIDE_SEARCH` | 512 | Not searchable in extended search. |
| `AF_HIDE` | 1008 | Hidden everywhere (all `AF_HIDE_*` flags). |
| `AF_FORCE_LOAD` | 1024 | Always loaded, even when not displayed anywhere. |

### Read-only

| Constant | Value | Effect |
|---|---|---|
| `AF_READONLY_ADD` | 2048 | Read-only on add pages. |
| `AF_READONLY_EDIT` | 4096 | Read-only on edit pages. |
| `AF_READONLY` | 6144 | Always read-only (`AF_READONLY_ADD \| AF_READONLY_EDIT`). |

### Display

| Constant | Value | Effect |
|---|---|---|
| `AF_NO_LABEL` / `AF_NOLABEL` | 8192 | No label in forms. |
| `AF_BLANK_LABEL` / `AF_BLANKLABEL` | 16384 | Empty label in forms. |
| `AF_NO_SORT` / `AF_NOSORT` | 32768 | Cannot be sorted in record lists. |
| `AF_SEARCHABLE` | 65536 | Searchable from the record list (quick search row). |
| `AF_TOTAL` | 131072 | Adds a total row for this column in lists. |
| `AF_POPUP` | 262144 | Use a pop-up window, where supported. |

### Miscellaneous

| Constant | Value | Effect |
|---|---|---|
| `AF_CASCADE_DELETE` | 524288 | The attribute's `delete()` is called when the owning record is deleted. |
| `AF_LARGE` | 1048576 | Relations: show a link to a select page instead of a list of all records. |
| `AF_NO_FILTER` | 2097152 | Relations: ignore destination filters when selecting records. |
| `AF_PARENT` | 4194304 | Parent field of a parent/child tree. Required by `TreeNode`. |
| `AF_NO_QUOTES` | 8388608 | Do not quote the value in SQL. |

## Storage, load and disabled types

These are not flags. They are values for `setStorageType()`, `setLoadType()` and `setDisabledModes()` on `Attributes\Attribute`. See [Attributes](../guide/attributes.md).

| Constant | Value | Meaning |
|---|---|---|
| `NOSTORE` / `NOLOAD` | 0 | Do not store or load the attribute. |
| `PRESTORE` / `PRELOAD` | 1 | Store before the normal attributes, or load before `select()`. |
| `POSTSTORE` / `POSTLOAD` | 2 | Store after the normal attributes, or load after `select()`. |
| `ADDTOQUERY` | 4 | Store or load through the node's main query (`addToQuery()`). |
| `DISABLED_VIEW` | 1 | Disabled in view mode. |
| `DISABLED_EDIT` | 2 | Disabled in edit mode. |
| `DISABLED_ALL` | 3 | Disabled in both. |

## Class-specific attribute flags

### `BoolAttribute`

| Constant | Value | Effect |
|---|---|---|
| `AF_BOOL_OBLIGATORY` | 33554432 | Makes the checkbox obligatory. The normal `AF_OBLIGATORY` is always removed from bool attributes. |
| `AF_BOOL_INLINE_LABEL` | 67108864 | Shows an extra label next to the checkbox (`<attribute>_label` language key). |
| `AF_BOOL_DISPLAY_CHECKBOX` | 134217728 | In view and list mode, shows a checkbox instead of "yes"/"no". |

### `DateAttribute`

`DateTimeAttribute` does not extend `DateAttribute`. It passes its flags to an internal `DateAttribute`, so these flags also apply to the date part of a `DateTimeAttribute`.

| Constant | Value | Effect |
|---|---|---|
| `AF_DATE_STRING` | 33554432 | The date is entered as an English date string (`strtotime`). |
| `AF_DATE_EMPTYFIELD` | 67108864 | The fields have an empty option. |
| `AF_DATE_NO_CALENDAR` | 134217728 | No popup calendar. |
| `AF_DATE_DISPLAY_DAY` | 268435456 | Shows the day of the week in display mode. |
| `AF_DATE_EDIT_NO_DAY` | 536870912 | Hides the day of the week in edit mode. |
| `AF_DATE_DEFAULT_EMPTY` | 1073741824 | Obligatory dates start empty instead of today. |

### `TimeAttribute`

| Constant | Value | Effect |
|---|---|---|
| `AF_TIME_SECONDS` | 33554432 | Also shows seconds. |
| `AF_TIME_DEFAULT_EMPTY` | 1073741824 | Always uses the empty value on new records. |

### `DurationAttribute`

| Constant | Value | Effect |
|---|---|---|
| `AF_DURATION_STRING` | 33554432 | The duration is entered as an English string (`strtotime`). |

### `FileAttribute` (and `MultipleFileAttribute`)

| Constant | Value | Effect |
|---|---|---|
| `AF_FILE_NO_UPLOAD` | 33554432 | Disables uploading. |
| `AF_FILE_NO_SELECT` | 67108864 | Disables selecting existing files. |
| `AF_FILE_NO_CHECKBOX_DELETE` / `AF_FILE_NO_DELETE` | 134217728 | Hides the delete checkbox. |
| `AF_FILE_NO_AUTOPREVIEW` | 268435456 | Does not detect the file type; shows only the file name. |
| `AF_FILE_PHYSICAL_DELETE` | 536870912 | Removes the file from disk on delete. |
| `AF_FILE_POPUP` | 262144 | Alias of `AF_POPUP`: preview in a popup instead of inline. |

### `PasswordAttribute`

| Constant | Value | Effect |
|---|---|---|
| `AF_PASSWORD_NO_VALIDATE` | 33554432 | Disables the current-password check when editing. |
| `AF_PASSWORD_NO_ENCODE` | 67108864 | Stores the value as entered instead of hashing it with `password_hash()`. |

### `IpAttribute`

| Constant | Value | Effect |
|---|---|---|
| `AF_IP_ALLOW_WILDCARDS` | 33554432 | Allows wildcards. Ignored when `AF_IP_STORENUMERIC` is set. |
| `AF_IP_STORENUMERIC` | 67108864 | Stores the address as a long integer. |
| `AF_IP_SINGLEFIELD` | 134217728 | Edits the address in one field instead of four. |

### `UrlAttribute`

| Constant | Value | Effect |
|---|---|---|
| `AF_URL_ALLOWWRAP` | 33554432 | Adds spaces after `/`, `?` and `&` so long URLs wrap. |
| `AF_URL_STRIPHTTP` | 67108864 | Hides `http://` in display; the link stays intact. |

### `ListAttribute`

These flags are inherited by `MultiSelectAttribute`, `MultiSelectListAttribute` and `RadioAttribute`.

| Constant | Value | Effect |
|---|---|---|
| `AF_NO_TRANSLATION` | 33554432 | Do not translate the option labels. |
| `AF_LIST_NO_OBLIGATORY_NULL_ITEM` | 67108864 | No empty option on obligatory lists, even when `list_obligatory_null_item` is set. |
| `AF_LIST_NO_NULL_ITEM` | 134217728 | Never add an empty option. |
| `AF_LIST_OBLIGATORY_NULL_ITEM` | 268435456 | Add an empty option to obligatory lists. |

### `RadioAttribute`

| Constant | Value | Effect |
|---|---|---|
| `AF_DISPLAY_VERT` | 67108864 | Display the radio buttons vertically. This bit is the same as `AF_LIST_NO_OBLIGATORY_NULL_ITEM`. |

### `MultiSelectAttribute`

`AF_NO_TOGGLELINKS` below uses the same bit as the inherited `AF_LIST_NO_OBLIGATORY_NULL_ITEM`.

| Constant | Value | Effect |
|---|---|---|
| `AF_NO_TOGGLELINKS` | 67108864 | Hides the select-all/none links (only shown with more than 4 values). |
| `AF_CHECK_ALL` | 134217728 | All options are checked by default. |
| `AF_LINKS_BOTTOM` | 268435456 | Puts the toggle links below the options. |

### `MultiSelectListAttribute`

| Constant | Value | Effect |
|---|---|---|
| `AF_NO_TOGGLELINKS` | 536870912 | Hides the toggle links. This is a different bit from `MultiSelectAttribute::AF_NO_TOGGLELINKS`. |

### Other attributes

| Class | Constant | Value | Effect |
|---|---|---|---|
| `WeekdayAttribute` | `AF_WEEKDAY_SMALL_EDIT` | 33554432 | Lays out the weekday checkboxes on one line. |
| `DummyAttribute` | `AF_DUMMY_SHOW_LABEL` | 33554432 | Shows the label of the dummy attribute. |
| `LiveTextPreviewAttribute` | `AF_LIVETEXT_SHOWLABEL` | 33554432 | Alias of `AF_DUMMY_SHOW_LABEL`. |
| `LiveTextPreviewAttribute` | `AF_LIVETEXT_NL2BR` | 67108864 | Converts newlines to `<br />` in the preview. |
| `TabbedPane` | `AF_TABBEDPANE_NO_AUTO_HIDE_LABEL` | 33554432 | Does not automatically hide the labels of the attributes inside the pane. |

## Relation flags

### `ManyToOneRelation`

| Constant | Value | Effect |
|---|---|---|
| `AF_RELATION_AUTOLINK` / `AF_MANYTOONE_AUTOLINK` | 33554432 | Adds edit/view links next to the selected item. |
| `AF_RELATION_NO_NULL_ITEM` / `AF_MANYTOONE_NO_NULL_ITEM` | 67108864 | Never adds an empty option. |
| `AF_RELATION_AUTOCOMPLETE` / `AF_MANYTOONE_AUTOCOMPLETE` | 134217728 | Uses autocomplete instead of a dropdown or select page. |
| `AF_MANYTOONE_LAZY` | 268435456 | Loads the related record lazily instead of joining it. |
| `AF_MANYTOONE_OBLIGATORY_NULL_ITEM` | 536870912 | Adds an empty option to obligatory relations. |

### `OneToManyRelation`

| Constant | Value | Effect |
|---|---|---|
| `AF_RESTRICTED_DELETE` | 33554432 | The master record can be deleted only when it has no child records. |
| `AF_ONETOMANY_SHOW_ADD` | 268435456 | Shows the relation in add mode. Children are kept in the session until the master is saved, which only works in simple cases. |

### `OneToOneRelation`

| Constant | Value | Effect |
|---|---|---|
| `AF_ONETOONE_ADD` | 33554432 | Overrides the default no-add behaviour. |
| `AF_ONETOONE_ERROR` | 67108864 | Enables error notifications and triggers. |
| `AF_ONETOONE_INTEGRATE` | 134217728 | Shows the related fields as if they belonged to this node, with no divider or section heading. |
| `AF_ONETOONE_LAZY` | 268435456 | Lazy loading instead of a query join. |
| `AF_ONETOONE_RESPECT_TABS` | 536870912 | In integrate mode, uses this attribute's tabs instead of the destination node's tabs. |

### `ManyToManyRelation` family

| Class | Constant | Value | Effect |
|---|---|---|---|
| `ManyToManyRelation` | `AF_MANYTOMANY_DETAILVIEW` | 536870912 | Links related records to their view page. |
| `ManyBoolRelation` | `AF_MANYBOOL_AUTOLINK` | 33554432 | Adds a link to add records to the related table. |
| `ManyBoolRelation` | `AF_MANYBOOL_NO_TOOLBAR` | 67108864 | Hides the select all / none / inverse links. |
| `ManyToManySelectRelation` | `AF_MANYTOMANYSELECT_DETAILEDIT` | 33554432 | Links selected records to their edit page. |
| `ManyToManySelectRelation` | `AF_MANYTOMANYSELECT_DETAILADD` | 67108864 | Adds a link to create a new related record. |
| `ManyToManySelectRelation` | `AF_MANYTOMANYSELECT_NO_AUTOCOMPLETE` | 134217728 | Uses a plain select instead of autocomplete. |
