﻿#include <filesystem>
#include <iostream>
#include <string_view>

#include <DxLib.h>
#include <lua_includes.h>
#include <sol.hpp>
#include <argagg.hpp>

#include "DxLua_application.h"
#include "DxLua.h"

namespace {

using namespace remyroez;

// 解析機の準備
static const argagg::parser argparser{ {
	{ "hotreload", {"-r", "--hotreload"}, u8"スクリプト／アーカイブファイルを指定した間隔（ミリ秒）で監視し、変更があれば再起動します", 1},
	{ "hotswap", {"-s", "--hotswap"}, u8"スクリプト／アーカイブファイルを指定した間隔（ミリ秒）で監視し、変更があれば再読み込みします", 1},
	{ "console", {"-c", "--console"}, u8"コンソールを使用します", 0},
	{ "window", {"-w", "--window"}, u8"強制的にウィンドウモードで起動します", 0},
	{ "fullscreen", {"-f", "--fullscreen"}, u8"強制的にフルスクリーンモードで起動します", 0},
	{ "version", {"-v", "--version"}, u8"バージョン情報を表示します", 0},
	{ "help", {"-h", "--help"}, u8"今表示している引数のヘルプを表示します", 0},
} };

// スクリプトの終了関数を呼ぶ
template <typename... T>
application::done_code call_luafn(sol::object &dxLua, std::string_view name, std::ostringstream &message, T &&... args) {
	application::done_code done = application::done_code::none;

	if (!dxLua.valid()) {
		// DxLua ライブラリが不正な状態
		done = application::done_code::error;

	} else if (sol::object function = dxLua.as<sol::table>()[name]; !function.is<sol::function>()) {
		// オブジェクトが関数ではなかった
		if (function.is<sol::nil_t>()) {
			// nil の場合は呼ばなかったことにする
			done = application::done_code::none;

		} else {
			done = application::done_code::error;
		}

	} else if (auto protected_result = function.as<sol::protected_function>().call(std::forward<T>(args)...); !protected_result.valid()) {
		// 関数の呼び出しに失敗した
		done = application::done_code::error;

		// エラーメッセージを返す
		sol::error err = protected_result;
		message << err.what() << std::endl;

	} else {
		// 成功
		done = application::done_code::exit;

		// 戻り値
		sol::object run_result = protected_result;
		if (!run_result.as<bool>()) {
			done = application::done_code::none;

		} else if (run_result.as<std::string>() == "reload") {
			done = application::done_code::reload;

		} else if (run_result.as<std::string>() == "swap") {
			done = application::done_code::swap;
		}
	}

	return done;
}

} // namespace

namespace remyroez {

// デフォルトファイル名
const std::filesystem::path application::default_filename = "main.lua";

// デフォルトディレクトリ
const std::filesystem::path application::default_directory = "game";

// デフォルトアーカイブ
const std::filesystem::path application::default_archive = "game.dxa";

// 起動処理
bool application::setup(int argc, const char **argv) {
	bool succeeded = false;

	if (!parse_arguments(argc, argv)) {
		// 引数の解析に失敗

	} else if (!setup_console()) {
		// コンソールのセットアップに失敗

	} else {
		// すべて成功
		succeeded = _setupped = true;
	}

	return succeeded;
}

// 終了処理
bool application::teardown() {
	bool succeeded = false;

	if (!_setupped) {
		// セットアップしていない

	} else if (!teardown_console()) {
		// コンソール破棄に失敗

	} else {
		// すべて成功
		_setupped = false;
		succeeded = true;
	}

	return succeeded;
}

// 起動処理
application::done_code application::boot() {
	done_code done = done_code::exit;

	if (!setup_lua()) {
		// Lua のセットアップに失敗
		done = done_code::error;

	} else if (!configure_engine()) {
		// エンジンのセットアップに失敗
		done = done_code::error;

	} else {
		// 実行
		done = run();
	}

	if (!teardown_lua()) {
		// Lua の破棄に失敗

	} else {
		// 成功
	}

	return done;
}

// 実行処理
application::done_code application::run() {
	done_code done = done_code::exit;

	// エラーメッセージ
	std::ostringstream message;

	// スクリプトの読み込み
	bool loaded_script = false;
	if (!load_script(message)) {
		// スクリプトの読み込みに失敗
		message << std::endl << u8"スクリプトの読み込みに失敗しました";
		std::cerr << message.str() << std::endl;

	} else {
		loaded_script = true;
	}

	if (!setup_engine()) {
		// エンジンの初期化に失敗
		done = done_code::error;

	} else {
		// 初期化呼び出し済みかどうか
		bool inited = false;
		while (loaded_script) {
			// 初期化ステップ
			if (inited) {
				// 初期化呼び出し済み

			} else {
				// 初期化呼び出し
				done = call_init(message);
				inited = true;
			}

			// 実行ステップ
			if (!inited) {
				// 初期化呼び出しが成功していない

			} else if (done != done_code::none) {
				// 既に終了していたら何もしない

			} else {
				// 実行呼び出し
				done = call_run(message);

				// 終了ステップ
				if (done == done_code::reload) {
					// reload なら実行
					auto end_done = call_end(message);

					// none 以外なら終了コードの上書き
					if (end_done != done_code::none) {
						done = end_done;
					}

				} else if (done != done_code::exit) {
					// exit 以外で終了していたら実行しない

				} else {
					// 終了呼び出し
					done = call_end(message);
				}
			}

			// 終了コード別に処理
			if (done == done_code::error) {
				// エラーメッセージ出力
				if (auto message_string = message.str(); !message_string.empty()) {
					std::cerr << message_string << std::endl;
				}

			} else if (done == done_code::swap) {
				message.clear();
				loaded_script = false;
				if (!load_script(message)) {
					// スクリプトの読み込みに失敗
					message << std::endl << u8"スクリプトの再読み込みに失敗しました";
					std::cerr << message.str() << std::endl;

				} else {
					// 読み込み成功したので再実行
					loaded_script = true;
					std::cout << u8"スクリプトを再読み込みしました" << std::endl;
					done = done_code::none;
					continue;
				}
			}
			break;
		}

		// エラー画面へ
		if (auto message_string = message.str(); !message_string.empty()) {
			done = error(message_string, done);
		}

		teardown_engine();
	}

	// リスタート時の出力
	if (done == done_code::reload) {
		std::cout << u8"スクリプトをリロードしています…" << std::endl;
	}

	return done;
}

// エラー画面
application::done_code application::error(std::string_view message, done_code done) {
	// エラー出力
	DxLib::clsDx();
	DxLib::printfDx(L"%s\n", message.data());
	DxLib::printfDx(L"\nＦ５キーを押すとリロード、ＥＳＣキーを押すと終了します\n");
	DxLib::ScreenFlip();

	// 監視用のタイマー
	// NOTE: オプションでウォッチモードが指定されていなければ監視しない
	const auto interval = (_option.watch != watch_mode::none) ? _option.watch_interval * 1000LL : -1LL;
	auto timer = interval;
	auto before = DxLib::GetNowHiPerformanceCount();

	// エラー画面ループ
	while (DxLib::ProcessMessage() == 0) {
		// スクリプト監視
		if (timer > 0) {
			// 時間経過
			auto now = DxLib::GetNowHiPerformanceCount();
			auto dt = now - before;
			before = now;
			timer -= dt;

			// 監視
			if (timer <= 0) {
				// スクリプトが変更されていればリロード
				if (DxLua::watch(*_dxLua)) {
					done = done_code::reload;
					break;
				}
				timer = interval;
			}
		}

		// F5 でリロード
		if (DxLib::CheckHitKey(KEY_INPUT_F5)) {
			done = done_code::reload;
			break;
		}

		// ESC で終了
		if (DxLib::CheckHitKey(KEY_INPUT_ESCAPE)) {
			break;
		}
	}

	return done;
}

// スクリプトの初期化関数を呼ぶ
application::done_code application::call_init(std::ostringstream &message) {
	return ::call_luafn(*_dxLua, "Init", message, sol::as_args(_option.args));
}

// スクリプトの実行関数を呼ぶ
application::done_code application::call_run(std::ostringstream &message) {
	std::string watch = "";
	switch (_option.watch) {
	case watch_mode::reload: watch = "reload"; break;
	case watch_mode::swap: watch = "swap"; break;
	case watch_mode::none: watch = ""; break;
	default: watch = ""; break;
	}
	return ::call_luafn(*_dxLua, "Run", message, watch, _option.watch_interval);
}

// スクリプトの終了関数を呼ぶ
application::done_code application::call_end(std::ostringstream &message) {
	return ::call_luafn(*_dxLua, "End", message);
}

// Lua のセットアップ
bool application::setup_lua() {
	// Lua ステートの構築
	_state = std::make_unique<sol::state>(sol::state(sol::c_call<decltype(&application::panic), &application::panic>));

	// 標準ライブラリの展開
	_state->open_libraries(
		sol::lib::base,
		sol::lib::package,
		sol::lib::string,
		sol::lib::os,
		sol::lib::math,
		sol::lib::table,
		sol::lib::debug,
		sol::lib::bit32,
		sol::lib::io,
		sol::lib::ffi
	);

	// DxLua ライブラリの展開
	_dxLua = _state->require("dx", sol::c_call<decltype(&DxLua::open_library), &DxLua::open_library>);

	// 監視ファイルの追加
	if (_option.watch != watch_mode::none) {
		DxLua::add_watchee(*_dxLua, _option.watch_path);
	}

	// ベースパスの設定
	DxLua::set_base_path(*_dxLua, _option.base_path);

	return _dxLua->is<sol::table>();
}

// Lua の破棄
bool application::teardown_lua() {
	// DxLua ライブラリの破棄
	if (_dxLua) {
		// _dxLua.reset();
	}

	// Lua ステートの破棄
	if (_state) {
		_state->stack_clear();
		_state->collect_garbage();
		_state.reset();
	}
	return true;
}

// 引数の解析
bool application::parse_arguments(int argc, const char **argv) {
	// オプションのリセット
	_option = option{};
	_option.filename = application::default_filename;
	_option.path = _option.filename;

	// 引数の解析
	argagg::parser_results args;
	try {
		args = argparser.parse(argc, argv);

	} catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		return false;
	}

	// ファイル監視
	if (auto hotreload = args["hotreload"]; hotreload) {
		// ホットリロード
		_option.watch = watch_mode::reload;
		_option.watch_interval = hotreload.as<int>();

	} else if (auto hotswap = args["hotswap"]; hotswap) {
		// ホットスワップ
		_option.watch = watch_mode::swap;
		_option.watch_interval = hotswap.as<int>();
	}

	// コンソール使用
	if (args["console"]) {
		_option.console = true;
	}

	// フルスクリーン／ウィンドウモード
	if (args["fullscreen"]) {
		_option.window = false;

	} else if (args["window"]) {
		_option.window = true;
	}

	// バージョン
	if (args["version"]) {
		_option.version = true;
	}

	// ヘルプ
	if (args["help"]) {
		_option.help = true;
	}

	// パスの解析
	if (args.pos.size() > 0) {
		// 指定パス
		std::filesystem::path path = args.as<std::string>();
		_option.argpath = path;

		// 残りの引数を保持
		_option.args = args.all_as<std::string>();
		_option.args.erase(_option.args.begin());

		// 絶対パスに変換
		if (!path.is_absolute()) {
			path = std::filesystem::absolute(path);
		}

		// パスの対象別に読み込みパスの作成
		if (std::filesystem::is_directory(path)) {
			// ディレクトリ
			std::filesystem::current_path(path);
			_option.path = _option.filename;
			_option.watch_path = _option.path;

		} else if (std::filesystem::is_regular_file(path)) {
			// ファイル
			auto ext = path.extension();
			if (ext == ".lua") {
				// スクリプト
				_option.filename = path.filename().string();
				std::filesystem::current_path(path.remove_filename());
				_option.path = _option.filename;
				_option.watch_path = _option.path;

			} else if (ext == ".dxa") {
				// アーカイブ
				std::filesystem::current_path(path.remove_filename());
				auto archive = path.stem();
				_option.path = archive / _option.filename;
				_option.watch_path = path.filename();
				_option.base_path = archive;
			}
		}

	} else {
		// パスが指定されていない
		if (std::filesystem::exists(application::default_filename)) {
			// スクリプト main.lua があった
			_option.argpath = _option.path;
			_option.watch_path = _option.argpath;

		} else if (auto dirfilepath = application::default_directory / application::default_filename; std::filesystem::exists(dirfilepath)) {
			// game ディレクトリの下にスクリプト main.lua があった
			std::filesystem::current_path(application::default_directory);
			_option.argpath = dirfilepath;
			_option.watch_path = _option.argpath;

		} else if (std::filesystem::exists(application::default_archive)) {
			// アーカイブ game.dxa があった
			_option.path = application::default_directory / application::default_filename;
			_option.argpath = _option.path;
			_option.watch_path = application::default_archive;
			_option.base_path = application::default_archive.stem();
		}
	}

	return true;
}

// コンソールのセットアップ
bool application::setup_console() {
	bool succeeded = false;

#ifdef _WIN32
	// コンソールの使用
	if (_option.console && !has_console()) {
		if (AttachConsole(ATTACH_PARENT_PROCESS)) {
			// 既存のコンソールをアタッチ
			_attached_console = true;

		} else {
			// 新規でコンソールを作成
			_alloced_console = AllocConsole();
			if (_alloced_console) {
				SetConsoleTitle(TEXT("DxLua Console"));
			}
		}

		// 文字コード設定
		SetConsoleOutputCP(CP_UTF8);
	}

	// 標準入出力関数から使用できるように設定
	if (has_console()) {
		// アタッチ／作成したコンソールへ標準入出力を設定
		FILE *fp;
		freopen_s(&fp, "CONIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
	}

	succeeded = has_console();
#else
	succeeded = true;
#endif // _WIN32

	// バージョン情報
	if (_option.version) {
		std::cout << "DxLua 0.1.0" << std::endl;
	}

	// ヘルプ
	if (_option.help) {
		std::cout << u8"使い方:" << std::endl;
		std::cout << u8"    DxLua [オプション...] <パス> [<サブ引数>...]" << std::endl;
		std::cout << std::endl;
		std::cout << u8"引数:" << std::endl;
		std::cout << u8"    <パス>" << std::endl;
		std::cout << u8"        Lua スクリプトファイル" << std::endl;
		std::cout << u8"        または main.lua が入っているディレクトリ" << std::endl;
		std::cout << u8"        または main.lua が入っている dxa ファイル" << std::endl;
		std::cout << u8"    <サブ引数>" << std::endl;
		std::cout << u8"        スクリプトに渡される任意の数の引数" << std::endl;
		std::cout << std::endl;
		std::cout << u8"オプション:" << std::endl;
		std::cout << argparser << std::endl;
	}

	return succeeded;
}

// コンソールの開放
bool application::teardown_console() {
#ifdef _WIN32
	// コンソール開放
	if (has_console()) {
		FreeConsole();
		_attached_console = false;
		_alloced_console = false;
	}
#endif
	return true;
}

// エンジンの初期設定
bool application::configure_engine() {
	DxLib::DxLib_GlobalStructInitialize();
	DxLib::SetMainWindowText(L"DxLua");
	DxLib::ChangeWindowMode(_option.window);
	DxLib::SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
	//DxLib::SetOutApplicationLogValidFlag(FALSE);

	return true;
}

// エンジンのセットアップ
bool application::setup_engine() {
	bool succeeded = false;

	if (_initialized_engine) {
		// 初期化済み
		succeeded = true;

	} else if (DxLib::DxLib_Init() == -1) {
		// ＤＸライブラリの初期化に失敗

	} else {
		// 成功
		_initialized_engine = succeeded = true;
	}

	return succeeded;
}

// エンジンの破棄
bool application::teardown_engine() {
	bool succeeded = false;

	if (!_initialized_engine) {
		// 未初期化
		succeeded = true;

	} else if (DxLib::DxLib_End() == -1) {
		// ＤＸライブラリの終了に失敗

	} else {
		// 成功
		_initialized_engine = false;
		succeeded = true;
	}

	return succeeded;
}

// スクリプトの読み込み
bool application::load_script(std::ostringstream &message) {
	bool succeeded = false;

	// パス
	auto &path = _option.path;

	// ローカル／アーカイブからファイルを探す
	std::string strTmp1 = path.string().c_str(); 
	std::string strTmp2 = path.filename().string().c_str(); 
	auto size = DxLib::FileRead_size(std::wstring(strTmp1.begin(),strTmp1.end()).c_str());
	if (size < 0) {
		// 見つからなかった

	} else {
		// ローカル／アーカイブからファイルを開く
		auto file = DxLib::FileRead_open(std::wstring(strTmp1.begin(),strTmp1.end()).c_str());

		// バッファ
		std::vector<std::byte> buffer;
		buffer.resize(size);

		// ファイルの内容を取得
		if (DxLib::FileRead_read(buffer.data(), size, file) >= 0) {
			// 内容を Lua として読み込み
			auto result = _state->load_buffer((const char *)buffer.data(), buffer.size(), path.filename().string().c_str());
			if (result.valid()) {
				// 読み込みに成功したので実行
				auto presult = result.call();

				if (presult.valid()) {
					// 成功
					succeeded = true;

				} else {
					// 実行に失敗したのでエラーメッセージを返す
					sol::error err = presult;
					message << err.what() << std::endl;
				}

			} else {
				// 読み込みに失敗したのでエラーメッセージを返す
				sol::error err = result;
				message << err.what() << std::endl;
			}
		}

		// ファイルを閉じる
		DxLib::FileRead_close(file);
	}

	return succeeded;
}

// Lua 用パニック関数
void application::panic(sol::optional<std::string> maybe_msg) {
	std::cerr << "Lua is in a panic state and will now abort() the application" << std::endl;
	if (maybe_msg) {
		const std::string &msg = maybe_msg.value();
		std::cerr << "\terror message: " << msg << std::endl;
	}
	// When this function exits, Lua will exhibit default behavior and abort()
}

} // namespace remyroez
