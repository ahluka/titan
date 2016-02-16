#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Holds configuration data, loaded at startup
 * and saved at exit. Available globally as g_Config.
 */
struct ConfigData {
	const char *filename;	/* the filename given to LoadConfig() */

	/* From here keep it laid out the same as the INI file */

	/* general */
	char *gameName;
	char *version;

	/* renderer */
	int windowWidth;
	int windowHeight;
        bool vsync;

	/* filesystem */
	char *filesRoot;
};

extern struct ConfigData g_Config;

ecode_t Config_Load(const char *filename);
ecode_t Config_Save(const char *filename);

#endif /* __CONFIG_H__ */
