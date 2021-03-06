#include "base.h"
#include "sstr.h"
#include "panic.h"
#include "memory.h"

#define BLOCK_SIZE 128
#define BLOCK_COUNT 1024

static mem_pool_t *s_Pool = NULL;

/*
 * validate_length
 */
static void validate_length(const char *str)
{
        if (strlen(str) >= BLOCK_SIZE)
                panic(fmt("String too large (%s)", str));
}

/*
 * sstr_init
 */
ecode_t sstr_init()
{
        if (s_Pool != NULL) {
                trace(CHAN_DBG, "sstr already initialised");
                return EFAIL;
        }

        /* Seems best to make this pool fixed size, at least for now. */
        s_Pool = create_pool(BLOCK_COUNT, BLOCK_SIZE, POOL_FIXEDSIZE, "sstr");
        return EOK;
}

/*
 * sstr_shutdown
 */
ecode_t sstr_shutdown()
{
        if (s_Pool == NULL) {
                trace(CHAN_DBG, "sstr wasn't initialised");
                return EFAIL;
        }

        destroy_pool(s_Pool);

        return EOK;
}

void sstrfree(char *str)
{
        PFree(s_Pool, str);
}

/*
 * sstrdup
 */
char *sstrdup(const char *str)
{
        assert(str != NULL);

        validate_length(str);
        char *ret = PAlloc(s_Pool);
        strncpy(ret, str, strlen(str));

	return ret;
}

/*
 * sstrdup_lower
 */
char *sstrdup_lower(const char *str)
{
        assert(str != NULL);

        char *copy = sstrdup(str);
        sstrlower(copy);
        return copy;
}

/*
 * sstrcat
 */
char *sstrcat(const char *first, const char *second)
{
        assert(first != NULL);
        assert(second != NULL);

        validate_length(first);
        validate_length(second);
        if (strlen(first) + strlen(second) >= BLOCK_SIZE)
                panic("concatenation would overflow");

        char *ret = PAlloc(s_Pool);
        strncpy(ret, first, strlen(first));
        strncat(ret, second, strlen(second));
        return ret;
}

/*
 * sstrfname
 */
char *sstrfname(const char *dir, const char *name, const char *ext)
{
        assert(name != NULL);

        validate_length(name);
        if (dir) validate_length(dir);
        if (ext) validate_length(ext);

        size_t namesz = strlen(name);

        if (dir) {
                if (strlen(dir) + namesz >= BLOCK_SIZE)
                        panic("dir + name would overflow");
        }

        if (ext) {
                if (strlen(ext) + namesz >= BLOCK_SIZE)
                        panic("name + ext would overflow");
        }

        char *ret = PAlloc(s_Pool);

        if (dir) strncpy(ret, dir, strlen(dir));
        strncat(ret, name, namesz);
        if (ext) strncat(ret, ext, strlen(ext));
        return ret;
}

/*
 * sstrlower
 */
void sstrlower(char *str)
{
        while (*str) {
                *str = tolower(*str);
                str++;
        }
}
