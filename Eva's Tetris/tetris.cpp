//*************
//Compilation Line: g++ -std=c++11 -o Tetris tetris.cpp -lncurses -lpthread
//This program allows the user to play a game of Tetris.
//Possible future expansions - Sound and Music system. Shape Previews
//*************
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <ncurses.h>
#include <pthread.h>
#include <mutex>

#define height 20         // Height of the playing grid, without the 'out of bounds' area.
#define width 10          // Width of the playing grid
#define level_lines 2     // Lines it takes to advance one level.
#define top_buffer 4      // height of 'out of bounds' space at the top of the game.
#define base_speed 800    // Starting speed of 'gravity', in milliseconds

// Used to control access to tetromino manipulation
std::mutex mtx;
std::mutex mtx_roll;    // Used to protect game score.

struct block{
  // Data attached to individual slots on the games' grid.
  int occupation;
  int color;
  int is_active;

  int col, row;
};
struct Hold_box{
  // Overhead for the hold piece controls
  int hold;
  int hold_use;
  int hold_call;
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
    void save(struct Hold_box *reserve);
};
void Shape::set()
{
  // Sets all grid spots pointed to by 'members' to active blocks.
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
  // Shifts the tetromino left one, if reachable
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
  // Shifts the tetromino right one, if reachable.
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
  // Shifts the tetromino downwards repeatedly until it hits a wall, landing it immedietly.
  int collision = 0;
  while(!collision)
    collision = drop();
  ENDER();
}
void Shape::rotate()
{
  // Rotates the tetromino, if space permits
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
  // Translate until rotation area is within bounds.
  {
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
  // Save the results if all points valid.
  if(valid)
  {
    blank();
    for(i=0;i<4;i++)
    {
      members[i] = rotation[i];
    }
    set();
  }

}
void Shape::save(struct Hold_box *reserve)
{
  //Removes the peice from the board.
  //Places the peice in Hold on the board if there is one, else drop next.

  if(reserve->hold_use == 0) // Hold use is zero if avaliable
  {
    down();
    blank(); // When down and blank are comboed, they remove the piece.
    // because of Down(), the dropper is gaurenteed to exit it's loop.
    if(reserve->hold != -1)
    {
      reserve->hold_call = reserve->hold;
    }
    reserve->hold = color;
    reserve->hold_use = 1;
  }
}
void Shape::ENDER()
{
  // Sets the tetromino inactive and sets it in place.
  int i;
  for(i=0;i<4;i++)
  {
    members[i]->is_active = 0;
  }
}

Shape::Shape(int color_, struct block** Grid_)
{
  // Initializes the tetromino that is being spawned at the top of the game.
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
  // Overhead data needed for the Game panel display and game.
  WINDOW* panel_win;
  int score;
  int new_score;
  int speed;
  int lines;
  int level;
  int next;
  struct Hold_box reserve;
};
struct thread_args{
  // Data needed to run the dropper_action and controller_action threads.
  struct block **Grid;
  int speed;
  WINDOW* Game;
  Shape* peice;

  struct Hold_box *reserve;
};

void color_definitions()
{
  init_color(COLOR_BLACK, 150,150,150);
  init_color(COLOR_MAGENTA, 1000,500,1000);
  init_color(COLOR_BLUE, 300,500,1000);
  init_color(COLOR_GREEN, 0,1000,0);
  init_color(COLOR_CYAN, 0,800,800);
  init_color(COLOR_RED, 800,0,0);

  init_pair(1,COLOR_CYAN,COLOR_BLACK);
  init_pair(2,COLOR_BLUE,COLOR_BLACK);
  init_pair(3,COLOR_WHITE,COLOR_BLACK);
  init_pair(4,COLOR_YELLOW,COLOR_BLACK);
  init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
  init_pair(6,COLOR_GREEN,COLOR_BLACK);
  init_pair(7,COLOR_RED,COLOR_BLACK);

  init_pair(8,COLOR_CYAN,COLOR_CYAN);
  init_pair(9,COLOR_BLUE,COLOR_BLUE);
  init_pair(10,COLOR_WHITE,COLOR_WHITE);
  init_pair(11,COLOR_YELLOW,COLOR_YELLOW);
  init_pair(12,COLOR_MAGENTA,COLOR_MAGENTA);
  init_pair(13,COLOR_GREEN,COLOR_GREEN);
  init_pair(14,COLOR_RED,COLOR_RED);
}

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

int check_bounds(struct block **Grid)
{
  // Decides if a tetromino is out of bounds. Returns false if it is.
  int col;
  for(col=0;col<width;col++)
  {
    if(Grid[top_buffer-1][col].occupation)
      return 0;
  }
  return 1;
}

void refresh_Game(WINDOW* Game, struct block **Grid)
{
  // Redraws the game board screen.
  wmove(Game, 0, 0);
  wclrtobot(Game);
  box(Game, 0, 0);
  wmove(Game,top_buffer, 0);
  whline(Game, '=', 30);
  int row, col;
  int lane[width];
  for(col=0;col<width;col++)
  {
    lane[col] = 0;
    for(row=0;row<height+top_buffer;row++)
    {
      if(Grid[row][col].occupation && Grid[row][col].is_active)
        lane[col] = 1;
    }
  }
  for(row=0; row< height+top_buffer; row++)
  {
    for(col=0; col< width; col++)
    {
      if(Grid[row][col].occupation)
      {
        if(lane[col] && !Grid[row][col].is_active)
        {
          wattron(Game,COLOR_PAIR(Grid[row][col].color+1+7));
          mvwprintw(Game, row+1, col*2+1, "$");
          wattroff(Game,COLOR_PAIR(Grid[row][col].color+1+7));
          lane[col] = 0;
        }
        else
        {
          wattron(Game,COLOR_PAIR(Grid[row][col].color+1));
          mvwprintw(Game, row+1, col*2+1, "%d", Grid[row][col].color );
          wattroff(Game,COLOR_PAIR(Grid[row][col].color+1));
        }
      }
    }
  }
  wmove(Game, 0, 0);
  wrefresh(Game);
}
void refresh_Panel(struct Panel_Data info)
{
  // Redraws the Information Panel.
  wmove(info.panel_win, 0, 0);
  wclrtobot(info.panel_win);
  box(info.panel_win, 0, 0);
  if(info.score != info.new_score)
    wattron(info.panel_win, COLOR_PAIR(1));
  mvwprintw(info.panel_win, 2, 1, "Score: %d", info.score);
  wattroff(info.panel_win, COLOR_PAIR(1));
  mvwprintw(info.panel_win, 3, 1, "Lines: %d", info.lines);
  mvwprintw(info.panel_win, 4, 1, "Level: %d", info.level);
  mvwprintw(info.panel_win, 5, 1, "Delay (ms): %d", info.speed);

  wattron(info.panel_win,COLOR_PAIR(info.next+1));
  mvwprintw(info.panel_win, 6, 1, "Next: %d", info.next);
  wattroff(info.panel_win,COLOR_PAIR(info.next+1));

  wattron(info.panel_win,COLOR_PAIR(info.reserve.hold+1));
  mvwprintw(info.panel_win, 7, 1, "Hold: %d", info.reserve.hold);
  wattroff(info.panel_win,COLOR_PAIR(info.reserve.hold+1));

  wrefresh(info.panel_win);
}

void* controller_action(void* arg)
{
  // Thread dedicated to handling user input throughout the game.
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
        args->peice->save(args->reserve);
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
  // Causes "Score!!!" to flash at a fixed point on the screen for few seconds. Does not pause the game.
  mtx.lock();
  attron(A_BOLD|A_BLINK);
  mvprintw(16,5,"Score!!!");
  refresh();
  attroff(A_BOLD|A_BLINK);
  mtx.unlock();
  sleep(3);
  mtx.lock();
  mvprintw(16,5,"        ");
  refresh();
  mtx.unlock();
  pthread_exit(NULL);
}
void* levelup_flash(void* arg)
{
  // Causes 'Leveled up!' to flash for a few seconds in a fixed position for a few seconds. Does not pause the game.
  mtx.lock();
  attron(A_BOLD|A_BLINK);
  mvprintw(15,5,"Leveled up!");
  refresh();
  attroff(A_BOLD|A_BLINK);
  mtx.unlock();
  sleep(4);
  mtx.lock();
  mvprintw(15,5,"           ");
  refresh();
  mtx.unlock();
  pthread_exit(NULL);
}
void* rolling_score(void* arg)
{
  struct Panel_Data* info = (struct Panel_Data*) arg;
  mtx_roll.lock();
  int diff = info->new_score - info->score;
  int parts = 50;
  int division = diff/parts;
  int remainder = diff%parts;
  while(info->new_score != info->score)
  {
    if(parts)
    {
      info->score += division;
      parts--;
    }
    else
    {
      info->score++;
      remainder--;
    }
    refresh_Panel(*info);
    usleep(50000);
  }
  mtx_roll.unlock();
  pthread_exit(NULL);
}

void drop_peice(WINDOW* Game, struct block **Grid, struct Panel_Data* info)
{
  /*
  Spawns a tetromino and allows the user to control it until it lands.
  Checks if the tetromino lands within bounds, returns false if it does.
  */
  // Returns 0 if peice dropped out of bounds, else 1.

  int speed = info->speed;
  int color;
  // Drop either a random peice or the Hold peice.
  if(info->reserve.hold_call != -1)
  {
    color = info->reserve.hold_call;
    info->reserve.hold_call = -1;
  }
  else
  {
    color = info->next;
    info->next = rand() % 7;
    info->reserve.hold_use = 0;
  }
  // Spawn the piece and display it on screen.
  Shape peice = Shape(color, Grid);
  refresh_Panel(*info);
  refresh_Game(Game, Grid);

  // Set up threads for Controller and Dropper
  struct thread_args args;
  {
    args.Grid = Grid;
    args.speed = speed;
    args.Game = Game;
    args.peice = &peice;
    args.reserve = &info->reserve;
  }
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
}

int remove_rows(struct block **Grid, WINDOW* Game)
{
  /*
  Finds and removes all 'complete' rows and drops down all above rows.
  Complete rows flash for a few seconds before being removed.
  Returns the number of complete rows removed.
  */
  int full_rows = 0;
  int row;
  for(row = 0; row<height+top_buffer; row++)
  {
    int in_row = 0;
    int col;
    // Detect full rows
    for(col=0; col<width; col++)
    {
      in_row += Grid[row][col].occupation;
    }
    // Flash and remove full rows
    if(in_row == width)
    {
      full_rows++;
      wattron(Game, A_BLINK);
      for(col=0; col<width; col++)
      {
        Grid[row][col].occupation = 0;
        mvwprintw(Game, row+1, col*2+1, "#");
      }
      wattroff(Game, A_BLINK);
      // Drop the remaining rows above.
      int dropping_row = row;
      while(dropping_row > 1)
      {
        for(col = 0; col < width; col++)
        {
          Grid[dropping_row][col].occupation = Grid[dropping_row-1][col].occupation;
          Grid[dropping_row][col].color = Grid[dropping_row-1][col].color;
        }
        dropping_row --;
      }
    }
  }
  if(full_rows)
  {
    wrefresh(Game);
    pthread_t flash;
    pthread_create(&flash, NULL, score_flash, NULL);
    sleep(1);
  }
  return full_rows;
}


int Tetris()
{
  /*
  Begins a lovely game of Tetris.
  Returns the score the player earned at game end.
  */
  // Initialize ncurses mode, ncurses color mode
  initscr();
  start_color();
  color_definitions();
  // Terminal dimension cheecking block.
  {
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
  }

  // Create new windows
  WINDOW *Panel;
  WINDOW *Game;
  Panel = newwin(10, 20, 3, 1);
  Game = newwin(height+top_buffer+2, width*2+2, 1, 22);

  // Seed randomizer with current time
  srand(time(0));

  // Initializing Grid..
  struct block **Grid;
  {
    Grid = new struct block * [height+top_buffer];
    for(int row=0 ;row<height+top_buffer; row++)
      Grid[row] = new struct block[width];
    // Start the grid Empty.
    for(int row=0; row<height+top_buffer; row++)
    {
      for(int col=0; col<width; col++)
      {
        struct block *current = &Grid[row][col];
        current->occupation = 0;
        current->color = 0;
        current->is_active = 0;
        current->row = row;
        current->col = col;
      }
    }
  }
  // Initialize Panel information
  struct Panel_Data info;
  {
    info.panel_win = Panel;
    info.score = 0;
    info.new_score = 0;
    info.speed = base_speed; // 1 second
    info.lines = 0; // Level up every 5 lines.
    info.level = 0;
    info.next = rand() % 7;
    info.reserve.hold = -1;
    info.reserve.hold_use = 0;
    info.reserve.hold_call = -1;
  }
  // Populate the windows we created earlier and display them.
  refresh_Panel(info);
  refresh_Game(Game, Grid);
  refresh();

  // Game loop begins.
  while(check_bounds(Grid))
  {
    drop_peice(Game, Grid, &info);
    // Check for completed rows, update information board.
    int lines_removed = remove_rows(Grid, Game);
    info.new_score = info.new_score + (info.level+1)*(base_score(lines_removed));
    info.lines = info.lines+lines_removed;
    int old_level = info.level;
    info.level = info.lines/level_lines;
    info.speed = base_speed * (pow(.9, info.level));

    if(old_level != info.level)
    {
      pthread_t flash;
      pthread_create(&flash, NULL, levelup_flash, NULL);
    }

    if(info.new_score != info.score)
    {
      pthread_t roller;
      pthread_create(&roller, NULL, rolling_score,  &info);
    }

    refresh_Panel(info);
    refresh_Game(Game, Grid);
  }
  // Ending Game.
  {
    wattron(Game, A_BOLD | A_BLINK);
    mvwprintw(Game, 10,5, "GAME OVER");
    wrefresh(Game);
    wmove(Game, 0, 0);
    sleep(5);

    for(int row=0 ;row<height+top_buffer; row++)
      delete[] Grid[row];
    delete[] Grid;

    delwin(Panel);
    delwin(Game);
    endwin();
  }
  return info.new_score;
}
