#include "base.h"
#include "panic.h"
#include "ch_hashtable.h"
#include "hash.h"
#include "list.h"
#include "memory.h"
#include "files.h"

typedef struct Datum {
	char *key;
	void *data;
} Datum;

struct HashTable {
	uint32_t size;
	enum HTPolicy policy;
	enum HTFreeType freeType;

	ListHead **table;
};

/*
 * HashMod
 *	Hash the given string and modulo it down between 0 and prime - 1.
 */
static uint32_t HashMod(const char *str, int prime)
{
	uint32_t h = Hash(str, strlen(str));
	return h % prime;
}

/*
 * IsPrime
 */
static bool IsPrime(int n)
{
	if (n <= 1)
		return false;

	for (int i = 2; i*i <= n; i++) {
		if (n % i == 0)
			return false;
	}

	return true;
}

/*
 * AllocTable
 */
static void AllocTable(HashTable *table, uint32_t size)
{
	assert(size > 0);

	if (!IsPrime(size)) {
		Trace(Fmt("WARNING: non-prime table size of %d", size));
	}

	table->size = size;
	table->table = MemAlloc(size * sizeof(ListHead *));

	for (int i = 0; i < size; i++) {
		table->table[i] = List_Create(LIST_FREE_DATA);
	}
}

/*
 * NewDatum
 */
static Datum *NewDatum(char *key, void *data)
{
	Datum *d = MemAlloc(sizeof(*d));
	d->key = key;
	d->data = data;
	return d;
}

/*
 * FindDatum
 */
static bool FindDatum_deepcmp(void *a, void *b)
{
	Datum *d1 = (Datum *) a;
	Datum *d2 = (Datum *) b;

	if (strcmp(d1->key, d2->key) == 0) {
		return true;
	}

	return false;
}

static ecode_t FindDatum(HashTable *table, char *key, uint32_t hash, void *data)
{
	if (table->table[hash] == NULL) {
		return EFAIL;
	}

	Datum datum = {key, data};
	if (List_Contains(table->table[hash], &datum, FindDatum_deepcmp, NULL)) {
		return EOK;
	}

	return EFAIL;
}

/*
 * HT_Create
 *	Create a hash table of the given size, which should be a prime number.
 *	The other args are detailed in ch_hashtable.h.
 */
 HashTable *HT_Create(uint32_t sizePrime,
 	enum HTFreeType freeType,
 	enum HTPolicy policy)
{
	HashTable *tbl = MemAlloc(sizeof(*tbl));

	AllocTable(tbl, sizePrime);
	tbl->policy = policy;
	tbl->freeType = freeType;

	return tbl;
}

/*
 * HT_Destroy
 *	Free all memory the given hash table used.
 */
static void FreeListData(void *listItem, void *unused)
{
	Datum *d = (Datum *) listItem;

	MemFree((void *) d->key);
	MemFree(d->data);
}

void HT_Destroy(HashTable *table)
{
	assert(table != NULL);

	for (int i = 0; i < table->size; i++) {
		if (table->freeType == HT_FREE_DATA) {
			List_ForEach(table->table[i], FreeListData, NULL);
		}

		List_Destroy(table->table[i]);
	}

	MemFree(table->table);
	MemFree(table);
}

/*
 * HT_Add
 */
ecode_t HT_Add(HashTable *table, char *key, void *data)
{
	assert(table != NULL);
	assert(key != NULL);

	uint32_t hash = HashMod(key, table->size);
	Datum *dat = NewDatum(key, data);

	if (FindDatum(table, key, hash, data) == EOK) {
		if (table->policy == HT_UNIQUE) {
			Trace(Fmt("WARNING: Duplicate in HT_UNIQUE table "
				"(key: '%s')", key));
			MemFree(dat);
			if (table->freeType == HT_FREE_DATA) {
				MemFree(key);
			}

			return EFAIL;
		}
	}

	List_Add(table->table[hash], dat);

#ifdef DEBUG_TRACING_ON
	Trace(Fmt("key '%s' at index '%u', bucket size %d",
		dat->key, hash, List_GetSize(table->table[hash])));
#endif

	return EOK;
}

/*
 * HT_Remove
 */
ecode_t HT_Remove(HashTable *table, char *key)
{
	assert(table != NULL);
	assert(key != NULL);

	uint32_t index = 0, hash = HashMod(key, table->size);
	Datum cmp = {key, NULL};

	if (!List_Contains(table->table[hash], &cmp, FindDatum_deepcmp, &index)) {
		return EFAIL;
	}

	List_RemoveAt(table->table[hash], index);

	return EOK;
}

/*
 * HT_Get
 */
ecode_t HT_Get(HashTable *table, char *key, void **_data)
{
	assert(table != NULL);
	assert(key != NULL);
	assert(_data != NULL);

	uint32_t hash = HashMod(key, table->size);

	if (table->table[hash] == NULL) {
		return EFAIL;
	}

	uint32_t index = 0;
	Datum cmp = {key, NULL};

	if (!List_Contains(table->table[hash], &cmp, FindDatum_deepcmp, &index)) {
		return EFAIL;
	}

	Datum *ret = List_At(table->table[hash], index);
	*_data = ret->data;
	return EOK;
}


static void TestDataset(HashTable *table);
void HT_Test()
{
	HashTable *t = HT_Create(383, HT_FREE_DATA, HT_UNIQUE);

	TestDataset(t);

	HT_Destroy(t);
}

// #define SZ_PRIME 383
// static bool s_Table[SZ_PRIME] = {false};
static void TestDataset(HashTable *table)
{
	FileHandle hnd = Files_OpenFile("hashtest.txt");
	const char *txt = (const char *) Files_GetData(hnd);
	char word[128] = {0};
	size_t ofs = 0;

	while (sscanf(txt + ofs, "%s\n", word) != EOF) {
		HT_Add(table, StrDup(word), NULL);
		ofs += strlen(word) + 1; /* +1 for the newline */
	}

	Files_CloseFile(hnd);
}
