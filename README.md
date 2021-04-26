# ATK Framework

ATK Framework is a special purpose PHP framework targeted at business applications.

It's targeted at developers who wish to focus on business logic, instead of coding HTML. Where other application frameworks mainly provide a large set of utility classes, ATK provides a complete framework that requires only small amounts of code to get usable applications, while maintaining full flexibility.

## Branches

ATK was originally developed by Ivo Jansch and iBuildings back in 2000. It's been actively developed until 2011 (version 6.6).

This repo is managed by Sintattica. We still have some active projects based on various versions of ATK. Since we didn't find a true alternative to ATK yet, we opted to keep it alive.

We currently have 4 branches:

* 8.2
* 9.0
* 9.1
* 9.2
* 10.0
* master

### 8.2
We call this the "classic" ATK, i.e. the closest to iBuildings' version. It's largely backward compatible, with no additional functions:

* lots of bug fixes
* added a Bootstrap theme
* improvements to Steelblue/Stillblue themes
* improvements to attributes (atkAttribute, atkDateAttribute, atkListAttribute, atkNumberAttribute, atkFieldSet...)
* improvements to relations (especially atkManyToOneRelation)
* improvements to search functionality
* better handling of dependencies
* better handling of form buttons and submit
* refactoring of meta fetching
* added a couple of utility functions

### 9.0

This is a deeply refactored version that's not backward compatible. It's been developed with the following objectives:

* composer support and PSR-4 compatibility
* a modern class system (PSR-1/PSR-2)
* PHP7 compatibility
* better handling of modules
* removal of deprecated functions
* no more themes, GUI is now Bootstrap-based
* [Smarty 3](http://www.smarty.net/v3_overview) integration
* [Select2](https://select2.github.io/) integration


### 9.1

Version like 9.0, but with jQuery only. No PRs accepted to avoid breaking legacy projects.

### 9.2
We are actively working on this version as we use it on production. The latest release is fixed on v9.2.1. Feel free to test it and report any bugs. 
For the latest developments (we do the best but there's no absolute guarantee of bug free code) you can use v9.2-dev where you can find our latest pushes. 


### 10.0

Contributions and advancements. PRs welcome!


## Contributions

We kindly ask you to:

- Make pull requests by creating a feature branch from 10.0
- Don't branch from master
- Avoid redundant comments
- Keep PSR-2 formatting
- Make the PR's topic "tight", don't modify dozens of classes or the whole app
- Only add or fix a well-defined feature, keeping your changes small

Thanks!

## Resources

### 8.2
The fine folks at iBuildings were kind enough to transfer the atk-framework.com domain to Sintattica and to give us the sources of two historically valuable resources for ATK: the [forum](http://atk-framework.com/forum/) and the [wiki](http://atk-framework.com/wiki/).

### 9.0, 9.1 and 10.0
Currently there's no documentation for v. 9 and above. If you have some, please contribute directly to Github's wiki.
