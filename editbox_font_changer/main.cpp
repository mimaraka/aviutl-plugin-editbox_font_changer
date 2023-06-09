//----------------------------------------------------------------------------------
//		エディットボックスフォント変更
//		ソースファイル
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include <windows.h>
#include <aviutl.hpp>
#include <mkaul/include/mkaul.hpp>

#pragma comment(lib, "C:/aviutl_libs/mkaul/mkaul.lib")


#define EFC_DEF_FONT_TEXT				"ＭＳ ゴシック"
#define EFC_DEF_FONT_SCRIPT				"Consolas"
#define EFC_DEF_FONT_HEIGHT				16
#define EFC_DEF_FONT_WEIGHT				FW_DONTCARE
#define EFC_DEF_ITALIC					FALSE
#define EFC_EDIT_ID_TEXT				22101
#define EFC_EDIT_ID_SCRIPT				22100
#define EFC_EDIT_ID_SCRIPT_2			22356
#define EFC_INI_KEY_TEXT_FONT_NAME		"text_font_name"
#define EFC_INI_KEY_TEXT_FONT_HEIGHT	"text_font_height"
#define EFC_INI_KEY_TEXT_FONT_WEIGHT	"text_font_weight"
#define EFC_INI_KEY_TEXT_ITALIC			"text_inalic"
#define EFC_INI_KEY_SCRIPT_FONT_NAME	"script_font_name"
#define EFC_INI_KEY_SCRIPT_FONT_HEIGHT	"script_font_height"
#define EFC_INI_KEY_SCRIPT_FONT_WEIGHT	"script_font_weight"
#define EFC_INI_KEY_SCRIPT_ITALIC		"script_inalic"

#define EFC_FILTER_NAME					"エディットボックスフォント変更"
#define EFC_FILTER_DEVELOPER			"mimaraka"
#define EFC_FILTER_VERSION				"v1.0.4"
#define EFC_FILTER_INFO					EFC_FILTER_NAME " " EFC_FILTER_VERSION " by " EFC_FILTER_DEVELOPER



// チェックボックスの数
#define	EFC_CHECK_NUM					2
// チェックボックスの名前
const char* check_name[] = {
	"フォント選択(テキスト)",
	"フォント選択(スクリプト制御)"
};
// チェックボックスの初期値 (値は0か1、-1でボタン)
int	check_default[] = {
	-1, -1
};

WNDPROC wndproc_obj_orig;
HWND g_hwnd_edit_text;
HWND g_hwnd_edit_script;
HFONT g_font_text, g_font_script;
int g_font_height_text, g_font_height_script;
bool g_italic_text, g_italic_script;
int g_font_weight_text, g_font_weight_script;
char g_font_name_text[LF_FACESIZE], g_font_name_script[LF_FACESIZE];
mkaul::exedit::Internal g_exedit_internal;



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(オブジェクト設定ウィンドウ)
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_objdialog_hooked(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static bool is_font_changed_text = false;
	static bool is_font_changed_script = false;

	switch (message) {
	case WM_CTLCOLOREDIT:
		if (::GetDlgCtrlID((HWND)lparam) == EFC_EDIT_ID_TEXT && !is_font_changed_text) {
			g_hwnd_edit_text = (HWND)lparam;
			g_font_text = ::CreateFont(
				g_font_height_text, 0,
				0, 0,
				g_font_weight_text,
				g_italic_text,
				FALSE, FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				NULL,
				g_font_name_text
			);

			::SendMessage((HWND)lparam, WM_SETFONT, (WPARAM)g_font_text, MAKELPARAM(TRUE, 0));

			is_font_changed_text = true;
		}
		else if (((::GetDlgCtrlID((HWND)lparam) == EFC_EDIT_ID_SCRIPT) || (::GetDlgCtrlID((HWND)lparam) >= EFC_EDIT_ID_SCRIPT_2)) && !is_font_changed_script) {
			char class_name[256] = "";
			::GetClassNameA((HWND)lparam, class_name, 256);

			if (::strcmp(class_name, "Edit") == 0) {
				g_hwnd_edit_script = (HWND)lparam;

				g_font_script = ::CreateFont(
					g_font_height_script, 0,
					0, 0,
					g_font_weight_script,
					g_italic_script,
					FALSE, FALSE,
					DEFAULT_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					NULL,
					g_font_name_script
				);

				::SendMessage((HWND)lparam, WM_SETFONT, (WPARAM)g_font_script, MAKELPARAM(TRUE, 0));

				is_font_changed_script = true;
			}
		}
		break;
	}

	return g_exedit_internal.get_wndproc_objdialog()(hwnd, message, wparam, lparam);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ
//---------------------------------------------------------------------
BOOL filter_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
{
	static CHOOSEFONT cf;
	static LOGFONT lf;
	static bool init = true;

	switch (message) {
	case AviUtl::FilterPlugin::WindowMessage::Init:
		// フォントの高さ(テキスト)
		g_font_height_text = fp->exfunc->ini_load_int(fp,
			EFC_INI_KEY_TEXT_FONT_HEIGHT,
			EFC_DEF_FONT_HEIGHT
		);
		// フォントの高さ(スクリプト制御)
		g_font_height_script = fp->exfunc->ini_load_int(fp,
			EFC_INI_KEY_SCRIPT_FONT_HEIGHT,
			EFC_DEF_FONT_HEIGHT
		);

		// フォントのウェイト(テキスト)
		g_font_weight_text = fp->exfunc->ini_load_int(fp,
			EFC_INI_KEY_TEXT_FONT_WEIGHT,
			EFC_DEF_FONT_WEIGHT
		);
		// フォントのウェイト(スクリプト制御)
		g_font_weight_script = fp->exfunc->ini_load_int(fp,
			EFC_INI_KEY_SCRIPT_FONT_WEIGHT,
			EFC_DEF_FONT_WEIGHT
		);

		// イタリック(テキスト)
		g_italic_text = fp->exfunc->ini_load_int(fp,
			EFC_INI_KEY_TEXT_ITALIC,
			EFC_DEF_ITALIC
		);
		// イタリック(スクリプト制御)
		g_italic_script = fp->exfunc->ini_load_int(fp,
			EFC_INI_KEY_SCRIPT_ITALIC,
			EFC_DEF_ITALIC
		);

		// フォント名(テキスト)
		fp->exfunc->ini_load_str(fp,
			EFC_INI_KEY_TEXT_FONT_NAME,
			g_font_name_text,
			EFC_DEF_FONT_TEXT
		);
		// フォント名(スクリプト制御)
		fp->exfunc->ini_load_str(fp,
			EFC_INI_KEY_SCRIPT_FONT_NAME,
			g_font_name_script,
			EFC_DEF_FONT_SCRIPT
		);

		cf.lStructSize = sizeof(CHOOSEFONT);
		cf.hwndOwner = hwnd;
		cf.lpLogFont = &lf;
		cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
		return 0;

	case AviUtl::FilterPlugin::WindowMessage::Exit:
		fp->exfunc->ini_save_int(fp, EFC_INI_KEY_TEXT_FONT_HEIGHT, g_font_height_text);
		fp->exfunc->ini_save_str(fp, EFC_INI_KEY_TEXT_FONT_NAME, g_font_name_text);
		fp->exfunc->ini_save_int(fp, EFC_INI_KEY_TEXT_ITALIC, g_italic_text);
		fp->exfunc->ini_save_int(fp, EFC_INI_KEY_TEXT_FONT_WEIGHT, g_font_weight_text);
		fp->exfunc->ini_save_int(fp, EFC_INI_KEY_SCRIPT_FONT_HEIGHT, g_font_height_script);
		fp->exfunc->ini_save_str(fp, EFC_INI_KEY_SCRIPT_FONT_NAME, g_font_name_script);
		fp->exfunc->ini_save_int(fp, EFC_INI_KEY_SCRIPT_ITALIC, g_italic_script);
		fp->exfunc->ini_save_int(fp, EFC_INI_KEY_SCRIPT_FONT_WEIGHT, g_font_weight_script);

		return 0;

	case WM_COMMAND:
		switch (wparam) {
			// テキスト
		case 0x2EE4:
			lf.lfHeight = g_font_height_text;
			::strcpy_s(lf.lfFaceName, g_font_name_text);
			lf.lfWeight = g_font_weight_text;
			lf.lfItalic = g_italic_text;

			if (!::ChooseFont(&cf))
				return 0;

			g_font_text = ::CreateFontIndirect(&lf);
			::SendMessage(g_hwnd_edit_text, WM_SETFONT, (WPARAM)g_font_text, MAKELPARAM(TRUE, 0));
			g_font_height_text = lf.lfHeight;
			g_italic_text = lf.lfItalic;
			g_font_weight_text = lf.lfWeight;
			::strcpy_s(g_font_name_text, lf.lfFaceName);

			return 0;

			// スクリプト制御
		case 0x2EE5:
			lf.lfHeight = g_font_height_script;
			::strcpy_s(lf.lfFaceName, g_font_name_script);
			lf.lfWeight = g_font_weight_script;
			lf.lfItalic = g_italic_script;

			if (!::ChooseFont(&cf))
				return 0;

			g_font_script = ::CreateFontIndirect(&lf);
			::SendMessage(g_hwnd_edit_script, WM_SETFONT, (WPARAM)g_font_script, MAKELPARAM(TRUE, 0));
			g_font_height_script = lf.lfHeight;
			g_italic_script = lf.lfItalic;
			g_font_weight_script = lf.lfWeight;
			::strcpy_s(g_font_name_script, lf.lfFaceName);

			return 0;
		}
	}
	return 0;
}



BOOL func_init(AviUtl::FilterPlugin* fp)
{
	if (!g_exedit_internal.init(fp)) {
		::MessageBox(NULL, "拡張編集(ver.0.92)が見つからないか、バージョンが異なります。", EFC_FILTER_NAME, MB_TOPMOST | MB_ICONERROR);
		return FALSE;
	}

	// オブジェクト設定ダイアログのフック
	mkaul::replace_var(g_exedit_internal.base_address() + 0x2e804, &wndproc_objdialog_hooked);

	return TRUE;
}



//---------------------------------------------------------------------
//		FILTER構造体のポインタを取得
//---------------------------------------------------------------------
auto __stdcall GetFilterTable()
{
	using Flag = AviUtl::FilterPluginDLL::Flag;
	static AviUtl::FilterPluginDLL filter = {
		.flag = Flag::AlwaysActive
				| Flag::ExInformation,
		.name = EFC_FILTER_NAME,
		.check_n = EFC_CHECK_NUM,
		.check_name = check_name,
		.check_default = check_default,
		.func_init = func_init,
		.func_WndProc = filter_wndproc,
		.information = EFC_FILTER_INFO
	};
	return &filter;
}