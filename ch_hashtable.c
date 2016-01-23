#include "base.h"
#include "panic.h"
#include "ch_hashtable.h"
#include "hash.h"
#include "list.h"
#include "memory.h"
#include "files.h"

struct HashTable {
	int size;
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
static void AllocTable(HashTable *table, int size, enum HTFreeType ft)
{
	if (size == 0) {
		Panic("Zero table size");
	}

	if (!IsPrime(size)) {
		Trace(Fmt("WARNING: non-prime table size of %d", size));
	}

	table->size = size;
	table->table = MemAlloc(size * sizeof(ListHead *));

	for (int i = 0; i < size; i++) {
		if (ft == HT_MANUAL) {
			table->table[i] = List_Create(LIST_MANUAL);
		} else if (ft == HT_FREE_DATA) {
			table->table[i] = List_Create(LIST_FREE_DATA);
		}
	}
}

/*
 * HT_Create
 *	Create a hash table of the given size, which should be a prime number.
 */
HashTable *HT_Create(int sizePrime, enum HTFreeType freeType)
{
	HashTable *tbl = MemAlloc(sizeof(*tbl));

	AllocTable(tbl, sizePrime, freeType);

	return tbl;
}

/*
 * HT_Destroy
 *	Free all memory the given hash table used.
 */
void HT_Destroy(HashTable *table)
{
	if (!table) {
		Panic("invalid table");
	}

	for (int i = 0; i < table->size; i++) {
		List_Destroy(table->table[i]);
	}

	MemFree(table->table);
	MemFree(table);
}

/*
 * HT_Add
 *	Add the given data to the given hash table, using the given key.
 */
ecode_t HT_Add(HashTable *table, const char *key, void *data)
{
	if (!table) {
		Panic("invalid table");
	}

	uint32_t index = HashMod(key, table->size);
	List_Add(table->table[index], data);

	Trace(Fmt("key '%s' at index '%u', bucket size %d",
		key, index, List_GetSize(table->table[index])));

	return EOK;
}

static void TestDataset(HashTable *table);
void HT_Test()
{
	HashTable *t = HT_Create(383, HT_MANUAL);

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
		HT_Add(table, word, 0);
		ofs += strlen(word) + 1; /* +1 for the newline */
	}

	Files_CloseFile(hnd);
}
