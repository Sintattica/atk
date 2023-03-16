This document's format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
## [9.9.18] - 2023-03-07
### Added
- Config: added UI configurations on config file

## [9.9.18] - 2023-03-07
### Fixed
- PasswordAttribute: fixed & tested password autocomplete

## [9.9.17] - 2023-03-07
### Fixed
- PasswordAttribute: fix password autocomplete

## [9.9.16] - 2023-02-20
### Added
- Node: added action download_file_attribute

### Changed
- Atk: refactor const atkselector


## [9.9.15] - 2023-02-17
### Added
- Relation: added selectRecordsMethod to set a record SelectHandler for DataGrid

### Fixed
- Attribute: fix action of ButtonAttribute when FileAttribute is a stream


## [9.9.14] - 2023-02-13
### Added
- Attribute: added "stream" property in FileAttribute


## [9.9.13] - 2023-02-13
### Fixed
- Relation: fix "remove" button of many-to-many select relation


## [9.9.12] - 2023-02-10
### Changed
- Relation: refactor ManyToManySelectRelation
- Attribute: refactor DateAttribute
- Node: refactor function exportFileName of Node


## [9.9.11] - 2023-02-02
### Changed
- Export: updated export filename


## [9.9.10] - 2023-02-01
### Fixed
- Export: fixed missing nested attribute value on export action


## [9.9.9] - 2023-01-30
### Fixed
- Relation: fix setAttributesFlags in OneToMany


## [9.9.8] - 2023-01-23
### Added
- Node: added recordListDropdownStartIndex property to customize recordListTdFirst in the datagrid list

### Fixed
- Attribute: fix do update when clicked SubmitButtonAttribute


## [9.9.7] - 2023-01-18
### Fixed
- Datagrid: fixed datagrid top scroller design
- Attribute: fixed hide/show attribute layout


## [9.9.6] - 2023-01-11
### Added
- Attribute: added textWrap property
- Atk: added new layout for "record actions" in DataGridList

### Changed
- Atk: reduced body font-size
- Atk: reduced column padding in DataGridList


## [9.9.1] - 2022-12-22
### Fixed
- Menu: added badge on menu action item in the navbar menu


## [9.9.0] - 2022-12-22
### Added
- Menu: added badge on menu ActionItem


## [9.8.0] - 2022-12-20
### Added
- Attribute: added CkAttribute stripTagsEnabled property
- Relation: added display list mode (ul and badge pill)
- IndexPage: username link goes in edit when user has permissions
- Tools: added param to force whole words in truncateHTML function

### Changed
- Attribute: refactor edit render in DateAttribute

### Fixed
- Atk: fix FR and IT translations
- Node: fix default descriptor
- Attribute: fix CurrencyAttribute edit()
- Relation: fix width of select2 in ManyToOne


## [9.7.0] - 2022-12-13
### Added
- Node: added new function setAttributesFlags() called automatically by adminPage, addPage, viewPage and editPage


## [9.6.14] - 2022-12-01
### Fixed
- Select2: Updated layout margins for multiselect


## [9.6.13] - 2022-11-25
### Added
- Node: added function getDefaultNestedAttribute

### Fixed
- Attribute: fix filename check in edit in FileAttribute


## [9.6.12] - 2022-11-08
### Fixed
- Style: fix max-width of helpModal in box.tpl


## [9.6.11] - 2022-11-03
### Changed
- Attribute: removed default param 'atkselector' in ActionButtonAttribute when other params are specified

### Fixed
- Attribute: fix default text of ButtonAttribute


## [9.6.10] - 2022-10-10
### Fixed
- Handler: fix nested attribute loading in EditcopyHandler


## [9.6.9] - 2022-10-07
### Fixed
- Attribute: added setMultipleSearch on ExpressionListAttribute
- Attribute: fix search in NestedDateTimeAttribute


## [9.6.8] - 2022-09-30
### Fixed
- Attribute: fix FileAttribute edit when file not found


## [9.6.7] - 2022-09-21
### Added
- UIStateColors: added color "orange ultra light"


## [9.6.6] - 2022-09-14
### Added
- Node: added adminPage node help in the top right corner of the adminPage box 


## [9.6.5] - 2022-09-13
### Added
- Node: added bookmarkLink, legend and filter buttons in the box.tpl

### Changed
- NestedBoolAttribute: support for search field


## [9.6.4] - 2022-09-12
### Added
- NestedBoolAttribute: support for search field (WIP)


## [9.6.3] - 2022-09-09
### Added
- Attribute: added NestedDateTimeAttribute


## [9.6.2] - 2022-09-08
### Added
- CkAttribute: added support to enterMode

### Changed
- Attribute: moved minWidth, maxHeight and maxChars from TextAttribute to Attribute

### Fixed
- MultiListAttribute: fixed fluent setters


## [9.6.1] - 2022-09-01
### Added
- Node: added hidePageTitle flag to hide title of the page

### Changed:
- Icon: update system atk icon to support fontawesome v6


## [9.6.0] - 2022-08-23
### Added
- Attribute: added UIStateColorListAttribute

### Changed
- Node: refactor template in confirmAction with multiple records


## [9.5.2] - 2022-07-28
### Changed
- Node: added argument $atkSelectors of checkConfirmAction to manage multiple atkselector


## [9.5.1] - 2022-07-28
### Added
- UIStateColors: added blue color

### Changed:
- adminPage: added admin-header class instead of list-header

### Fixed
- Legend: fix trad


## [9.5.0] - 2022-07-27
### Added
- UIStateColors: added new colors green, cyan, red, yellow, orange in light or strong version
- Tools: added new string functions strStartsWith and strEndsWith
- Node: added support to legend in the adminHeader
- NestedAttribute: added function setForceUpdate setting same value to the relative nested attribute field
- adminHeader: added adminHeaderFilterButtons to manage filter buttons in adminHeader

### Fix
- IndexPage: fix user link (top right) for administrator
- JsonAttribure: fix update when readonly and force update enabled


## [9.4.0] - 2022-07-07
### Added
- Node: added function getAtkError
- Node: added function updateDbIncludes
- Node: added function addAttributesFlags
- Node: added function removeAttributesFlags
- Node: added function hasAction
- Attribute: added TitleAttribute
- FileAttribute: added thumbnail class attribute
- UpdatedByAttribute: added possibility to force created_by user to administrator
- IndexPage: added function getPage
- IndexPage: added function getUi
- Attribute: added ListBoolAttribute
- CkAttribute: added enter mode constant (ENTER_MODE_P, ENTER_MODE_BR, ENTER_MODE_DIV)

### Changed
- Node: refactor function signatures
- Attribute: renamed variable $rec to $record
- FileAttribute: renamed function AddSlash to addSlash
- CkAttribute: changed default display mode to MODE_SCROLL

### Fix
- Attribute: refactor ButtonAttribute


## [9.3] - 2022
Contains mainly bugfixes and small changes. 


## [9.2] - 2021
This branch starts from v9.1 and it contains an updated style based on AdminLte framework. 
We are making other changes that will be rolled out in the future, generally we will be work 
on fixing existing functionalities on the version 9 line.


## [9.1] - 2018
Removed all custom javascript and added JQuery. 