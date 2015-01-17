#include <stdio.h>
#include <inttypes.h>

#include <glib.h>

#include "test_timer.h"

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/timer_increments_by_one_second", test_timer_increments_by_one_second);
	g_test_add_func("/timer_increments_twice_clock_with_multiple_of_two", test_timer_increments_twice_clock_with_multiple_of_two);

	return g_test_run();
}

