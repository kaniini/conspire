#ifndef CONFIGDB_H
#define CONFIGDB_H

#include <glib.h>

/**
 * ConfigDb:
 *
 * A configuration database handle, opened with settings_open().
 **/
typedef struct _ConfigDb ConfigDb;


G_BEGIN_DECLS

    /**
     * settings_open:
     *
     * Opens the configuration database.
     *
     * Return value: A configuration database handle.
     **/
    ConfigDb *settings_open();

    /**
     * settings_close:
     * @db: A configuration database handle.
     *
     * Closes the configuration database.
     **/
    void settings_close(ConfigDb *db);

    /**
     * settings_get_string:
     * @db: A configuration database handle.
     * @section: The section of the configuration database to search.
     * @key: The name of the field in the configuration database to look up.
     * @value: Pointer to a buffer to put the data in.
     *
     * Searches the configuration database for a value.
     *
     * Return value: TRUE if successful, FALSE otherwise.
     **/
    gboolean settings_get_string(ConfigDb *db,
                                   const gchar *section,
                                   const gchar *key,
                                   gchar **value);

    /**
     * settings_get_int:
     * @db: A configuration database handle.
     * @section: The section of the configuration database to search.
     * @key: The name of the field in the configuration database to look up.
     * @value: Pointer to an integer to put the data in.
     *
     * Searches the configuration database for a value.
     *
     * Return value: TRUE if successful, FALSE otherwise.
     **/
    gboolean settings_get_int(ConfigDb *db,
                                const gchar *section,
                                const gchar *key,
                                gint *value);

    /**
     * settings_get_bool:
     * @db: A configuration database handle.
     * @section: The section of the configuration database to search.
     * @key: The name of the field in the configuration database to look up.
     * @value: Pointer to a boolean to put the data in.
     *
     * Searches the configuration database for a value.
     *
     * Return value: TRUE if successful, FALSE otherwise.
     **/
    gboolean settings_get_bool(ConfigDb *db,
                                 const gchar *section,
                                 const gchar *key,
                                 gboolean *value);

    /**
     * settings_get_float:
     * @db: A configuration database handle.
     * @section: The section of the configuration database to search.
     * @key: The name of the field in the configuration database to look up.
     * @value: Pointer to a floating point integer to put the data in.
     *
     * Searches the configuration database for a value.
     *
     * Return value: TRUE if successful, FALSE otherwise.
     **/
    gboolean settings_get_float(ConfigDb *db,
                                  const gchar *section,
                                  const gchar *key,
                                  gfloat *value);

    /**
     * settings_get_double:
     * @db: A configuration database handle.
     * @section: The section of the configuration database to search.
     * @key: The name of the field in the configuration database to look up.
     * @value: Pointer to a double-precision floating point integer to put the data in.
     *
     * Searches the configuration database for a value.
     *
     * Return value: TRUE if successful, FALSE otherwise.
     **/
    gboolean settings_get_double(ConfigDb *db,
                                   const gchar *section,
                                   const gchar *key,
                                   gdouble *value);

    /**
     * settings_set_string:
     * @db: A configuration database handle.
     * @section: The section of the configuration database to search.
     * @key: The name of the field in the configuration database to set.
     * @value: Pointer to a buffer containing the data.
     *
     * Sets a value in the configuration database.
     **/
    void settings_set_string(ConfigDb *db,
                               const gchar *section,
                               const gchar *key,
                               const gchar *value);

    /**
     * settings_set_int:
     * @db: A configuration database handle.
     * @section: The section of the configuration database to search.
     * @key: The name of the field in the configuration database to set.
     * @value: Pointer to an integer containing the data.
     *
     * Sets a value in the configuration database.
     **/
    void settings_set_int(ConfigDb *db,
                            const gchar *section,
                            const gchar *key,
                            gint value);

    /**
     * settings_set_bool:
     * @db: A configuration database handle.
     * @section: The section of the configuration database to search.
     * @key: The name of the field in the configuration database to set.
     * @value: Pointer to a boolean containing the data.
     *
     * Sets a value in the configuration database.
     **/
    void settings_set_bool(ConfigDb *db,
                             const gchar *section,
                             const gchar *key,
                             gboolean value);

    /**
     * settings_set_float:
     * @db: A configuration database handle.
     * @section: The section of the configuration database to search.
     * @key: The name of the field in the configuration database to set.
     * @value: Pointer to a floating point integer containing the data.
     *
     * Sets a value in the configuration database.
     **/
    void settings_set_float(ConfigDb *db,
                              const gchar *section,
                              const gchar *key,
                              gfloat value);

    /**
     * settings_set_double:
     * @db: A configuration database handle.
     * @section: The section of the configuration database to search.
     * @key: The name of the field in the configuration database to set.
     * @value: Pointer to a double precision floating point integer containing the data.
     *
     * Sets a value in the configuration database.
     **/
    void settings_set_double(ConfigDb *db,
                               const gchar *section,
                               const gchar *key,
                               gdouble value);

    /**
     * settings_unset_key:
     * @db: A configuration database handle.
     * @section: The section of the configuration database to search.
     * @key: The name of the field in the configuration database to set.
     *
     * Removes a value from the configuration database.
     **/
    void settings_unset_key(ConfigDb *db,
                              const gchar *section,
                              const gchar *key);

G_END_DECLS

ConfigDb *config;

#endif /* CONFIGDB_H */

