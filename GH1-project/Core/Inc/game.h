
#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include "tron_song.h"


#define MAX_BEATS 1024


//GAME SETTINGS
#define GETBIT(var, bitnum) (((var) >> (bitnum)) & 1)
#define NOTE_WIDTH 48
#define NOTE_HEIGHT 20
#define COLOR_RED     0xFFFF0000
#define COLOR_GREEN   0xFF00FF00
#define COLOR_BLUE    0xFF0000FF
#define COLOR_YELLOW  0xFFFFFF00
#define COLOR_WHITE   0xFFFFFFFF

#define DEBOUNCER_TOLERANCE_MS 20


typedef uint8_t row; //red, green, blue, yellow, white;


struct song {
    row *rows; //array of rows IN ORDER, a song will have rows equal to the number of beats in a song
    int number_of_rows; //aka number of beats
    int bpm; //Beats per minute / tempo
};

struct track {
    int track_length; // = 16;
    int track_width; // = 5;
};

struct gameState {
    //array of every row on the track
    row current_rows[16]; //16 rows displayed at a time
    row previous_rows[16]; //keeping track of the previous rows allows for clearing only the notes specifically instead of the entire screen, this is a basic memory more memory more speed optimization
    //could also put gameStateCurrentRow or could handle that at runtime
    struct song current_song;
    int current_pos_in_song;

    //FLAGS
	int game_started;
	int audio_started;

    //TIMING
    float beat_timer;

	int super_counter;
	float start_delay;
	uint32_t start_timestamp;
	uint8_t waiting_for_start_delay;
	uint32_t song_start_time;

    uint32_t white_collision_timestamp; //TODO flip the order of these for consistency, left to right on the game equals up to down should be the convention
    uint32_t yellow_collision_timestamp;
    uint32_t blue_collision_timestamp;
    uint32_t green_collision_timestamp;
    uint32_t red_collision_timestamp;

    uint32_t white_input_timestamp;
    uint32_t yellow_input_timestamp;
    uint32_t blue_input_timestamp;
    uint32_t green_input_timestamp;
    uint32_t red_input_timestamp;

    int note_input_debouncer; //0 = OFF (NOT BLOCKING),     1 = ON (BLOCKING INPUT) //TODO there will eventually be 5 so we can play multiple notes at once and debounce all 5 input lines simultaneously
    uint32_t note_input_debouncer_last_toggle_timestamp;

    int current_score;
};

extern struct gameState g_gameState;



int init_game();

void update_game(float delta);

void register_note_groundcollision_timestamps(row rowThatJustCollidedWithTheFloor);
void poll_note_inputs();
void note_input_debouncer_toggle_on();

void display_game(); //this one will be a little tricky





#endif
