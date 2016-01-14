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
};

extern struct ConfigData g_Config;

ecode_t LoadConfig(const char *filename);
ecode_t WriteConfig(const char *filename);

#endif /* __CONFIG_H__ */
