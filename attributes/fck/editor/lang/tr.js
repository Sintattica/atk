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
 * File Name: tr.js
 * 	Turkish language file.
 * 
 * Version:  2.0 RC3
 * Modified: 2005-03-01 17:26:18
 * 
 * File Authors:
 * 		[Astron of bRONX] Reha Biçer (reha@bilgiparki.com)
 */

var FCKLang =
{
// Language direction : "ltr" (left to right) or "rtl" (right to left).
Dir					: "ltr",

ToolbarCollapse		: "Toolbar Topla",
ToolbarExpand		: "Toolbar Genişlet",

// Toolbar Items and Context Menu
Save				: "Kaydet",
NewPage				: "Yeni Sayfa",
Preview				: "Ön izleme",
Cut					: "Kes",
Copy				: "Kopy",
Paste				: "Yapıştır",
PasteText			: "Düz text olarak yapıştır",
PasteWord			: "Word'ten yapıştır",
Print				: "Yazdır",
SelectAll			: "Tümünü Seç",
RemoveFormat		: "Format'ı temizle",
InsertLinkLbl		: "Link",
InsertLink			: "Link'i Ekle/Düzenle",
RemoveLink			: "Link'i kaldır",
Anchor				: "Insert/Edit Anchor",	//MISSING
InsertImageLbl		: "Resim",
InsertImage			: "Resim Ekle/Düzenle",
InsertTableLbl		: "Tablo",
InsertTable			: "Tablo Ekle/Düzenle",
InsertLineLbl		: "Çizgi",
InsertLine			: "Yatay Çizgi Ekle",
InsertSpecialCharLbl: "Özel Karakterler",
InsertSpecialChar	: "Özel Karakter Ekle",
InsertSmileyLbl		: "Smiley",
InsertSmiley		: "Smiley Ekle",
About				: "FCKeditor Hakkında",
Bold				: "Kalın",
Italic				: "İtalik",
Underline			: "Alttan çizgili",
StrikeThrough		: "Strike Through",
Subscript			: "Alta at",
Superscript			: "Üste at",
LeftJustify			: "Sola Daya",
CenterJustify		: "Ortala",
RightJustify		: "Sağa Daya",
BlockJustify		: "Blokla",
DecreaseIndent		: "Parağraf başını düşür",
IncreaseIndent		: "Parağraf başını arttır",
Undo				: "Geri al",
Redo				: "Tekrar yap",
NumberedListLbl		: "Sayılar",
NumberedList		: "Sayıları Ekle/Sil",
BulletedListLbl		: "Noktalama",
BulletedList		: "Noktalama Ekle/Sil",
ShowTableBorders	: "Tablo Çercevelerini Göster",
ShowDetails			: "Detayları Göster",
Style				: "Style",
FontFormat			: "Format",
Font				: "Font",
FontSize			: "Büyüklük",
TextColor			: "Yazı Rengi",
BGColor				: "Arkaplan Rengi",
Source				: "Kaynak",
Find				: "Bul",
Replace				: "Bul ve Değiştir",
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
EditLink			: "Link'i Düzenle",
InsertRow			: "Satır Ekle",
DeleteRows			: "Satırları Sil",
InsertColumn		: "Sütun Ekle",
DeleteColumns		: "Sütunları Sil",
InsertCell			: "Hücre Ekle",
DeleteCells			: "Hücreleri Sil",
MergeCells			: "Hücreleri Birleştir",
SplitCell			: "Hücrelere Böl",
CellProperties		: "Hücre Özellikleri",
TableProperties		: "Tablo Özellikleri",
ImageProperties		: "Resim Özellikleri",

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

FontFormats			: "Normal;Formatted;Address;Heading 1;Heading 2;Heading 3;Heading 4;Heading 5;Heading 6;Paragraph (DIV)",	// 2.0: The last entry has been added.

// Alerts and Messages
ProcessingXHTML		: "XHTML işlemi yapılıyor. Lütfen beklerin...",
Done				: "Tamamlandı",
PasteWordConfirm	: "Yapıştırmak istediğiniz yazı Word'den kopyalanmış gibi görünecektir. Yapıştırma yapmadan önce onu silmek istermisiniz?",
NotCompatiblePaste	: "Bu işlem Internet Explorer versiyon 5.5 veya üzeri için geçerlidir. Temizlemenden yapıştırmak istermisiniz?",
UnknownToolbarItem	: "Bilinmeyen toolbar parçası \"%1\"",
UnknownCommand		: "Bilinmeyen komut adı \"%1\"",
NotImplemented		: "İşlem tamamlanamadı",
UnknownToolbarSet	: "\"%1\" Toolbar parçası yoktur",

// Dialogs
DlgBtnOK			: "TAMAM",
DlgBtnCancel		: "Vazgeç",
DlgBtnClose			: "Kapat",
DlgBtnBrowseServer	: "Browse Server",	//MISSING
DlgAdvancedTag		: "Gelişmiş",
DlgOpOther			: "&lt;Other&gt;",	//MISSING

// General Dialogs Labels
DlgGenNotSet		: "&lt;ayarlı değil&gt;",
DlgGenId			: "Id",
DlgGenLangDir		: "Dil yönü",
DlgGenLangDirLtr	: "Soldan Sağa (LTR)",
DlgGenLangDirRtl	: "Sağdan Sola (RTL)",
DlgGenLangCode		: "Dil Kodu",
DlgGenAccessKey		: "Giriş Anahtarı",
DlgGenName			: "Adı",
DlgGenTabIndex		: "Tab Indeksi",
DlgGenLongDescr		: "Uzun URL açıklaması",
DlgGenClass			: "Stylesheet Sınıfları",
DlgGenTitle			: "Danışman Başlığı",
DlgGenContType		: "Danışman İçerik Türü",
DlgGenLinkCharset	: "Karakter Set'e bağlı Kaynak",
DlgGenStyle			: "Style",

// Image Dialog
DlgImgTitle			: "Resim Özellikleri",
DlgImgInfoTab		: "Resim Bilgisi",
DlgImgBtnUpload		: "Sunucuya Gönder",
DlgImgURL			: "URL",
DlgImgUpload		: "Upload",
DlgImgAlt			: "Alternatif Yazı",
DlgImgWidth			: "Genişlik",
DlgImgHeight		: "Yükseklik",
DlgImgLockRatio		: "Kilit Oranı",
DlgBtnResetSize		: "Orjinal Büyüklük",
DlgImgBorder		: "Çerceve",
DlgImgHSpace		: "Yatay Boşluk",
DlgImgVSpace		: "Dikay Boşluk",
DlgImgAlign			: "Hizala",
DlgImgAlignLeft		: "Sol",
DlgImgAlignAbsBottom: "Abs Alt",
DlgImgAlignAbsMiddle: "Abs Orta",
DlgImgAlignBaseline	: "Taban çizgisi",
DlgImgAlignBottom	: "Alt",
DlgImgAlignMiddle	: "Orta",
DlgImgAlignRight	: "Sağ",
DlgImgAlignTextTop	: "Yazı üste",
DlgImgAlignTop		: "Üst",
DlgImgPreview		: "Önizleme",
DlgImgAlertUrl		: "Lütfen resmin URL'sini yazın",

// Link Dialog
DlgLnkWindowTitle	: "Link",
DlgLnkInfoTab		: "Link Bilgisi",
DlgLnkTargetTab		: "Hedef",

DlgLnkType			: "Link Türü",
DlgLnkTypeURL		: "URL",
DlgLnkTypeAnchor	: "Bu sayfadaki Anchor",
DlgLnkTypeEMail		: "E-Mail",
DlgLnkProto			: "Protokol",
DlgLnkProtoOther	: "&lt;diğer&gt;",
DlgLnkURL			: "URL",
DlgLnkAnchorSel		: "Bir Anchor seçin",
DlgLnkAnchorByName	: "Anchor Adına Göre",
DlgLnkAnchorById	: "Element Id'ye Göre",
DlgLnkNoAnchors		: "&lt;Bu dosya içinde Anchorlar hazır değil&gt;",
DlgLnkEMail			: "E-Mail Adresi",
DlgLnkEMailSubject	: "Mesajı Konusu",
DlgLnkEMailBody		: "Mesaj Ayrıntısı",
DlgLnkUpload		: "Upload",
DlgLnkBtnUpload		: "Sunucuya Gönder",

DlgLnkTarget		: "Hedef",
DlgLnkTargetFrame	: "&lt;frame&gt;",
DlgLnkTargetPopup	: "&lt;popup penceresi&gt;",
DlgLnkTargetBlank	: "Yeni Pencereye (_blank)",
DlgLnkTargetParent	: "Ana Pencereye (_parent)",
DlgLnkTargetSelf	: "Aynı Pencereye (_self)",
DlgLnkTargetTop		: "En üstteki Pencereye (_top)",
DlgLnkTargetFrameName	: "Target Frame Name",	//MISSING
DlgLnkPopWinName	: "Popup Pencere Adı",
DlgLnkPopWinFeat	: "Popup Pencere Özellikleri",
DlgLnkPopResize		: "Büyüyebilir",
DlgLnkPopLocation	: "Yer Bar'ı",
DlgLnkPopMenu		: "MenÜ Bar",
DlgLnkPopScroll		: "Kaydırma Barları",
DlgLnkPopStatus		: "Durum Bar'ı",
DlgLnkPopToolbar	: "Toolbar",
DlgLnkPopFullScrn	: "Tüm Ekran (IE)",
DlgLnkPopDependent	: "Bağımlı (Netscape)",
DlgLnkPopWidth		: "Genişlik",
DlgLnkPopHeight		: "Yükseklik",
DlgLnkPopLeft		: "Sol Taraf",
DlgLnkPopTop		: "Üst Taraf",

DlnLnkMsgNoUrl		: "Lütfen URL link türünü yazın",
DlnLnkMsgNoEMail	: "Lütfen email adresini yazın",
DlnLnkMsgNoAnchor	: "Lütfen bir anchor seçin",

// Color Dialog
DlgColorTitle		: "Renk Seç",
DlgColorBtnClear	: "Temizle",
DlgColorHighlight	: "Parlak",
DlgColorSelected	: "Seçilmiş",

// Smiley Dialog
DlgSmileyTitle		: "Smiley Ekle",

// Special Character Dialog
DlgSpecialCharTitle	: "Özel Karakter Seç",

// Table Dialog
DlgTableTitle		: "Tablo Özellikleri",
DlgTableRows		: "Satırlar",
DlgTableColumns		: "Sütünlar",
DlgTableBorder		: "Çerceve genişliği",
DlgTableAlign		: "Hizalama",
DlgTableAlignNotSet	: "<Ayarlı değil>",
DlgTableAlignLeft	: "Sol",
DlgTableAlignCenter	: "Orta",
DlgTableAlignRight	: "Sağ",
DlgTableWidth		: "Genişlik",
DlgTableWidthPx		: "pixel'ler",
DlgTableWidthPc		: "oran",
DlgTableHeight		: "Yükseklik",
DlgTableCellSpace	: "Hücre boşlukları",
DlgTableCellPad		: "Hücre kaydırma",
DlgTableCaption		: "Başlık",

// Table Cell Dialog
DlgCellTitle		: "Hücre Özellikleri",
DlgCellWidth		: "Genişlik",
DlgCellWidthPx		: "pixel'ler",
DlgCellWidthPc		: "oran",
DlgCellHeight		: "Yükseklik",
DlgCellWordWrap		: "Kelime sığdır",
DlgCellWordWrapNotSet	: "&lt;Ayarlı değil&gt;",
DlgCellWordWrapYes	: "Evet",
DlgCellWordWrapNo	: "Hazır",
DlgCellHorAlign		: "Yatay Hizalama",
DlgCellHorAlignNotSet	: "&lt;Ayarlı değil&gt;",
DlgCellHorAlignLeft	: "Sol",
DlgCellHorAlignCenter	: "Orta",
DlgCellHorAlignRight: "sağ",
DlgCellVerAlign		: "Dikey Hizalama",
DlgCellVerAlignNotSet	: "&lt;Ayarlı değil&gt;",
DlgCellVerAlignTop	: "Üst",
DlgCellVerAlignMiddle	: "Orta",
DlgCellVerAlignBottom	: "Alt",
DlgCellVerAlignBaseline	: "Taban çizgisi",
DlgCellRowSpan		: "Satır Zinciri",
DlgCellCollSpan		: "Sütun Zinciri",
DlgCellBackColor	: "Zemin Rengi",
DlgCellBorderColor	: "Çerceve Rengi",
DlgCellBtnSelect	: "Seç...",

// Find Dialog
DlgFindTitle		: "Arama",
DlgFindFindBtn		: "Ara",
DlgFindNotFoundMsg	: "Belirtmiş olduğunuz kelime bulunamadı.",

// Replace Dialog
DlgReplaceTitle			: "Kelime Değiştirme",
DlgReplaceFindLbl		: "Bulunacak olan:",
DlgReplaceReplaceLbl	: "Değiştirilmesi gereken:",
DlgReplaceCaseChk		: "Büyüklük karşılaştır",
DlgReplaceReplaceBtn	: "Değiştir",
DlgReplaceReplAllBtn	: "Tümünü Değiştir",
DlgReplaceWordChk		: "Tüm cümleyi karşılaştır",

// Paste Operations / Dialog
PasteErrorPaste	: "Web browser'ınızn güvenlik ayarlarından dolayı editörümüz otomatik olarak yapıştırma işlemini yapamamaktadır. Lütfen (Ctrl+V) tuşlarını kullanarak işleminizi gerçekleştirin.",
PasteErrorCut	: "Web browser'ınızn güvenlik ayarlarından dolayı editörümüz otomatik olarak kesme işlemini yapamamaktadır. Lütfen (Ctrl+X) tuşlarını kullanarak işleminizi gerçekleştirin.",
PasteErrorCopy	: "Web browser'ınızn güvenlik ayarlarından dolayı editörümüz otomatik olarak kopyalama işlemini yapamamaktadır. Lütfen (Ctrl+C) tuşlarını kullanarak işleminizi gerçekleştirin.",

PasteAsText		: "Düz text olarak yapıştır",
PasteFromWord	: "Word'ten yapıştır",

DlgPasteMsg		: "Editörümüz, Web browser'ınızın <b>güvenlik ayarlarından</b> dolayı otomatik olarak yapıştırma işlemini yapamamaktadır.<BR>Lütfen yapıştırmak için <STRONG>Ctrl+V</STRONG> tuşlarına başın ve daha sonra <STRONG>TAMAM</STRONG>'a tıklayın.",

// Color Picker
ColorAutomatic	: "Otomatik",
ColorMoreColors	: "Diğer Renkler...",

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
DlgAboutVersion		: "versiyon",
DlgAboutLicense		: "GNU Lesser General Public License kuralları adı altında lisanslanmıştır",
DlgAboutInfo		: "Daha fazla bilgi için, "
}