/*
 * keystore.c
 * Copyright 2019 Peter Jones <pjones@redhat.com>
 *
 */

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/random.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ahkbahks.h"

struct keystore_key {
	keyring_item_t id;
	keyring_item_t data;
	struct keyring *keyring;
};

typedef struct keystore_key keystore_key_t;

struct keyring {
	struct stat stat;
	const char * const file;
	size_t sz;
	const char * const name;
	int fd;
	size_t nkeys;
	keystore_key_t *keys;
	keystore_key_id_cmp idcmp;
	keystore_key_data_cmp datacmp;
	void *priv;
};

struct keystore {
	size_t nkeyrings;
	keyring_t *keyrings;
};

PUBLIC void
keyring_fini(struct keyring *kr)
{
	if (kr->name) {
		size_t len = strlen(kr->name) + 1;
		memset((char *)kr->name, 0, len);
		free((char *)kr->name);
	}
	if (kr->file)
		free((char *)kr->file);

	memset(kr, 0, sizeof(struct keyring));
}

PUBLIC void
keyring_init(struct keyring *kr)
{
	memset(kr, 0, sizeof(*kr));

	kr->fd = -1;

}

PUBLIC struct keyring *
keyring_alloc(keystore_t *ks, const char * const path)
{
	keyring_t *new_keyrings, *kr;
	size_t kr_per_page, nmemb;
	kr_per_page = getpagesize() / sizeof (struct keyring);

	nmemb = ALIGN(ks->nkeyrings+1, kr_per_page);

	new_keyrings = reallocarray(ks->keyrings, nmemb, sizeof(struct keyring));
	if (!new_keyrings)
		return NULL;

	kr = &new_keyrings[ks->nkeyrings++];
	keyring_init(kr);
	return kr;
}

static int
keycmp(const void *k0p, const void *k1p)
{
	const struct keystore_key * const k0 = (const struct keystore_key * const)k0p;
	const struct keystore_key * const k1 = (const struct keystore_key * const)k1p;
	keystore_key_id_cmp idcmp;
	keystore_key_data_cmp datacmp;
	int rc;

	if (!k1 || !k0)
		return (intptr_t)k1 - (intptr_t)k0;

	if (k0->keyring != k1->keyring)
		return k1->keyring - k0->keyring;

	idcmp = k0->keyring->idcmp;
	datacmp = k1->keyring->datacmp;

	rc = idcmp(&k0->id, &k1->id, k0->keyring->priv);
	if (rc != 0)
		return rc;

	if (datacmp)
		rc = datacmp(&k0->data, &k1->data, &k0->keyring->priv);

	return rc;
}

PUBLIC keystore_t *
keystore_alloc(void)
{
	keystore_t *ks;

	ks = calloc(1, sizeof(keystore_t));
	if (!ks)
		return NULL;

	keystore_init(ks);
	return ks;
}

PUBLIC void
keystore_init(keystore_t *ks)
{
	size_t kr_per_page;

	memset(ks, 0, sizeof(*ks));

	kr_per_page = getpagesize() / sizeof (struct keyring);
	ks->keyrings = calloc(ALIGN(1, kr_per_page), sizeof(struct keyring));
}

PUBLIC void
keystore_fini(keystore_t *ks)
{
	if (!ks)
		return;

	for (size_t i = 0; i < ks->nkeyrings; i++)
		keyring_fini(&ks->keyrings[i]);

	if (ks->keyrings)
		free(ks->keyrings);
	ks->nkeyrings = 0;
	ks->keyrings = NULL;
}


PUBLIC void
keystore_free(keystore_t **keystore)
{
}

PUBLIC keyring_t *
keystore_keyring_new(keystore_t *keystore,
		     keystore_key_id_cmp idcmp,
		     keystore_key_data_cmp valcmp,
		     void *priv)
{
	return NULL;
}

PUBLIC keyring_t *
keystore_keyring_free(keyring_t *keyring)
{
	return NULL;
}

PUBLIC int NONNULL(1, 2, 3)
keyring_add_key(keyring_t *keyring,
		keyring_item_t *key_id,
		keyring_item_t *key_data)
{
	return -1;
}

PUBLIC int NONNULL(1, 2)
keyring_del_key(keyring_t *keyring, keyring_item_t *key_id)
{
	return -1;
}

PUBLIC int NONNULL(1, 2, 3)
keyring_get_key(keyring_t *keyring,
		keyring_item_t *key_id,
		keyring_item_t **key_data)
{
	return -1;
}

// vim:fenc=utf-8:tw=75:noet
