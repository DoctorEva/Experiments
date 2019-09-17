// Compilation Line: g++ -std=c++11 -o Tetris tetris.cpp -lncurses -lpthread

#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>

#include <string>
#include <cstring>
#include <fstream>

#include <math.h>
#include <unistd.h>
#include <algorithm> // max(), min()

#include <ncurses.h>
#include <pthread.h>
#include <mutex>

#define height 20         // Height of the playing grid, without the 'out of bounds' area.
#define width 10          // Width of the playing grid
#define level_lines 2     // Lines it takes to advance one level.
#define top_buffer 4      // height of 'out of bounds' space at the top of the game.
#define base_speed 800    // Starting speed of 'gravity', in milliseconds

// Used to control access to tetromino manipulation during multithreading.
std::mutex mtx;

struct block{
  // Data attached to individual slots on the games' grid.
  int occupation;
  int color;
  int is_active;

  int col, row;
};


class Shape{
  // Tetromino class, provides tetromino manipulation.
  private:
    int color;
    struct block** Grid;
    void blank();
    void set();
    struct block** members;
  public:
    int center_row;
    int center_col;
    Shape(int color_, struct block** Grid_);
    void rotate();
    int drop();
    void left();
    void right();
    void down();
    void ENDER();
    int check_bounds();
};
void Shape::set()
{
  // Sets all grid spots pointed to by members to active blocks.
  int i;
  for (i=0;i<4;i++)
  {
    members[i]->color = color;
    members[i]->occupation = 1;
    members[i]->is_active = 1;
  }
}
void Shape::blank()
{
  // Clears all grid spots pointed to by 'members'
  int i;
  for (i=0;i<4;i++)
  {
    members[i]->occupation = 0;
    members[i]->is_active = 0;
  }
}
int Shape::drop()
{
  // Shifts the tetromino down one. Returns TRUE (1) if the move is invalid.
  int i;
  for(i=0;i<4;i++)
  {
    if( members[i]->row < top_buffer+height-1 )
    {
      if( Grid[members[i]->row+1][members[i]->col].occupation &&
          !Grid[members[i]->row+1][members[i]->col].is_active)
      {
        return 1;
      }
    }
    else
    {
      return 1;
    }
  }
  blank();
  for(i=0;i<4;i++)
  {
    members[i] = &Grid[members[i]->row+1][members[i]->col];
  }
  center_row++;
  set();
  return 0;
}
void Shape::left()
{
  // Shifts the tetromino left one, if possible
  int i;
  for(i=0;i<4;i++)
  {
    if( members[i]->col == 0 )
    {
      return;
    }
    else if( Grid[members[i]->row][members[i]->col-1].occupation &&
             !Grid[members[i]->row][members[i]->col-1].is_active)
    {
      return;
    }
  }
  blank();
  for(i=0;i<4;i++)
  {
    members[i] = &Grid[members[i]->row][members[i]->col-1];
  }
  center_col--;
  set();
}
void Shape::right()
{
  // Shifts the tetromino right one, if possible.
  int i;
  for(i=0;i<4;i++)
  {
    if( members[i]->col == width )
    {
      return;
    }
    else if( Grid[members[i]->row][members[i]->col+1].occupation &&
             !Grid[members[i]->row][members[i]->col+1].is_active)
    {
      return;
    }
  }
  blank();
  for(i=0;i<4;i++)
  {
    members[i] = &Grid[members[i]->row][members[i]->col+1];
  }
  center_col++;
  set();
}
void Shape::down()
{
  // Sends the tetromino downwards repeatedly, landing it immedietly.
  int collision = 0;
  while(!collision)
    collision = drop();
  ENDER();
}
void Shape::rotate()
{
  // Rotates the tetromino, if possible.
  // For each point, Caclulate rotation location.
  struct block* rotation[4];
  int i;
  int coords[4][2];
  for(i=0;i<4;i++)
  {
    //Translate to the orgin,
    int x = (members[i]->row) - center_row;
    int y = (members[i]->col) - center_col;
    //rotate
    int x1 =  -y;
    int y1 = x;
    //Translate back
    coords[i][0] = x1 + center_row;
    coords[i][1] = y1 + center_col;
  }
  // Translate until rotation area is within bounds. TODO
  int out_left = 0; // Distance that we are out of bounds by, respectively.
  int out_right = width-1;
  int out_bot = height+top_buffer-1;
  int out_top = 0;
  for(i=0;i<4;i++)
  {
    out_left = std::min(out_left, coords[i][1]);
    out_right = std::max(out_right, coords[i][1]);
    out_bot = std::max(out_bot, coords[i][0]);
    out_top = std::min(out_top, coords[i][0]);
  }
  if(out_left < 0)
  {
    for(i=0;i<4;i++)
      coords[i][1] -= out_left;
  }
  else if(out_right > width-1)
  {
    for(i=0;i<4;i++)
      coords[i][1] -= out_right - (width-1);
  }
  if(out_bot > height+top_buffer-1)
  {
    for(i=0;i<4;i++)
      coords[i][0] -= out_bot - (height+top_buffer-1);
  }
  else if(out_top < 0)
  {
    for(i=0;i<4;i++)
      coords[i][0] -= out_top;
  }
  // Set virtual points to actual valid grid locations.
  for(i=0;i<4;i++)
  {
    rotation[i] = &Grid[coords[i][0]][coords[i][1]];
  }

  //Check if all points are valid.
  int valid = 1;
  for(i=0;i<4;i++)
  {
    if(rotation[i]->occupation && !rotation[i]->is_active)
    {
      valid = 0; // Conflicts with a finalized block, invalid rotation.
    }
  }
  if(valid)
  {
    // Save the results
    blank();
    for(i=0;i<4;i++)
    {
      members[i] = rotation[i];
    }
    set();
  }

}
void Shape::ENDER()
{
  // Sets the tetromino in place after landing.
  int i;
  for(i=0;i<4;i++)
  {
    members[i]->is_active = 0;
  }
}
int Shape::check_bounds()
{
  // Decides if a tetromino is out of bounds. Returns true if it is.
  int i;
  for(i=0;i<4;i++)
  {
    if(members[i]->row >= top_buffer)
      return 1;
  }
  return 0;
}
Shape::Shape(int color_, struct block** Grid_)
{
  // Initializes the tetromino that is being spawned.
  color = color_;
  Grid = Grid_;
  center_col = 5;
  center_row = 1;
  members = new struct block* [4];
  // Set member blocks.
  members[0] = &Grid[center_row][center_col];
  switch(color)
  {
    case 0: // I Peice
    members[1] = &Grid[center_row][center_col-1];
    members[2] = &Grid[center_row][center_col+1];
    members[3] = &Grid[center_row][center_col+2];
      break;
    case 1: // J Peice
    members[1] = &Grid[center_row-1][center_col];
    members[2] = &Grid[center_row][center_col+1];
    members[3] = &Grid[center_row][center_col+2];
      break;
    case 2: // L Peice
    members[1] = &Grid[center_row-1][center_col];
    members[2] = &Grid[center_row][center_col-1];
    members[3] = &Grid[center_row][center_col-2];
      break;
    case 3: // 0 Peice
    members[1] = &Grid[center_row-1][center_col];
    members[2] = &Grid[center_row][center_col+1];
    members[3] = &Grid[center_row-1][center_col+1];
      break;
    case 4: // T Peice
    members[1] = &Grid[center_row-1][center_col];
    members[2] = &Grid[center_row][center_col+1];
    members[3] = &Grid[center_row][center_col-1];
      break;
    case 5: // Z Peice
    members[1] = &Grid[center_row-1][center_col];
    members[2] = &Grid[center_row][center_col-1];
    members[3] = &Grid[center_row-1][center_col+1];
      break;
    case 6: // S Peice
    members[1] = &Grid[center_row-1][center_col];
    members[2] = &Grid[center_row-1][center_col-1];
    members[3] = &Grid[center_row][center_col+1];
      break;
  }
  // Light em up.
  set();
}

struct Panel_Data{
  // Data needed for the Game panel.
  WINDOW* panel_win;
  int score;
  int speed;
  int lines;
  int level;
  int next;
};
struct thread_args{
  // Data needed to run the dropper_action and controller_action threads.
  struct block **Grid;
  int speed;
  WINDOW* Game;
  Shape* peice;
};

int base_score(int lines_removed)
{
  // Returns the score gained from lines scored before other modifications.
  int base = 0;
  if (lines_removed == 1)
    base = 40;
  else if (lines_removed == 2)
    base = 100;
  else if (lines_removed == 3)
    base = 300;
  else if (lines_removed == 4)
    base = 1200;

  return base;
}
void refresh_Game(WINDOW* Game, struct block **Grid)
{
  // Redraws the game grid screen.
  wmove(Game, 0, 0);
  wclrtobot(Game);
  box(Game, 0, 0);
  wmove(Game,top_buffer, 0);
  whline(Game, '=', 30);
  int row;
  for(row=0; row< height+top_buffer; row++)
  {
    for(int col=0; col< width; col++)
    {
      if(Grid[row][col].occupation)
      {
        switch(Grid[row][col].color) // TODO - Implement colors.
        {
          case 0:
            break;
        }
        mvwprintw(Game, row+1, col*2+1, "%d", Grid[row][col].color );
      }
    }
  }
  wmove(Game, 0, 0);
  wrefresh(Game);
}
void refresh_Panel(struct Panel_Data info)
{
  // Redraws the Panel.
  wmove(info.panel_win, 0, 0);
  wclrtobot(info.panel_win);
  box(info.panel_win, 0, 0);
  mvwprintw(info.panel_win, 2, 1, "Score: %d", info.score);
  mvwprintw(info.panel_win, 3, 1, "Lines: %d", info.lines);
  mvwprintw(info.panel_win, 4, 1, "Level: %d", info.level);
  mvwprintw(info.panel_win, 5, 1, "Delay (ms): %d", info.speed);
  mvwprintw(info.panel_win, 6, 1, "Next: %d", info.next);
  wrefresh(info.panel_win);
}

void* controller_action(void* arg)
{
  // Thread dedicated to handling user input.
  struct thread_args *args = (struct thread_args*) arg;

  noecho();
  while(1)
  {
    int ch = getch();
    mtx.lock();
    switch(ch)
    {
      case 67: // Right Arrow
        args->peice->right();
        break;
      case 68: // Left Arrow
        args->peice->left();
        break;
      case 66: // Down Arrow
        args->peice->down();
        break;
      case 32: // Space bar
        args->peice->rotate();
        break;
      case 65: // Up Arrow
        //args->peice->save();
        break;
    }
    refresh_Game(args->Game, args->Grid);
    mtx.unlock();
  }
  pthread_exit(NULL);
}
void* dropper_action(void* arg)
{
  // Thread dedicated to tetromino 'gravity'
  struct thread_args *args = (struct thread_args*) arg;

  int collision = 0;
  while(!collision)
  {
    usleep( args->speed * 1000);
    mtx.lock();
    collision = args->peice->drop();
    refresh_Game(args->Game, args->Grid);
    mtx.unlock();
  }
  mtx.lock();
  args->peice->ENDER();
  mtx.unlock();
  pthread_exit(NULL);
}
void* score_flash(void* arg)
{
  // Causes "Score!!!" to flash at a fixed point on the screen.
  attron(A_BOLD|A_BLINK);
  mvprintw(16,5,"Score!!!");
  refresh();
  attroff(A_BOLD|A_BLINK);
  sleep(3);
  mvprintw(16,5,"        ");
  refresh();
  pthread_exit(NULL);
}
void* levelup_flash(void* arg)
{
  attron(A_BOLD|A_BLINK);
  mvprintw(15,5,"Leveled up!");
  refresh();
  attroff(A_BOLD|A_BLINK);
  sleep(4);
  mvprintw(15,5,"           ");
  refresh();
  pthread_exit(NULL);
}

int drop_peice(WINDOW* Game, struct block **Grid, struct Panel_Data* info)
{
  /*
  Spawns a tetromino and allows the user to control it until it lands.
  Checks if the tetromino lands within bounds, returns false if it does.
  */
  // Returns 0 if peice dropped out of bounds, else 1.
  struct thread_args args;
  int color = info->next;
  int speed = info->speed;
  info->next = rand() % 7;

  Shape peice = Shape(color, Grid);
  refresh_Panel(*info);
  refresh_Game(Game, Grid);

  args.Grid = Grid;
  args.speed = speed;
  args.Game = Game;
  args.peice = &peice;
  pthread_t controller;
  pthread_t dropper;
  pthread_attr_t attr;
  void *status;

  if(pthread_create(&controller, NULL, controller_action, (void*) &args))
    exit(-1); // Failed to create controller thread.
  if(pthread_create(&dropper, NULL, dropper_action, (void*) &args))
    exit(-1); // Failed to create dropper thread.

  pthread_join(dropper,&status);

  pthread_cancel(controller); // Tell the controller to finish.

  refresh_Game(Game, Grid);

  //return args.peice->current_row > top_buffer;
  return peice.check_bounds();
}

int remove_rows(struct block **Grid)
{
  /*
  Finds and removes all 'complete' rows and drops down all above rows,
  Returns the number of complete rows removed.
  */
  int full_rows = 0;
  int row;
  for(row = 0; row<height+top_buffer; row++)
  {
    int in_row = 0;
    int col;
    for(col=0; col<width; col++)
    {
      in_row += Grid[row][col].occupation;
    }
    if(in_row == width)
    {
      full_rows++;
      for(col=0; col<width; col++)
        Grid[row][col].occupation = 0;

      // Drop the remaining rows above.
      int dropping_row = row;
      while(dropping_row > 1)
      {
        for(col = 0; col < width; col++)
        {
          Grid[dropping_row][col].occupation = Grid[dropping_row-1][col].occupation;
        }
        dropping_row --;
      }
    }
  }
  if(full_rows)
  {
    pthread_t flash;
    pthread_create(&flash, NULL, score_flash, NULL);
  }
  return full_rows;
}


int PlayGame()
{
  /*
  Begins a lovely game of Tetris.
  Returns the score the player earned at game end.
  */
  initscr();
  WINDOW *Panel;
  WINDOW *Game;

  int term_x, term_y;
  getmaxyx(stdscr, term_y, term_x);
  while(term_y < height+top_buffer+3 || term_x < 50)
  {
    mvprintw(0, 0, "Please enlarge your terminal to 50x%d chars for Tetris!",height+top_buffer+3);
    if(term_y < height+top_buffer+3)
      mvprintw(1,0, "Down more!");
    else if(term_x < 50)
      mvprintw(1,0, "Right more!");
    refresh();
    getmaxyx(stdscr, term_y, term_x);
  }
  clear();

  Panel = newwin(10, 20, 3, 1);
  Game = newwin(height+top_buffer+2, width*2+2, 1, 22);
  // Initializing Grid..
  struct block **Grid;
  Grid = new struct block * [height+top_buffer];
  for(int row=0 ;row<height+top_buffer; row++)
    Grid[row] = new struct block[width];
  // Start the grid Empty.
  for(int row=0; row<height+top_buffer; row++)
  {
    for(int col=0; col<width; col++)
    {
      Grid[row][col].occupation = 0;
      Grid[row][col].color = 0;
      Grid[row][col].is_active = 0;
      Grid[row][col].row = row;
      Grid[row][col].col = col;
    }
  }

  struct Panel_Data info;
  info.panel_win = Panel;
  info.score = 0;
  info.speed = base_speed; // 1 second
  info.lines = 0; // Level up every 5 lines.
  info.level = 0;
  info.next = rand() % 7;
  refresh_Panel(info);
  refresh_Game(Game, Grid);
  refresh();

  while(drop_peice(Game, Grid, &info))
  {
    int lines_removed = remove_rows(Grid);
    info.score = info.score + (info.level+1)*(base_score(lines_removed));
    info.lines = info.lines+lines_removed;
    int old_level = info.level;
    info.level = info.lines/level_lines;
    info.speed = base_speed * (pow(.9, info.level));

    if(old_level != info.level)
      {
        pthread_t flash;
        pthread_create(&flash, NULL, levelup_flash, NULL);
      }

    refresh_Panel(info);
    refresh_Game(Game, Grid);
  }
  wattron(Game, A_BOLD | A_BLINK);
  mvwprintw(Game, 10,5, "GAME OVER");
  wrefresh(Game);
  wmove(Game, 0, 0);
  sleep(5);

  delwin(Panel);
  delwin(Game);
  endwin();

  return info.score;
}


int main(int argc, char** argv)
{
  // A small program to test a player's tetris skills.
  puts("Welcome to Terminal Tetris!");
  puts("Space to rotate peice, arrows to move. Good luck!");
  puts("=== Press Enter to Continue ===");
  getchar();
  int score = PlayGame();
  printf("Final Score :: %d\nThanks for playing!\n", score);
  return 0;
}