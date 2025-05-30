/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2007 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#ifdef _WIN32
#ifdef __MINGW64__
#define _WIN32_IE 0x501
#else
#define _WIN32_IE 0x400
#endif
#endif

#include "al_config.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "al_alMain.h"

#ifdef _WIN32_IE
#include <shlobj.h>
#endif

typedef struct ConfigEntry {
    char *key;
    char *value;
} ConfigEntry;

typedef struct ConfigBlock {
    char *name;
    ConfigEntry *entries;
    size_t entryCount;
} ConfigBlock;

static ConfigBlock *cfgBlocks;
static size_t cfgCount;

static char buffer[1024];

static void LoadConfigFromFile(FILE *f)
{
    ConfigBlock *curBlock = cfgBlocks;
    ConfigEntry *ent;

    while(fgets(buffer, sizeof(buffer), f))
    {
        size_t i = 0;

        while(isspace(buffer[i]))
            i++;
        if(!buffer[i] || buffer[i] == '#')
            continue;

        memmove(buffer, buffer+i, strlen(buffer+i)+1);

        if(buffer[0] == '[')
        {
            ConfigBlock *nextBlock;

            i = 1;
            while(buffer[i] && buffer[i] != ']')
                i++;

            if(!buffer[i])
            {
                 AL_PRINT("config parse error: bad line \"%s\"\n", buffer);
                 continue;
            }
            buffer[i] = 0;

            do {
                i++;
                if(buffer[i] && !isspace(buffer[i]))
                {
                    if(buffer[i] != '#')
                        AL_PRINT("config warning: extra data after block: \"%s\"\n", buffer+i);
                    break;
                }
            } while(buffer[i]);

            nextBlock = NULL;
            for(i = 0;i < cfgCount;i++)
            {
                if(strcasecmp(cfgBlocks[i].name, buffer+1) == 0)
                {
                    nextBlock = cfgBlocks+i;
//                    AL_PRINT("found block '%s'\n", nextBlock->name);
                    break;
                }
            }

            if(!nextBlock)
            {
                nextBlock = realloc(cfgBlocks, (cfgCount+1)*sizeof(ConfigBlock));
                if(!nextBlock)
                {
                     AL_PRINT("config parse error: error reallocating config blocks\n");
                     continue;
                }
                cfgBlocks = nextBlock;
                nextBlock = cfgBlocks+cfgCount;
                cfgCount++;

                nextBlock->name = strdup(buffer+1);
                nextBlock->entries = NULL;
                nextBlock->entryCount = 0;

//                AL_PRINT("found new block '%s'\n", nextBlock->name);
            }
            curBlock = nextBlock;
            continue;
        }

        /* Look for the option name */
        i = 0;
        while(buffer[i] && buffer[i] != '#' && buffer[i] != '=' &&
              !isspace(buffer[i]))
            i++;

        if(!buffer[i] || buffer[i] == '#' || i == 0)
        {
            AL_PRINT("config parse error: malformed option line: \"%s\"\n", buffer);
            continue;
        }

        /* Seperate the option */
        if(buffer[i] != '=')
        {
            buffer[i++] = 0;

            while(isspace(buffer[i]))
                i++;
            if(buffer[i] != '=')
            {
                AL_PRINT("config parse error: option without a value: \"%s\"\n", buffer);
                continue;
            }
        }
        /* Find the start of the value */
        buffer[i++] = 0;
        while(isspace(buffer[i]))
            i++;

        /* Check if we already have this option set */
        ent = curBlock->entries;
        while((size_t)(ent-curBlock->entries) < curBlock->entryCount)
        {
            if(strcasecmp(ent->key, buffer) == 0)
                break;
            ent++;
        }

        if((size_t)(ent-curBlock->entries) >= curBlock->entryCount)
        {
            /* Allocate a new option entry */
            ent = realloc(curBlock->entries, (curBlock->entryCount+1)*sizeof(ConfigEntry));
            if(!ent)
            {
                 AL_PRINT("config parse error: error reallocating config entries\n");
                 continue;
            }
            curBlock->entries = ent;
            ent = curBlock->entries + curBlock->entryCount;
            curBlock->entryCount++;

            ent->key = strdup(buffer);
            ent->value = NULL;
        }

        /* Look for the end of the line (Null term, new-line, or #-symbol) and
           eat up the trailing whitespace */
        memmove(buffer, buffer+i, strlen(buffer+i)+1);

        i = 0;
        while(buffer[i] && buffer[i] != '#' && buffer[i] != '\n')
            i++;
        do {
            i--;
        } while(isspace(buffer[i]));
        buffer[++i] = 0;

        free(ent->value);
        ent->value = strdup(buffer);

//        AL_PRINT("found '%s' = '%s'\n", ent->key, ent->value);
    }
}

void ReadALConfig(void)
{
    FILE *f;

    cfgBlocks = calloc(1, sizeof(ConfigBlock));
    cfgBlocks->name = strdup("general");
    cfgCount = 1;

#ifdef _WIN32
    if(SHGetSpecialFolderPathA(NULL, buffer, CSIDL_APPDATA, FALSE) != FALSE)
    {
        size_t p = strlen(buffer);
        snprintf(buffer+p, sizeof(buffer)-p, "\\alsoft.ini");
        f = fopen(buffer, "rt");
        if(f)
        {
            LoadConfigFromFile(f);
            fclose(f);
        }
    }
#else
    f = fopen("/etc/openal_alsoft.conf", "r");
    if(f)
    {
        LoadConfigFromFile(f);
        fclose(f);
    }
    if(getenv("HOME") && *(getenv("HOME")))
    {
        snprintf(buffer, sizeof(buffer), "%s/.alsoftrc", getenv("HOME"));
        f = fopen(buffer, "r");
        if(f)
        {
            LoadConfigFromFile(f);
            fclose(f);
        }
    }
#endif
    if(getenv("ALSOFT_CONF"))
    {
        f = fopen(getenv("ALSOFT_CONF"), "r");
        if(f)
        {
            LoadConfigFromFile(f);
            fclose(f);
        }
    }
}

void FreeALConfig(void)
{
    size_t i;

    for(i = 0;i < cfgCount;i++)
    {
        size_t j;
        for(j = 0;j < cfgBlocks[i].entryCount;j++)
        {
           free(cfgBlocks[i].entries[j].key);
           free(cfgBlocks[i].entries[j].value);
        }
        free(cfgBlocks[i].entries);
        free(cfgBlocks[i].name);
    }
    free(cfgBlocks);
    cfgBlocks = NULL;
    cfgCount = 0;
}

const char *GetConfigValue(const char *blockName, const char *keyName, const char *def)
{
    size_t i, j;

    if(keyName)
    {
        if(!blockName)
            blockName = "general";

        for(i = 0;i < cfgCount;i++)
        {
            if(strcasecmp(cfgBlocks[i].name, blockName) != 0)
                continue;

            for(j = 0;j < cfgBlocks[i].entryCount;j++)
            {
                if(strcasecmp(cfgBlocks[i].entries[j].key, keyName) == 0)
                {
                    if(cfgBlocks[i].entries[j].value[0])
                        return cfgBlocks[i].entries[j].value;
                    return def;
                }
            }
        }
    }

    return def;
}

int ConfigValueExists(const char *blockName, const char *keyName)
{
    const char *val = GetConfigValue(blockName, keyName, "");
    return !!val[0];
}

int GetConfigValueInt(const char *blockName, const char *keyName, int def)
{
    const char *val = GetConfigValue(blockName, keyName, "");

    if(!val[0]) return def;
    return strtol(val, NULL, 0);
}

float GetConfigValueFloat(const char *blockName, const char *keyName, float def)
{
    const char *val = GetConfigValue(blockName, keyName, "");

    if(!val[0]) return def;
#ifdef HAVE_STRTOF
    return strtof(val, NULL);
#else
    return (float)strtod(val, NULL);
#endif
}

int GetConfigValueBool(const char *blockName, const char *keyName, int def)
{
    const char *val = GetConfigValue(blockName, keyName, "");

    if(!val[0]) return !!def;
    return (strcasecmp(val, "true") == 0 || strcasecmp(val, "yes") == 0 ||
            strcasecmp(val, "on") == 0 || atoi(val) != 0);
}
