#include "base.h"
#include "panic.h"
#include "memory.h"
#include "map.h"
#include "ini.h"
#include "files.h"

#define MAPS_DIR "maps/"

/*
 * load_map
 */
static char *parse_tileset_val(const char *val)
{
        char *ret = sstrdup(val);
        char *p = ret;

        while (*p && *p != ',')
                p++;

        *p = '\0';
        return ret;
}

static void parse_tile_data(struct map *map, const char *val)
{
        uint32_t w = map->width, h = map->height;

        assert(w > 0 && h > 0);
        map->data = MemAlloc(sizeof(*map->data) * w * h);
}

#define MATCH(s, k) strcmp(sec, s) == 0 && strcmp(key, k) == 0

static int
handler(void *usr, const char *sec, const char *key, const char *val)
{
        struct map *m = (struct map *) usr;

        if (MATCH("header", "width")) {
                m->width = atoi(val);
        } else if (MATCH("header", "height")) {
                m->height = atoi(val);
        } else if (MATCH("header", "tilewidth")) {
                m->tile_width = atoi(val);
        } else if (MATCH("header", "tileheight")) {
                m->tile_height = atoi(val);
        } else if (MATCH("tilesets", "tileset")) {
                char *parsed = parse_tileset_val(val);
                m->tileset = sstrcat(MAPS_DIR, parsed);
                sstrfree(parsed);
        } else if (MATCH("layer", "data")) {
                parse_tile_data(m, val);
        }

        return 1;
}
#undef MATCH

struct map *load_map(const char *name)
{
        assert(name != NULL);

        char *path = sstrcat(get_root_path(), name);
        struct map *ret = MemAlloc(sizeof(*ret));

        if (ini_parse(path, handler, ret) < 0) {
                MemFree(ret);
                trace(CHAN_INFO, fmt("failed to load map '%s'", name));
                return NULL;
        }

        ret->name = sstrdup(name);
        trace(CHAN_INFO, fmt("loaded '%s':", name));
        trace(CHAN_INFO, fmt("  %ux%u tiles, %ux%u px", ret->width, ret->height,
                ret->tile_width, ret->tile_height));
        trace(CHAN_INFO, fmt("  tileset: %s", ret->tileset));

        sstrfree(path);
        return ret;
}

/*
 * free_map
 */
void free_map(struct map *m)
{
        assert(m != NULL);

        sstrfree(m->tileset);
        sstrfree(m->name);
        MemFree(m->data);
        MemFree(m);
}
