#include "botcore_main.hxx"
#include <stdexcept>
#include "amx.h"
#include "amxaux.h"
#include <botcore_easy_json.hxx>
#include <botcore_logprint.hxx>
#include "botcore_config_controller.hxx"
#include "botcore_plugin_manager.hxx"

#define CONFIG_PATH "./configCore.json"

void clearString(std::string &s)
{
	std::fill(s.begin(), s.end(), '\0');
	s.clear();
}

namespace amx_natives
{
	logprint AmxNativesLogs("AMX", "./logs/amx_logs.log");
	static cell AMX_NATIVE_CALL n_logi(AMX *amx, const cell *params)
	{
		// int numparams = (int)(params[0] / sizeof(cell)) - 4;
		// printf("- %d:%d:%d\n", params[0], (1 * sizeof(cell)), numparams);
		// char *str;
		// amx_StrParam_Type(amx, params[1], str, char *);
		// if (str != nullptr)
		// 	AmxNativesLogs.LOGI("%s", str);
		cell *_cstr;
		int _length;
		char *_result;
		amx_GetAddr(amx, params[1], &_cstr);
		amx_StrLen(_cstr, &_length);

		printf("LOG_E: Адрес строки: %p, длина: %d\n", (void *)_cstr, _length); // Отладка адреса и длины строки

		if (_length > 0 && (_result = (char *)malloc((_length + 1) * sizeof(*_result))) != NULL)
		{
			amx_GetString(_result, _cstr, 0, _length + 1);
			// printf("%s\n", _result);
			AmxNativesLogs.LOGI("%s", _result);
			free(_result);
		}
		return 1;
	}
	static cell AMX_NATIVE_CALL n_loge(AMX *amx, const cell *params)
	{
		// int numparams = (int)(params[0] / sizeof(cell)) - 4;
		// printf("- %d:%d:%d\n", params[0], (1 * sizeof(cell)), numparams);
		//  char *str = nullptr;
		//  amx_StrParam_Type(amx, params[1], str, char *);
		//  if (str != nullptr)
		//  	AmxNativesLogs.LOGE(str);
		cell *_cstr;
		int _length;
		char *_result;
		amx_GetAddr(amx, params[1], &_cstr);
		amx_StrLen(_cstr, &_length);

		printf("LOG_I: Адрес строки: %p, длина: %d\n", (void *)_cstr, _length); // Отладка адреса и длины строки

		if (_length > 0 && (_result = (char *)malloc((_length + 1) * sizeof(*_result))) != NULL)
		{
			amx_GetString(_result, _cstr, 0, _length + 1);
			// printf("%s\n", _result);
			AmxNativesLogs.LOGE("%s", _result);
			free(_result);
		}
		return 1;
	}

	const AMX_NATIVE_INFO bstring_Natives[] = {
		{"LOGI", n_logi},
		{"LOGE", n_loge},
		{NULL, NULL}
		/* terminator */
	};
}

int main()
{
	logprint CoreLogs("CORE", "./logs/core.log");

	PluginManager pluginManager;

	std::string pluginTestPath = "./libplugin.dylib";

	if (pluginManager.loadPlugin(pluginTestPath))
	{
		pluginManager.executePluginFunction("pluginFunction");
		pluginManager.unloadPlugin();
	}

	std::string configVers,
		pluginsPath,
		mainScript,
		plugins;
	bool hotReloadAMX;
	ConfigManager &config = ConfigManager::getInstance();
	try
	{
		int response_ej = easy_json::file_get_value_string(CONFIG_PATH, "configVersion", configVers);
		if (response_ej != 0)
		{
			CoreLogs.LOGE("Error extracting config path (configVersion): %d", response_ej);
		}
		else
			config.setConfig("configVersion", configVers, ConfigManager::AccessType::RO);

		response_ej = easy_json::file_get_value_string(CONFIG_PATH, "pluginsPath", pluginsPath);
		if (response_ej != 0)
		{
			CoreLogs.LOGE("Error extracting config path (pluginsPath): %d", response_ej);
		}
		else
			config.setConfig("pluginsPath", pluginsPath, ConfigManager::AccessType::RO);

		response_ej = easy_json::file_get_value_string(CONFIG_PATH, "mainScript", mainScript);
		if (response_ej != 0)
		{
			CoreLogs.LOGE("Error extracting config path (mainScript): %d", response_ej);
		}
		else
			config.setConfig("mainScript", mainScript, ConfigManager::AccessType::RO);

		response_ej = easy_json::file_get_value_string(CONFIG_PATH, "plugins", mainScript);
		if (response_ej != 0)
		{
			CoreLogs.LOGE("Error extracting config path (plugins): %d", response_ej);
		}
		else
			config.setConfig("plugins", plugins, ConfigManager::AccessType::RO);

		response_ej = easy_json::file_get_value_bool(CONFIG_PATH, "hotReloadAMX", hotReloadAMX);
		if (response_ej != 0)
		{
			CoreLogs.LOGE("Error extracting config path (hotReloadAMX): %d", response_ej);
		}
		else
			config.setConfig("hotReloadAMX", hotReloadAMX, ConfigManager::AccessType::RO);
	}
	catch (const std::exception &e)
	{
		CoreLogs.LOGE("Error: %s", e.what());
	}

	clearString(configVers);
	clearString(pluginsPath);
	clearString(mainScript);
	clearString(plugins);

	AMX amx;
	cell ret = 0;
	int err;

	try
	{
		long memsize = aux_ProgramSize((char *)config.getConfig("mainScript").c_str());
		if (memsize == 0)
		{
			return -1;
		}

		void *program = malloc(memsize);

		if (program == NULL)
		{
			return -1;
		}

		err = aux_LoadProgram(&amx, (char *)config.getConfig("mainScript").c_str(), program);
		if (err != AMX_ERR_NONE)
		{
			CoreLogs.LOGE("Error load AMX file: %d", err);
			return err;
		}
	}
	catch (const std::exception &e)
	{
		CoreLogs.LOGE("Error: %s", e.what());
		return -1;
	}

	err = amx_Register(&amx, amx_natives::bstring_Natives, -1);
	if (err != AMX_ERR_NONE)
	{
		CoreLogs.LOGE("Error register native: %d", err);
		aux_FreeProgram(&amx);
		return err;
	}

	err = amx_Exec(&amx, &ret, AMX_EXEC_MAIN);
	while (err == AMX_ERR_SLEEP)
	{
		err = amx_Exec(&amx, &ret, AMX_EXEC_CONT);
	}

	if (err != AMX_ERR_NONE)
	{
		CoreLogs.LOGE("Error execute: %d", err);
	}

	aux_FreeProgram(&amx);

	return 0;
}
