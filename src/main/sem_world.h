#ifndef _SEM_WORLD_H
#define _SEM_WORLD_H

typedef struct sem_world sem_world;
typedef struct sem_track sem_track;

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_train.h"

typedef enum {
	BLANK, TRACK, POINTS
} sem_tile_class;

struct sem_track {
	unit_vector start;
	unit_vector end;	
	sem_track* next; /**< when two tracks appear on the same tile, this points to the second piece of track */
};

typedef struct {
	sem_tile_class class;	
	sem_track* track;
	sem_track* points[3];		
} sem_tile;

struct sem_world {
	sem_dynamic_array* trains;
	uint32_t max_x;
	uint32_t max_y;
	sem_tile* tiles;
};

typedef struct {
	/**
	 * @brief The new direction for the head of the train.
	 */
	unit_vector direction;

	/**
	 * @brief The track which the head of the train must now occupy.
	 */
	sem_track* track;

	/**
	 * @brief Indicates whether or not the points need switching to make the track active to prevent derailment.
	 *
	 * If @c true, indicates that the train has been accepted onto the tile,
	 * but that the track on which it must run is not currently active.
	 * The train must either make that track active by
	 * calling sem_points_activate() or it must derail.
	 */
	bool need_points_switch;
} sem_tile_acceptance;

/**
 * @brief Initialise a world with \ref BLANK tiles and no trains.
 *
 * Once initialised, a sem_world should be destroyed by calling sem_world_destroy().
 * 
 * @param world A pointer to an uninitialised sem_world whose dimensions are already specified.
 * @return \ref SEM_OK on success, or \ref SEM_ERROR if there was insufficient memory to initialise the world.
 */
sem_success sem_world_init_blank(sem_world* world);

/**
 * @brief Destroy an initialised world.
 *
 * The world's trains and tiles are freed.
 */
void sem_world_destroy(sem_world* world);

/**
 * @brief Add a train to the world
 * 
 * The train should already be initialised such that its cars are placed on 
 * the track belonging to the underlying tiles.
 *
 * @return \ref SEM_OK on success, or \ref SEM_ERROR if there was insufficient memory to add the train to the world.
 */
sem_success sem_world_add_train(sem_world* world, sem_train* train);

sem_tile* sem_tile_at_coord(sem_world* world, sem_coordinate* c);

sem_tile* sem_tile_at(sem_world* world, uint32_t x, uint32_t y);

/**
 * Accept a train that has moved onto a new tile.
 *
 * @param relocated_track If the train is accepted onto the new tile, this points to the track which the head of the train now occupies, otherwise NULL.
 */
sem_success sem_tile_accept(sem_train* train, sem_tile* tile, sem_track** relocated_track);

void sem_tile_switch_points(sem_tile* tile);

void sem_tile_set_track(sem_tile* tile, sem_track* track);

void sem_track_set(sem_track* track, unit_vector start, unit_vector end);

#endif
