#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <gb/drawing.h>
#include <stdio.h>
#include "IDs.h"
#include "Cursor.h"
#define SIM_WIDTH 160
#define SIM_HEIGHT 144
#define CHUNK_SIZE 8 // the current size of each chunk is 8x8
#define GRID_WIDTH 20
#define GRID_HEIGHT 18

typedef struct Cursor // the cursor, I guess...
{
    uint8_t SelectedDot;
    // uint8_t brushSize;
    uint8_t x; // pixel cord of cursor
    uint8_t y;

} CursorStruct;

typedef struct Dot
{
    uint8_t ID; // what kind of material are they?
    uint8_t color;
    // BOOLEAN canUpdate; // 0 for updates, 1 for no updates. Becomes 1 if the oldX and oldY are the same as last frame.
    // uint8_t oldX;      // the position last frame, used to tell if this dot should be skipped for performance reasons
    // uint8_t oldY;

} Dot;

// Create a structure to represent a chunk
typedef struct
{
    struct Dot chunkData[CHUNK_SIZE][CHUNK_SIZE];
} Chunk;

struct Cursor Cursor;

// Create a 2D array of chunks to represent the grid
Chunk grid[GRID_WIDTH][GRID_HEIGHT];

void Simulate(uint8_t gridx, uint8_t gridy, uint8_t x, uint8_t y)
{
    uint8_t tempid = grid[gridx][gridy].chunkData[x][y].ID;
    switch (tempid)
    {

    case SAND:
        if (y > 0 && grid[gridx][gridy].chunkData[x][y - 1].ID == AIR) // if down is empty
        {
            grid[gridx][gridy].chunkData[x][y].ID = AIR;
            grid[gridx][gridy].chunkData[x][y - 1].ID = SAND;
        }
        else if (x > 0 && y > 0 && grid[gridx][gridy].chunkData[x - 1][y - 1].ID == AIR) // if down and left is empty
        {
            grid[gridx][gridy].chunkData[x][y].ID = AIR;
            grid[gridx][gridy].chunkData[x - 1][y - 1].ID = SAND;
        }
        else if (x < CHUNK_SIZE - 1 && y > 0 && grid[gridx][gridy].chunkData[x + 1][y - 1].ID == AIR) // if down and right is empty
        {
            grid[gridx][gridy].chunkData[x][y].ID = AIR;
            grid[gridx][gridy].chunkData[x + 1][y - 1].ID = SAND;
        }
        break;

    case WATER:
        if (y > 0 && grid[gridx][gridy].chunkData[x][y - 1].ID == AIR) // if down is empty
        {
            grid[gridx][gridy].chunkData[x][y].ID = AIR;
            grid[gridx][gridy].chunkData[x][y - 1].ID = WATER;
        }
        else if (x > 0 && y > 0 && grid[gridx][gridy].chunkData[x - 1][y - 1].ID == AIR) // if down and left is empty
        {
            grid[gridx][gridy].chunkData[x][y].ID = AIR;
            grid[gridx][gridy].chunkData[x - 1][y - 1].ID = WATER;
        }
        else if (x < CHUNK_SIZE - 1 && y > 0 && grid[gridx][gridy].chunkData[x + 1][y - 1].ID == AIR) // if down and right is empty
        {
            grid[gridx][gridy].chunkData[x][y].ID = AIR;
            grid[gridx][gridy].chunkData[x + 1][y - 1].ID = WATER;
        }
        else if (x > 0 && y > 0 && grid[gridx][gridy].chunkData[x - 1][y].ID == AIR) // if left is empty
        {
            grid[gridx][gridy].chunkData[x][y].ID = AIR;
            grid[gridx][gridy].chunkData[x - 1][y].ID = WATER;
        }
        else if (x < CHUNK_SIZE - 1 && y > 0 && grid[gridx][gridy].chunkData[x + 1][y].ID == AIR) // if right is empty
        {
            grid[gridx][gridy].chunkData[x][y].ID = AIR;
            grid[gridx][gridy].chunkData[x + 1][y].ID = WATER;
        }
        break;

    default:
        break;
    }
}

void display(void)
{

    for (uint8_t gridx = 0; gridx < GRID_WIDTH; gridx++)
    {
        for (uint8_t gridy = GRID_HEIGHT; gridy > 0; gridy--)
        {
            for (uint8_t x = 0; x < CHUNK_SIZE; x++)
            {
                for (uint8_t y = CHUNK_SIZE; y > 0; y--)
                {
                    if (grid[gridx][gridy].chunkData[x][y].ID != AIR)
                    {
                        plot((gridx * 8) + x, (gridy * 8) + y, grid[gridx][gridy].chunkData[x][y].color, SOLID);
                        Simulate(gridx, gridy, x, y);
                    }
                }
            }
        }
    }
}

void PlaceParticle(uint8_t x, uint8_t y, uint8_t ID)
{
    uint8_t gridx = x % 8;
    uint8_t gridy = y % 8;
    uint8_t chunkx = x;
    uint8_t chunky = y;
    if (x <= 0)
    {
        chunkx = 0;
    }
    else
    {
        chunkx = (uint8_t)(x / 8);
    }

    if (y <= 0)
    {
        chunky = 0;
    }
    else
    {

        chunky = (uint8_t)(y / 8);
    }

    switch (ID)
    {
    case EMPTY:
        grid[gridx][gridy].chunkData[chunkx][chunky].ID = AIR;
        break;

    case SAND:
        grid[gridx][gridy].chunkData[chunkx][chunky].ID = SAND;
        grid[gridx][gridy].chunkData[chunkx][chunky].color = DMG_LITE_GRAY;
        break;

    default:
        break;
    }
}

void main(void)
{
    cpu_fast();
    SHOW_BKG;
    SHOW_SPRITES;

    set_sprite_data(0, 2, CursorGRAPHIC);
    set_sprite_tile(0, 1);
    // Xstart = getXstart();
    // Ystart = getYstart();
    Cursor.x = 80;
    Cursor.y = 72;
    Cursor.SelectedDot = 1;
    // Cursor.brushSize = 1;

    PlaceParticle(10, 10, SAND);

    while (1)
    {
        uint8_t cur = joypad();

        if (cur & J_A)
        {

            PlaceParticle(Cursor.x, Cursor.y, Cursor.SelectedDot);
        }

        if (cur & J_B)
        {
        }

        if (cur & J_SELECT)
        {
            if (Cursor.SelectedDot != NUM_OF_MATERIALS)
            {
                Cursor.SelectedDot++;
            }
            else
            {
                Cursor.SelectedDot = 0;
            }
        }

        if (cur & J_START)
        {
        }

        if (cur & J_UP)
        {
            Cursor.y--;
        }
        else if (cur & J_DOWN)
        {
            Cursor.y++;
        }
        else if (cur & J_LEFT)
        {
            Cursor.x--;
        }
        else if (cur & J_RIGHT)
        {
            Cursor.x++;
        }
        move_sprite(0, Cursor.x + 8, Cursor.y + 16);
        display();
        vsync();
    }
}
