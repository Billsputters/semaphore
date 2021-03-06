#include <glib.h>

#include "test_heap.h"

#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_heap.h"

void test_heap_removes_earliest(sem_dynamic_array* heap, const void* data);
void test_heap_removes_nothing_in_empty_heap(sem_dynamic_array* heap, const void* data);

void test_heap_setup(sem_dynamic_array* heap, const void* data);
void test_heap_teardown(sem_dynamic_array* heap, const void* data);

void test_heap_setup(sem_dynamic_array* heap, const void* data) {
	#pragma unused(data)
	sem_heap_init(heap);
}

void test_heap_teardown(sem_dynamic_array* heap, const void* data) {
	#pragma unused(data)
	sem_dynamic_array_destroy(heap);
}

void add_tests_heap() {
	add_test_heap("/heap/removes_earliest", test_heap_removes_earliest);
	add_test_heap("/heap/removes_nothing_in_empty_heap", test_heap_removes_nothing_in_empty_heap);
}

void add_test_heap(const char *test_name, void (*test)(sem_dynamic_array*, const void* data)) {
	g_test_add(test_name, sem_dynamic_array, NULL, test_heap_setup, test, test_heap_teardown);
}

void test_heap_removes_earliest(sem_dynamic_array* heap, const void* data) {
	#pragma unused(data)

	sem_action a1, a2, a3;
	a1.time = 2000L;
	a2.time = 3000L;
	a3.time = 1000L;

	sem_heap_insert(heap, &a1);
	sem_heap_insert(heap, &a2);
	sem_heap_insert(heap, &a3);
	
	g_assert_true(sem_heap_remove_earliest(heap) == &a3);
	g_assert_true(sem_heap_remove_earliest(heap) == &a1);
	g_assert_true(sem_heap_remove_earliest(heap) == &a2);
}

void test_heap_removes_nothing_in_empty_heap(sem_dynamic_array* heap, const void* data) {
	#pragma unused(data)

	g_assert_null(sem_heap_remove_earliest(heap));
}
