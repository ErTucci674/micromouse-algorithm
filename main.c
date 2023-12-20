/*
**************************************************************************************
* Micromouse Algorithm
*
* Copyright (c) 2023 Alessandro Amatucci Girlanda
*
* This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike
* 4.0 International License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-nc-sa/4.0/ or send a letter to Creative
* Commons, PO Box 1866, Mountain View, CA 94042, USA.
*
* You are free to:
*   - Share — copy and redistribute the material in any medium or format
*   - Adapt — remix, transform, and build upon the material
*
* Under the following terms:
*   - Attribution — You must give appropriate credit, provide a link to the license, and
*                  indicate if changes were made. You may do so in any reasonable manner,
*                  but not in any way that suggests the licensor endorses you or your use.
*   - NonCommercial — You may not use the material for commercial purposes.
*   - ShareAlike — If you remix, transform, or build upon the material, you must
*                  distribute your contributions under the same license as the original.
*
* No additional restrictions — You may not apply legal terms or technological measures
* that legally restrict others from doing anything the license permits.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 8 // MAX Maze Length
#define LENGTH 6
#define HEIGHT 8

#define BITS 15          // 4 bits/walls around each cell
#define TOT_CELLS 48     // Total number of cells to be found
#define TOT_DEST_CELLS 4 // Number of destination cells
#define Y_DEST 3         // Destination y-coordinate
#define X_DEST 3         // Destination x-coordinate
#define LAST_VIS_CELLS 4 // Last temporary visited cells

// Walls
#define NORTH_WALL 0b1000 // 8
#define EAST_WALL 0b0100  // 4
#define SOUTH_WALL 0b0010 // 2
#define WEST_WALL 0b0001  // 1

#define VERT_WALLS (NORTH_WALL | SOUTH_WALL)
#define HORIZ_WALLS (WEST_WALL | EAST_WALL)

// Cells Checking
#define CELL_VISITED 0b10000 // 16
#define CELL_USED 0b100000   // 32 - Cell used for the final route
#define CELL_OUT 0b1000000   // 64 - Out of Bound
#define DEST_CELL 0b10000000 // 128 - Destination cells

// Mouse Pointing Direction
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

// Generating a maze to simulate an environement
void Generate_Maze(unsigned int maze[HEIGHT][LENGTH])
{
    unsigned int r, c;

    for (r = 0; r < SIZE; r++)
        for (c = 0; c < SIZE; c++)
        {
            maze[r][c] = 15;
        }
}

// Generating the right route to get to destination
void Generate_Route(unsigned int maze[HEIGHT][LENGTH])
{
    unsigned int maze_route[HEIGHT][LENGTH] =
        {
            {6, 7, 7, 7, 7, 3},
            {14, 9, 10, 12, 9, 10},
            {10, 6, 15, 5, 3, 10},
            {10, 10, 14, 3, 14, 11},
            {10, 8, 12, 9, 10, 10},
            {14, 5, 5, 3, 10, 10},
            {10, 4, 3, 12, 3, 8},
            {8, 4, 13, 5, 13, 1}};

    unsigned int r, c;

    for (r = 0; r < HEIGHT; r++)
        for (c = 0; c < LENGTH; c++)
        {
            if ((maze_route[(HEIGHT - 1) - r][c] & NORTH_WALL) == NORTH_WALL)
            {
                // Removing North wall from current cell
                if ((maze[r][c] & NORTH_WALL) == NORTH_WALL)
                    maze[r][c] ^= NORTH_WALL;
                // Removing South wall from next cell
                if (r < (HEIGHT - 1))
                    if ((maze[r + 1][c] & SOUTH_WALL) == SOUTH_WALL)
                        maze[r + 1][c] ^= SOUTH_WALL;
            }
            if ((maze_route[(HEIGHT - 1) - r][c] & EAST_WALL) == EAST_WALL)
            {
                // Removing East wall from current cell
                if ((maze[r][c] & EAST_WALL) == EAST_WALL)
                    maze[r][c] ^= EAST_WALL;
                // Removing West wall from next cell
                if (c < (LENGTH - 1))
                    if ((maze[r][c + 1] & WEST_WALL) == WEST_WALL)
                        maze[r][c + 1] ^= WEST_WALL;
            }
            if ((maze_route[(HEIGHT - 1) - r][c] & SOUTH_WALL) == SOUTH_WALL)
            {
                // Removing South wall from current cell
                if ((maze[r][c] & SOUTH_WALL) == SOUTH_WALL)
                    maze[r][c] ^= SOUTH_WALL;

                // Removing North wall from previous cell
                if (r != 0)
                    if ((maze[r - 1][c] & NORTH_WALL) == NORTH_WALL)
                        maze[r - 1][c] ^= NORTH_WALL;
            }
            if ((maze_route[(HEIGHT - 1) - r][c] & WEST_WALL) == WEST_WALL)
            {
                if ((maze[r][c] & WEST_WALL) == WEST_WALL)
                    maze[r][c] ^= WEST_WALL;

                if (c != 0)
                    if ((maze[r][c - 1] & EAST_WALL) == EAST_WALL)
                        maze[r][c - 1] ^= EAST_WALL;
            }
        }
}

// Show the "values" of the walls
void Print_Maze(unsigned int maze[HEIGHT][LENGTH])
{
    int r, c;

    printf("--- Generated Maze ----\n");
    for (r = (HEIGHT - 1); r > -1; r--)
    {
        for (c = 0; c < LENGTH; c++)
        {
            printf("%d\t", maze[r][c]);
        }
        printf("\n");
    }
    printf("\n\n");
}

void Print_Mouse_Maze(unsigned int mouse_maze[SIZE][SIZE])
{
    int r, c, cell_value;

    printf("--- Maze Known by the Mouse ---\n");
    for (r = (SIZE - 1); r > -1; r--)
    {
        for (c = 0; c < SIZE; c++)
        {
            cell_value = mouse_maze[r][c];

            // we want to see just the walls
            // OR if the cell is OUT OF BOUNDS
            if ((cell_value & CELL_OUT) == CELL_OUT)
                cell_value = CELL_OUT;
            else
            {
                if ((cell_value & DEST_CELL) == DEST_CELL)
                    cell_value ^= DEST_CELL;
                if ((cell_value & CELL_VISITED) == CELL_VISITED)
                    cell_value ^= CELL_VISITED;
                if ((cell_value & CELL_USED) == CELL_USED)
                    cell_value ^= CELL_USED;
            }

            printf("%d\t", cell_value);
        }
        printf("\n");
    }
    printf("\n\n");

    printf("--- Final Route to Destination ---\n");
    for (r = (SIZE - 1); r > -1; r--)
    {
        for (c = 0; c < SIZE; c++)
        {
            if ((mouse_maze[r][c] & CELL_USED) == CELL_USED)
                printf("1");
            else
                printf("0");
            printf("\t");
        }
        printf("\n");
    }
}

struct Mouse_Settings
{
    unsigned int pos_x, pos_y;   // Current position and orientation
    unsigned int dirs[4], m_dir; // 4 possible directions of the mouse
    unsigned int sides_found, dest_cells_found, destination_found, cells_found;
    unsigned int prev_cells[SIZE][SIZE], prev_cell, prev_cell_x, prev_cell_y; // where the last # visited cells will be stored
};

void Mouse_Setup(struct Mouse_Settings *p_mouse)
{
    unsigned int r, c;

    // Mouse Starting position
    p_mouse->pos_x = 0;
    p_mouse->pos_y = 0;

    // Possible directions referred to walls
    p_mouse->dirs[0] = NORTH_WALL;
    p_mouse->dirs[1] = EAST_WALL;
    p_mouse->dirs[2] = SOUTH_WALL;
    p_mouse->dirs[3] = WEST_WALL;
    // Poiting Upwards/North
    p_mouse->m_dir = NORTH;

    // Nothing is known
    p_mouse->sides_found = 0;
    p_mouse->dest_cells_found = 0;
    p_mouse->destination_found = 0;
    p_mouse->cells_found = 1; // Mouse knows the first cell

    // Clearing -> Cells previously visited
    for (r = 0; r < SIZE; r++)
        for (c = 0; c < SIZE; c++)
            p_mouse->prev_cells[r][c] = 0;

    p_mouse->prev_cell = 1; // This will represent the first of the last 8 visited cells
    p_mouse->prev_cell_x = 99;
    p_mouse->prev_cell_y = 99;
    p_mouse->prev_cells[0][0] = p_mouse->prev_cell; // Starting position
}

void Mouse_Maze_Setup(unsigned int mouse_maze[SIZE][SIZE])
{
    unsigned int r, c;

    for (r = 0; r < SIZE; r++)
        for (c = 0; c < SIZE; c++)
        {
            mouse_maze[r][c] = 0;
            if (r == 0)
                mouse_maze[r][c] |= SOUTH_WALL;
            else if (r == (SIZE - 1))
                mouse_maze[r][c] |= NORTH_WALL;
            if (c == 0)
                mouse_maze[r][c] |= WEST_WALL;
            else if (c == (SIZE - 1))
                mouse_maze[r][c] |= EAST_WALL;
        }

    // Setting first cell of the maze (3 mandatory walls)
    mouse_maze[0][0] |= EAST_WALL;
    mouse_maze[0][0] |= CELL_VISITED;
    mouse_maze[0][1] |= WEST_WALL; // Caused by the setup of the first cell

    // Setting Main destination cell
    mouse_maze[3][3] |= DEST_CELL;
}

void Mouse_Exploring(unsigned int maze[HEIGHT][LENGTH], unsigned int mouse_maze[SIZE][SIZE], struct Mouse_Settings *p_mouse, unsigned int discovered_cells[SIZE][SIZE])
{
    unsigned int chosen_dir, temp_prev_cell, stop = 1;
    int turn_check;

    unsigned int i, r, c, temp_dir, prev_cell_check;

    unsigned int poss_dirs[4];
    for (i = 0; i < 4; i++) // Setting array
        poss_dirs[i] = 99;

    temp_dir = p_mouse->m_dir;
    // Find all of the cells - Flood Fill
    if (p_mouse->cells_found != TOT_CELLS)
    {
        for (i = 0; i < 4; i++)
        {
            if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & p_mouse->dirs[temp_dir]) != p_mouse->dirs[temp_dir]) // checking for a wall
            {
                // Checking if North is Out of Bound
                if ((temp_dir == NORTH) && ((mouse_maze[p_mouse->pos_y + 1][p_mouse->pos_x] & CELL_OUT) != CELL_OUT))
                {
                    // Checking if next cell has been visited
                    if (
                        ((mouse_maze[p_mouse->pos_y + 1][p_mouse->pos_x] & CELL_VISITED) != CELL_VISITED) ||
                        // Get out of the Destination Loop
                        ((p_mouse->dest_cells_found == TOT_DEST_CELLS) && ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & DEST_CELL) == DEST_CELL) && ((mouse_maze[p_mouse->pos_y + 1][p_mouse->pos_x] & DEST_CELL) != DEST_CELL)))
                    {
                        chosen_dir = temp_dir;
                        stop = 0;
                        break;
                    }
                    else
                    {
                        poss_dirs[i] = NORTH;                                            // possible route
                        if (p_mouse->prev_cells[p_mouse->pos_y + 1][p_mouse->pos_x] > 0) // checking if north cell is the previous cell
                            prev_cell_check = NORTH;
                    }
                }

                // Checking if East is Out of Bound
                else if ((temp_dir == EAST) && ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x + 1] & CELL_OUT) != CELL_OUT))
                {
                    if (
                        ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x + 1] & CELL_VISITED) != CELL_VISITED) ||
                        // Get out of the Destination Loop
                        ((p_mouse->dest_cells_found == TOT_DEST_CELLS) && ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & DEST_CELL) == DEST_CELL) && ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x + 1] & DEST_CELL) != DEST_CELL)))
                    {
                        chosen_dir = temp_dir;
                        stop = 0;
                        break;
                    }
                    else
                    {
                        poss_dirs[i] = EAST;                                             // possible route
                        if (p_mouse->prev_cells[p_mouse->pos_y][p_mouse->pos_x + 1] > 0) // checking if east cell is the previous cell
                            prev_cell_check = EAST;
                    }
                }

                // Checking if South is Out of Bound
                else if ((temp_dir == SOUTH) && ((mouse_maze[p_mouse->pos_y - 1][p_mouse->pos_x] & CELL_OUT) != CELL_OUT))
                {
                    if (
                        ((mouse_maze[p_mouse->pos_y - 1][p_mouse->pos_x] & CELL_VISITED) != CELL_VISITED) ||
                        // Get out of the Destination Loop
                        ((p_mouse->dest_cells_found == TOT_DEST_CELLS) && ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & DEST_CELL) == DEST_CELL) && ((mouse_maze[p_mouse->pos_y - 1][p_mouse->pos_x] & DEST_CELL) != DEST_CELL)))
                    {
                        chosen_dir = temp_dir;
                        stop = 0;
                        break;
                    }
                    else
                    {
                        poss_dirs[i] = SOUTH;                                            // possible route
                        if (p_mouse->prev_cells[p_mouse->pos_y - 1][p_mouse->pos_x] > 0) // checking if south cell is the previous cell
                            prev_cell_check = SOUTH;
                    }
                }

                // Checking if West is Out of Bound
                else if ((temp_dir == WEST) && ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x - 1] & CELL_OUT) != CELL_OUT))
                {
                    if (
                        ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x - 1] & CELL_VISITED) != CELL_VISITED) ||
                        // Get out of the Destination Loop
                        ((p_mouse->dest_cells_found == TOT_DEST_CELLS) && ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & DEST_CELL) == DEST_CELL) && ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x - 1] & DEST_CELL) != DEST_CELL)))
                    {
                        chosen_dir = temp_dir;
                        stop = 0;
                        break;
                    }
                    else
                    {
                        poss_dirs[i] = WEST;                                             // possible route
                        if (p_mouse->prev_cells[p_mouse->pos_y][p_mouse->pos_x - 1] > 0) // checking if west cell is the previous cell
                            prev_cell_check = WEST;
                    }
                }
            }

            // Check next way to the right (clockwise)
            temp_dir += 1;
            if (temp_dir > WEST)
                temp_dir = NORTH;
        }

        // If all cells around have already been visited give a priority
        // Priorities - NOT previous cell, Out of the loop if all dest cells have been found, clockwise choice
        if (stop == 1)
            for (i = 0; i < 4; i++)
                if (poss_dirs[i] != 99)
                {
                    chosen_dir = poss_dirs[i];
                    stop = 0;
                    if (chosen_dir != prev_cell_check)
                    {
                        break; // prev cell secondary option
                    }
                }
    }

    // If all cells have been found, move back to first cell
    // Follow the lowest number of the discovered cells order
    else if (discovered_cells[p_mouse->pos_y][p_mouse->pos_x] != 1) // Don't move if inside first cell
    {
        temp_dir = discovered_cells[p_mouse->pos_y][p_mouse->pos_x];

        // North checking
        if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & NORTH_WALL) != NORTH_WALL) // Wall check
            if (discovered_cells[p_mouse->pos_y + 1][p_mouse->pos_x] < temp_dir)
            {
                temp_dir = discovered_cells[p_mouse->pos_y + 1][p_mouse->pos_x];
                chosen_dir = NORTH; // Lower number check
            }

        // East checking
        if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & EAST_WALL) != EAST_WALL) // Wall check
            if (discovered_cells[p_mouse->pos_y][p_mouse->pos_x + 1] < temp_dir)
            {
                temp_dir = discovered_cells[p_mouse->pos_y][p_mouse->pos_x + 1];
                chosen_dir = EAST; // Lower number check
            }

        // South checking
        if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & SOUTH_WALL) != SOUTH_WALL) // Wall check
            if (discovered_cells[p_mouse->pos_y - 1][p_mouse->pos_x] < temp_dir)
            {
                temp_dir = discovered_cells[p_mouse->pos_y - 1][p_mouse->pos_x];
                chosen_dir = SOUTH; // Lower number check
            }

        // West checking
        if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & WEST_WALL) != WEST_WALL) // Wall check
            if (discovered_cells[p_mouse->pos_y][p_mouse->pos_x - 1] < temp_dir)
            {
                temp_dir = discovered_cells[p_mouse->pos_y][p_mouse->pos_x - 1];
                chosen_dir = WEST; // Lower number check
            }
        // printf("\n\ntemp_dir = %d\n\n", temp_dir);
        stop = 0;
    }

    if (stop == 0) // move if allowed
    {
        //---Turn Right/Left---
        // Negative = left; Positive = Right; Zero = No turning
        turn_check = chosen_dir - p_mouse->m_dir;
        if ((turn_check == 3) || (turn_check == -3))
            turn_check /= -3;

        // Simulation purposes - Directly set Mouse Direction
        p_mouse->m_dir = chosen_dir;

        // Storing current cell as previous cell before movement
        p_mouse->prev_cell_x = p_mouse->pos_x;
        p_mouse->prev_cell_y = p_mouse->pos_y;

        // Forward Movement
        if (p_mouse->m_dir == NORTH)
            p_mouse->pos_y += 1;
        else if (p_mouse->m_dir == EAST)
            p_mouse->pos_x += 1;
        else if (p_mouse->m_dir == SOUTH)
            p_mouse->pos_y -= 1;
        else if (p_mouse->m_dir == WEST)
            p_mouse->pos_x -= 1;

        //--- Checking if the mouse went back to previously visited cells ---
        // Current cell is not part of the last # of visited cells
        if (p_mouse->prev_cells[p_mouse->pos_y][p_mouse->pos_x] == 0)
        {
            if (p_mouse->prev_cell != LAST_VIS_CELLS)
                p_mouse->prev_cell += 1;
            else
                for (r = 0; r < SIZE; r++)
                    for (c = 0; c < SIZE; c++)
                        if (p_mouse->prev_cells[r][c] > 0)
                            p_mouse->prev_cells[r][c] -= 1;

            p_mouse->prev_cells[p_mouse->pos_y][p_mouse->pos_x] = p_mouse->prev_cell;
        }

        // Mouse came bake to one of the # previously visited cells
        else if ((mouse_maze[p_mouse->prev_cell_y][p_mouse->prev_cell_x] & CELL_OUT) != CELL_OUT) // Checking if the mouse's last cell was an a CELL OUT
        {
            temp_prev_cell = p_mouse->prev_cell;
            while (temp_prev_cell > p_mouse->prev_cells[p_mouse->pos_y][p_mouse->pos_x])
            {
                for (r = 0; r < SIZE; r++)
                    for (c = 0; c < SIZE; c++)
                    {
                        if ((r == 0) && (c == 0))
                            continue;
                        else if (p_mouse->prev_cells[r][c] == temp_prev_cell)
                        {
                            if (
                                // North
                                (
                                    ((mouse_maze[r][c] & NORTH_WALL) != NORTH_WALL) &&
                                    ((mouse_maze[r + 1][c] & CELL_OUT) != CELL_OUT) &&
                                    ((mouse_maze[r + 1][c] & CELL_VISITED) != CELL_VISITED))

                                ||

                                // East
                                (
                                    ((mouse_maze[r][c] & EAST_WALL) != EAST_WALL) &&
                                    ((mouse_maze[r][c + 1] & CELL_OUT) != CELL_OUT) &&
                                    ((mouse_maze[r][c + 1] & CELL_VISITED) != CELL_VISITED))

                                ||

                                // South
                                (
                                    ((mouse_maze[r][c] & SOUTH_WALL) != SOUTH_WALL) &&
                                    ((mouse_maze[r - 1][c] & CELL_OUT) != CELL_OUT) &&
                                    ((mouse_maze[r - 1][c] & CELL_VISITED) != CELL_VISITED))

                                ||

                                // West
                                (
                                    ((mouse_maze[r][c] & WEST_WALL) != WEST_WALL) &&
                                    ((mouse_maze[r][c - 1] & CELL_OUT) != CELL_OUT) &&
                                    ((mouse_maze[r][c - 1] & CELL_VISITED) != CELL_VISITED)))
                                break;
                            else if ((mouse_maze[r][c] & DEST_CELL) != DEST_CELL)
                            {
                                mouse_maze[r][c] |= CELL_OUT;
                            }
                        }
                    }
                temp_prev_cell -= 1;
            }

            // Resetting the last # visited cells memory
            for (r = 0; r < SIZE; r++)
                for (c = 0; c < SIZE; c++)
                    p_mouse->prev_cells[r][c] = 0;
            p_mouse->prev_cell = 1;
            p_mouse->prev_cells[p_mouse->pos_y][p_mouse->pos_x] = p_mouse->prev_cell;
        }
    }
}

void Walls_Check(unsigned int maze[HEIGHT][LENGTH], struct Mouse_Settings *p_mouse, unsigned int mouse_maze[SIZE][SIZE])
{
    unsigned int i, walls_no = 0;

    // Adding walls in the unvisited cells
    if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & CELL_VISITED) != CELL_VISITED) // Check if mouse has already been in current cell
    {
        //--- ADDING WALLS ---
        // North
        if ((maze[p_mouse->pos_y][p_mouse->pos_x] & NORTH_WALL) == NORTH_WALL) // Checking if there is a wall in front
        {
            mouse_maze[p_mouse->pos_y][p_mouse->pos_x] |= NORTH_WALL; // Add top wall
            if (p_mouse->pos_y != (SIZE - 1))
                mouse_maze[p_mouse->pos_y + 1][p_mouse->pos_x] |= SOUTH_WALL; // Add back wall to the following row
        }
        // East
        if ((maze[p_mouse->pos_y][p_mouse->pos_x] & EAST_WALL) == EAST_WALL) // Checking if there is a wall on the right
        {
            mouse_maze[p_mouse->pos_y][p_mouse->pos_x] |= EAST_WALL; // Add right wall
            if (p_mouse->pos_x != (SIZE - 1))
                mouse_maze[p_mouse->pos_y][p_mouse->pos_x + 1] |= WEST_WALL; // Add left wall to the following column
        }
        // South
        if ((maze[p_mouse->pos_y][p_mouse->pos_x] & SOUTH_WALL) == SOUTH_WALL) // Checking if there is a wall below
        {
            mouse_maze[p_mouse->pos_y][p_mouse->pos_x] |= SOUTH_WALL; // Add bottom wall
            if (p_mouse->pos_y != 0)
                mouse_maze[p_mouse->pos_y - 1][p_mouse->pos_x] |= NORTH_WALL; // Add top wall to the previous row
        }
        // West
        if ((maze[p_mouse->pos_y][p_mouse->pos_x] & WEST_WALL) == WEST_WALL) // Checking if there is a wall on the left
        {
            mouse_maze[p_mouse->pos_y][p_mouse->pos_x] |= WEST_WALL; // Add left wall
            if (p_mouse->pos_x != 0)
                mouse_maze[p_mouse->pos_y][p_mouse->pos_x - 1] |= EAST_WALL; // Add right wall to the previous column
        }

        //--- Destination Cells --- (setting cells beside the registered dest cell)
        if (p_mouse->dest_cells_found != TOT_DEST_CELLS)
            if (
                // Checking if a destination cell is on North
                ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & NORTH_WALL) != NORTH_WALL) && ((mouse_maze[p_mouse->pos_y + 1][p_mouse->pos_x] & DEST_CELL) == DEST_CELL) ||
                // Checking if a destination cell is on East
                ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & EAST_WALL) != EAST_WALL) && ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x + 1] & DEST_CELL) == DEST_CELL) ||
                // Checking if a destination cell is on South
                ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & SOUTH_WALL) != SOUTH_WALL) && ((mouse_maze[p_mouse->pos_y - 1][p_mouse->pos_x] & DEST_CELL) == DEST_CELL) ||
                // Checking if a destination cell is on West
                ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & WEST_WALL) != WEST_WALL) && ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x - 1] & DEST_CELL) == DEST_CELL))
                // If a destination cell is found on either side of the current cell - check the walls arrangement
                if (
                    ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & VERT_WALLS) != VERT_WALLS) &&
                    ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & HORIZ_WALLS) != HORIZ_WALLS))
                {
                    // If the walls arrangement does not match the destination entrance one then set the current cell as dest cell
                    mouse_maze[p_mouse->pos_y][p_mouse->pos_x] |= DEST_CELL;
                    p_mouse->dest_cells_found += 1;
                }

        mouse_maze[p_mouse->pos_y][p_mouse->pos_x] |= CELL_VISITED;
        p_mouse->cells_found += 1;
    }

    // Checking which one is the 6/8 cells side
    if (p_mouse->sides_found == 0)
    {
        unsigned int r, c;

        // The mouse knows when it ARRIVES in the cell
        if (p_mouse->pos_y >= (SIZE - 2))
        {
            // Shifting maze to the left (adding RIGHT walls)
            for (r = 0; r < SIZE; r++)
            {
                mouse_maze[r][SIZE - 3] |= EAST_WALL;
                mouse_maze[r][SIZE - 2] = mouse_maze[r][SIZE - 1] = 99;
            }

            p_mouse->sides_found = 1;
        }

        else if (p_mouse->pos_x >= (SIZE - 2))
        {
            // Shifting top rows (adding TOP walls)
            for (c = 0; c < SIZE; c++)
            {
                mouse_maze[SIZE - 3][c] |= NORTH_WALL;
                mouse_maze[SIZE - 2][c] = mouse_maze[SIZE - 1][c] = 99;
            }

            p_mouse->sides_found = 1;
        }
    }

    // Checking if Destination has been found
    if (p_mouse->destination_found == 0)
        if ((p_mouse->pos_y == 3) && (p_mouse->pos_x == 3))
            p_mouse->destination_found = 1;

    // Setting Out of Bound Cell
    if (((p_mouse->pos_y == 0) & (p_mouse->pos_x != 0)) || ((p_mouse->pos_y != 0) & (p_mouse->pos_x == 0)) || ((p_mouse->pos_y != 0) & (p_mouse->pos_x != 0))) // Excluding the first cell
    {
        for (i = 0; i < 4; i++)
            if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & p_mouse->dirs[i]) == p_mouse->dirs[i]) // Checking how many walls are in the current cell
                walls_no += 1;

        //---Out cells count as walls---

        // North cell
        if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & NORTH_WALL) != NORTH_WALL)
            if ((mouse_maze[p_mouse->pos_y + 1][p_mouse->pos_x] & CELL_OUT) == CELL_OUT)
                walls_no += 1;

        // East cell
        if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & EAST_WALL) != EAST_WALL)
            if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x + 1] & CELL_OUT) == CELL_OUT)
                walls_no += 1;

        // South cell
        if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & SOUTH_WALL) != SOUTH_WALL)
            if ((mouse_maze[p_mouse->pos_y - 1][p_mouse->pos_x] & CELL_OUT) == CELL_OUT)
                walls_no += 1;

        // West cell
        if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & WEST_WALL) != WEST_WALL)
            if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x - 1] & CELL_OUT) == CELL_OUT)
                walls_no += 1;

        if (walls_no == 3)
            mouse_maze[p_mouse->pos_y][p_mouse->pos_x] |= CELL_OUT; // This cell brings nowhere
    }
}

void Solving(unsigned int mouse_maze[SIZE][SIZE], unsigned int discovered_cells[SIZE][SIZE])
{
    unsigned int r, c, r_check, c_check, cell_check;

    mouse_maze[0][0] |= CELL_USED; // First cell will always be used

    // Store order number of the cell destination
    cell_check = discovered_cells[Y_DEST][X_DEST];

    // Following discovered cells order
    while (cell_check != 1)
    {
        for (r = 0; r < SIZE; r++)
            for (c = 0; c < SIZE; c++)
                if (discovered_cells[r][c] == cell_check)
                {
                    mouse_maze[r][c] |= CELL_USED;
                    r_check = r;
                    c_check = c; // storing found position
                }

        // Checking around current "cell_check" which cell has the next LOWEST order value

        // NORTH value check
        if ((mouse_maze[r_check][c_check] & NORTH_WALL) != NORTH_WALL)
            if ((discovered_cells[r_check + 1][c_check] < cell_check) && (discovered_cells[r_check + 1][c_check] > 0))
                cell_check = discovered_cells[r_check + 1][c_check];

        // EAST value check
        if ((mouse_maze[r_check][c_check] & EAST_WALL) != EAST_WALL)
            if ((discovered_cells[r_check][c_check + 1] < cell_check) && (discovered_cells[r_check][c_check + 1] > 0))
                cell_check = discovered_cells[r_check][c_check + 1];

        // SOUTH value check
        if ((mouse_maze[r_check][c_check] & SOUTH_WALL) != SOUTH_WALL)
            if ((discovered_cells[r_check - 1][c_check] < cell_check) && (discovered_cells[r_check - 1][c_check] > 0))
                cell_check = discovered_cells[r_check - 1][c_check];

        // WEST value check
        if ((mouse_maze[r_check][c_check] & WEST_WALL) != WEST_WALL)
            if ((discovered_cells[r_check][c_check - 1] < cell_check) && (discovered_cells[r_check][c_check - 1] > 0))
                cell_check = discovered_cells[r_check][c_check - 1];
    }
}

void ToDest(unsigned int mouse_maze[SIZE][SIZE], struct Mouse_Settings *p_mouse)
{
    // Storing current cell mapping
    p_mouse->prev_cell_x = p_mouse->pos_x;
    p_mouse->prev_cell_y = p_mouse->pos_y;

    while ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & DEST_CELL) != DEST_CELL)
    {
        // Moving towards the direction of the next cell SET AS USED
        // Also, making sure it does not go to previous cell

        // NORTH
        if (((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & NORTH_WALL) != NORTH_WALL) && ((p_mouse->pos_y + 1) != p_mouse->prev_cell_y))
            if ((mouse_maze[p_mouse->pos_y + 1][p_mouse->pos_x] & CELL_USED) == CELL_USED)
            {
                p_mouse->prev_cell_y = p_mouse->pos_y;
                p_mouse->pos_y += 1;
            }

            // EAST
            else if (((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & EAST_WALL) != EAST_WALL) && ((p_mouse->pos_x + 1) != p_mouse->prev_cell_x))
                if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x + 1] & CELL_USED) == CELL_USED)
                {
                    p_mouse->prev_cell_x = p_mouse->pos_x;
                    p_mouse->pos_x += 1;
                }

                // SOUTH
                else if (((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & SOUTH_WALL) != SOUTH_WALL) && ((p_mouse->pos_y - 1) != p_mouse->prev_cell_y))
                    if ((mouse_maze[p_mouse->pos_y - 1][p_mouse->pos_x] & CELL_USED) == CELL_USED)
                    {
                        p_mouse->prev_cell_y = p_mouse->pos_y;
                        p_mouse->pos_y -= 1;
                    }

                    // WEST
                    else if (((mouse_maze[p_mouse->pos_y][p_mouse->pos_x] & WEST_WALL) != WEST_WALL) && ((p_mouse->pos_x - 1) != p_mouse->prev_cell_x))
                        if ((mouse_maze[p_mouse->pos_y][p_mouse->pos_x - 1] & CELL_USED) == CELL_USED)
                        {
                            p_mouse->prev_cell_x = p_mouse->pos_x;
                            p_mouse->pos_x -= 1;
                        }
    }
}

int main()
{
    unsigned int maze[HEIGHT][LENGTH];
    unsigned int mouse_maze[SIZE][SIZE];

    // Maze Generation and Simulation
    Generate_Maze(maze);
    Generate_Route(maze);
    Print_Maze(maze);

    // Preparing Initial Mouse Setup
    struct Mouse_Settings mouse;
    struct Mouse_Settings *p_mouse = &mouse;
    Mouse_Setup(p_mouse);
    Mouse_Maze_Setup(mouse_maze);

    unsigned int i, count = 2, discovered_cells[SIZE][SIZE];
    int r, c;
    // Setting the Temporary Maze
    for (r = 0; r < SIZE; r++)
        for (c = 0; c < SIZE; c++)
            discovered_cells[r][c] = 0;
    discovered_cells[0][0] = 1;

    while ((p_mouse->cells_found != TOT_CELLS) || (p_mouse->pos_y != 0) || (p_mouse->pos_x != 0))
    {
        Mouse_Exploring(maze, mouse_maze, p_mouse, discovered_cells);
        Walls_Check(maze, p_mouse, mouse_maze);

        // Editing the Maze table showing the movement of the mouse
        if (discovered_cells[p_mouse->pos_y][p_mouse->pos_x] == 0)
        {
            discovered_cells[p_mouse->pos_y][p_mouse->pos_x] = count;
            count++;
        }
    }

    Solving(mouse_maze, discovered_cells);
    Print_Mouse_Maze(mouse_maze);
    // ToDest(mouse_maze, p_mouse);

    // Visualizing Movement
    printf("--- Mouse Pattern ---\n");
    for (r = (SIZE - 1); r > -1; r--)
    {
        for (c = 0; c < SIZE; c++)
            printf("%d\t", discovered_cells[r][c]);
        printf("\n");
    }
    printf("\n\n");

    // Visualizing last 8 cells visited
    printf("--- Last Visited Cells ---\n");
    for (r = (SIZE - 1); r > -1; r--)
    {
        for (c = 0; c < SIZE; c++)
            printf("%d\t", p_mouse->prev_cells[r][c]);
        printf("\n");
    }
    printf("\n\n");

    printf("pos_y = %d\tpos_x = %d\tcells_found = %d\tm-dir = %d", p_mouse->pos_y, p_mouse->pos_x, p_mouse->cells_found, p_mouse->m_dir);

    return 0;
}