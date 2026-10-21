# Attribute reference

[← Documentation index](../README.md)

This page catalogues every attribute class in [`src/Attributes`](../../src/Attributes) and [`src/Attributes/NestedAttributes`](../../src/Attributes/NestedAttributes). All classes live in the namespace `Sintattica\Atk\Attributes`. For general concepts (flags, lifecycle, callbacks), read [Attributes](attributes.md) first. Relation attributes are covered in [Relations](relations.md).

Notes on the tables:

- **Constructor** lists the parameters after `$name, $flags` when the class adds any. "inherited" means the class has no constructor of its own.
- **Notable flags** lists class-specific `AF_*` constants. Generic flags are in the [Attribute flags reference](../reference/attribute-flags.md).

## Base and text

| Class | Constructor | Purpose | Notable flags / API |
|---|---|---|---|
| `Attribute` | `($name, $flags = 0)` | Plain single-line text field. Base class of everything. | All generic `AF_*` flags. |
| `TextAttribute` | `($name, $flags = 0, array $options = [])` | Multi-line text (textarea). | `setDisplayMode()` for long values in lists. |
| `HtmlAttribute` | `($name, $flags = 0, $nl2br = false)` | Text rendered as raw HTML in display mode. | — |
| `CkAttribute` | `($name, $flags = 0, $options = [])` | Rich-text editor (CKEditor 5). | `ENTER_MODE_P`, `ENTER_MODE_BR`, `ENTER_MODE_DIV`; global defaults in config `ck_options`. |
| `EmailAttribute` | `($name, $flags = 0, $search = false)` | E-mail address with format validation. | — |
| `UrlAttribute` | `($name, $flags = 0)` | URL field, rendered as a link. | `AF_URL_ALLOWWRAP`, `AF_URL_STRIPHTTP`; constants `ABSOLUTE`, `RELATIVE`, `ANCHOR` for accepted URL kinds. |
| `PasswordAttribute` | `($name, $flags = 0, $generate = false, $restrictions = [])` | Password field. Hashes with `password_hash()`. Changing a password requires the old one. | `AF_PASSWORD_NO_VALIDATE`, `AF_PASSWORD_NO_ENCODE`. Complexity rules come from `$restrictions` or the `password_min*` config keys. |
| `IpAttribute` | `($name, $flags = 0)` | IPv4 address. | `AF_IP_ALLOW_WILDCARDS`, `AF_IP_STORENUMERIC`, `AF_IP_SINGLEFIELD`. |
| `JsonAttribute` | inherited from `TextAttribute` | JSON column, pretty-printed in display. Also used internally to store [nested attributes](#nested-json-attributes). | `enableRawMode()`, `disableRawMode()`, `setJsonIndentChar()`, `setJsonNewlineChar()`. |
| `FileWriterAttribute` | `($name, $flags, $filename)` | Text whose content is read from and written to a file instead of the database. | — |
| `FormatAttribute` | `($name, $flags, $format)` | Edits a string that follows a fixed format mask. | — |
| `ParserAttribute` | `($name, $flags, $text)` | Read-only text built from a template with `[field]` placeholders (`Utils\StringParser`). | Not stored. |
| `LiveTextPreviewAttribute` | `($name, $flags, $masterAttribute)` | Live preview of another text attribute while it is edited. | `AF_LIVETEXT_SHOWLABEL`, `AF_LIVETEXT_NL2BR`. |

## Numbers

| Class | Constructor | Purpose | Notable flags / API |
|---|---|---|---|
| `NumberAttribute` | `($name, $flags = 0, $decimals = null)` | Integer or decimal number. Range search (`between`, `greaterthan`, …). | Thousands separator controlled by config `numberattribute_use_thousands_separator`. |
| `CurrencyAttribute` | `($name, $flags = 0, $currencysymbol = '', $decimals = 2, $decimalseparator = '', $thousandsseparator = '')` | Money value with a currency symbol. | — |
| `CalculatorAttribute` | `($name, $flags, $calculation)` | Column computed from an expression over other attributes, e.g. `"[price]*[quantity]"`. | Combine with `AF_TOTAL` for list totals. |
| `DurationAttribute` | `($name, $flags = 0, $resolution = '1m', $maxtime = '10h')` | Length of time, stored as a number of minutes. | `AF_DURATION_STRING`; constants `DURATIONFORMAT_TIME`, `DURATIONFORMAT_DECIMAL`. |
| `WeekdayAttribute` | `($name, $flags = 0, $extra = [])` | Bitmask of weekdays (checkboxes). | `AF_WEEKDAY_SMALL_EDIT`; constants `WD_MONDAY` … `WD_SUNDAY`. |
| `VersionAttribute` | `()` (no parameters) | Update counter of the record. The docblock recommends incrementing it with a database trigger. | — |
| `RowCounterAttribute` | `($name, $flags = 0)` | Sequential row number in lists. | Not stored. |

## Dates and times

| Class | Constructor | Purpose | Notable flags / API |
|---|---|---|---|
| `DateAttribute` | `($name, $flags = 0, $format_edit = '', $format_view = '', $min = 0, $max = 0)` | Date picker for `DATE` columns. Search is a `between` range. | `AF_DATE_STRING`, `AF_DATE_EMPTYFIELD`, `AF_DATE_NO_CALENDAR`, `AF_DATE_DISPLAY_DAY`, `AF_DATE_EDIT_NO_DAY`, `AF_DATE_DEFAULT_EMPTY`. |
| `DateTimeAttribute` | `($name, $flags = 0)` | Date and time for `DATETIME` columns. Internally combines a date and a time attribute. | Accepts the `DateAttribute::AF_DATE_*` flags. |
| `TimeAttribute` | `($name, $flags = 0, $beginTime = 0, $endTime = 23, $steps = ['00','15','30','45'])` | Time chosen from predefined hours and minute steps. | `AF_TIME_SECONDS`, `AF_TIME_DEFAULT_EMPTY`. |
| `TimeZoneAttribute` | `($name, $flags = 0)` | List of time zones. | — |
| `CreateStampAttribute` | inherited from `UpdateStampAttribute` | Stores the date and time the record was created. | Readonly and hidden in add (inherited). |
| `UpdateStampAttribute` | `($name, $flags = 0)` | Stores the date and time of the last modification. | Always adds `AF_READONLY \| AF_HIDE_ADD`. |

## Choices and lists

| Class | Constructor | Purpose | Notable flags / API |
|---|---|---|---|
| `ListAttribute` | `($name, $flags, $optionArray, $valueArray = null)` | Dropdown with predefined options. `$valueArray` defaults to `$optionArray`. Options are translated unless `AF_NO_TRANSLATION` is set. | `AF_NO_TRANSLATION`, `AF_LIST_NO_OBLIGATORY_NULL_ITEM`, `AF_LIST_NO_NULL_ITEM`, `AF_LIST_OBLIGATORY_NULL_ITEM` (global default: config `list_obligatory_null_item`); `setMultipleSearch()`. |
| `MultiListAttribute` | `($name, $flags, $optionArray, $valueArray = null)` | Multiple choice from a list. The docblock says to use it instead of `MultiSelectAttribute` and `MultiSelectListAttribute`. | Values are stored separated by `FIELD_SEPARATOR_DEFAULT` (`\|`); display separator constants `DISPLAY_SEPARATOR_TAG`, `DISPLAY_SEPARATOR_COMMA`. |
| `JsonMultiListAttribute` | `($name, $flags, $optionArray, $valueArray = null)` | Like `MultiListAttribute`, but stores the selection as a JSON array. | — |
| `MultiSelectAttribute` | `($name, $flags, $optionArray, $valueArray = null)` | **Deprecated.** Checkboxes; values stored separated by `\|`. | `AF_NO_TOGGLELINKS`, `AF_CHECK_ALL`, `AF_LINKS_BOTTOM`. |
| `MultiSelectListAttribute` | `($name, $flags, $optionArray, $valueArray = null)` | Multi-select list box. | `AF_NO_TOGGLELINKS`. |
| `FlagAttribute` | `($name, $flags, $optionArray, $valueArray = null)` | Edits a bitmask; values must be powers of two. | Inherits from `MultiSelectAttribute`. |
| `RadioAttribute` | `($name, $flags, $optionArray, $valueArray = null)` | Radio buttons instead of a dropdown. | `AF_DISPLAY_VERT` (vertical layout). |
| `RadioDetailsAttribute` | `($name, $flags, $options, $details)` | Radio buttons that reveal other attributes when a given option is selected. | — |
| `BoolAttribute` | `($name, $flags = 0)` | Checkbox (true/false). | `AF_BOOL_OBLIGATORY`, `AF_BOOL_INLINE_LABEL`, `AF_BOOL_DISPLAY_CHECKBOX`. |
| `SwitchAttribute` | `($name, $flags = 0, $switchOptions = [])` | Boolean rendered as a Bootstrap switch. | — |
| `ListBoolAttribute` | `($name, $flags = 0)` | Nullable yes/no/empty dropdown. Use a `CHAR` column. | — |
| `CountryAttribute` | `($name, $flags = 0, $switch = 'world', $optionArray = [], $valueArray = null, $defaultToCurrent = true)` | ISO country list. | — |
| `StateAttribute` | `($name, $flags = 0, $switch = 'usa', $defaulttocurrent = true)` | US states list. | — |
| `ColorPickerAttribute` | `($name, $flags = 0, $colorPickerOptions = [])` | Colour picker (bootstrap-colorpicker). | — |
| `UIStateColorListAttribute` | `($name, $flags = 0)` | Dropdown of the UI state colours defined in `AdminLte\UIStateColors`. | — |

## Files

| Class | Constructor | Purpose | Notable flags / API |
|---|---|---|---|
| `FileAttribute` | `($name, $flags = 0, $dir = null)` | Upload, select or remove a file in a directory. `$dir` is a path or `[path, url]`. When it is empty, `['./' . default_upload_dir, '/' . default_upload_url]` from config is used. The constructor always adds `AF_CASCADE_DELETE`, `AF_FILE_NO_SELECT` and `AF_FILE_PHYSICAL_DELETE`. | `AF_FILE_NO_UPLOAD`, `AF_FILE_NO_SELECT`, `AF_FILE_NO_CHECKBOX_DELETE` (alias `AF_FILE_NO_DELETE`), `AF_FILE_NO_AUTOPREVIEW`, `AF_FILE_PHYSICAL_DELETE`, `AF_FILE_POPUP`. PDF/JPEG/PNG can be shown inline. |
| `MultipleFileAttribute` | `($name, $flags, $dir)` | Several files stored in a single column. | Inherits the `FileAttribute` flags. |

## Computed and SQL expressions

None of these classes are stored. They are loaded through SQL.

| Class | Constructor | Purpose |
|---|---|---|
| `ExpressionAttribute` | `($name, $flags, $expression, $searchType = '')` | Any SQL expression or subquery as a read-only column. `$searchType` (`'string'` by default, or `'number'`, `'date'`) selects how it is searched. |
| `ExpressionBoolAttribute` | `($name, $flags, $expression)` | Expression shown as a yes/no value. |
| `ExpressionCountAttribute` | `($name, $flags, $table, $condition)` | `COUNT(*)` of rows in `$table` matching `$condition`. |
| `ExpressionDateAttribute` | `($name, $flags, $expression, $dateAttribute = null)` | Expression rendered and searched like a date. |
| `ExpressionDateTimeAttribute` | `($name, $flags, $expression, $dateTimeAttribute = null)` | Expression rendered like a date-time. |
| `ExpressionFileAttribute` | `($name, $flags, $expression, $fileAttribute = null)` | Expression rendered like a file (preview or thumbnail via `showOnlyPreview()`, `useThumbnail()`). |
| `ExpressionListAttribute` | `($name, $flags, $expression, $listAttribute, $searchType = '')` | Expression rendered and searched through a `ListAttribute`. The list attribute must have the same name. |
| `AggregatedColumn` | `($name, $flags, $template, $searchFields = [])` | One list column that shows several attributes through a template, and searches in `$searchFields`. |
| `FuzzySearchAttribute` | `($name, $flags, $searchnode, $callback, $mode = 'all')` | Keyword field that searches another node and passes the matches to a callback. |

## Layout and UI

These classes are not stored.

| Class | Constructor | Purpose | Notable flags / API |
|---|---|---|---|
| `DummyAttribute` | `($name, $flags = 0, $text = '')` | Static text or HTML between other fields. | `AF_DUMMY_SHOW_LABEL`. |
| `HiddenAttribute` | `($name, $flags = 0, $text = '')` | Hidden form field that posts `$text`. | — |
| `SepAttribute` | `($name, $flags = 0, $class = '')` | Horizontal separator. | — |
| `TitleAttribute` | `($name, $flags = 0, $text = '', $sep = '')` | Section title inside a form. | — |
| `FieldSet` | `($name, $flags, $template)` | Groups several attributes into one form row. The template uses `[attribute]` placeholders, e.g. `'[zip] [city]'`. | — |
| `TabbedPane` | `($name, $flags = 0, $tabs = [])` | Nested tabs inside a form. `$tabs` maps tab names to attribute names. | `AF_TABBEDPANE_NO_AUTO_HIDE_LABEL`. |
| `ButtonAttribute` | `($name, $flags = 0)` | Base class for buttons in forms. | `setUIState()`, `removeUIState()`. |
| `SubmitButtonAttribute` | inherited | Button that submits the form and runs a server-side callback. | `onClick(callable $cb)` receives the record; `setDoUpdate()`. |
| `ActionButtonAttribute` | `($name, $flags = 0)` | Button that opens another node or action. | `setNode()`, `setAction()`, `setTarget()`, `addParam()`, `addParamFieldName()`, `setSessionStatus()`, `setSaveForm()`. |
| `ModalActionAttribute` | `($name, $flags = 0, $text = '', ?string $uiState = UIStateColors::COLOR_DEFAULT)` | Button that opens a confirmation modal, then posts an action. | `setModalTitle()`, `setBody()`, `setOkLabel()`, `setCancelLabel()`, `addParam()`, `addHiddenParam()`, `setAtkActionName()`. |
| `StateColorAttribute` | `($name, $flags = 0)` | Coloured dot or badge, optionally with text. Every node adds one internally for row colours. | `setColor()`, `setColorCondition(callable)`, `setTextContent()`, `setBordered()`, `setColoredText()`, `setShape()` (`SHAPE_ROUND`, `SHAPE_SQUARE`, `SHAPE_FLUID`), `setSize()` (`SIZE_SM`, `SIZE_MD`, `SIZE_LG`). |

```php
$button = new SubmitButtonAttribute('set_today', ButtonAttribute::AF_DUMMY_SHOW_LABEL);
$button->setText('Set current date');
$button->onClick(function ($record) {
    return $this->getDb()->query('UPDATE demo SET date_a = NOW() WHERE id = ' . (int)$record['id']);
});
$this->add($button);
```

## Security and users

| Class | Constructor | Purpose |
|---|---|---|
| `ProfileAttribute` | `($name, $flags = 0, $parentAttrName = '')` | Editor for the privileges of a group or profile: a checkbox per node action. Use it in your group node. See [Security](security.md). |
| `CreatedByAttribute` | inherited from `UpdatedByAttribute` | Stores the user who created the record. |
| `UpdatedByAttribute` | `($name, $flags = 0)` | Stores the user who last modified the record. It is a `ManyToOneRelation` to the node set in config `auth_usernode`, which must be defined, and always adds `AF_READONLY \| AF_HIDE_ADD \| AF_LARGE`. Constant `FORCE_ADMIN`. |

## Nested JSON attributes

Namespace `Sintattica\Atk\Attributes\NestedAttributes`. These classes store their value as a key inside a JSON column. The extra constructor argument `$nestedAttributeField` names that column. How they work is explained in [Attributes](attributes.md#nested-json-attributes).

| Class | Constructor | Based on |
|---|---|---|
| `NestedAttribute` | `($name, $flags, string $nestedAttributeField)` | `Attribute` |
| `NestedBoolAttribute` | `($name, $flags, string $nestedAttributeField)` | `BoolAttribute` |
| `NestedDateAttribute` | `($name, $flags, string $nestedAttributeField, $format_edit = '', $format_view = '', $min = 0, $max = 0)` | `DateAttribute` |
| `NestedDateTimeAttribute` | `($name, $flags, string $nestedAttributeField, $format_edit = '', $format_view = '', $min = 0, $max = 0)` | `DateTimeAttribute` |
| `NestedListAttribute` | `($name, $flags, string $nestedAttributeField, $optionArray, $valueArray = null)` | `ListAttribute` |
| `NestedMultiListAttribute` | `($name, $flags, string $nestedAttributeField, $optionArray, $valueArray = null)` | `MultiListAttribute` |
| `NestedNumberAttribute` | `($name, $flags, string $nestedAttributeField, $decimals = null)` | `NumberAttribute` |

Supporting types:

- `NestedAttributeInterface` declares `getSearchCondition()`.
- The trait `JsonSearchable` provides JSON-aware `getOrderByStatement()`, `getSearchCondition()` and `buildJSONExtractValue()`.

## See also
- [Attributes](attributes.md)
- [Relations](relations.md)
- [Attribute flags reference](../reference/attribute-flags.md)
