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
 * File Name: gl.js
 * 	Galician language file.
 * 
 * Version:  2.0 RC3
 * Modified: 2005-03-01 17:26:17
 * 
 * File Authors:
 * 		Fernando Riveiro Lopez
 */

var FCKLang =
{
// Language direction : "ltr" (left to right) or "rtl" (right to left).
Dir					: "ltr",

ToolbarCollapse		: "Ocultar Ferramentas",
ToolbarExpand		: "Mostrar Ferramentas",

// Toolbar Items and Context Menu
Save				: "Gardar",
NewPage				: "Nova Páxina",
Preview				: "Vista Previa",
Cut					: "Cortar",
Copy				: "Copiar",
Paste				: "Pegar",
PasteText			: "Pegar como texto plano",
PasteWord			: "Pegar dende Word",
Print				: "Imprimir",
SelectAll			: "Seleccionar todo",
RemoveFormat		: "Eliminar Formato",
InsertLinkLbl		: "Ligazón",
InsertLink			: "Inserir/Editar Ligazón",
RemoveLink			: "Eliminar Ligazón",
Anchor				: "Insert/Edit Anchor",	//MISSING
InsertImageLbl		: "Imaxe",
InsertImage			: "Inserir/Editar Imaxe",
InsertTableLbl		: "Tabla",
InsertTable			: "Inserir/Editar Tabla",
InsertLineLbl		: "Liña",
InsertLine			: "Inserir Liña Horizontal",
InsertSpecialCharLbl: "Carácter Special",
InsertSpecialChar	: "Inserir Carácter Especial",
InsertSmileyLbl		: "Smiley",
InsertSmiley		: "Inserir Smiley",
About				: "Acerca de FCKeditor",
Bold				: "Negrita",
Italic				: "Cursiva",
Underline			: "Sub-raiado",
StrikeThrough		: "Tachado",
Subscript			: "Subíndice",
Superscript			: "Superíndice",
LeftJustify			: "Aliñar á Esquerda",
CenterJustify		: "Centrado",
RightJustify		: "Aliñar á Dereita",
BlockJustify		: "Xustificado",
DecreaseIndent		: "Disminuir Sangría",
IncreaseIndent		: "Aumentar Sangría",
Undo				: "Desfacer",
Redo				: "Refacer",
NumberedListLbl		: "Lista Numerada",
NumberedList		: "Inserir/Eliminar Lista Numerada",
BulletedListLbl		: "Marcas",
BulletedList		: "Inserir/Eliminar Marcas",
ShowTableBorders	: "Mostrar Bordes das Taboas",
ShowDetails			: "Mostrar Marcas Parágrafo",
Style				: "Estilo",
FontFormat			: "Formato",
Font				: "Tipo",
FontSize			: "Tamaño",
TextColor			: "Cor do Texto",
BGColor				: "Cor do Fondo",
Source				: "Código Fonte",
Find				: "Procurar",
Replace				: "Substituir",
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
EditLink			: "Editar Ligazón",
InsertRow			: "Inserir Fila",
DeleteRows			: "Borrar Filas",
InsertColumn		: "Inserir Columna",
DeleteColumns		: "Borrar Columnas",
InsertCell			: "Inserir Cela",
DeleteCells			: "Borrar Cela",
MergeCells			: "Unir Celas",
SplitCell			: "Partir Celas",
CellProperties		: "Propriedades da Cela",
TableProperties		: "Propriedades da Taboa",
ImageProperties		: "Propriedades Imaxe",

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

FontFormats			: "Normal;Formateado;Enderezo;Enacabezado 1;Encabezado 2;Encabezado 3;Encabezado 4;Encabezado 5;Encabezado 6;Paragraph (DIV)",	// 2.0: The last entry has been added.

// Alerts and Messages
ProcessingXHTML		: "Procesando XHTML. Por facor, agarde...",
Done				: "Feiro",
PasteWordConfirm	: "Parece que o texto que quere pegar está copiado do Word.¿Quere limpar o formato antes de pegalo?",
NotCompatiblePaste	: "Este comando está disponible para Internet Explorer versión 5.5 ou superior. ¿Quere pegalo sen limpar o formato?",
UnknownToolbarItem	: "Ítem de ferramentas descoñecido \"%1\"",
UnknownCommand		: "Nome de comando descoñecido \"%1\"",
NotImplemented		: "Comando non implementado",
UnknownToolbarSet	: "O conxunto de ferramentas \"%1\" non existe",

// Dialogs
DlgBtnOK			: "OK",
DlgBtnCancel		: "Cancelar",
DlgBtnClose			: "Pechar",
DlgBtnBrowseServer	: "Browse Server",	//MISSING
DlgAdvancedTag		: "Advanzado",
DlgOpOther			: "&lt;Other&gt;",	//MISSING

// General Dialogs Labels
DlgGenNotSet		: "&lt;non definido&gt;",
DlgGenId			: "Id",
DlgGenLangDir		: "Orientación do Idioma",
DlgGenLangDirLtr	: "Esquerda a Dereita (LTR)",
DlgGenLangDirRtl	: "Dereita a Esquerda (RTL)",
DlgGenLangCode		: "Código do Idioma",
DlgGenAccessKey		: "Chave de Acceso",
DlgGenName			: "Nome",
DlgGenTabIndex		: "Índice de Tabulación",
DlgGenLongDescr		: "Descrición Completa da URL",
DlgGenClass			: "Clases da Folla de Estilos",
DlgGenTitle			: "Título",
DlgGenContType		: "Tipo de Contido",
DlgGenLinkCharset	: "Fonte de Caracteres Vinculado",
DlgGenStyle			: "Estilo",

// Image Dialog
DlgImgTitle			: "Propriedades da Imaxe",
DlgImgInfoTab		: "Información da Imaxe",
DlgImgBtnUpload		: "Enviar ó Servidor",
DlgImgURL			: "URL",
DlgImgUpload		: "Carregar",
DlgImgAlt			: "Texto Alternativo",
DlgImgWidth			: "Largura",
DlgImgHeight		: "Altura",
DlgImgLockRatio		: "Proporcional",
DlgBtnResetSize		: "Tamaño Orixinal",
DlgImgBorder		: "Límite",
DlgImgHSpace		: "Esp. Horiz.",
DlgImgVSpace		: "Esp. Vert.",
DlgImgAlign			: "Aliñamento",
DlgImgAlignLeft		: "Esquerda",
DlgImgAlignAbsBottom: "Abs Inferior",
DlgImgAlignAbsMiddle: "Abs Centro",
DlgImgAlignBaseline	: "Liña Base",
DlgImgAlignBottom	: "Pé",
DlgImgAlignMiddle	: "Centro",
DlgImgAlignRight	: "Dereita",
DlgImgAlignTextTop	: "Tope do Texto",
DlgImgAlignTop		: "Tope",
DlgImgPreview		: "Vista Previa",
DlgImgAlertUrl		: "Por favor, escriba a URL da imaxe",

// Link Dialog
DlgLnkWindowTitle	: "Ligazón",
DlgLnkInfoTab		: "Información da Ligazón",
DlgLnkTargetTab		: "Referencia a esta páxina",

DlgLnkType			: "Tipo de Ligazón",
DlgLnkTypeURL		: "URL",
DlgLnkTypeAnchor	: "Referencia nesta páxina",
DlgLnkTypeEMail		: "E-Mail",
DlgLnkProto			: "Protocolo",
DlgLnkProtoOther	: "&lt;outro&gt;",
DlgLnkURL			: "URL",
DlgLnkAnchorSel		: "Seleccionar unha Referencia",
DlgLnkAnchorByName	: "Por Nome de Referencia",
DlgLnkAnchorById	: "Por Element Id",
DlgLnkNoAnchors		: "&lt;Non hai referencias disponibles no documento&gt;",
DlgLnkEMail			: "Enderezo de E-Mail",
DlgLnkEMailSubject	: "Asunto do Mensaxe",
DlgLnkEMailBody		: "Corpo do Mensaxe",
DlgLnkUpload		: "Carregar",
DlgLnkBtnUpload		: "Enviar ó servidor",

DlgLnkTarget		: "Destino",
DlgLnkTargetFrame	: "&lt;frame&gt;",
DlgLnkTargetPopup	: "&lt;Xanela Emerxente&gt;",
DlgLnkTargetBlank	: "Nova Xanela (_blank)",
DlgLnkTargetParent	: "Xanela Pai (_parent)",
DlgLnkTargetSelf	: "Mesma Xanela (_self)",
DlgLnkTargetTop		: "Xanela Primaria (_top)",
DlgLnkTargetFrameName	: "Target Frame Name",	//MISSING
DlgLnkPopWinName	: "Nome da Xanela Emerxente",
DlgLnkPopWinFeat	: "Características da Xanela Emerxente",
DlgLnkPopResize		: "Axustable",
DlgLnkPopLocation	: "Barra de Localización",
DlgLnkPopMenu		: "Barra de Menú",
DlgLnkPopScroll		: "Barras de Desplazamento",
DlgLnkPopStatus		: "Barra de Estado",
DlgLnkPopToolbar	: "Barra de Ferramentas",
DlgLnkPopFullScrn	: "A Toda Pantalla (IE)",
DlgLnkPopDependent	: "Dependente (Netscape)",
DlgLnkPopWidth		: "Largura",
DlgLnkPopHeight		: "Altura",
DlgLnkPopLeft		: "Posición Esquerda",
DlgLnkPopTop		: "Posición dende Arriba",

DlnLnkMsgNoUrl		: "Por favor, escriba a ligazón URL",
DlnLnkMsgNoEMail	: "Por favor, escriba o enderezo de e-mail",
DlnLnkMsgNoAnchor	: "Por favor, seleccione un destino",

// Color Dialog
DlgColorTitle		: "Seleccionar Color",
DlgColorBtnClear	: "Nengunha",
DlgColorHighlight	: "Destacado",
DlgColorSelected	: "Seleccionado",

// Smiley Dialog
DlgSmileyTitle		: "Inserte un Smiley",

// Special Character Dialog
DlgSpecialCharTitle	: "Seleccione Caracter Especial",

// Table Dialog
DlgTableTitle		: "Propiedades da Taboa",
DlgTableRows		: "Filas",
DlgTableColumns		: "Columnas",
DlgTableBorder		: "Tamaño do Borde",
DlgTableAlign		: "Aliñamento",
DlgTableAlignNotSet	: "<Non Definido>",
DlgTableAlignLeft	: "Esquerda",
DlgTableAlignCenter	: "Centro",
DlgTableAlignRight	: "Ereita",
DlgTableWidth		: "Largura",
DlgTableWidthPx		: "pixels",
DlgTableWidthPc		: "percent",
DlgTableHeight		: "Altura",
DlgTableCellSpace	: "Marxe entre Celas",
DlgTableCellPad		: "Marxe interior",
DlgTableCaption		: "Título",

// Table Cell Dialog
DlgCellTitle		: "Propriedades da Cela",
DlgCellWidth		: "Largura",
DlgCellWidthPx		: "pixels",
DlgCellWidthPc		: "percent",
DlgCellHeight		: "Altura",
DlgCellWordWrap		: "Axustar Liñas",
DlgCellWordWrapNotSet	: "&lt;Non Definido&gt;",
DlgCellWordWrapYes	: "Si",
DlgCellWordWrapNo	: "Non",
DlgCellHorAlign		: "Aliñamento Horizontal",
DlgCellHorAlignNotSet	: "&lt;Non definido&gt;",
DlgCellHorAlignLeft	: "Esquerda",
DlgCellHorAlignCenter	: "Centro",
DlgCellHorAlignRight: "Dereita",
DlgCellVerAlign		: "Aliñamento Vertical",
DlgCellVerAlignNotSet	: "&lt;Non definido&gt;",
DlgCellVerAlignTop	: "Arriba",
DlgCellVerAlignMiddle	: "Medio",
DlgCellVerAlignBottom	: "Abaixo",
DlgCellVerAlignBaseline	: "Liña de Base",
DlgCellRowSpan		: "Ocupar Filas",
DlgCellCollSpan		: "Ocupar Columnas",
DlgCellBackColor	: "Color de Fondo",
DlgCellBorderColor	: "Color de Borde",
DlgCellBtnSelect	: "Seleccionar...",

// Find Dialog
DlgFindTitle		: "Procurar",
DlgFindFindBtn		: "Procurar",
DlgFindNotFoundMsg	: "Non te atopou o texto indicado.",

// Replace Dialog
DlgReplaceTitle			: "Substituir",
DlgReplaceFindLbl		: "Texto a procurar:",
DlgReplaceReplaceLbl	: "Substituir con:",
DlgReplaceCaseChk		: "Coincidir Mai./min.",
DlgReplaceReplaceBtn	: "Substituir",
DlgReplaceReplAllBtn	: "Substitiur Todo",
DlgReplaceWordChk		: "Coincidir con toda a palabra",

// Paste Operations / Dialog
PasteErrorPaste	: "Os axustes de seguridade do seu navegador non permiten que o editor realice automáticamente as tarefas de pegado. Por favor, use o teclado para iso (Ctrl+V).",
PasteErrorCut	: "Os axustes de seguridade do seu navegador non permiten que o editor realice automáticamente as tarefas de corte. Por favor, use o teclado para iso (Ctrl+X).",
PasteErrorCopy	: "Os axustes de seguridade do seu navegador non permiten que o editor realice automáticamente as tarefas de copia. Por favor, use o teclado para iso (Ctrl+C).",

PasteAsText		: "Pegar como texto plano",
PasteFromWord	: "Pegar dende Word",

DlgPasteMsg		: "O editor non pode executar automáticamente o pegado debido ós <STRONG>axustes de seguridade</STRONG> do seu navegador.<BR>Por favor, pegue dentro do seguinte cadro usando o atallo de teclado (<STRONG>Ctrl+V</STRONG>) e pulse <STRONG>OK</STRONG>.",

// Color Picker
ColorAutomatic	: "Automático",
ColorMoreColors	: "Máis Cores...",

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
DlgAboutVersion		: "versión",
DlgAboutLicense		: "Licencia concedida baixo os termos da GNU Lesser General Public License",
DlgAboutInfo		: "Para máis información visitar:"
}