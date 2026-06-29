#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "stm32f4xx_hal.h"
#include "ili9341.h"
#include "audio.h"

struct gameState g_gameState;
row g_rows[MAX_BEATS]; //TODO should this go in the header? will this be in SRAM? if it goes in the header it needs extern right?


const unsigned char tron_song_bin[] = {
    83, 79, 78, 71, 71, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
    1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
    2, 0, 0, 2, 0, 0, 2, 2, 2, 4, 4, 8, 8, 1, 1, 1,
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 1, 0, 0, 2, 2, 2, 4, 4, 8, 8, 1, 1, 1,
    1, 1, 1, 8, 8, 8, 8, 8, 0, 4, 4, 4, 4, 4, 4, 1,
    1, 1, 1, 1, 1, 1, 1, 2, 2, 4, 4, 8, 8, 1, 1, 1,
    1, 1, 1, 8, 8, 8, 8, 8, 0, 4, 4, 4, 4, 4, 4, 0,
    0, 0, 0, 0, 0, 0, 0, 2, 2, 4, 4, 8, 8, 1, 1, 1,
    1, 1, 1, 4, 4, 4, 4, 4, 0, 2, 2, 2, 2, 2, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 2, 2, 4, 4, 8, 8, 1, 1, 1,
    1, 1, 1, 16, 16, 16, 16, 16, 0, 8, 8, 8, 8, 8, 8, 1,
    1, 1, 1, 1, 1, 1, 1, 2, 2, 4, 4, 8, 8, 1, 1, 1,
    1, 1, 1, 8, 8, 8, 8, 8, 0, 4, 4, 4, 4, 4, 4, 1,
    1, 1, 1, 1, 1, 1, 1, 2, 2, 4, 4, 8, 8, 1, 1, 1,
    1, 1, 1, 8, 8, 8, 8, 8, 0, 4, 4, 4, 4, 4, 4, 1,
    1, 1, 1, 1, 1, 1, 1, 2, 2, 4, 4, 8, 8, 2, 2, 4,
    4, 8, 1, 1, 1, 1, 1, 1, 16, 16, 16, 16, 16, 16, 16, 8,
    8, 8, 8, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1
};
const unsigned int tron_song_bin_len = 333;


int init_game()
{
	//verify the song data, in this case its an unsigned char[], the first 4 bytes should be an ASCII for "SONG"
    if (memcmp(tron_song_bin, "SONG", 4) != 0)
    {
        printf("SONGfile header check failed!\n");
        exit(1);
    }


    uint16_t row_count; //rewrote this implementation for tron_song being a c array, the 3 lines above is for parsing the song when its a binary file
    memcpy(&row_count, &tron_song_bin[4], sizeof(row_count)); //bytes 4 and 5 in the song data is a 16bit number of how many rows the song is,



    const uint8_t *ptr = &tron_song_bin[6]; //skip header

    for (int i = 0; i < row_count; i++)
    {
        row currentRow;

        memcpy(&currentRow, ptr, sizeof(row));
        ptr += sizeof(row);

        g_rows[i] = currentRow;
    }

    struct song gameCurrentSong; //declare this on the stack and then add it to g_gameState which is global when you're done appending its members
    gameCurrentSong.number_of_rows = row_count;
    gameCurrentSong.bpm = 100; //TODO HARDCODED
    gameCurrentSong.rows = g_rows;

    g_gameState.current_song = gameCurrentSong;
    g_gameState.current_pos_in_song = 0;
    g_gameState.audio_started = 0;
    g_gameState.super_counter = 0;


    for(int i =0; i<16; i++) //load the first 16 rows onto the track
    {
	    g_gameState.current_rows[i] = g_gameState.current_song.rows[i];
    }


    //initialize all the global game state variables
    g_gameState.beat_timer = 0.0f;
    g_gameState.start_delay = 2000; //2000ms
    g_gameState.song_start_time = 0;


    g_gameState.white_collision_timestamp = 0;
    g_gameState.yellow_collision_timestamp = 0;
    g_gameState.blue_collision_timestamp = 0;
    g_gameState.green_collision_timestamp = 0;
    g_gameState.red_collision_timestamp = 0;

    g_gameState.white_input_timestamp = 0;
    g_gameState.yellow_input_timestamp = 0;
    g_gameState.blue_input_timestamp = 0;
    g_gameState.green_input_timestamp = 0;
    g_gameState.red_input_timestamp = 0;

    g_gameState.note_input_debouncer = 0;
    g_gameState.note_input_debouncer_last_toggle_timestamp = 0;

    g_gameState.current_score = 0;

    g_gameState.game_started = 0;

    return 0;
}


void update_game(float delta_unused)
{
    if(!g_gameState.game_started) //if game hasn't started yet start the game
    {
        int white_btn_state = HAL_GPIO_ReadPin(GPIOD, btn0_Pin);

        if(white_btn_state == GPIO_PIN_SET)
        {
            g_gameState.game_started = 1;
            g_gameState.waiting_for_start_delay = 1;
            g_gameState.start_timestamp = HAL_GetTick();

            ILI9341_FastClear(ILI9341_BLACK);
            Audio_Start("exporttestm.wav");
        }

        return;
    }

    // ===== WAIT FOR START DELAY =====
    if(g_gameState.waiting_for_start_delay)
    {
        if(HAL_GetTick() - g_gameState.start_timestamp < g_gameState.start_delay)
        {
            return;
        }

        g_gameState.waiting_for_start_delay = 0;

        //anchor song start time
        g_gameState.song_start_time = HAL_GetTick();
        g_gameState.current_pos_in_song = 0;
    }


    uint32_t elapsed_ms = HAL_GetTick() - g_gameState.song_start_time;

    const float time_correction = 1.055f;

    float adjusted_time = (float)elapsed_ms * time_correction;

    //convert BPM to ms per row
    float ms_per_row = (60.0f / g_gameState.current_song.bpm) * 1000.0f / 4.0f;

    //where we SHOULD be in the song
    uint32_t expected_row = (uint32_t)(adjusted_time / ms_per_row);

    //catch up if behind
    while(g_gameState.current_pos_in_song < expected_row)
    {
        uint8_t new_row = g_gameState.current_song.rows[g_gameState.current_pos_in_song + 16];

        memcpy(g_gameState.previous_rows, g_gameState.current_rows, 16);

        for (int i = 1; i < 16; i++) {
            if(i == 15)
                register_note_groundcollision_timestamps(g_gameState.current_rows[0]);

            g_gameState.current_rows[i - 1] = g_gameState.current_rows[i];
        }

        g_gameState.current_rows[15] = new_row;
        g_gameState.current_pos_in_song++;
    }

    if (adjusted_time >= 60000.0f)
    {
        __NVIC_SystemReset(); //song is over,  reset game
    }
}



void register_note_groundcollision_timestamps(row rowThatJustCollidedWithTheFloor)
{
    if(GETBIT(rowThatJustCollidedWithTheFloor, 4)) //RED
    {
        g_gameState.red_collision_timestamp = HAL_GetTick();
    }
    if(GETBIT(rowThatJustCollidedWithTheFloor, 3)) //GREEN
    {
    	g_gameState.green_collision_timestamp = HAL_GetTick();
    }
    if(GETBIT(rowThatJustCollidedWithTheFloor, 2)) //BLUE
    {
    	g_gameState.blue_collision_timestamp = HAL_GetTick();
    }
    if(GETBIT(rowThatJustCollidedWithTheFloor, 1)) //YELLOW
    {
    	g_gameState.yellow_collision_timestamp = HAL_GetTick();
    }
    if(GETBIT(rowThatJustCollidedWithTheFloor, 0)) //WHITE
    {
    	g_gameState.white_collision_timestamp = HAL_GetTick();
    }
}


void poll_note_inputs()
{
	if(poll_input_debouncer())
	{
		int white_btn_state = HAL_GPIO_ReadPin(GPIOD, 			btn0_Pin);	//white i think //TODO should we do direct read from register instead of HAL? how much faster even is that ?
		int yellow_btn_state = HAL_GPIO_ReadPin(GPIOD, 			btn1_Pin);	//yellow
		int blue_btn_state = HAL_GPIO_ReadPin(GPIOG, 			btn2_Pin);	//blue
		int green_btn_state = HAL_GPIO_ReadPin(GPIOG, 			btn3_Pin);	//green
		int red_btn_state = HAL_GPIO_ReadPin(GPIOB, 			btn4_Pin);	//red //TODO could be reverse order from this


		if(white_btn_state == GPIO_PIN_SET)
		{
			g_gameState.white_input_timestamp = HAL_GetTick();
			update_score_based_on_comparing_timestamps(g_gameState.white_input_timestamp, g_gameState.white_collision_timestamp);
			note_input_debouncer_toggle_on();
		}

		if(yellow_btn_state == GPIO_PIN_SET)
		{
			g_gameState.yellow_input_timestamp = HAL_GetTick();
			update_score_based_on_comparing_timestamps(g_gameState.yellow_input_timestamp, g_gameState.yellow_collision_timestamp);
			note_input_debouncer_toggle_on();
		}

		if(blue_btn_state == GPIO_PIN_SET)
		{
			g_gameState.blue_input_timestamp = HAL_GetTick();
			update_score_based_on_comparing_timestamps(g_gameState.blue_input_timestamp, g_gameState.blue_collision_timestamp);
			note_input_debouncer_toggle_on();
		}

		if(green_btn_state == GPIO_PIN_SET)
		{
			g_gameState.green_input_timestamp = HAL_GetTick();
			update_score_based_on_comparing_timestamps(g_gameState.green_input_timestamp, g_gameState.green_collision_timestamp);
			note_input_debouncer_toggle_on();
		}

		if(red_btn_state == GPIO_PIN_SET)
		{
			g_gameState.red_input_timestamp = HAL_GetTick();
			update_score_based_on_comparing_timestamps(g_gameState.red_input_timestamp, g_gameState.red_collision_timestamp);
			note_input_debouncer_toggle_on();
		}
	}
}


void note_input_debouncer_toggle_on()
{
	g_gameState.note_input_debouncer = 1;
	g_gameState.note_input_debouncer_last_toggle_timestamp = HAL_GetTick();
}


int poll_input_debouncer()
{
	if(HAL_GetTick() > g_gameState.note_input_debouncer_last_toggle_timestamp + DEBOUNCER_TOLERANCE_MS)
	{
		g_gameState.note_input_debouncer = 0;
		return 1; //allow input
	}
	else //still blocked
	{
		return 0;
	}
}


void update_score_based_on_comparing_timestamps(uint32_t buttonInputRegisteredTimestamp, uint32_t correspondingColorGroundCollisionTimestamp)
{
    uint32_t diff;

    if (buttonInputRegisteredTimestamp > correspondingColorGroundCollisionTimestamp)
        diff = buttonInputRegisteredTimestamp - correspondingColorGroundCollisionTimestamp;
    else
        diff = correspondingColorGroundCollisionTimestamp - buttonInputRegisteredTimestamp;

    if (diff <= 100)
    {
        g_gameState.current_score += 20;
    }
    else if (diff <= 200) //LATE
    {
        g_gameState.current_score += 5;
    }
    else if (diff <= 300) //MISS
    {
        g_gameState.current_score -= 20;
    }
}

void handle_events()
{
	poll_note_inputs();
}



void draw_row(uint8_t row, int x, int y, int width, int height) //TODO change all these to fast fill rectangle
{
    if(GETBIT(row, 4)) //RED
    {
        ILI9341_FastFillRectangle(x, y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_RED);
    }
    if(GETBIT(row, 3)) //GREEN
    {
        ILI9341_FastFillRectangle(x + NOTE_WIDTH , y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_GREEN);
    }
    if(GETBIT(row, 2)) //BLUE
    {
        ILI9341_FastFillRectangle(x + (NOTE_WIDTH * 2) , y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_BLUE);
    }
    if(GETBIT(row, 1)) //YELLOW
    {
        ILI9341_FastFillRectangle(x + (NOTE_WIDTH * 3) , y , NOTE_WIDTH, NOTE_HEIGHT, ILI9341_YELLOW);
    }
    if(GETBIT(row, 0)) //WHITE
    {
        ILI9341_FastFillRectangle(x + (NOTE_WIDTH * 4) , y , NOTE_WIDTH, NOTE_HEIGHT, ILI9341_WHITE);
    }
}


void erase_row(uint8_t row, int x, int y, int width, int height)  //TODO change all these to fast fill rectangle //TODO what is int width and int height even used for??
{
    if(GETBIT(row, 4))
        ILI9341_FastFillRectangle(x, y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_BLACK);

    if(GETBIT(row, 3))
        ILI9341_FastFillRectangle(x + NOTE_WIDTH, y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_BLACK);

    if(GETBIT(row, 2))
        ILI9341_FastFillRectangle(x + NOTE_WIDTH * 2, y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_BLACK);

    if(GETBIT(row, 1))
        ILI9341_FastFillRectangle(x + NOTE_WIDTH * 3, y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_BLACK);

    if(GETBIT(row, 0))
        ILI9341_FastFillRectangle(x + NOTE_WIDTH * 4, y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_BLACK);
}


void erase_note(row noteMask, int y)
{
    if(GETBIT(noteMask, 4)) // RED
        ILI9341_FastFillRectangle(0, y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_BLACK);

    if(GETBIT(noteMask, 3)) // GREEN
        ILI9341_FastFillRectangle(NOTE_WIDTH, y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_BLACK);

    if(GETBIT(noteMask, 2)) // BLUE
        ILI9341_FastFillRectangle(NOTE_WIDTH * 2, y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_BLACK);

    if(GETBIT(noteMask, 1)) // YELLOW
        ILI9341_FastFillRectangle(NOTE_WIDTH * 3, y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_BLACK);

    if(GETBIT(noteMask, 0)) // WHITE
        ILI9341_FastFillRectangle(NOTE_WIDTH * 4, y, NOTE_WIDTH, NOTE_HEIGHT, ILI9341_BLACK);
}


void display_game()
{
	if(!g_gameState.game_started) //if game hasn't started yet
	{
		ILI9341_WriteString(10, 10, "PRESS WHITE TO START", Font_11x18, ILI9341_WHITE, ILI9341_BLACK);
	}
	else
	{
		for(int i = 0; i < 16; i++) //in this loop we use objects to_erase and to_draw to designate only what has changed from one draw call to the next.
									//we then use that to only update what is to be changed on the screen. This is way better than clearing the entire screen and redrawing everything every draw call
		{
			row previous_row = g_gameState.previous_rows[i];
			row current_row = g_gameState.current_rows[i];

			int y = (16 - 1 - i) * NOTE_HEIGHT;


			row to_erase = previous_row & ~current_row;  //notes that disappeared
			row to_draw = current_row & ~previous_row;   //notes that appeared

			if(to_erase)
				erase_note(to_erase, y);

			if(to_draw)
				draw_row(to_draw, 0, y, NOTE_WIDTH, NOTE_HEIGHT);
		}

		//DISPLAY THE SCORE
		char buffer[32];
		snprintf(buffer, sizeof(buffer), "Score: %d", g_gameState.current_score);

		ILI9341_WriteString(10, 10, buffer, Font_11x18, ILI9341_WHITE, ILI9341_BLACK);
	}
}


