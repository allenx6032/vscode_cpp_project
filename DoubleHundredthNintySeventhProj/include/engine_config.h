/*
** EPITECH PROJECT, 2021
** config
** File description:
** Config for RPG
*/

#ifndef CONFIG_H_
#define CONFIG_H_

typedef struct config_data_s
{
    char *key;
    char *value;
    char *section;
} config_data_t;

typedef struct config_s
{
    char *file_str;
    char **file_array;
    config_data_t **all_values;
} config_t;

// Return a ptr of the created config
// Return null when config can't be created
config_t *config_load(char *path);

// Return value from the selected key
// Return null when key don't exist
char *config_get(const config_t *config, char *key);

// Return 1 when the key exist and 0 when not
int config_key_exist(const config_t *config, char *key);

// Return 1 when the key exist and defined
// Return 0 when don't exist or is not defined
int config_is_value_defined(const config_t *config, char *key);

// Set a value from the selected key
// If the value is not found, it is created
void config_set(config_t *config, char *key, char *value);

// Write the config to the selected path
// Return 1 if config has been writed and 0 when not
int config_save(const config_t *config, char *path);

// Unload ressources taken by the config
// The ptr will be NULL if everything is fine
void config_unload(config_t *config);

// Things only for RPG

// Load RPG configuration
// If default values don't exist it will be created
void load_rpg_config(config_t *config);

#endif /* !CONFIG_H_ */
