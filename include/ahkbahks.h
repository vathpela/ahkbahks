/*
 * ahkbahks.h
 * Copyright 2019 Peter Jones <pjones@redhat.com>
 */

#ifndef AHKBAHKS_H_
#define AHKBAHKS_H_

#include <stdint.h>

struct keyring_item {
	const uint8_t * const data;
	size_t size;
};
typedef struct keyring_item keyring_item_t;

typedef int (*keystore_key_id_cmp)(const keyring_item_t * const a,
				   const keyring_item_t * const b,
				   void *priv)
	__attribute__((__nonnull__(1, 2)));
typedef int (*keystore_key_data_cmp)(const keyring_item_t * const a,
				     const keyring_item_t * const b,
				     void *priv)
	__attribute__((__nonnull__(1, 2)));

typedef struct keystore keystore_t;
typedef struct keyring keyring_t;

extern keystore_t *keystore_alloc(void)
	;
extern void keystore_init(keystore_t *keystore)
	__attribute__((__visibility__("default")));
extern void keystore_fini(keystore_t *keystore)
	__attribute__((__visibility__("default")));
extern void keystore_free(keystore_t **keystore)
	__attribute__((__visibility__("default")));

extern keyring_t *keystore_keyring_new(keystore_t *keystore,
				       keystore_key_id_cmp idcmp,
				       keystore_key_data_cmp datacmp,
				       void *priv)
	__attribute__((__visibility__("default")));
extern keyring_t *keystore_keyring_free(keyring_t *keyring)
	__attribute__((__visibility__("default")));

extern int keyring_add_key(keyring_t *keyring,
			   keyring_item_t *key_id,
			   keyring_item_t *key_data)
	__attribute__((__visibility__("default")))
	__attribute__((__nonnull__(1, 2, 3)));
extern int keyring_del_key(keyring_t *keyring,
			   keyring_item_t *key_id)
	__attribute__((__visibility__("default")))
	__attribute__((__nonnull__(1, 2)));
extern int keyring_get_key(keyring_t *keyring,
			   keyring_item_t *key_id,
			   keyring_item_t **key_data)
	__attribute__((__visibility__("default")))
	__attribute__((__nonnull__(1, 2, 3)));

#endif /* !AHKBAHKS_H_ */
// vim:fenc=utf-8:tw=75:noet
