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
#define GRID_BASE_X 81
#define GRID_BASE_Y 0

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

volatile int pixel_buffer_start; // global variable
volatile char *char_buffer_start = (char *)0xC9000000;

//global variables
//Player GameBoards 0 = not attacked, 1 = attacked and missed, 2 = attacked and hit
int Player1GameBoard[10][10];
int Player2GameBoard[10][10];

//ship segment class w/h default values
//EX:
//ShipSegment seg1 = ShipSegmentDefault;
struct ShipSegmentS
{
    bool hit; //default not hit
    int type; //0 = hull, 1 = stern, 2 = bow
    int X;    //X coord (on gameboard grid (0-9))
    int Y;    //Y coord (on gameboard grid (0-9))
} ShipSegmentDefault = {false};
typedef struct ShipSegmentS ShipSegment;

//Ship class with default Ship types
//EX:
//Ship playerCarrier = Carrier;
struct Ship
{
    bool sunk;        //default not sunk
    short int colour; //color number (using macros defined above)
    int type;         // 5 = Aircraft Carrier, 4 = Battleship, 3 = Destroyer/Submarine, 2 = Patrol Boat
    ShipSegment *Segments;
};
typedef struct Ship Ship;
Ship Carrier = {false, BLUE, 5};
Ship Battleship = {false, GREEN, 4};
Ship Destroyer = {false, CYAN, 3};
Ship Submarine = {false, MAGENTA, 3};
Ship PatrolBoat = {false, GREY, 2};

//Player struct
struct Player
{
    bool won;
    Ship Ships[];
};

void Setup();
void swap(int *, int *);
void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void DrawGrid();
void DrawCursor(int gridx, int gridy);
void drawShipSegment(ShipSegment seg, short int colour);
void drawShip(Ship ship);
void drawHit(int X, int Y);
void drawMiss(int X, int Y);
void ClearGridSeg(int gridx, int gridy);
void ChooseHitPlacement();
char WaitForButtonPress();
void ClearBoard();
bool inBounds(int x, int y);
void DrawWordLine(char *cs, int lineY, int x);
void clearText();
Ship rotateShip(Ship myShip);
Ship rotateDown(Ship myShip);
Ship rotateUp(Ship myShip);

void drawShipTest();

int main(void)
{
    volatile int *pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;

    Setup();

    clear_screen();
    clearText();
    DrawWordLine("P1 Turn", 0, 0);
    DrawGrid();

    drawShipTest();
	//ChooseHitPlacement();
	drawHit(1,7);
	drawMiss(0,7);
}

Ship rotateShip(Ship myShip){
	bool rotateToDown = true;
	if (myShip.Segments[0].X == myShip.Segments[1].X){
		// if two X coords are same (ship is vertical) and need to rotate up
		rotateToDown = false;
	}
	if (rotateToDown){//rotate Down
		myShip = rotateDown(myShip);
	}
	else { //rotate Up
		myShip = rotateUp(myShip);
	}
	return myShip;
}

Ship rotateUp(Ship myShip){
	if ((myShip.Segments[0].X + myShip.type - 1) > 9) return myShip;
	int len = myShip.type;
	int allY = myShip.Segments[0].Y;
	int startX = myShip.Segments[0].X;
	for (int iter = 1; iter < len; iter++){ //for all points after the topRight Point
		myShip.Segments[iter].X = startX + iter; //move down X as needed
		myShip.Segments[iter].Y = allY; //all have same Y
	}
	return myShip;
}

Ship rotateDown(Ship myShip){
	if ((myShip.Segments[0].Y + myShip.type - 1) > 9) return myShip;
	int len = myShip.type;
	int allX = myShip.Segments[0].X;
	int startY = myShip.Segments[0].Y;
	for (int iter = 1; iter < len; iter++){//for all points after the topRight Point
		myShip.Segments[iter].X = allX;//all have same X
		myShip.Segments[iter].Y = startY + iter;//move down Y as needed
	}
	return myShip;
}

void drawShipTest()
{
    ShipSegment seg1 = ShipSegmentDefault;
    ShipSegment seg2 = ShipSegmentDefault;
    ShipSegment seg3 = ShipSegmentDefault;
    ShipSegment seg4 = ShipSegmentDefault;
    ShipSegment seg5 = ShipSegmentDefault;
    seg1.type = 0;
    seg1.X = 1;
    seg1.Y = 7;
    seg2.type = 0;
    seg2.X = 1;
    seg2.Y = 6;
    seg3.type = 0;
    seg3.X = 1;
    seg3.Y = 5;
    seg4.type = 0;
    seg4.X = 1;
    seg4.Y = 4;
    seg5.type = 0;
    seg5.X = 1;
    seg5.Y = 3;

    ShipSegment seg6 = ShipSegmentDefault;
    ShipSegment seg7 = ShipSegmentDefault;
    ShipSegment seg8 = ShipSegmentDefault;
    ShipSegment seg9 = ShipSegmentDefault;
    ShipSegment seg0 = ShipSegmentDefault;
    seg6.type = 0;
    seg6.X = 5;
    seg6.Y = 7;
    seg7.type = 0;
    seg7.X = 5;
    seg7.Y = 6;
    seg8.type = 0;
    seg8.X = 5;
    seg8.Y = 5;
    seg9.type = 0;
    seg9.X = 5;
    seg9.Y = 4;
    seg0.type = 0;
    seg0.X = 5;
    seg0.Y = 3;

    ShipSegment myArr[] = {seg1, seg2, seg3, seg4, seg5};
    ShipSegment tArr[] = {seg6, seg7, seg8, seg9, seg0};
    Ship myShip = Carrier;
    Ship tShip = Carrier;
    myShip.Segments = myArr;
    tShip.Segments = tArr;
    drawShip(myShip);
	tShip = rotateShip(tShip);
    drawShip(tShip);
	//Ship t2Ship = rotateShip(tShip);
    //drawShip(t2Ship);
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
    //setting  Player GameBoards to all 0 (empty)
    for (int P1GBSetter = 0; P1GBSetter < 10; P1GBSetter++)
    {
        for (int P1GBSetter2 = 0; P1GBSetter2 < 10; P1GBSetter2++)
        {
            Player1GameBoard[P1GBSetter][P1GBSetter2] = 0;
            Player2GameBoard[P1GBSetter][P1GBSetter2] = 0;
        }
    }
}

//Draws game grid
void DrawGrid()
{
    //Draws Rows
    for (int row = 0; row < 10; row++)
    {
        draw_line(GRID_BASE_X - 1, 23 + 24 * row, RESOLUTION_X - 1, 23 + 24 * row, GREY);
    }
    //Draws columns
    for (int col = 0; col < 12; col++)
    {
        draw_line(GRID_BASE_X - 2 + 24 * col, 0, GRID_BASE_X - 2 + 24 * col, RESOLUTION_Y - 1, GREY);
    }
}

//Draws Cursor on specified square
void DrawCursor(int gridx, int gridy)
{
    //gets top left pixel position
    int x0 = GRID_BASE_X + gridx * DIST_NEXT;
    int y0 = gridy * DIST_NEXT;

    //Draws the 4 line segments
    if (y0 > 0)
    {
        draw_line(x0 - 2, y0 - 1, 22 + x0, y0 - 1, RED); // top
    }
    draw_line(x0 - 2, 23 + y0, 22 + x0, 23 + y0, RED);   // bot
    draw_line(x0 - 2, y0, x0 - 2, GRID_WIDTH + y0, RED); // left
    draw_line(22 + x0, y0, 22 + x0, 22 + y0, RED);       // right
}
//NEED TO CHECK IF POSITION HAS ALREADY BEEN GUESSED!
void ChooseHitPlacement()
{
    int count = 0;
    int x_start, y_start;
    x_start = y_start = 5;
    DrawCursor(x_start, y_start);
    while (count < 5)
    {
        char key = WaitForButtonPress();
        if (key == 'X')
        {
            break;
        }
        if (key == '>')
        { //Right
            x_start++;
            if (!inBounds(x_start, y_start))
                x_start--;
        }
        else if (key == '<')
        { //LEFT
            x_start--;
            if (!inBounds(x_start, y_start))
                x_start++;
        }
        else if (key == '^')
        { //UP
            y_start--;
            if (!inBounds(x_start, y_start))
                y_start++;
        }
        else if (key == 'v')
        { //DOWN
            y_start++;
            if (!inBounds(x_start, y_start))
                y_start--;
        }
        DrawGrid();

        DrawCursor(x_start, y_start);
        //count++;
    }
    printf("SHOTS FIYAED AT (%d, %d)", x_start, y_start);
}

char WaitForButtonPress()
{
    volatile int *key_ptr = (int *)KEY_BASE;
    volatile int *sw_ptr = (int *)SW_BASE;

    *(key_ptr + 2) = 0xF;
    *(key_ptr + 3) = 0xF;

    int key_val = *(key_ptr + 3);
    int sw_val = 0;
    bool upDown = 0;
    while (key_val == 0)
    {
        key_val = *(key_ptr + 3);
        sw_val = *(sw_ptr);
        if ((sw_val & 0x1) == 1)
        {
            upDown = 1;
        }
        else if (upDown)
        {
            return 'X';
        }
    }

    if (key_val == 1) //KEY0
    {                 //DOWN
        return 'v';
    }
    else if (key_val == 2) //KEY1
    {                      //UP
        return '^';
    }
    else if (key_val == 4) //KEY2
    {                      //RIGHT
        return '>';
    }
    else if (key_val == 8) //KEY3
    {                      //LEFT
        return '<';
    }
    return 'L';
}
//void draw_line(int x0, int y0, int x1, int y1, short int line_color)
void drawShipSegment(ShipSegment seg, short int colour){
    if (!inBounds(seg.X, seg.Y))
        return;
    int x0 = GRID_BASE_X + seg.X * DIST_NEXT;
    int y0 = GRID_BASE_Y + seg.Y * DIST_NEXT;
    for (int iterX = 0; iterX < (GRID_WIDTH); iterX++)
    {
        draw_line(x0 + iterX - 1, y0, x0 + iterX - 1, y0 + GRID_WIDTH - 1, colour);
    }
}

void drawShip(Ship ship){
    for (int segIter = 0; segIter < ship.type; segIter++)
    {
        drawShipSegment(ship.Segments[segIter], ship.colour);
    }
}

void drawHit(int X, int Y){
	//draw first diagonal of cross
	if (!inBounds(X, Y))
        return;
    int x0 = GRID_BASE_X + X * DIST_NEXT;
    int y0 = GRID_BASE_Y + Y * DIST_NEXT;
	for (int iter = 0; iter < 5; iter++){
		draw_line(x0 + iter-1, y0, x0+GRID_WIDTH-5 + iter-1, y0+GRID_WIDTH-1, RED);
		draw_line(x0 + iter-1, y0+GRID_WIDTH-1, x0+GRID_WIDTH-5 + iter-1, y0, RED);
	}
}

void drawMiss(int X, int Y){
	if (!inBounds(X, Y))
        return;
	int x0 = GRID_BASE_X + X * DIST_NEXT;
    int y0 = GRID_BASE_Y + Y * DIST_NEXT;
	draw_line(x0+8,y0+9,x0+8,y0+11,GREEN);
	draw_line(x0+9,y0+8,x0+9,y0+12,GREEN);
	draw_line(x0+10,y0+7,x0+10,y0+13,GREEN);
	draw_line(x0+11,y0+7,x0+11,y0+13,GREEN);
	draw_line(x0+12,y0+8,x0+12,y0+12,GREEN);
	draw_line(x0+13,y0+9,x0+13,y0+11,GREEN);
}

void ClearGridSeg(int gridx, int gridy)
{
    //gets top left pixel position
    int x0 = GRID_BASE_X + gridx * DIST_NEXT;
    int y0 = gridy * DIST_NEXT;

    for (int dx = 0; dx < GRID_WIDTH; dx++)
    {
        for (int dy = 0; dy < GRID_WIDTH; dy++)
        {
            plot_pixel(x0 + dx, y0 + dy, 0x0);
        }
    }
}

void ClearBoard()
{
    for (int GridX = 0; GridX < 10; GridX++)
    {
        for (int GridY = 0; GridY < 10; GridY++)
        {
            ClearGridSeg(GridX, GridY);
        }
    }
}

bool inBounds(int x, int y)
{
    if ((x >= 0 && x < 10) && (y >= 0 && y < 10))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void DrawWordLine(char *cs, int lineY, int x)
{

    int offset = (lineY << 7) + x;
    while (*(cs))
    {
        *(char_buffer_start + offset) = *(cs); // write to the character buffer
        cs++;
        offset++;
    }
}

void clearText()
{
    int offset = 0;
    for (int pixelNum = 0; pixelNum < 4800; pixelNum++)
    {
        *(char_buffer_start + offset) = 0; // Clear spot
        offset++;
    }
}
