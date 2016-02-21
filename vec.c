#include "base.h"
#include "panic.h"
#include "vec.h"

/*
* VParseStr
*/
static bool valid_vector(const char *str)
{
        size_t n = strlen(str);

        if (str[0] != '(') {
                trace(CHAN_INFO, "vector must start with '('");
                trace(CHAN_INFO, fmt("  got: %s", str));
                return false;
        }

        if (str[n-1] != ')') {
                trace(CHAN_INFO, "vector must end with '('");
                trace(CHAN_INFO, fmt("  got: %s", str));
                return false;
        }

        return true;
}

static char *remove_spaces(const char *str)
{
        char *copy = sstrdup(str);
        char *pc = copy;
        const char *ps = str;

        for ( ; *ps != '\0'; ps++) {
                if (*ps == ' ' && *(ps + 1) == ' ')
                        continue;

                *pc++ = *ps;
        }

        *pc = '\0';
        return copy;
}

ecode_t VParseStr(const char *str, vec2_t out)
{
        assert(str != NULL);
        assert(out != NULL);

        // trace(CHAN_DBG, fmt("parsing: %s", str));

        if (!valid_vector(str))
                return EFAIL;

        vec_t x = 0, y = 0;
        char *v = remove_spaces(str);
        if (sscanf(v, "(%f %f)", &x, &y) != 2) {
                trace(CHAN_INFO, fmt("failed to parse %s", str));
                return EFAIL;
        }

        sstrfree(v);

        VSet(out, x, y);
        return EOK;
}

/*
 * lerp
 */
float lerp(float from, float to, float t)
{
        return from + ((to - from) * t);
}

/*
 * VLerp
 */
void VLerp(vec2_t out, vec2_t from, vec2_t to, vec_t t)
{
        out[X] = lerp(from[X], to[X], t);
        out[Y] = lerp(from[Y], to[Y], t);
}

void VecTests()
{
        // vec2_t from = {1.0f, 2.0f};
        // vec2_t to = {5.0f, 6.0f};
        // vec2_t r;
        //
        // VLerp(r, from, to, 0.75f);
        //
        // trace(CHAN_DBG, fmt("%2.2f, %2.2f", r[X], r[Y]));
        // trace(CHAN_DBG, fmt("%2.2f", lerp(3, 5, 0.5f)));
}
