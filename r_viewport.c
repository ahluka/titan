#include "base.h"
#include "r_viewport.h"

/*
 * create_viewport
 */
struct viewport create_viewport(int x, int y, int w, int h)
{
        struct viewport ret = {x, y, w, h, 0.0f, 0.0f};
        return ret;
}

/*
 * viewport_contains
 */
bool viewport_contains(struct viewport *vp, vec2_t v)
{
        assert(vp != NULL);

        if (v[X] >= vp->r.x && v[X] <= vp->r.w &&
                v[Y] >= vp->r.y && v[Y] <= vp->r.h) {
                return true;
        }

        return false;
}

/*
 * viewport_contains_xy
 */
bool viewport_contains_xy(struct viewport *vp, int x, int y)
{
        vec2_t v = {x, y};
        return viewport_contains(vp, v);
}
