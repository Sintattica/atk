/*
 * FCKeditor - The text editor for internet
 * Copyright (C) 2003-2004 Frederico Caldeira Knabben
 * 
 * Licensed under the terms of the GNU Lesser General Public License:
 * 		http://www.opensource.org/licenses/lgpl-license.php
 * 
 * For further information visit:
 * 		http://www.fckeditor.net/
 * 
 * File Name: ro.js
 * 	Romanian language file.
 * 
 * Version:  2.0 RC3
 * Modified: 2005-03-01 17:26:18
 * 
 * File Authors:
 * 		Adrian Nicoara
 */

var FCKLang =
{
// Language direction : "ltr" (left to right) or "rtl" (right to left).
Dir					: "ltr",

ToolbarCollapse		: "Ascunde bara cu optiuni",
ToolbarExpand		: "Expandeaza bara cu optiuni",

// Toolbar Items and Context Menu
Save				: "Salveaza",
NewPage				: "Pagina noua",
Preview				: "Previzualizare",
Cut					: "Taie",
Copy				: "Copiaza",
Paste				: "Adauga",
PasteText			: "Adauga ca text simplu",
PasteWord			: "Adauga din Word",
Print				: "Printeaza",
SelectAll			: "Selecteaza tot",
RemoveFormat		: "nlatura formatarea",
InsertLinkLbl		: "Link (Legatura web)",
InsertLink			: "Insereaza/Editeaza link (legatura web)",
RemoveLink			: "nlatura link (legatura web)",
Anchor				: "Insert/Edit Anchor",	//MISSING
InsertImageLbl		: "Imagine",
InsertImage			: "Insereaza/Editeaza imagine",
InsertTableLbl		: "Tabel",
InsertTable			: "Insereaza/Editeaza tabel",
InsertLineLbl		: "Linie",
InsertLine			: "Insereaza linie orizonta",
InsertSpecialCharLbl: "Caracter special",
InsertSpecialChar	: "Insereaza caracter special",
InsertSmileyLbl		: "Figura expresiva (Emoticon)",
InsertSmiley		: "Insereaza Figura expresiva (Emoticon)",
About				: "Despre FCKeditor",
Bold				: "ngrosat (bold)",
Italic				: "nclinat (italic)",
Underline			: "Subliniat (underline)",
StrikeThrough		: "Taiat (strike through)",
Subscript			: "Indice (subscript)",
Superscript			: "Putere (superscript)",
LeftJustify			: "Aliniere la stnga",
CenterJustify		: "Aliniere centrala",
RightJustify		: "Aliniere la dreapta",
BlockJustify		: "Aliniere n bloc (Block Justify)",
DecreaseIndent		: "Scade indentarea",
IncreaseIndent		: "Creste indentarea",
Undo				: "Starea anterioara (undo)",
Redo				: "Starea ulterioara (redo)",
NumberedListLbl		: "Lista numerotata",
NumberedList		: "Insereaza/Sterge lista numerotata",
BulletedListLbl		: "Lista cu puncte",
BulletedList		: "Insereaza/Sterge lista cu puncte",
ShowTableBorders	: "Arata marginile tabelului",
ShowDetails			: "Arata detalii",
Style				: "Stil",
FontFormat			: "Formatare",
Font				: "Font",
FontSize			: "Marime",
TextColor			: "Culoarea textului",
BGColor				: "Coloarea fundalului",
Source				: "Sursa",
Find				: "Gaseste",
Replace				: "nlocuieste",
SpellCheck			: "Check Spell",	//MISSING
UniversalKeyboard	: "Universal Keyboard",	//MISSING

Form			: "Form",	//MISSING
Checkbox		: "Checkbox",	//MISSING
RadioButton		: "Radio Button",	//MISSING
TextField		: "Text Field",	//MISSING
Textarea		: "Textarea",	//MISSING
HiddenField		: "Hidden Field",	//MISSING
Button			: "Button",	//MISSING
SelectionField	: "Selection Field",	//MISSING
ImageButton		: "Image Button",	//MISSING

// Context Menu
EditLink			: "Editeaza Link",
InsertRow			: "Insereaza Row",
DeleteRows			: "Sterge Rows",
InsertColumn		: "Insereaza Column",
DeleteColumns		: "Sterge Columns",
InsertCell			: "Insereaza Cell",
DeleteCells			: "Sterge celule",
MergeCells			: "Uneste celule",
SplitCell			: "mparte celula",
CellProperties		: "Proprietatile celulei",
TableProperties		: "Proprietatile tabelului",
ImageProperties		: "Proprietatile imaginii",

AnchorProp			: "Anchor Properties",	//MISSING
ButtonProp			: "Button Properties",	//MISSING
CheckboxProp		: "Checkbox Properties",	//MISSING
HiddenFieldProp		: "Hidden Field Properties",	//MISSING
RadioButtonProp		: "Radio Button Properties",	//MISSING
ImageButtonProp		: "Image Button Properties",	//MISSING
TextFieldProp		: "Text Field Properties",	//MISSING
SelectionFieldProp	: "Selection Field Properties",	//MISSING
TextareaProp		: "Textarea Properties",	//MISSING
FormProp			: "Form Properties",	//MISSING

FontFormats			: "Normal;Formatat;Adresa;Titlu 1;Titlu 2;Titlu 3;Titlu 4;Titlu 5;Titlu 6;Paragraf (DIV)",	// 2.0: The last entry has been added.

// Alerts and Messages
ProcessingXHTML		: "Procesam XHTML. Va rugam asteptati...",
Done				: "Am terminat",
PasteWordConfirm	: "Textul pe care doriti sa-l adaugati pare a fi formatat pentru Word. Doriti sa-l curatati de aceasta formatare nainte de a-l adauga?",
NotCompatiblePaste	: "Aceasta facilitate e disponibila doar pentru Microsoft Internet Explorer, versiunea 5.5 sau ulterioara. Vreti sa-l adaugati fara a-i fi nlaturat formatarea?",
UnknownToolbarItem	: "Obiectul \"%1\" din bara cu optiuni necunoscut",
UnknownCommand		: "Comanda \"%1\" necunoscuta",
NotImplemented		: "Comanda neimplementata",
UnknownToolbarSet	: "Grupul din bara cu optiuni \"%1\" nu exista",

// Dialogs
DlgBtnOK			: "Bine",
DlgBtnCancel		: "Anulare",
DlgBtnClose			: "nchidere",
DlgBtnBrowseServer	: "Browse Server",	//MISSING
DlgAdvancedTag		: "Avansat",
DlgOpOther			: "&lt;Other&gt;",	//MISSING

// General Dialogs Labels
DlgGenNotSet		: "&lt;nesetat&gt;",
DlgGenId			: "Id",
DlgGenLangDir		: "Directia cuvintelor",
DlgGenLangDirLtr	: "stnga-dreapta (LTR)",
DlgGenLangDirRtl	: "dreapta-stnga (RTL)",
DlgGenLangCode		: "Codul limbii",
DlgGenAccessKey		: "Tasta de acces",
DlgGenName			: "Nume",
DlgGenTabIndex		: "Indexul tabului",
DlgGenLongDescr		: "Descrierea lunga URL",
DlgGenClass			: "Clasele cu stilul paginii (CSS)",
DlgGenTitle			: "Titlul consultativ",
DlgGenContType		: "Tipul consultativ al titlului",
DlgGenLinkCharset	: "Setul de caractere al resursei legate",
DlgGenStyle			: "Stil",

// Image Dialog
DlgImgTitle			: "Proprietatile imaginii",
DlgImgInfoTab		: "Informatii despre imagine",
DlgImgBtnUpload		: "Trimite la server",
DlgImgURL			: "URL",
DlgImgUpload		: "ncarca",
DlgImgAlt			: "Text alternativ",
DlgImgWidth			: "Latime",
DlgImgHeight		: "naltime",
DlgImgLockRatio		: "Pastreaza proportiile",
DlgBtnResetSize		: "Reseteaza marimea",
DlgImgBorder		: "Margine",
DlgImgHSpace		: "HSpace",
DlgImgVSpace		: "VSpace",
DlgImgAlign			: "Aliniere",
DlgImgAlignLeft		: "Stnga",
DlgImgAlignAbsBottom: "Jos absolut (Abs Bottom)",
DlgImgAlignAbsMiddle: "Mijloc absolut (Abs Middle)",
DlgImgAlignBaseline	: "Linia de jos (Baseline)",
DlgImgAlignBottom	: "Jos",
DlgImgAlignMiddle	: "Mijloc",
DlgImgAlignRight	: "Dreapta",
DlgImgAlignTextTop	: "Text sus",
DlgImgAlignTop		: "Sus",
DlgImgPreview		: "Previzualizare",
DlgImgAlertUrl		: "Va rugam sa scrieti URL-ul imaginii",

// Link Dialog
DlgLnkWindowTitle	: "Link (Legatura web)",
DlgLnkInfoTab		: "Informatii despre link (Legatura web)",
DlgLnkTargetTab		: "Tinta (Target)",

DlgLnkType			: "Tipul link-ului (al legaturii web)",
DlgLnkTypeURL		: "URL",
DlgLnkTypeAnchor	: "Ancora n aceasta pagina",
DlgLnkTypeEMail		: "E-Mail",
DlgLnkProto			: "Protocol",
DlgLnkProtoOther	: "&lt;altul&gt;",
DlgLnkURL			: "URL",
DlgLnkAnchorSel		: "Selectati o ancora",
DlgLnkAnchorByName	: "dupa numele ancorei",
DlgLnkAnchorById	: "dupa Id-ul elementului",
DlgLnkNoAnchors		: "&lt;Nici o ancora disponibila n document&gt;",
DlgLnkEMail			: "Adresa de e-mail",
DlgLnkEMailSubject	: "Subiectul mesajului",
DlgLnkEMailBody		: "Continutul mesajului",
DlgLnkUpload		: "ncarca",
DlgLnkBtnUpload		: "Trimite la server",

DlgLnkTarget		: "Tinta (Target)",
DlgLnkTargetFrame	: "&lt;frame&gt;",
DlgLnkTargetPopup	: "&lt;fereastra popup&gt;",
DlgLnkTargetBlank	: "Fereastra noua (_blank)",
DlgLnkTargetParent	: "Fereastra parinte (_parent)",
DlgLnkTargetSelf	: "Aceeasi fereastra (_self)",
DlgLnkTargetTop		: "Fereastra din topul ierarhiei (_top)",
DlgLnkTargetFrameName	: "Target Frame Name",	//MISSING
DlgLnkPopWinName	: "Numele ferestrei popup",
DlgLnkPopWinFeat	: "Proprietatile ferestrei popup",
DlgLnkPopResize		: "Scalabila",
DlgLnkPopLocation	: "Bara de locatie",
DlgLnkPopMenu		: "Bara de meniu",
DlgLnkPopScroll		: "Scroll Bars",
DlgLnkPopStatus		: "Bara de status",
DlgLnkPopToolbar	: "Bara de optiuni",
DlgLnkPopFullScrn	: "Tot ecranul (Full Screen)(IE)",
DlgLnkPopDependent	: "Dependent (Netscape)",
DlgLnkPopWidth		: "Latime",
DlgLnkPopHeight		: "naltime",
DlgLnkPopLeft		: "Pozitia la stnga",
DlgLnkPopTop		: "Pozitia la dreapta",

DlnLnkMsgNoUrl		: "Va rugam sa scrieti URL-ul",
DlnLnkMsgNoEMail	: "Va rugam sa scrieti adresa de e-mail",
DlnLnkMsgNoAnchor	: "Va rugam sa selectati o ancora",

// Color Dialog
DlgColorTitle		: "Selecteaza culoare",
DlgColorBtnClear	: "Curata",
DlgColorHighlight	: "Subliniaza (Highlight)",
DlgColorSelected	: "Selectat",

// Smiley Dialog
DlgSmileyTitle		: "Insereaza o figura expresiva (Emoticon)",

// Special Character Dialog
DlgSpecialCharTitle	: "Selecteaza caracter special",

// Table Dialog
DlgTableTitle		: "Proprietatile tabelului",
DlgTableRows		: "Linii",
DlgTableColumns		: "Coloane",
DlgTableBorder		: "Marimea marginii",
DlgTableAlign		: "Aliniament",
DlgTableAlignNotSet	: "<Nesetat>",
DlgTableAlignLeft	: "Stnga",
DlgTableAlignCenter	: "Centru",
DlgTableAlignRight	: "Dreapta",
DlgTableWidth		: "Latime",
DlgTableWidthPx		: "pixeli",
DlgTableWidthPc		: "procente",
DlgTableHeight		: "naltime",
DlgTableCellSpace	: "Spatiu ntre celule",
DlgTableCellPad		: "Spatiu n cadrul celulei",
DlgTableCaption		: "Titlu (Caption)",

// Table Cell Dialog
DlgCellTitle		: "Proprietatile celulei",
DlgCellWidth		: "Latime",
DlgCellWidthPx		: "pixeli",
DlgCellWidthPc		: "procente",
DlgCellHeight		: "naltime",
DlgCellWordWrap		: "Desparte cuvintele (Wrap)",
DlgCellWordWrapNotSet	: "&lt;Nesetat&gt;",
DlgCellWordWrapYes	: "Da",
DlgCellWordWrapNo	: "Nu",
DlgCellHorAlign		: "Aliniament orizontal",
DlgCellHorAlignNotSet	: "&lt;Nesetat&gt;",
DlgCellHorAlignLeft	: "Stnga",
DlgCellHorAlignCenter	: "Centru",
DlgCellHorAlignRight: "Dreapta",
DlgCellVerAlign		: "Aliniament vertical",
DlgCellVerAlignNotSet	: "&lt;Nesetat&gt;",
DlgCellVerAlignTop	: "Sus",
DlgCellVerAlignMiddle	: "Mijloc",
DlgCellVerAlignBottom	: "Jos",
DlgCellVerAlignBaseline	: "Linia de jos (Baseline)",
DlgCellRowSpan		: "Lungimea n linii (Span)",
DlgCellCollSpan		: "Lungimea n coloane (Span)",
DlgCellBackColor	: "Culoarea fundalului",
DlgCellBorderColor	: "Culoarea marginii",
DlgCellBtnSelect	: "Selectati...",

// Find Dialog
DlgFindTitle		: "Gaseste",
DlgFindFindBtn		: "Gaseste",
DlgFindNotFoundMsg	: "Textul specificat nu a fost gasit.",

// Replace Dialog
DlgReplaceTitle			: "Replace",
DlgReplaceFindLbl		: "Gaseste:",
DlgReplaceReplaceLbl	: "nlocuieste cu:",
DlgReplaceCaseChk		: "Deosebeste majuscule de minuscule (Match case)",
DlgReplaceReplaceBtn	: "nlocuieste",
DlgReplaceReplAllBtn	: "nlocuieste tot",
DlgReplaceWordChk		: "Doar cuvintele ntregi",

// Paste Operations / Dialog
PasteErrorPaste	: "Setarile de securitate ale navigatorului (browser) pe care l folositi nu permit editorului sa execute automat operatiunea de adaugare. Va rugam folositi tastatura (Ctrl+V).",
PasteErrorCut	: "Setarile de securitate ale navigatorului (browser) pe care l folositi nu permit editorului sa execute automat operatiunea de taiere. Va rugam folositi tastatura (Ctrl+X).",
PasteErrorCopy	: "Setarile de securitate ale navigatorului (browser) pe care l folositi nu permit editorului sa execute automat operatiunea de copiere. Va rugam folositi tastatura (Ctrl+C).",

PasteAsText		: "Adauga ca text simplu (Plain Text)",
PasteFromWord	: "Adauga din Word",

DlgPasteMsg		: "Editor nu a putut executa automat adaugarea din cauza <STRONG>setarilor de securitate</STRONG> ale navigatorului (browser) dvs.<BR>Va rugam adaugati inauntrul casutei folosind tastatura (<STRONG>Ctrl+V</STRONG>) si apasati <STRONG>Bine</STRONG>.",

// Color Picker
ColorAutomatic	: "Automatic",
ColorMoreColors	: "Mai multe culori...",

// Document Properties
DocProps		: "Document Properties",	//MISSING

// Anchor Dialog
DlgAnchorTitle		: "Anchor Properties",	//MISSING
DlgAnchorName		: "Anchor Name",	//MISSING
DlgAnchorErrorName	: "Please type the anchor name",	//MISSING

// Speller Pages Dialog
DlgSpellNotInDic		: "Not in dictionary",	//MISSING
DlgSpellChangeTo		: "Change to",	//MISSING
DlgSpellBtnIgnore		: "Ignore",	//MISSING
DlgSpellBtnIgnoreAll	: "Ignore All",	//MISSING
DlgSpellBtnReplace		: "Replace",	//MISSING
DlgSpellBtnReplaceAll	: "Replace All",	//MISSING
DlgSpellBtnUndo			: "Undo",	//MISSING
DlgSpellNoSuggestions	: "- No suggestions -",	//MISSING
DlgSpellProgress		: "Spell check in progress...",	//MISSING
DlgSpellNoMispell		: "Spell check complete: No misspellings found",	//MISSING
DlgSpellNoChanges		: "Spell check complete: No words changed",	//MISSING
DlgSpellOneChange		: "Spell check complete: One word changed",	//MISSING
DlgSpellManyChanges		: "Spell check complete: %1 words changed",	//MISSING

IeSpellDownload			: "Spell checker not installed. Do you want to download it now?",	//MISSING

// Button Dialog
DlgButtonText	: "Text (Value)",	//MISSING
DlgButtonType	: "Type",	//MISSING

// Checkbox and Radio Button Dialogs
DlgCheckboxName		: "Name",	//MISSING
DlgCheckboxValue	: "Value",	//MISSING
DlgCheckboxSelected	: "Selected",	//MISSING

// Form Dialog
DlgFormName		: "Name",	//MISSING
DlgFormAction	: "Action",	//MISSING
DlgFormMethod	: "Method",	//MISSING

// Select Field Dialog
DlgSelectName		: "Name",	//MISSING
DlgSelectValue		: "Value",	//MISSING
DlgSelectSize		: "Size",	//MISSING
DlgSelectLines		: "lines",	//MISSING
DlgSelectChkMulti	: "Allow multiple selections",	//MISSING
DlgSelectOpAvail	: "Available Options",	//MISSING
DlgSelectOpText		: "Text",	//MISSING
DlgSelectOpValue	: "Value",	//MISSING
DlgSelectBtnAdd		: "Add",	//MISSING
DlgSelectBtnModify	: "Modify",	//MISSING
DlgSelectBtnUp		: "Up",	//MISSING
DlgSelectBtnDown	: "Down",	//MISSING
DlgSelectBtnSetValue : "Set as selected value",	//MISSING
DlgSelectBtnDelete	: "Delete",	//MISSING

// Textarea Dialog
DlgTextareaName	: "Name",	//MISSING
DlgTextareaCols	: "Columns",	//MISSING
DlgTextareaRows	: "Rows",	//MISSING

// Text Field Dialog
DlgTextName			: "Name",	//MISSING
DlgTextValue		: "Value",	//MISSING
DlgTextCharWidth	: "Character Width",	//MISSING
DlgTextMaxChars		: "Maximum Characters",	//MISSING
DlgTextType			: "Type",	//MISSING
DlgTextTypeText		: "Text",	//MISSING
DlgTextTypePass		: "Password",	//MISSING

// Hidden Field Dialog
DlgHiddenName	: "Name",	//MISSING
DlgHiddenValue	: "Value",	//MISSING

// Bulleted List Dialog
BulletedListProp	: "Bulleted List Properties",	//MISSING
NumberedListProp	: "Numbered List Properties",	//MISSING
DlgLstType			: "Type",	//MISSING
DlgLstTypeCircle	: "Circle",	//MISSING
DlgLstTypeDisk		: "Disk",	//MISSING
DlgLstTypeSquare	: "Square",	//MISSING
DlgLstTypeNumbers	: "Numbers (1, 2, 3)",	//MISSING
DlgLstTypeLCase		: "Lowercase Letters (a, b, c)",	//MISSING
DlgLstTypeUCase		: "Uppercase Letters (A, B, C)",	//MISSING
DlgLstTypeSRoman	: "Small Roman Numerals (i, ii, iii)",	//MISSING
DlgLstTypeLRoman	: "Large Roman Numerals (I, II, III)",	//MISSING

// Document Properties Dialog
DlgDocGeneralTab	: "General",	//MISSING
DlgDocBackTab		: "Background",	//MISSING
DlgDocColorsTab		: "Colors and Margins",	//MISSING
DlgDocMetaTab		: "Meta Data",	//MISSING

DlgDocPageTitle		: "Page Title",	//MISSING
DlgDocLangDir		: "Language Direction",	//MISSING
DlgDocLangDirLTR	: "Left to Right (LTR)",	//MISSING
DlgDocLangDirRTL	: "Right to Left (RTL)",	//MISSING
DlgDocLangCode		: "Language Code",	//MISSING
DlgDocCharSet		: "Character Set Encoding",	//MISSING
DlgDocCharSetOther	: "Other Character Set Encoding",	//MISSING

DlgDocDocType		: "Document Type Heading",	//MISSING
DlgDocDocTypeOther	: "Other Document Type Heading",	//MISSING
DlgDocIncXHTML		: "Include XHTML Declarations",	//MISSING
DlgDocBgColor		: "Background Color",	//MISSING
DlgDocBgImage		: "Background Image URL",	//MISSING
DlgDocBgNoScroll	: "Nonscrolling Background",	//MISSING
DlgDocCText			: "Text",	//MISSING
DlgDocCLink			: "Link",	//MISSING
DlgDocCVisited		: "Visited Link",	//MISSING
DlgDocCActive		: "Active Link",	//MISSING
DlgDocMargins		: "Page Margins",	//MISSING
DlgDocMaTop			: "Top",	//MISSING
DlgDocMaLeft		: "Left",	//MISSING
DlgDocMaRight		: "Right",	//MISSING
DlgDocMaBottom		: "Bottom",	//MISSING
DlgDocMeIndex		: "Document Indexing Keywords (comma separated)",	//MISSING
DlgDocMeDescr		: "Document Description",	//MISSING
DlgDocMeAuthor		: "Author",	//MISSING
DlgDocMeCopy		: "Copyright",	//MISSING
DlgDocPreview		: "Preview",	//MISSING

// About Dialog
DlgAboutAboutTab	: "About",	//MISSING
DlgAboutBrowserInfoTab	: "Browser Info",	//MISSING
DlgAboutVersion		: "versiune",
DlgAboutLicense		: "Licentiat sub termenii GNU Lesser General Public License",
DlgAboutInfo		: "Pentru informatii amanuntite, vizitati"
}