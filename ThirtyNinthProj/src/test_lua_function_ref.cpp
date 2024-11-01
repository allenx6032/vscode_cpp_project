#include "lua_tinker.h"
#include"test.h"


lua_tinker::lua_function_ref g_lua_func_ref;
LUA_TEST(luafunction_ref)
{
	g_test_func_set["test_lua_luafunction_ref_1"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_luafunction_ref_1()
					local upval = 0;
					local localtest = function (intval)
						upval = upval + intval;
						return upval;
					end
					return localtest;
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		lua_tinker::lua_function_ref lua_func = lua_tinker::call<decltype(lua_func)>(L, "test_lua_luafunction_ref_1");
		lua_func.invoke<int>(7);
		return 14 == lua_func.invoke<int>(7);
	};
	g_test_func_set["test_lua_luafunction_ref_2"] = [L]()->bool
	{
		std::string luabuf =
			R"(function test_lua_luafunction_ref_2_1()
					local upval = 0;
					local localtest = function (intval)
						upval = upval + intval;
						return upval;
					end
					return localtest;
				end
				function test_lua_luafunction_ref_2_2(func)
					func(7);
					return func(7);
				end
			)";
		lua_tinker::dostring(L, luabuf.c_str());

		lua_tinker::lua_function_ref lua_func = lua_tinker::call<decltype(lua_func)>(L, "test_lua_luafunction_ref_2_1");
		return 14 == lua_tinker::call<int>(L, "test_lua_luafunction_ref_2_2", lua_func);
	};


	g_test_func_set["test_lua_luafunction_ref_3"] = [L]()->bool
	{
		std::string luabuf =
			R"( g_test_func_table={test=1,};
				function g_test_func_table.func1(val)
					return val + 1;
				end;
				
			)";
		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::table_onstack func_table(L, "g_test_func_table");
		lua_tinker::lua_function_ref lua_func = func_table.get<decltype(lua_func)>("func1");
		return 7 == lua_func.invoke<int>(6);
	};


	g_test_func_set["test_lua_luafunction_ref_4"] = [L]()->bool
	{
		std::string luabuf =
			R"( return function (val)
					return val + 1;
				end;
			)";
		auto lua_func = lua_tinker::dostring<lua_tinker::lua_function_ref>(L, luabuf.c_str());
		return 7 == lua_func.invoke<int>(6);
	};

	

	std::string luabuf =
		R"( 	function g_lua_func_test(val)
					return val + 1;
				end;
				
			)";
	lua_tinker::dostring(L, luabuf.c_str());
	g_test_func_set["test_lua_luafunction_ref_5"] = [L]()->bool
	{
		if (g_lua_func_ref.empty())
		{
			g_lua_func_ref = lua_tinker::get<decltype(g_lua_func_ref)>(L, "g_lua_func_test");
			lua_tinker::register_lua_close_callback(L, [](lua_State* L)
														{ g_lua_func_ref.reset(); });
		}
		return g_lua_func_ref.invoke<int>(8) == 9;
	};

		

	g_test_func_set["test_lua_luafunction_ref_6"] = [L]()->bool
	{
		std::string luabuf =
			R"(
				test_lua_luafunction_ref_6 = {
					[1]=10, test1=5,
					testFunc1 = function(param1)
						return {
							[1]=3, test2=22,
							testFunc2 = function()
								return param1;
							end,
						};
					end,
				};
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::table_onstack table(L, "test_lua_luafunction_ref_6");
		lua_tinker::lua_function_ref my_func_ref = table.get<decltype(my_func_ref)>("testFunc1");

		//not a good idea, just for test
		lua_tinker::table_ref tt_ref = my_func_ref.invoke<lua_tinker::table_ref>("test_upval");		//add table to register, erase table on stack
		lua_tinker::table_onstack tt = tt_ref.push_table_to_stack();	//push table from register to stack

		//should be use this way
		//lua_tinker::lua_function_ref<lua_tinker::table_onstack> my_func_ref = table.get<decltype(my_func_ref)>("testFunc1");
		//lua_tinker::table_onstack tt = my_func_ref("test_upval");
		//lua_tinker::table_ref tt_ref = lua_tinker::table_ref::make_table_ref(tt);

		lua_tinker::lua_function_ref ref = tt.get<decltype(ref)>("testFunc2");
		return tt.get<int>(1) == 3 && ref.invoke<std::string>() == "test_upval";
	};


	g_test_func_set["test_lua_luafunction_ref_7"] = [L]()->bool
	{
		std::string luabuf =
			R"(
				test_lua_luafunction_ref_7 = {
					[1]=10, test1=5,
					testFunc1 = function(param1)
						return {
							[1]=3, test2=22,
							testFunc2 = function()
								return {
									{key1="key1_1", key2="key1_2", key3="key1_3"},
									{key1="key2_1", key2="key2_2", key3="key2_3"},
								};
							end,
						};
					end,
				};
			)";

		lua_tinker::dostring(L, luabuf.c_str());
		lua_tinker::table_onstack table(L, "test_lua_luafunction_ref_7");
		lua_tinker::lua_function_ref my_func_ref = table.get<decltype(my_func_ref)>("testFunc1");

		lua_tinker::table_onstack tt = my_func_ref.invoke<lua_tinker::table_onstack>("test_upval");

		lua_tinker::lua_function_ref fun2 = tt.get<decltype(fun2)>("testFunc2");
		lua_tinker::table_onstack tt2 = fun2.invoke<lua_tinker::table_onstack>();
		lua_tinker::table_onstack tt2_1 = tt2.get<decltype(tt2_1)>(1);
		std::string key1_1 = tt2_1.get<std::string>("key1");

		std::vector<std::map<std::string, std::string>> datamap = tt2.to_container<decltype(datamap)>();
		

		return key1_1 == "key1_1" && datamap[0]["key2"] == "key1_2" && datamap[1]["key3"] == "key2_3";
	};

		g_test_func_set["test_lua_luafunction_ref_8"] = [L]()->bool
	{
		std::string luabuf_1 =
		R"( local local_table = {};
			function local_table.Test(val)
				return val;
			end
			return function (name,...)
				local func = local_table[name];
				return func(...);
			end

		)";
		std::string luabuf_2 =
		R"( local local_table = {};
			function local_table.Test1(val)
				return val+1;
			end
			return function (name,...)
				local func = local_table[name];
				return func(...);
			end

		)";
		lua_tinker::lua_function_ref func_in_lua_1 = lua_tinker::dostring<lua_tinker::lua_function_ref>(L, luabuf_1.c_str());
		lua_tinker::lua_function_ref func_in_lua_2 = lua_tinker::dostring<lua_tinker::lua_function_ref>(L, luabuf_2.c_str());
		bool result_1 = func_in_lua_1.invoke<int>("Test", 1,2,3) == 1;
		bool result_2 = func_in_lua_2.invoke<int>("Test1", 1,2,3) == 2;
		return result_1 && result_2;
	};

}