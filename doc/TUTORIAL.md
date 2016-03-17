# Surgeons general warning!!!

This tutorial is based on a pre alpha atk9, it can and certainlly will change
a lot as the development of atk9 advances.
It is provided to get you going on atk9, specially if you are an old atk user 
who is lost in the brave new world of modern php (As I am :-) )


# Introduction

I'm assuming you are an old pre atk9 user and know about
basic atk concepts as modules, nodes and actions.
In the case you are completely new to the atk world i'll try
to briefly explain the core concept so you can follow the 
tutorial.
An Atk application consist of modules, every module have one or
more nodes, every node has a certain number of actions.
So, to create an atk app, you need to create Modules that contains
nodes and nodes wich can execute certain actions.
The "standard" actions are:

- admin: Show a List of table rows
- add: Show an add Form
- edit: Shows an edit Form
- view: Shows a view Form
- delete: Shows a delete Form

Besides the "standard" actions, you can also declare your own.
Most of the time, a node have a correspondence to a database
table and provides you with CRUD functionallity, validations
and a lot of extras that I will not discuss in this tutorial.

# Pre - Requisites

In order to begin using atk9 you will need to have composer installed
in your computer.
If you don't have composer go to:

www.getcomposer.org

And read de installation instructions.
It's advisable to read composer "Basig usage" documentation.

# Dive In

Once you have composer installed we are gonna need a folder
to work on, so create a folder named i.e. MyApp

Inside this folder we are gonna need the following 
"composer.json" file:

```
{
    "require": {
				"sintattica/atk": "9.*"
    },
		"autoload": {
			"psr-4": {"App\\Modules\\": "App/Modules"}
		}
}
```

Create the file "composer.json" inside MyApp directory
and issue the command:

composer.phar install

Composer will create a directory called "vendor"
with all the libraries you are gonna need.

There are several directories an atk9 application needs to run
properly, we need to create the following directories:

- Modules Directory: This will contains our app's modules, execute:

> mkdir -p App/Modules

- Temp Directory: This will be used to atk9 to compile the smarty templates, execute:
		
> mkdir -p var/atktmp
> chmod -R 777 var/atktmp

- config Directory: This will contain our atk.php config file, execute:

> cp -R vendor/sintattica/atk/src/Resources/config/ .

- templates Directory: This contains our app templates, execute:

> cp -R vendor/sintattica/atk/src/Resources/templates/ .

- bundle Directory: This contains the App resources, execute

> mkdir -p bundles/atk
> cp vendor/sintattica/atk/src/Resources/public/  bundles/atk

	
Now enter the mysql cli with:

> mysql  -u root -p 

and create a database for us to play with the following mysql command
		
> create database atk;

And grant permissions to it with:

> grant all on atk.* to atk@localhost identified by 'atk';

Now we can leave the mysql cli with:

> exit

He have created a database called atk and granted all privileges
to the atk user with atk password, we need to reflect these parameters
in the config file, edit the file config/atk.php and edit the database
section:

```
    /************************** DATABASE SETTINGS ******************************/

    'db' => [
        'default' => [
            'host' => Config::env('DB_HOST', 'localhost'),
            'db' => Config::env('DB_NAME', 'atk'),
            'user' => Config::env('DB_USER', 'atk'),
            'password' => Config::env('DB_PASSWORD', 'atk'),
            'charset' => Config::env('DB_CHARSET', 'utf8'),
            'driver' => Config::env('DB_DRIVER', 'MySqli'),
        ],
    ],
```
    


with the correct database parameters

We are all set now, in order to serve the application we
will be using the built in php server, execute the following
command:

> php -S localhost:8000

Open a browser and navigate to the following url

> http://localhost:8000

And you should see your first atk9 app.

You will probably found a couple of messages in red, these
are because our application does not have any module, it's 
an empty app so far, our next step will be the creation of
a module.

To Create a module we will make a subdirectory of App/Modules
i.e.:

> mkdir -p App/Modules/Test

Inside our Test module folder we will create a Module.php file
with the following content:

```
<?php
namespace App\Modules\Test;

use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Menu;

class Module extends \Sintattica\Atk\Core\Module
{
  public static  $module="test";

  public function __construct(Atk $atk, Menu $menu)
  {
    parent::__construct($atk, $menu);
    $this->addNodeToMenu("Hello", "miprimernodito", "hola_mundo");
    $this->registerNode("myfirstnode", "App\Modules\Test\MyFirstNode", array("hello_world"));

  }

  public function boot()
  {
  }
}
?>

```

In the previous Module definition we are declaring a node,
the MyFirstNide node and an action on that node, the hello_world
action. 
In order to create the node, let's create a file called:

MyFirstNode.php in the module folder, with the following content:

```
<?php

namespace App\Modules\Test;

use Sintattica\Atk\Core\Atk\Node;
use Sintattica\Atk\Db;

class MyFirstNode extends \Sintattica\Atk\Core\Node
{
  public function __create($uri, $flags)
  {
  }

  public function action_hello_world()
  {
    die("It works!!!! ");
  }
}


?>
```
Now in order to make it work, we need to edit the config/atk.php
file and declare our recently created Module, edit the config file:

 /*
     * @var array List of enabled modules
     * eg: [App\Modules\App\Module::class, App\Modules\Auth\Module::class,]
     *
     */
    'modules' => [],

to

 /*
     * @var array List of enabled modules
     * eg: [App\Modules\App\Module::class, App\Modules\Auth\Module::class,]
     *
     */
    'modules' => [App\Modules\Test\Module::class],

And it's ready to test, launch the webserver again with:

php -S localhost:8000

And test it.

Good Luck!
