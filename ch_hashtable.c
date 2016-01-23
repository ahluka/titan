#include "base.h"
#include "panic.h"
#include "ch_hashtable.h"
#include "hash.h"
#include "files.h"


#define SZ_PRIME 383
static bool s_Table[SZ_PRIME] = {false};

static uint32_t HashMod(char *str)
{
	uint32_t h = Hash(str, strlen(str));
	return h % SZ_PRIME;
}

static void TestDataset()
{
	FileHandle hnd = Files_OpenFile("hashtest.txt");
	const char *txt = (const char *) Files_GetData(hnd);
	char word[128] = {0};
	size_t ofs = 0;

	while (sscanf(txt + ofs, "%s\n", word) != EOF) {
		uint32_t h = Hash(word, strlen(word));
		uint32_t m = h % SZ_PRIME;

		Trace(Fmt("'%s' hashes to %u index %u", word, h, m));

		if (s_Table[m]) {
			Panic(Fmt("Collision detected! %u already used", m));
		}

		s_Table[m] = true;
		ofs += strlen(word) + 1; /* +1 for the newline */
	}

	Files_CloseFile(hnd);
}


void HT_Test()
{
	TestDataset();
}
