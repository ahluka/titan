/*
 * map.h
 *      Map loading and render representation.
 *      For now only single-layer maps are supported, but multiple layers will
 *      be implemented in the future.
 */
#pragma once

struct map {
        char *name;
        uint32_t width, height; /* in tiles */
        uint32_t tile_width, tile_height;
        char *tileset;
        uint16_t *data;
};

struct map *load_map(const char *name);
void free_map(struct map *m);
