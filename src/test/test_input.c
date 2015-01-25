#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <glib.h>

#include "test_input.h"
#include "test_heap.h"

#include "sem_action_list.h"
#include "sem_input.h"
#include "sem_heap.h"
#include "sem_train.h"
#include "sem_world.h"

void test_input_null_action_for_unoccupied_coordinate(void);
void test_input_toggles_train_state(sem_heap* heap, const void* data);

void add_tests_input(void) {
	g_test_add_func("/input/null_action_for_unoccupied_coordinate", test_input_null_action_for_unoccupied_coordinate);
	add_test_heap("/input/toggles_train_state", test_input_toggles_train_state);
}

void test_input_null_action_for_unoccupied_coordinate() {
	sem_train train;
	train.x = 1;
	train.y = 4;

	sem_world world;
	world.train = &train;

	sem_input_event input;
	input.x = 3;
	input.y = 2;

	sem_action* action = NULL;

	sem_train_input_act_upon(&input, &world, &action);

	g_assert_null(action);
}

void test_input_toggles_train_state(sem_heap* heap, const void* data) {
	#pragma unused(data)

	sem_train train;
	train.x = 1;
	train.y = 4;
	train.moving = false;

	sem_world world;
	world.train = &train;

	sem_input_event input;
	input.time = 3000L;
	input.x = 1;
	input.y = 4;

	sem_action* change_state = NULL;

	sem_train_input_act_upon(&input, &world, &change_state);
	change_state->function(heap, change_state);

	free(change_state);

	sem_action* move = sem_heap_remove_earliest(heap);
	g_assert_cmpuint(move->time, ==, 3000L);

	move->function(heap, move);
	free(move);

	g_assert_true(train.moving == true);
	g_assert_true(train.x == 2);
}