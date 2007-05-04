
/*****************************************************************************\
 *                                                                           *
 * Soothsayer, an extensible predictive text entry system                    *
 * ------------------------------------------------------                    *
 *                                                                           *
 * Copyright (C) 2004  Matteo Vescovi <matteo.vescovi@yahoo.co.uk>            *
 *                                                                           *
 * This program is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU General Public License               *
 * as published by the Free Software Foundation; either version 2            *
 * of the License, or (at your option) any later version.                    *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.*
 *                                                                           *
\*****************************************************************************/


#include "soothsayer.h"

#include <string>
#include <vector>
#include <list>
#include <ncurses.h>
#include <sstream>


void disclaimer();

void draw_title_win(WINDOW*);
void draw_history_win(WINDOW*, std::string);
void draw_previous_suggestions(std::vector<std::string>, const int);

int main()
{
    Soothsayer soothsayer;

    // init ncurses
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    clear();
    refresh();

    disclaimer();

    const int TITLE_WIN_HEIGHT  = 6;
    const int TITLE_WIN_WIDTH   = COLS;
    const int TITLE_WIN_BEGIN_Y = 0;
    const int TITLE_WIN_BEGIN_X = 0;
    WINDOW* title_win = newwin(TITLE_WIN_HEIGHT, TITLE_WIN_WIDTH, TITLE_WIN_BEGIN_Y, TITLE_WIN_BEGIN_X);
    draw_title_win(title_win);

    const int HISTORY_WIN_HEIGHT  = 5;
    const int HISTORY_WIN_WIDTH   = COLS;
    const int HISTORY_WIN_BEGIN_Y = TITLE_WIN_BEGIN_Y + TITLE_WIN_HEIGHT + 1;
    const int HISTORY_WIN_BEGIN_X = 0;
    WINDOW* history_win = newwin(HISTORY_WIN_HEIGHT, HISTORY_WIN_WIDTH, HISTORY_WIN_BEGIN_Y, HISTORY_WIN_BEGIN_X);
    draw_history_win(history_win, std::string(""));

    mvprintw(LINES - 1, 0, "Press F12 to quit.");
    refresh();

    std::vector<std::string> words;
    int c;
    char str[2];
    str[1] = '\0';
    do {
	c = getch();
	if ((KEY_F0 < c) && (c <= KEY_F(words.size())) && (c - KEY_F0 <= words.size())) {
	    std::string message = "Selecting word " + words[c - KEY_F0 - 1];
	    mvprintw(LINES - 3, 0, message.c_str());
	    move(LINES, COLS);
	    refresh();
	    soothsayer.complete(words[c - KEY_F0 - 1]);
	    words = soothsayer.predict(' ');
	} else {
	    str[0] = static_cast<char>( c );
	    words = soothsayer.predict(std::string(str));
	}
	draw_history_win(history_win, soothsayer.history());
	draw_previous_suggestions(words, HISTORY_WIN_BEGIN_Y + HISTORY_WIN_HEIGHT + 1);
    } while( c != KEY_F(12) );


    delwin(title_win);
    delwin(history_win);
    endwin();

    return 0;
}


void draw_history_win(WINDOW* win, std::string str)
{
    wclear( win );
    box( win, 0, 0 );
    mvwprintw( win, 1, 1, str.c_str() );
    wrefresh( win );
}


void drawMsgWin( WINDOW* win, std::vector<std::string> words )
{
    wclear( win );
    box( win, 0, 0 );
	
    int i = 1;
    std::vector<std::string>::const_iterator j = words.begin();
    while( j != words.end() ) {
	mvwprintw( win, i, 1, j->c_str() );
	i++;
	j++;
    }

    wrefresh( win );
}

void draw_previous_suggestions(std::vector<std::string> words, const int starty)
{
    static std::list< std::vector<std::string> > previousSuggestions;
    static std::vector< WINDOW* > windows;

    // clear out existing windows
    for (std::vector< WINDOW* >::iterator winit = windows.begin();
         winit != windows.end();
         winit++) {
        wclear(*winit);
        wrefresh(*winit);
        delwin(*winit);
    }
    windows.clear();

    previousSuggestions.insert(previousSuggestions.begin(), words);

    int startx = 0;

    for (std::list< std::vector<std::string> >::const_iterator listit = previousSuggestions.begin();
	 listit != previousSuggestions.end();
	 listit++) {

	int height = listit->size() + 2;
	int width = 14;

	WINDOW* win = newwin(height, width, starty, startx);
	wclear(win);
	box(win, 0, 0);

	int line = 1;
	for (std::vector<std::string>::const_iterator strit = listit->begin();
	     strit != listit->end();
	     strit++) {
	    
	    mvwprintw(win, line, 1, strit->c_str());
	    line++;
	}

	wrefresh(win);
        windows.push_back(win);
	startx += width + 2;
    }
}

void disclaimer()
{
    int topBottomBorder = (LINES/8);
    int borderWinHeight = 15;
    int borderWinWidth  = 70;
    int sideBorder      = (COLS - borderWinWidth) / 2;
    WINDOW* borderWin     = newwin(borderWinHeight, borderWinWidth, topBottomBorder, sideBorder);
    WINDOW* disclaimerWin = newwin(borderWinHeight-2, borderWinWidth-2, topBottomBorder+1, sideBorder+1);
    box(borderWin, 0, 0);
    wrefresh(borderWin);
    wprintw(disclaimerWin,
	    "Soothsayer demo\n"
	    "---------------\n"
	    "This program is intended as a demonstration of Soothsayer ONLY.\n"
	    "\n"
	    "The Soothsayer project aims to provide an intelligent predictive\n"
	    "text entry platform.\n"
	    "\n"
	    "Its intent is NOT to provide a predictive text entry user interface.\n"
	    "Think of Soothsayer as the predictive backend that sits behind a\n"
	    "shiny user interface and does all the predictive heavy lifting.\n"
	);
    mvwprintw(disclaimerWin, (borderWinHeight-2)-1, 1, "Press any key to continue...");
    wrefresh(disclaimerWin);

    getch();

    delwin(disclaimerWin);
    delwin(borderWin);

    clear();
    refresh();
}

void draw_title_win(WINDOW* title_win)
{
    wclear(title_win);
    box(title_win, 0, 0);
    mvwprintw(title_win, 1, 1, "Soothsayer demo ", PACKAGE_VERSION);
    mvwprintw(title_win, 2, 1, "Copyright (C) Matteo Vescovi");
    mvwprintw(title_win, 3, 1, "This is free software; see the source for copying conditions.  There is NO");
    mvwprintw(title_win, 4, 1, "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
    wrefresh(title_win);
}
