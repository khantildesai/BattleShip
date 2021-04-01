/* This files provides address values that exist in the system */

#define SDRAM_BASE 0xC0000000
#define FPGA_ONCHIP_BASE 0xC8000000
#define FPGA_CHAR_BASE 0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define TIMER_BASE 0xFF202000
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/*Game constants*/
#define GRID_WIDTH 23
#define DIST_NEXT 24 //Add to postion to get same spot in next square
#define GRID_BASE_X 41
#define GRIDE_BASE_Y 0

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void Setup();
void swap(int *, int *);
void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void DrawGrid();
void DrawCursor(int gridx, int gridy);


volatile int pixel_buffer_start; // global variable

int main(void)
{
    volatile int *pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;

    clear_screen();
    DrawGrid();
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

//Clears the entire screen by plotting black pixels
void clear_screen()
{
    for (int x = 0; x < 320; x++)
    {
        for (int y = 0; y < 240; y++)
        {
            plot_pixel(x, y, 0x0000);
        }
    }
    return;
}

//Draws a line segment
void draw_line(int x0, int y0, int x1, int y1, short int line_color)
{

    //checks if y increases faster than x
    bool is_steep = abs(y1 - y0) > abs(x1 - x0);

    //If y increases too fast swap x and y, so x will increase faster
    if (is_steep)
    {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }

    //Checks if X0 is greater than x1
    if (x0 > x1)
    {
        //Swaps x0 and x1
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -1 * (deltax / 2);
    int y = y0;

    int y_step;

    //Calculates if y should be increasing or decreasing
    if (y0 < y1)
    {
        y_step = 1;
    }
    else
    {
        y_step = -1;
    }

    //Iterates through number of xs
    for (int x = x0; x <= x1; x++)
    {
        //if steep y and x are reversed (Guards for vertical lines)
        if (is_steep)
        {
            plot_pixel(y, x, line_color);
        }
        else // Not steep normal x and y
        {
            plot_pixel(x, y, line_color);
        }

        //Recalculate Error by adding delta y
        error = error + deltay;

        //Check if error is positive
        if (error >= 0)
        {
            //Pos Error => add y_step to y and subtract dela x from error
            y += y_step;
            error -= deltax;
        }
    }
}

//Swaps two integers by pass by reference
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
    return;
}

void Setup()
{
}

//Draws game grid
void DrawGrid()
{
    //Draws Rows
    for (int row = 0; row < 10; row++)
    {
        draw_line(40, 23 + 24 * row, 280, 23 + 24 * row, GREY);
    }
    //Draws columns
    for (int col = 0; col < 11; col++)
    {
        draw_line(40 + 24 * col, 0, 40 + 24 * col, RESOLUTION_Y - 1, GREY);
    }
}

//Draws Cursor on specified square
void DrawCursor(int gridx, int gridy)
{
    //gets top left pixel position
    int x0 = GRID_BASE_X + gridx * DIST_NEXT;
    int y0 = gridy * DIST_NEXT;

    //Draws the 4 line segments
    draw_line(x0, y0, 63 + gridx * 24, y0, RED);   // top
    draw_line(x0, 22 + y0, 22 + x0, 22 + y0, RED); // bot
    draw_line(x0, gridy * 24, x0, 22 + y0, RED);   // left
    draw_line(22 + x0, y0, 22 + x0, 22 + y0, RED); // right
}