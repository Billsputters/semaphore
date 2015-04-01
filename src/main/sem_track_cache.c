#include <stdlib.h>

#include "sem_track_cache.h"
#include "sem_parser.h"

void track_free(gpointer track);

sem_success sem_track_cache_init(sem_track_cache* track_cache) {
	track_cache->table = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, track_free);
	return SEM_OK;
}

sem_success sem_track_cache_find(sem_track_cache* track_cache, char* track_description, sem_track** track) {
	*track = g_hash_table_lookup(track_cache->table, track_description);

	if (*track == NULL) {
		*track = malloc(sizeof(sem_track));
		if (*track == NULL) return sem_set_error("Failed to allocated memory for track");
		g_hash_table_insert(track_cache->table, track_description, *track);

	}

	sem_track_parse(*track, track_description);

	return SEM_OK;
}

void sem_track_cache_destroy(sem_track_cache* track_cache) {
	g_hash_table_destroy(track_cache->table);
}

void track_free(gpointer track) {
	free(track);
}
