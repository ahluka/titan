#include "base.h"
#include "panic.h"
#include "vec.h"

static bool valid_vector(const char *str)
{
        size_t n = strlen(str);

        if (str[0] != '(') {
                Trace(CHAN_INFO, "vector must start with '('");
                Trace(CHAN_INFO, Fmt("  got: %s", str));
                return false;
        }

        if (str[n-1] != ')') {
                Trace(CHAN_INFO, "vector must end with '('");
                Trace(CHAN_INFO, Fmt("  got: %s", str));
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

/*
 * VParseStr
 */
ecode_t VParseStr(const char *str, vec2_t out)
{
        assert(str != NULL);
        assert(out != NULL);

        // Trace(CHAN_DBG, Fmt("parsing: %s", str));

        if (!valid_vector(str))
                return EFAIL;

        vec_t x = 0, y = 0;
        char *v = remove_spaces(str);
        if (sscanf(v, "(%f %f)", &x, &y) != 2) {
                Trace(CHAN_INFO, Fmt("failed to parse %s", str));
                return EFAIL;
        }

        sstrfree(v);

        VSet(out, x, y);
        return EOK;
}

void VecTests()
{

}