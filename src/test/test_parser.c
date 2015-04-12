#include <glib.h>
#include <stdlib.h>

#include "test_parser.h"
#include "test_track_cache.h"
#include "sem_compass.h"
#include "sem_parser.h"
#include "sem_strings.h"
#include "sem_world.h"

void test_parser_bad_tile_class(void);
void test_parser_n_s_track(sem_track_cache* track_cache, const void* data);
void test_parser_e_w_track(sem_track_cache* track_cache, const void* data);
void test_parser_n_s_e_w_track(void);
void test_parser_two_way_points(sem_track_cache* track_cache, const void* data);

void add_tests_parser() {
	g_test_add_func("/parser/bad_tile_class", test_parser_bad_tile_class);
	add_test_track_cache("/parser/n_s_track", test_parser_n_s_track);
	add_test_track_cache("/parser/e_w_track", test_parser_e_w_track);
	g_test_add_func("/parser/n_s_e_w_track", test_parser_n_s_e_w_track);
	add_test_track_cache("/parser/two_way_points", test_parser_two_way_points);
}

void test_parser_bad_tile_class() {
	char track_description[16] = "badclass";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_tile tile;
	g_assert_true(sem_tile_parse(&tile, &tokens, NULL) != SEM_OK);
}

void test_parser_n_s_track(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[16] = "track N-S";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_tile tile;
	sem_tile_parse(&tile, &tokens, track_cache);

	g_assert_true(tile.class == TRACK);
	g_assert_true(tile.track->start == SEM_NORTH);
	g_assert_true(tile.track->end == SEM_SOUTH);
}

void test_parser_e_w_track(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[16] = "track E-W";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_tile tile;
	sem_tile_parse(&tile, &tokens, track_cache);

	g_assert_true(tile.class == TRACK);
	g_assert_true(tile.track->start == SEM_EAST);
	g_assert_true(tile.track->end == SEM_WEST);
}

void test_parser_n_s_e_w_track() {
	char track_description[16] = "track N-S+E-W";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_track* track;
	sem_track_parse(track_description, &track);

	g_assert_true(track->start == SEM_NORTH);
	g_assert_true(track->end == SEM_SOUTH);
	g_assert_true(track->next->start == SEM_EAST);
	g_assert_true(track->next->end == SEM_WEST);

	free(track);
}

void test_parser_two_way_points(sem_track_cache* track_cache, const void* data) {
	#pragma unused(data)
	char track_description[32] = "points W-E W-NE";
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, track_description, " ");

	sem_tile tile;
	sem_tile_parse(&tile, &tokens, track_cache);

	g_assert_true(tile.class == POINTS);
	g_assert_true(tile.track->start == SEM_WEST);
	g_assert_true(tile.track->end == SEM_EAST);

	g_assert_true(tile.points[0]->start == SEM_WEST);
	g_assert_true(tile.points[0]->end == SEM_EAST);
}