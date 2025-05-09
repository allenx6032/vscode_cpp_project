﻿#include <DxLib.h>
#include <lua_includes.h>
#include <sol.hpp>

#include "luaapi.h"

namespace DxLua::detail {

void port_type(sol::state_view &lua, sol::table &t) {

	// MATRIX
	{
		// ユーザー型定義
		auto usertype = t.new_usertype<DxLib::MATRIX>(
			"MATRIX",
			"m", sol::property([](DxLib::MATRIX &self) { return std::ref(self); }),
			sol::meta_function::index, [](const DxLib::MATRIX &self, sol::stack_object key, sol::this_state L) {
				int index = 1;
				if (auto maybe_numeric_key = key.as<sol::optional<int>>()) {
					index = *maybe_numeric_key;
				}
				if (index <= 0) {
					index = 1;

				} else if (index > sizeof(DxLib::MATRIX::m) / sizeof(DxLib::MATRIX::m[0])) {
					index = sizeof(DxLib::MATRIX::m) / sizeof(DxLib::MATRIX::m[0]);
				}
				return std::ref(self.m[index - 1]);
			},
			sol::meta_function::length, [](const DxLib::MATRIX &self) {
				return sizeof(DxLib::MATRIX::m) / sizeof(DxLib::MATRIX::m[0]);
			}
			);

		// ユーザー型をテーブルとして取得
		sol::table table = t["MATRIX"];

		// メタテーブルの作成
		sol::table metatable = table[sol::metatable_key] = lua.create_table();

		// 専用の new 関数を用意
		table["new"] = [](sol::object arg) {
			DxLib::MATRIX matrix;
			memset(&matrix, 0, sizeof(matrix));
			if (!arg.is<sol::table>()) {
				// テーブルではない

			} else if (sol::table argt = arg.as<sol::table>()) {
				for (size_t i = 0; i < sizeof(DxLib::MATRIX::m) / sizeof(DxLib::MATRIX::m[0]); ++i) {
					if (sol::object obj = argt[i + 1]; obj.is<sol::table>()) {
						sol::table t = obj;
						for (size_t j = 0; j < sizeof(DxLib::MATRIX::m[0]) / sizeof(DxLib::MATRIX::m[0][0]); ++j) {
							matrix.m[i][j] = t.get_or(j + 1, 0.f);
						}
					}
				}
			}
			return matrix;
		};

		// ユーザー型テーブルの呼び出しで new を呼ぶように対応
		metatable["__call"] = [](sol::stack_object self, sol::variadic_args va) { return sol::table(self)["new"](va); };
	}

	// VECTOR
	{
		// ユーザー型定義
		auto VECTOR = t.new_usertype<DxLib::VECTOR>(
			"VECTOR",
			"x", &DxLib::VECTOR::x,
			"y", &DxLib::VECTOR::y,
			"z", &DxLib::VECTOR::z,
			sol::meta_function::to_string, [](const DxLib::VECTOR &v) {
				std::ostringstream ost;
				ost << "{ x = " << v.x << ", y = " << v.y << ", z = " << v.z << " }";
				return ost.str();
			}
		);

		// ユーザー型をテーブルとして取得
		sol::table tableVECTOR = t["VECTOR"];

		// メタテーブルの作成
		sol::table metaVECTOR = tableVECTOR[sol::metatable_key] = lua.create_table();

		// 専用の new 関数を用意
		tableVECTOR["new"] = [](sol::variadic_args va) {
			if (va.leftover_count() > 0 && va[0].is<sol::table>()) {
				sol::table t = va[0].as<sol::table>();
				if (t[1].get_type() == sol::type::number) {
					return DxLib::VECTOR{
						t[1].get_or<float>(0),
						t[2].get_or<float>(0),
						t[3].get_or<float>(0)
					};

				} else {
					return DxLib::VECTOR{
						t["x"].get_or<float>(0),
						t["y"].get_or<float>(0),
						t["z"].get_or<float>(0)
					};
				}

			} else {
				return DxLib::VECTOR{
					va.leftover_count() > 0 && va[0].is<float>() ? va[0].as<float>() : 0,
					va.leftover_count() > 1 && va[0].is<float>() ? va[1].as<float>() : 0,
					va.leftover_count() > 2 && va[0].is<float>() ? va[2].as<float>() : 0
				};
			}
		};

		// ユーザー型テーブルの呼び出しで new を呼ぶように対応
		metaVECTOR["__call"] = [](sol::stack_object self, sol::variadic_args va) { return sol::table(self)["new"](va); };

		// ライブラリに追加
		DXLUA_PORT_EX(t, FLOAT3, VECTOR);
	}

	// COLOR_F
	{
		// 定義する型
		using type = DxLib::COLOR_F;

		// ユーザー型定義
		auto usertype = t.new_usertype<type>(
			"COLOR_F",
			"r", &type::r,
			"g", &type::g,
			"b", &type::b,
			"a", &type::a,
			sol::meta_function::to_string, [](const type &v) {
				std::ostringstream ost;
				ost << "{ r = " << v.r << ", g = " << v.g << ", b = " << v.b << ", a = " << v.a << " }";
				return ost.str();
			}
		);

		// ユーザー型をテーブルとして取得
		sol::table table = t["COLOR_F"];

		// メタテーブルの作成
		sol::table metatable = table[sol::metatable_key] = lua.create_table();

		// 専用の new 関数を用意
		table["new"] = [](sol::variadic_args va) {
			if (va.leftover_count() > 0 && va[0].is<sol::table>()) {
				sol::table t = va[0].as<sol::table>();
				if (t[1].get_type() == sol::type::number) {
					return type {
						t[1].get_or<float>(0),
						t[2].get_or<float>(0),
						t[3].get_or<float>(0),
						t[4].get_or<float>(0)
					};

				} else {
					return type {
						t["r"].get_or<float>(0),
						t["g"].get_or<float>(0),
						t["b"].get_or<float>(0),
						t["a"].get_or<float>(0)
					};
				}

			} else {
				return type {
					va_get(va, 0, 0.f),
					va_get(va, 1, 0.f),
					va_get(va, 2, 0.f),
					va_get(va, 3, 0.f)
				};
			}
		};

		// ユーザー型テーブルの呼び出しで new を呼ぶように対応
		metatable["__call"] = [](sol::stack_object self, sol::variadic_args va) { return sol::table(self)["new"](va); };
	}

	// COLOR_U8
	{
		// 定義する型
		using type = DxLib::COLOR_U8;

		// ユーザー型定義
		auto usertype = t.new_usertype<type>(
			"COLOR_U8",
			"b", &type::b,
			"g", &type::g,
			"r", &type::r,
			"a", &type::a
		);

		// ユーザー型をテーブルとして取得
		sol::table table = t["COLOR_U8"];

		// メタテーブルの作成
		sol::table metatable = table[sol::metatable_key] = lua.create_table();

		// 専用の new 関数を用意
		table["new"] = [](sol::object arg) {
			type instance;
			memset(&instance, 0, sizeof(instance));
			if (!arg.is<sol::table>()) {
				// テーブルではない

			} else if (sol::table argt = arg.as<sol::table>()) {
				if (argt[1].get_type() == sol::type::number) {
					instance.b = argt[1].get_or<BYTE>(0);
					instance.g = argt[2].get_or<BYTE>(0);
					instance.r = argt[3].get_or<BYTE>(0);
					instance.a = argt[4].get_or<BYTE>(0);

				} else {
					instance.b = argt["b"].get_or<BYTE>(0);
					instance.g = argt["g"].get_or<BYTE>(0);
					instance.r = argt["r"].get_or<BYTE>(0);
					instance.a = argt["a"].get_or<BYTE>(0);
				}
			}
			return instance;
		};

		// ユーザー型テーブルの呼び出しで new を呼ぶように対応
		metatable["__call"] = [](sol::stack_object self, sol::variadic_args va) { return sol::table(self)["new"](va); };
	}

	// FLOAT4
	{
		// 定義する型
		using type = DxLib::FLOAT4;

		// ユーザー型定義
		auto usertype = t.new_usertype<type>(
			"FLOAT4",
			"x", &type::x,
			"y", &type::y,
			"z", &type::z,
			"w", &type::w,
			sol::meta_function::to_string, [](const type &v) {
				std::ostringstream ost;
				ost << "{ x = " << v.x << ", y = " << v.y << ", z = " << v.z << ", w = " << v.w << " }";
				return ost.str();
			}
		);

		// ユーザー型をテーブルとして取得
		sol::table table = t["FLOAT4"];

		// メタテーブルの作成
		sol::table metatable = table[sol::metatable_key] = lua.create_table();

		// 専用の new 関数を用意
		table["new"] = [](sol::variadic_args va) {
			if (va.leftover_count() > 0 && va[0].is<sol::table>()) {
				sol::table t = va[0].as<sol::table>();
				if (t[1].get_type() == sol::type::number) {
					return type{
						t[1].get_or<float>(0),
						t[2].get_or<float>(0),
						t[3].get_or<float>(0),
						t[4].get_or<float>(0)
					};

				} else {
					return type{
						t["x"].get_or<float>(0),
						t["y"].get_or<float>(0),
						t["z"].get_or<float>(0),
						t["w"].get_or<float>(0)
					};
				}

			} else {
				return type{
					va_get(va, 0, 0.f),
					va_get(va, 1, 0.f),
					va_get(va, 2, 0.f),
					va_get(va, 3, 0.f)
				};
			}
		};

		// ユーザー型テーブルの呼び出しで new を呼ぶように対応
		metatable["__call"] = [](sol::stack_object self, sol::variadic_args va) { return sol::table(self)["new"](va); };
	}

	// VERTEX3D
	{
		// 定義する型
		using type = DxLib::VERTEX3D;

		// ユーザー型定義
		auto usertype = t.new_usertype<type>(
			"VERTEX3D",
			"pos", &type::pos,
			"norm", &type::norm,
			"dif", &type::dif,
			"spc", &type::spc,
			"u", &type::u,
			"v", &type::v,
			"su", &type::su,
			"sv", &type::sv
		);

		// ユーザー型をテーブルとして取得
		sol::table table = t["VERTEX3D"];

		// メタテーブルの作成
		sol::table metatable = table[sol::metatable_key] = lua.create_table();

		// 専用の new 関数を用意
		table["new"] = [](sol::object arg) {
			type instance;
			memset(&instance, 0, sizeof(instance));
			if (!arg.is<sol::table>()) {
				// テーブルではない

			} else if (sol::table argt = arg.as<sol::table>()) {
			}
			return instance;
		};

		// ユーザー型テーブルの呼び出しで new を呼ぶように対応
		metatable["__call"] = [](sol::stack_object self, sol::variadic_args va) { return sol::table(self)["new"](va); };
	}

	// MV1_COLL_RESULT_POLY
	{
		// 定義する型
		using type = DxLib::MV1_COLL_RESULT_POLY;

		// ユーザー型定義
		auto usertype = t.new_usertype<type>(
			"MV1_COLL_RESULT_POLY",
			"HitFlag", sol::property([](type &self) { return self.HitFlag != FALSE; }),
			"HitPosition", sol::readonly(&type::HitPosition),
			"FrameIndex", sol::readonly(&type::FrameIndex),
			"MeshIndex", sol::readonly(&type::MeshIndex),
			"PolygonIndex", sol::readonly(&type::PolygonIndex),
			"MaterialIndex", sol::readonly(&type::MaterialIndex),
			"Position", sol::property([](type &self) { return std::ref(self.Position); }),
			"Normal", sol::readonly(&type::Normal),
			"PositionWeight", sol::property([](type &self) { return std::ref(self.PositionWeight); }),
			"PosMaxWeightFrameIndex", sol::property([](type &self) { return std::ref(self.PosMaxWeightFrameIndex); })
		);

		// ユーザー型をテーブルとして取得
		sol::table table = t["MV1_COLL_RESULT_POLY"];

		// メタテーブルの作成
		sol::table metatable = table[sol::metatable_key] = lua.create_table();

		// 専用の new 関数を用意
		table["new"] = [](sol::object arg) {
			type instance;
			memset(&instance, 0, sizeof(instance));
			if (!arg.is<sol::table>()) {
				// テーブルではない

			} else if (sol::table argt = arg.as<sol::table>()) {
			}
			return instance;
		};

		// ユーザー型テーブルの呼び出しで new を呼ぶように対応
		metatable["__call"] = [](sol::stack_object self, sol::variadic_args va) { return sol::table(self)["new"](va); };
	}
	
	// MV1_COLL_RESULT_POLY_DIM
	{
		// 定義する型
		using type = DxLib::MV1_COLL_RESULT_POLY_DIM;

		// ユーザー型定義
		auto usertype = t.new_usertype<type>(
			"MV1_COLL_RESULT_POLY_DIM",
			"HitNum", sol::readonly(&type::HitNum),
			"Dim", sol::property([](type &self) {
				return std::ref(self);
			}),
			sol::meta_function::index, [](const type &self, sol::stack_object key, sol::this_state L) {
				int index = 1;
				if (auto maybe_numeric_key = key.as<sol::optional<int>>()) {
					index = *maybe_numeric_key;
				}
				if (index <= 0) {
					index = 1;

				} else if (index > self.HitNum) {
					index = self.HitNum;
				}
				return std::ref(self.Dim[index - 1]);
			},
			sol::meta_function::length, [](const type &self) {
				return self.HitNum;
			}
		);
		
		// ユーザー型をテーブルとして取得
		sol::table table = t["MV1_COLL_RESULT_POLY_DIM"];

		// メタテーブルの作成
		sol::table metatable = table[sol::metatable_key] = lua.create_table();

		// 専用の new 関数を用意
		table["new"] = [](sol::object arg) {
			type instance;
			memset(&instance, 0, sizeof(instance));
			if (!arg.is<sol::table>()) {
				// テーブルではない

			} else if (sol::table argt = arg.as<sol::table>()) {
			}
			return instance;
		};

		// ユーザー型テーブルの呼び出しで new を呼ぶように対応
		metatable["__call"] = [](sol::stack_object self, sol::variadic_args va) { return sol::table(self)["new"](va); };
	}

	// MATERIALPARAM
	{
		// 定義する型
		using type = DxLib::MATERIALPARAM;

		// ユーザー型定義
		auto usertype = t.new_usertype<type>(
			"MATERIALPARAM",
			"Diffuse", &type::Diffuse,
			"Ambient", &type::Ambient,
			"Specular", &type::Specular,
			"Emissive", &type::Emissive,
			"Power", &type::Power
		);

		// ユーザー型をテーブルとして取得
		sol::table table = t["MATERIALPARAM"];

		// メタテーブルの作成
		sol::table metatable = table[sol::metatable_key] = lua.create_table();

		// 専用の new 関数を用意
		table["new"] = [](sol::object arg) {
			type instance;
			memset(&instance, 0, sizeof(instance));
			if (!arg.is<sol::table>()) {
				// テーブルではない

			} else if (sol::table argt = arg.as<sol::table>()) {
			}
			return instance;
		};

		// ユーザー型テーブルの呼び出しで new を呼ぶように対応
		metatable["__call"] = [](sol::stack_object self, sol::variadic_args va) { return sol::table(self)["new"](va); };
	}

	// HITRESULT_LINE
	{
		// 定義する型
		using type = DxLib::HITRESULT_LINE;

		// ユーザー型定義
		auto usertype = t.new_usertype<type>(
			"HITRESULT_LINE",
			"HitFlag", sol::property([](type &self) { return self.HitFlag != FALSE; }),
			"Position", sol::readonly(&type::Position)
			);

		// ユーザー型をテーブルとして取得
		sol::table table = t["HITRESULT_LINE"];

		// メタテーブルの作成
		sol::table metatable = table[sol::metatable_key] = lua.create_table();

		// 専用の new 関数を用意
		table["new"] = [](sol::object arg) {
			type instance;
			memset(&instance, 0, sizeof(instance));
			if (!arg.is<sol::table>()) {
				// テーブルではない

			} else if (sol::table argt = arg.as<sol::table>()) {
			}
			return instance;
		};

		// ユーザー型テーブルの呼び出しで new を呼ぶように対応
		metatable["__call"] = [](sol::stack_object self, sol::variadic_args va) { return sol::table(self)["new"](va); };
	}

	// DINPUT_JOYSTATE
	{
		// ユーザー型定義
		auto DINPUT_JOYSTATE = t.new_usertype<DxLib::DINPUT_JOYSTATE>(
			"DINPUT_JOYSTATE",
			"X", &DxLib::DINPUT_JOYSTATE::X,
			"Y", &DxLib::DINPUT_JOYSTATE::Y,
			"Z", &DxLib::DINPUT_JOYSTATE::Z,
			"Rx", &DxLib::DINPUT_JOYSTATE::Rx,
			"Ry", &DxLib::DINPUT_JOYSTATE::Ry,
			"Rz", &DxLib::DINPUT_JOYSTATE::Rz,
			"Slider", sol::property([](DxLib::DINPUT_JOYSTATE &self) { return std::ref(self.Slider); }),
			"POV", sol::property([](DxLib::DINPUT_JOYSTATE &self) { return std::ref(self.POV); }),
			"Buttons", sol::property([](DxLib::DINPUT_JOYSTATE &self) { return std::ref(self.Buttons); }),
			sol::meta_function::equal_to, [](const DxLib::DINPUT_JOYSTATE &self, const DxLib::DINPUT_JOYSTATE &other) {
				return
					self.X == other.X
					&& self.Y == other.Y
					&& self.Z == other.Z
					&& self.Rx == other.Rx
					&& self.Ry == other.Ry
					&& self.Rz == other.Rz
					&& (memcmp(self.Slider, other.Slider, sizeof(DxLib::DINPUT_JOYSTATE::Slider)) == 0)
					&& (memcmp(self.POV, other.POV, sizeof(DxLib::DINPUT_JOYSTATE::POV)) == 0)
					&& (memcmp(self.Buttons, other.Buttons, sizeof(DxLib::DINPUT_JOYSTATE::Buttons)) == 0)
				;
			}
		);

		// ユーザー型をテーブルとして取得
		sol::table table = t["DINPUT_JOYSTATE"];

		// メタテーブルの作成
		sol::table metatable = table[sol::metatable_key] = lua.create_table();

		// 専用の new 関数を用意
		table["new"] = [](sol::object arg) {
			// 空インスタンス
			DxLib::DINPUT_JOYSTATE instance;
			memset(&instance, 0, sizeof(instance));

			if (!arg.is<sol::table>()) {
				// テーブルではない

			} else if (sol::table argt = arg.as<sol::table>()) {
				// テーブルからコピー
				instance.X = argt["X"].get_or(0);
				instance.Y = argt["Y"].get_or(0);
				instance.Z = argt["Z"].get_or(0);
				instance.Rx = argt["Rx"].get_or(0);
				instance.Ry = argt["Ry"].get_or(0);
				instance.Rz = argt["Rz"].get_or(0);

				// Slider
				if (sol::object obj = argt["Slider"]; obj.is<sol::table>()) {
					sol::table t = obj;
					for (int i = 0; i < 2; ++i) {
						instance.Slider[i] = t[i + 1].get_or(0);
					}
				}

				// POV
				if (sol::object obj = argt["POV"]; obj.is<sol::table>()) {
					sol::table t = obj;
					for (int i = 0; i < 4; ++i) {
						instance.POV[i] = t[i + 1].get_or(0U);
					}
				}

				// Buttons
				if (sol::object obj = argt["Buttons"]; obj.is<sol::table>()) {
					sol::table t = obj;
					for (int i = 0; i < 32; ++i) {
						instance.Buttons[i] = t[i + 1].get_or(0U);
					}
				}
			}
			return instance;
		};

		// ユーザー型テーブルの呼び出しで new を呼ぶように対応
		metatable["__call"] = [](sol::stack_object self, sol::variadic_args va) { return sol::table(self)["new"](va); };
	}

	// IPDATA
	{
		// ユーザー型定義
		auto IPDATA = t.new_usertype<DxLib::IPDATA>(
			"IPDATA",
			"d1", &DxLib::IPDATA::d1,
			"d2", &DxLib::IPDATA::d2,
			"d3", &DxLib::IPDATA::d3,
			"d4", &DxLib::IPDATA::d4,
			sol::meta_function::to_string, [](const DxLib::IPDATA &ipdata) {
				std::ostringstream ost;
				ost
					<< "{ d1 = " << ipdata.d1
					<< ", d2 = " << ipdata.d2
					<< ", d3 = " << ipdata.d3
					<< ", d4 = " << ipdata.d4
					<< " }";
				return ost.str();
			}
		);
	}

	// IPDATA_IPv6
	{
		// ユーザー型定義
		auto IPDATA_IPv6 = t.new_usertype<DxLib::IPDATA_IPv6>(
			"IPDATA_IPv6",
			"ScopeID", &DxLib::IPDATA_IPv6::ScopeID,
			sol::meta_function::to_string, [](const DxLib::IPDATA_IPv6 &ipdata) {
				std::ostringstream ost;
				ost
					<< "{ ScopeID = " << ipdata.ScopeID
					<< " }";
				return ost.str();
			}
		);
	}
}

} // namespace DxLua::detail
