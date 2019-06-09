#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// src:   http://spinout182.com/src/plane.c
// -lncurses

void init_color_pairs() {
    short f, b;
    for( f = 0; f < COLORS; ++f )
        for( b = 0; b < COLORS; ++b )
            init_pair( f * COLORS + b, f, b );
}

#define SET_COLOR(fore, bg)                wattrset(stdscr, COLOR_PAIR(COLOR_##fore * COLORS + COLOR_##bg))

#define PLANE1  "|\\"
#define PLANE2 "}=-O>"
#define PLANE3  "|/"
#define PLANE	\
{\
	move(h-1, 6);\
	printw(PLANE1);\
	move(h, 5);\
	printw(PLANE2);\
	move(h+1,6);\
	printw(PLANE3);\
	move(0,0);\
	refresh();\
}

#define ENEMY1   "/|"
#define ENEMY2 "<O-={"
#define ENEMY3   "\\|"
#define ENEMY(y,x)	\
{\
	move(y-1, x+2);\
	printw(ENEMY1);\
	move(y, x);\
	printw(ENEMY2);\
	move(y+1,x+2);\
	printw(ENEMY3);\
}

#define bullet(y,x)	\
{\
	SET_COLOR(BLACK, CYAN);\
	mvaddch(y,x,'*');\
}

#define RF	(rand() / ((RAND_MAX)+1.))

#define count(x)	sizeof(x)/sizeof(x[0])
#define diff(x,y)	abs(x-y)

int
main()
{
    initscr();
    start_color();
    noecho();
    raw();
    cbreak();
    timeout(0);
    keypad(stdscr, 1);
    
    init_color_pairs();
    
    int x,y,p,h, key, c, die, score, bullets[50][2], enemies[50][3];
    
    start:
    
    p=0;
    h=13;
    die = 0;
    score = 0;
    memset(bullets, 0, sizeof(bullets));
    memset(enemies, 0, sizeof(enemies));
    
    loop:
    
    //usleep(15000);

    usleep(15000 * 25);

    p++;
    key = getch();
    if(key>0)
    {
        switch(key)
        {
        case KEY_UP:
        case 'k':
            (h > 1) ? h -- : 0;
            break;
        case KEY_DOWN:
        case 'j':
            (h < 25) ? h ++ : 0;
            break;
        case ' ':
            for(x=0;x<count(bullets);x++){
                if(!bullets[x][0]){
                    bullets[x][0] = 5;
                    bullets[x][1] = h;
                    break;
                }
            }
            break;
        case  27:
        case 'q':
        case 'Q':
            goto end;
        }
    }
    
    if((rand() & 0x3F) == 4){
        for(x=0;x<count(enemies);x++){
            if(!enemies[x][0]){
                enemies[x][0] = 95;
                enemies[x][1] = 8;
                enemies[x][1] += (rand() & 1) ? -(rand() & 7)-1 : (rand() & 7)-1;
                enemies[x][2] = 0;
                break;
            }
        }
    }
    
    for(x=0;x<100;x++)
    {
        for(y=0;y<26;y++)
        {
            y-=20;
            if(sin((float)(x+p)/23) > ((float)y / 5.0f))
                SET_COLOR(WHITE, CYAN);
            else
            {
                SET_COLOR(GREEN, GREEN);
                if(x == 6&& (h-18 > y))
                    die = 1;
            }
            y+=20;
            mvaddch(y,x,'.');
        }
    }
    
    for(x=0;x<count(bullets);x++){
        
        bullets[x][0] += (bullets[x][0]) ? 1 : 0;
        if(bullets[x][0] > 99)
            bullets[x][0] = 0;
        else if(bullets[x][0])
            bullet(bullets[x][1],bullets[x][0]);
    }
    
    for(x=0;x<count(enemies);x++){
        enemies[x][0] -= (enemies[x][0] && !enemies[x][2]) ? 1 : 0;
        if(enemies[x][0] == 1){
            enemies[x][0] = 0;
            score --;
        }
        if(enemies[x][2]){
            enemies[x][2]--;
            SET_COLOR(RED,YELLOW);
            if(!enemies[x][2]){
                enemies[x][0] = 0;
            }
        }
        else{
            SET_COLOR(BLACK, CYAN);
            if(enemies[x][0] && enemies[x][0] < 10 && diff(h, enemies[x][1]) < 3){
                die= 1;
                goto planedie;
            }
            for(y=0;y<count(bullets);y++){
                if(bullets[y][0] && diff(enemies[x][0],bullets[y][0]) < 2 && diff(enemies[x][1],bullets[y][1]) < 2){
                    score++;
                  planedie:
                    SET_COLOR(RED, YELLOW);
                    enemies[x][2] = 4;
                    bullets[y][0] = 0;
                    break;
                }
            }
        }
        if(enemies[x][0])
            ENEMY(enemies[x][1],enemies[x][0]);
    }
    SET_COLOR(GREEN, BLACK);
    move(26,0);
    //printw("Score: %i                   ", score);
    if(!die)
    {
        SET_COLOR(BLUE, WHITE);
        PLANE;
    }
    else
    {
        SET_COLOR(RED, YELLOW);
        PLANE;
        sleep(2);
        goto start;
    }
    
    goto loop;
    
    end:
    refresh();
    endwin();
    return 0;
}
