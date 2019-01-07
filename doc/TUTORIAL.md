# The ATK Book
[TOC]

## What is it? 
ATK is a PHP Framework intended to build business application. ATK has some very high level capabilities for CRUD building, validating inputs and controlling access.
ATK lets yo build a table "admin", a paginated list of rows in a database table, this list comes paired with create, read, update and delete forms with allmost zero coding on your part.
ATK is the right tool if you are writing an application where editing database tables is the main functionallity and design and presentation of individual pages are not a strong concern (Because ATK generates the pages for you automagically).

## A little history
ATK names comes from "Achievo Tool Kit". Achievo was a project planning software written by a Ducth company called iBuildings. ATK was the framework developed to help create the application and was later released as a stand alone tool, hence it's name.
iBuildings created and maintained ATK until 2006 when they stopped supporting it, from that time on, several forks has been made by people wanting to keep it alive.
The guys at Sintattica.it talked to iBuildings founder, Ivo Jansch who handed them the ATK wiki and forum in order to keep those resources online.
Sintatica made several improvements on ATK, moving the version from 6.7 (The last iBuildings release) to ATK 8. But ATK 8 was irremediably old. ATK was written initially with PHP4 and a lot of water has passed under the bridge since ATK first appearence, so the guys at Sinttatica.it decided to go a little further and rebuild ATK with modern PHP and modern tools, this new version is called ATK 9 and makes use of modern PHP object orientation constructs and tools. 
This book will cover how to build applications with ATK 9, this book will not discuss differences with previous version at all, if you are an old ATK user keep in mind that while ATK 9 is "philosophically" similar to older versions, it is not retro compatible and if you want to port an old ATK pre 9 app you will find that some heavy lifting is in order, hopefully, you will also find that it worths the trouble too.

## Let's dive in: Building our first app

Let build's a conference app our app will allow us to register the Speaker, the conference titles, and the conference attendants for each conference.

### Getting the necesary tools.

We will be using a debian based Linux distro in this book.
We will need to have **git** installed, in a debian based Linux distribution it can 
be installed with:

`sudo apt-get install git`


Now we will need **composer.phar** Composer is a tool for dependency management in PHP. It allows you to declare the libraries your project depends on and it will manage (install/update) them for you.
To grab a copy please execute:
`
 php -r "readfile('https://getcomposer.org/installer');" > composer-setup.php 
`

And then, run the setup script wth:
`
./composer-setup.php
`

This will leave a **composer.phar** file in your work directory, now you can get rid of the setup script with :

`rm composer-setup.php`

To simplify typing, rename **composer.phar** to just composer with:

`mv composer.phar composer`

And make sure it is executable with:

`chmod +x composer`

Finally, you should put composer in the path to be readily available when needed, please run:

`sudo mv composer.phar /usr/local/bin/composer`

Now we are gonna need to clone the Sintattica/atk-skeleton project. The skeleton project is an empty project to serve as boiler plate for your own project. In order to grab a copy you will need git:

`
git clone https://github.com/Sintattica/atk-skeleton.git conference
`

This should download a copy of the skeleton project in a directory called **conference**, the directory should have the following structure:

```
conference/
├── atk-skeleton.sql
├── composer.json
├── config
│   ├── app.php
│   ├── atk.php
│   ├── parameters.dev.php
│   ├── parameters.dist.php
│   ├── parameters.prod.php
│   ├── parameters.staging.php
├── README.md
├── languages
│   ├── en.php
│   └── it.php
├── src
│   └── Modules
│       ├── App
│       │   ├── languages
│       │   │   ├── en.php
│       │   │   └── it.php
│       │   ├── Module.php
│       │   └── TestNode.php
│       └── Auth
│           ├── Groups.php
│           ├── languages
│           │   ├── en.php
│           │   └── it.php
│           ├── Module.php
│           ├── UsersGroups.php
│           └── Users.php
├── var
└── web
    ├── bundles
    │   └── atk -> ../../vendor/sintattica/atk/src/Resources/public
    ├── index.php
    └── images
        ├── brand_logo.png
        └── login_logo.png
```

Let's take a quick look to some files and directories:

- composer.json: It is the composer dependencies file, any time you need a new software library you should add its name here and run **composer update**.
- The config direcory contains the configuration files.
- The src directory: Our work will go mainly in this directory, this is the directory where our application sources will reside, more specifically in the modules directory.
- The var directory is for temporary files
- The web directory is the directory that will need to be served by a web server (Apache, Nginx, Lighttpd or any other).

Maybe you have observed that the web/bundles subdirectory is a symbolic link to an inexistent vendor directory, that directory is the directory that composer uses to store the downloaded dependencies and it will be created when composer updates the dependencies, let's do that with:

`
composer update
`

After composer finishes the updating you will have a vendor directory containing all the project dependencies.

### Creating a Database

As we've said, ATK is a business oriented framework and that implies that building CRUD interfaces for SQL Tables is a breeze, then, it is obvious that we are gonna need a Database, ATK has "drivers" for:

- MySQL
- PostGress

In this text we will gonna use MySQL.
Let's create a database called **conference** and grant all privileges to user **conference** with password **conference**.
The above requirment can be achieved by excuting:

`mysql -u  root -p `

And once you are inside the mysql cli prompt issue the following commands:

`create database conference;`

And :

`grant all on conference.* to conference@localhost identified by 'conference';`

If you take a look around the skeleton project maybe you noticed a file called **atk-skeleton.sql** lying in the root directory, this file contains the table definitions for ATK security system, your database should have these tables, we will create them with:

`mysql -u conference -p conference < atk-skeleton.sql `

Now, we will need to configure our application.

### Configuring our application

Main configuration options are specified in **config/** directory. Specifically, the parameters.xxx.php files (where xxx stands for dev, dist, prod or staging) contains per-site variables. For this tutorial, we'll work with "dev" environment so you'll have to specify values in **config/parameters.dev.php**.

Let's take a look at the contents of the file:
```
return [
    'atk' => [

        'identifier' => 'atk-skeleton-dev',

        'db' => [
            'default' => [
                'host' => 'localhost',
                'db' => 'atk-skeleton',
                'user' => 'root',
                'password' => '',
                'charset' => 'utf8',
                'driver' => 'MySqli',
            ],
        ],

        'debug' => 1,
        'meta_caching' => false,
        'auth_ignorepasswordmatch' => false,
        'administratorpassword' => '$2y$10$erDvMUhORJraJyxw9KXKKOn7D1FZNsaiT.g2Rdl/4V6qbkulOjUqi', // administrator
    ],
];

```
It's allready obvious that we need to change db, user and password from 'atk-skeleton'/'root'/'' to our database parameters ('conference'/'conference'/'conference'), but  what about the funny line ADMIN_PASSWORD?
The admin password is the administrative ATK password, when you login into an ATK application with the user **administrator**, all security is bypassed and you can do anything. It is the super user password.
You have to set an administrative password in the **.env** file, but you have to store it encrypted, ATK provides a tool to encrypt the password, and you invoke it like this:

` php ./vendor/sintattica/atk/src/Utils/generatehash.php demo`

The clear password is **demo**, once you run the command you'll get something like:

``` 
clean: demo
hash: $2y$10$HURwCzn3JJmSV.8UZEVW/eaO/RSlYKELKFacIwTyKSPssxp101XDC
```

Let's edit our parameters.dev.php file, to look like this:

```
<?php

return [
    'atk' => [

        'identifier' => 'atk-skeleton-dev',

        'db' => [
            'default' => [
                'host' => 'localhost',
                'db' => 'root',
                'user' => 'conference',
                'password' => 'conference',
                'charset' => 'utf8',
                'driver' => 'MySqli',
            ],
        ],

        'debug' => 1,
        'meta_caching' => false,
        'auth_ignorepasswordmatch' => false,
        'administratorpassword' => '$2y$10$HURwCzn3JJmSV.8UZEVW/eaO/RSlYKELKFacIwTyKSPssxp101XDC', // demo
    ],
];
```

But now that we specified the configuration in parameters.dev.php, how will the server know which parameters file to pick up ? The application will look up to an *environmental variable* called **APP_ENV**. The most simple way to set it is on the command line.

Ok, our basic configuration is done, now we can have a little gratification, let's 
take a look to our app, in order to do so, let's start our personal php web server with:

`APP_ENV=dev php -S 0.0.0.0:8000 -t web/`

Now open your browser and navigate to **http://localhost:8000** you should see a login form. You can now login with user **administrator** and password **demo**.
Most probably, the login form is shown in the italian language (As the Sintattica.it are italians that should come as not surprising), let's tell our app to show up in good old english, edit de **config/atk.php** file and change the line:

`'language' => 'it', `

to

`'language' => 'en',`

Taking a look to **vendor/sintattica/atk/src/Resources/languages/** you should see:

```
bp.php  cf.php  da.php  el.php  es.php  fr.php  id.php  ja.php  no.php  pt.php  sk.php  tr.php  zh.php  ca.php  cs.php  de.php  en.php  fi.php  hu.php  it.php  nl.php  pl.php  ru.php  sv.php  uk.php
```

This is the complete list of languages that atk is translated to, if your language isn't there, copy the **en.php** to your **xx.php**, translate it and add it to the project git.

### Our first Module
 *this is work in progress *
 
## Let's dive further: Adding a Relation				

 *this is work in progress *
