# Tutorial: a conference app

[← Documentation index](../README.md)

This tutorial builds a small application to manage **speakers**, **conferences** and **attendees**. It covers the everyday ATK workflow: a module, a few nodes, attributes, relations, the menu and translations.

It assumes you have read [Installation](installation.md) and [Configuration basics](configuration-basics.md). The code targets ATK 11.x on PHP 8.3 with MySQL.

## What ATK is

ATK is a PHP framework for building business applications. It shines when editing database tables is the main job of the application. You describe a table as a **node** made of **attributes**, and ATK generates the paginated admin list together with the search, add, edit, view and delete pages. You write almost no HTML: ATK builds the pages for you, while hooks and overrides let you customise everything.

## A little history

ATK stands for "Achievo ToolKit". Achievo was a project management application by the Dutch company iBuildings, and ATK was the framework behind it. ATK was later released as a stand-alone toolkit.

- iBuildings maintained ATK until version 6.x.
- Sintattica (sintattica.it) then took it over, with the blessing of ATK's founder Ivo Jansch, and continued it as ATK 8.
- ATK 9 was a deep rewrite: Composer, PSR-4, modern PHP and Bootstrap.
- The current line (10 and 11) adds the AdminLTE 3 interface, Smarty 5 and PHP 8.3.

ATK 9 and later are philosophically close to the old versions, but they are not backward compatible with them.

## 1. Create the project

Follow [Installation](installation.md) to create the project `conference/`. Use the `.env` and `config/atk.php` from [Configuration basics](configuration-basics.md). By the end you should have:

```
conference/
├── .env
├── composer.json             # with "App\\": "src/" autoloading
├── config/atk.php
├── src/
├── var/atktmp/
├── vendor/
└── web/
    ├── index.php
    └── bundles/atk -> ../../vendor/sintattica/atk/src/Resources/public
```

That configuration uses `authentication => 'config'` with no users defined. The only account that can log in is the built-in **administrator** user, with the password whose hash is in `ADMIN_PASSWORD`. Generate a hash with:

```bash
php vendor/sintattica/atk/src/Utils/generatehash.php demo
```

## 2. Create the database

```sql
CREATE DATABASE conference CHARACTER SET utf8mb4;
CREATE USER 'conference'@'localhost' IDENTIFIED BY 'conference';
GRANT ALL ON conference.* TO 'conference'@'localhost';

USE conference;

CREATE TABLE speaker (
  id INT AUTO_INCREMENT PRIMARY KEY,
  name VARCHAR(100) NOT NULL,
  email VARCHAR(150),
  bio TEXT
);

CREATE TABLE conference (
  id INT AUTO_INCREMENT PRIMARY KEY,
  title VARCHAR(200) NOT NULL,
  starts_at DATETIME NOT NULL,
  room VARCHAR(50),
  speaker_id INT NOT NULL,
  FOREIGN KEY (speaker_id) REFERENCES speaker(id)
);

CREATE TABLE attendee (
  id INT AUTO_INCREMENT PRIMARY KEY,
  name VARCHAR(100) NOT NULL,
  email VARCHAR(150) NOT NULL,
  conference_id INT NOT NULL,
  FOREIGN KEY (conference_id) REFERENCES conference(id) ON DELETE CASCADE
);
```

The primary keys use MySQL `AUTO_INCREMENT`. `MySqliQuery::addSequenceField()` detects this and uses the generated insert id. Without `AUTO_INCREMENT`, ATK would fall back to its own sequence table.

## 3. Our first module

Every node belongs to a **module**. A module is a `Core\Module` subclass listed in the `modules` config key. It has two methods:

- `register()` runs on every request, before authentication. It tells ATK which nodes exist.
- `boot()` runs only after the user has logged in. It is the place to add menu items.

`src/Modules/Conference/Module.php`:

```php
<?php

namespace App\Modules\Conference;

class Module extends \Sintattica\Atk\Core\Module
{
    public static string $module = 'conference';

    public function register()
    {
        $actions = ['admin', 'add', 'edit', 'view', 'delete'];

        $this->registerNode('speaker', Speaker::class, $actions);
        $this->registerNode('conference', Conference::class, $actions);
        $this->registerNode('attendee', Attendee::class, $actions);
    }

    public function boot()
    {
        $this->addNodeToMenu('speakers', 'speaker', 'admin');
        $this->addNodeToMenu('conferences', 'conference', 'admin');
        $this->addNodeToMenu('attendees', 'attendee', 'admin');
    }
}
```

Node URIs are `<module>.<node>`, for example `conference.speaker`. `registerNode()` adds the module prefix for you. Listing the actions also registers the node with the security system (see [Security](../guide/security.md)).

The config file references `App\Menu`. Create a minimal menu. Its `appendMenuItems()` can stay empty because the module adds its own items:

`src/Menu.php`:

```php
<?php

namespace App;

use Sintattica\Atk\Core\Menu\MenuBase;

class Menu extends MenuBase
{
    public function appendMenuItems()
    {
    }
}
```

## 4. The first node: speakers

A node maps one table. Attributes are added in the constructor. The attribute name is the column name.

`src/Modules/Conference/Speaker.php`:

```php
<?php

namespace App\Modules\Conference;

use Sintattica\Atk\Attributes\Attribute as A;
use Sintattica\Atk\Attributes\EmailAttribute;
use Sintattica\Atk\Attributes\NumberAttribute;
use Sintattica\Atk\Attributes\TextAttribute;
use Sintattica\Atk\Core\Node;

class Speaker extends Node
{
    public function __construct($nodeUri)
    {
        parent::__construct($nodeUri, Node::NF_ADD_LINK);

        $this->setTable('speaker');
        $this->setOrder('speaker.name');
        $this->setDescriptorTemplate('[name]');

        $this->add(new NumberAttribute('id', A::AF_AUTOKEY));
        $this->add(new A('name', A::AF_OBLIGATORY | A::AF_SEARCHABLE));
        $this->add(new EmailAttribute('email', A::AF_SEARCHABLE));
        $this->add(new TextAttribute('bio', A::AF_HIDE_LIST));
    }
}
```

What each line does:

- `NF_ADD_LINK` shows an "add" link on the admin page instead of an embedded add form. All node flags are in the [Node flags reference](../reference/node-flags.md).
- `setDescriptorTemplate('[name]')` defines how a speaker is shown when other nodes refer to it, for example in dropdowns.
- `AF_AUTOKEY` is a hidden, auto-increment primary key.
- `AF_SEARCHABLE` adds the column to the quick-search row of the list.
- `AF_HIDE_LIST` keeps the long bio out of the list.

Log in as `administrator` and open **Speakers** in the sidebar. You have a complete CRUD interface for the `speaker` table, with search, sorting, pagination and validation.

## 5. Adding a relation

A conference has one speaker: a **many-to-one** relation. A conference also has many attendees: a **one-to-many** relation.

`src/Modules/Conference/Conference.php`:

```php
<?php

namespace App\Modules\Conference;

use Sintattica\Atk\Attributes\Attribute as A;
use Sintattica\Atk\Attributes\DateTimeAttribute;
use Sintattica\Atk\Attributes\ListAttribute;
use Sintattica\Atk\Attributes\NumberAttribute;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Relations\ManyToOneRelation;
use Sintattica\Atk\Relations\OneToManyRelation;

class Conference extends Node
{
    public function __construct($nodeUri)
    {
        parent::__construct($nodeUri, Node::NF_ADD_LINK | Node::NF_EXPORT);

        $this->setTable('conference');
        $this->setOrder('conference.starts_at DESC');
        $this->setDescriptorTemplate('[title]');

        $this->add(new NumberAttribute('id', A::AF_AUTOKEY));
        $this->add(new A('title', A::AF_OBLIGATORY | A::AF_SEARCHABLE));
        $this->add(new DateTimeAttribute('starts_at', A::AF_OBLIGATORY));
        $this->add(new ListAttribute('room', A::AF_SEARCHABLE, ['Room A', 'Room B', 'Main hall']));

        // conference.speaker_id -> speaker.id
        $this->add(new ManyToOneRelation('speaker_id', A::AF_OBLIGATORY | A::AF_SEARCHABLE, 'conference.speaker'));

        // attendee.conference_id -> conference.id, shown on its own tab
        $this->add(new OneToManyRelation('attendees', A::AF_HIDE_LIST, 'conference.attendee', 'conference_id'), 'attendees');
    }
}
```

`src/Modules/Conference/Attendee.php`:

```php
<?php

namespace App\Modules\Conference;

use Sintattica\Atk\Attributes\Attribute as A;
use Sintattica\Atk\Attributes\EmailAttribute;
use Sintattica\Atk\Attributes\NumberAttribute;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Relations\ManyToOneRelation;

class Attendee extends Node
{
    public function __construct($nodeUri)
    {
        parent::__construct($nodeUri, Node::NF_ADD_LINK);

        $this->setTable('attendee');
        $this->setDescriptorTemplate('[name] ([email])');

        $this->add(new NumberAttribute('id', A::AF_AUTOKEY));
        $this->add(new A('name', A::AF_OBLIGATORY | A::AF_SEARCHABLE));
        $this->add(new EmailAttribute('email', A::AF_OBLIGATORY | A::AF_SEARCHABLE));
        $this->add(new ManyToOneRelation('conference_id', A::AF_OBLIGATORY | A::AF_SEARCHABLE, 'conference.conference'));
    }
}
```

What you get:

- **The many-to-one relation.** `ManyToOneRelation('speaker_id', ..., 'conference.speaker')` stores the speaker's primary key in `speaker_id`. It renders as a Select2 dropdown, labelled with the speaker's descriptor. In lists it shows the speaker's name.
- **The one-to-many relation.** `OneToManyRelation('attendees', ..., 'conference.attendee', 'conference_id')` is not a column. The fourth argument names the many-to-one attribute in the destination node that points back to the conference. When you edit a conference, an **Attendees** tab shows its attendees as an inline grid, with add, edit and delete. The second argument of `add()` (`'attendees'`) puts the attribute on that tab.
- **Multi-record actions.** `Node::NF_MRA` would normally add checkboxes to the attendee list, so you could delete several attendees at once. Do not enable it yet: on PHP 8 every list of an `NF_MRA` node currently fails with a `TypeError`, including the inline grid of the one-to-many relation. See [Known issues](../internals/known-issues.md#legacy-implode-argument-order).
- **CSV export.** `NF_EXPORT` on `Conference` adds an export action.

More relation types (one-to-one and the many-to-many family) are described in [Relations](../guide/relations.md).

## 6. Business rules with hooks

Nodes expose hooks you can override. Two examples follow: forbid conferences in the past, and normalise e-mail addresses.

**Per-attribute validation.** When a node has a method `<attribute>_validate(array &$record, string $mode)`, `Core\NodeValidator` calls it *instead of* the attribute's own `validate()`. It is only called when the value is not empty. Register errors with `Tools::triggerError()`, passing a language key as the error. Form values of date/time attributes are arrays (`year`, `month`, `day`, `hours`, `minutes`, `seconds`):

```php
use Sintattica\Atk\Core\Tools;

// in Conference
public function starts_at_validate(array &$record, string $mode): void
{
    $v = $record['starts_at'];
    $timestamp = mktime((int)$v['hours'], (int)$v['minutes'], 0, (int)$v['month'], (int)$v['day'], (int)$v['year']);
    if ($timestamp < time()) {
        Tools::atkTriggerError($record, $this->getAttribute('starts_at'), 'error_date_in_the_past');
    }
}
```

**Triggers.** Triggers run around the database operations. They must return `true` for the operation to continue. The parent `Node::preAdd()` does real work (it stores nested attribute values), so call it:

```php
// in Attendee
public function preAdd(array &$record, string $mode = 'add'): bool
{
    $record['email'] = strtolower(trim($record['email']));

    return parent::preAdd($record, $mode);
}
```

The overriding methods must keep the return types declared by `Core\Node` (`: bool` for triggers), or PHP refuses to load the class.

The full list of triggers (`preAdd`, `postAdd`, `preUpdate`, `postUpdate`, `preDelete`, `postDelete`, …) and naming-convention overrides is in [Node hooks and overrides](../guide/node-hooks.md).

## 7. Translations

Labels default to a humanised attribute name: `starts_at` is shown as "Starts at". To set your own labels, add a language file to the module.

`src/Modules/Conference/languages/en.php`:

```php
<?php

return [
    'speakers' => 'Speakers',
    'conferences' => 'Conferences',
    'attendees' => 'Attendees',
    'speaker_id' => 'Speaker',
    'starts_at' => 'Starts',
    'error_date_in_the_past' => 'The conference cannot start in the past',
];
```

Add an `it.php` (or any other language) with the same keys, and list the language in `supported_languages`. ATK ships its own translations for 25 languages in `vendor/sintattica/atk/src/Resources/languages/`.

The lookup order is described in [Internationalisation](../guide/i18n.md). For example, `conference_starts_at` overrides `starts_at` for the `conference` node only.

## 8. Real users and permissions

The administrator bypasses every check. For real users:

- switch to `authentication => 'db'`;
- choose a `securityscheme` (`level` or `group`);
- point the `auth_*` keys at your users, groups and access-rights tables.

The [atk-skeleton](https://github.com/Sintattica/atk-skeleton) `Auth` module is a complete example. See [Security](../guide/security.md).

## Where to go next

- [Core concepts](../guide/concepts.md): the mental model behind what you just built.
- [Nodes](../guide/nodes.md) and [Attributes](../guide/attributes.md): everything a node and an attribute can do.
- [Lists, export and import](../guide/lists-export-import.md).
