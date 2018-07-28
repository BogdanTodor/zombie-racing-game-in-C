#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

sprite_id race_car;

char *race_car_image =" O+O "
                      "  +  "
                      " {=} "
                      "[]+[]";

#define Max_Debris (5)
sprite_id debris[Max_Debris];
int debris_count = 5;

char * debris_img = "$$$$$"
                    "$$$$$";

#define Max_trees (2)
sprite_id tree[Max_trees];
int tree_count = 2;



char * tree_img = " TTT "
                  "TTTTT"
                  " TTT "
                  "  I  "
                  "  I  ";

sprite_id fuel_depot;

char * fuel_depot_img = "################"
                        "#              #"
                        "#    oooooo    #"
                        "#    o         #"
                        "#    oooooo    #"
                        "#    o         #"
                        "#    o         #"
                        "#              #"
                        "################";

// create finish line
sprite_id finish_line;

// make this a lot wider for very large screens

char * finish_line_img = "========================================================================================================================================================================================================"
                         "========================================================================================================================================================================================================";


bool game_over = false;
bool update_screen = true;
int speed = 0;
double distance_travelled = 0;
double fuel_total = 100;
double Time_before;
double Time_after;
double refuel_start;
double refuel_end;
double Time;
int health = 100;
double elapsed_time;
int midx;
int midy;
int w;
int h;
int on_road_left;
int on_road_right;
int tree_left_or_right;

// Part B global variables
#define Max_zombies (25)
sprite_id zombie[Max_zombies];
int zombie_count = 5;

char * zombie_img = " _(o_o)_|"  // 9 x 4
                    "|   O    "
                    "    O    "
                    "   | |   ";

int z_x;





// Collision between 2 objects
bool collision ( sprite_id object1, sprite_id object2 ) {
  int top_object1 = round(sprite_y(object1));
  int left_object1 = round(sprite_x(object1));
  int right_object1 = left_object1 + sprite_width(object1) - 1;
  int bottom_object1 = top_object1 + sprite_height(object1) - 1;

  int top_object2 = round(sprite_y(object2));
  int left_object2 = round(sprite_x(object2));
  int right_object2 = left_object2 + sprite_width(object2) - 1;
  int bottom_object2 = top_object2 + sprite_height(object2) - 1;

  if ( top_object2 > bottom_object1 ) return false;
  if ( top_object1 > bottom_object2 ) return false;
  if ( left_object1 > right_object2 ) return false;
  if ( left_object2 > right_object1 ) return false;

  return true;
}


// Part 1 - Splash screen
void splash_screen(void){
  setup_screen();
  midx = (screen_width()-1) / 2;
  midy = (screen_height()-1) / 2;

  draw_string(midx - 15, midy -10, "welcome to Race To Zombie Mountain");
  draw_string(midx - 15, midy - 9, "By Bogdan Todor, N9726471");
  draw_string(midx - 15, midy -6, "Controls:");
  draw_string(midx - 15, midy -5, "Accelerate: w");
  draw_string(midx - 15, midy - 4, "Decelerate: s");
  draw_string(midx - 15, midy - 3, "Move right: d");
  draw_string(midx - 15, midy - 2, "Move left:  a");
  draw_string(midx - 15, midy - 1, "Quit game:  q");
  draw_string(midx - 15, midy + 1, "press 'any key' to continue...");
  draw_string(midx - 15, midy + 3, "Instructions:");
  draw_string(midx - 15, midy + 4, "- Avoid taking damage as once health reaches 0, game is over");
  draw_string(midx - 15, midy + 5, "  over");
  draw_string(midx - 15, midy + 6, "- Avoid hitting obstacles and scenary. collision = -20 ");
  draw_string(midx - 15, midy + 7, "  health");
  draw_string(midx - 15, midy + 8, "- To refuel, come to a complete stop beside a fuel");
  draw_string(midx - 15, midy + 9, "  depot for 3 or more seconds");
  draw_string(midx - 15, midy + 10, "- Colliding with the fuel depot = -100 health");

  show_screen();
  wait_char();
  clear_screen();
}
// End of part 1

// Part 2 - Draw border
void draw_border(void){
  draw_line(0, 0, w, 0, '*');
  draw_line(w, 0, w, h, '*');
  draw_line(w, h, 0, h, '*');
  draw_line(0, h, 0, 0, '*');
}
// end of part 2

// Part 3 - Dashboard
void dashboard(void){
  //Draw road
  draw_line(on_road_left, h-1, on_road_left, 1, '#');
  draw_line(on_road_right, h-1, on_road_right, 1, '#');

  //Dashboard
  draw_line(1, 1, 22, 1, '-');
  draw_line(22, 1, 22, 8, '-');
  draw_line(22, 8, 1, 8, '-');
  draw_line(1, 8, 1, 1, '-');

  //Draw label
  draw_string(2, 2, "Dashboard");
  draw_string(2, 3, "Fuel:");
  draw_string(2, 4, "Speed:");
  draw_string(2, 5, "Health:");
  draw_string(2, 6, "Distance:");
  draw_string(2, 7, "Time:");

  //Draw dashboard values
  Time = Time_after - Time_before;
  elapsed_time = roundf(Time * 100) / 100;
  draw_int(13, 4, speed);
  draw_double(13, 6, distance_travelled); //reduce decimal places
  draw_double(13, 3, fuel_total);
  draw_double( 13, 7, elapsed_time); //reduce decimal places
  draw_double( 13, 5, health);
}

void clear_dashboard_area(void){
  // draw lines to erase the dashboard area
  draw_line(1, 1, 22, 1, ' ');
  draw_line(1, 2, 22, 2, ' ');
  draw_line(1, 3, 22, 3, ' ');
  draw_line(1, 4, 22, 4, ' ');
  draw_line(1, 5, 22, 5, ' ');
  draw_line(1, 6, 22, 6, ' ');
  draw_line(1, 7, 22, 7, ' ');
  draw_line(1, 8, 22, 8, ' ');
}
// End of part 3

// --------------------------start of object functions------------------------//
// Part 6 Scenery and obstacles

// Scenery
// Function that creates the tree objects
sprite_id create_tree( void ){
  int tree_y = rand() % (h + 1 - 1) + 1 ;
  int tree_x = rand() % (w - 5 + 1);

  while (tree_x <= on_road_right && (tree_x + 5) >= on_road_left ){
    tree_x = rand() % (w - 5);
  }

  sprite_id tree = sprite_create(tree_x, tree_y, 5, 5, tree_img);

  return tree;
}

// The function that dictates which conditions must be met for the tree to start
// scrolling down the screen and the speed at which the trees scroll down in
// relativity to the current speed of the car
void move_tree(sprite_id tree){
  if (round(sprite_y(race_car)) == (h - (h-4)/4)){
    sprite_move(tree, 0, +0.02 * speed);
  }
}

// Function that determines the random spawn point of the trees after they have
// scrolled off the bottom of the screen
void respawn_trees(bool fromCollision){

  for (int i = 0; i < tree_count; i++){
    int tree_x = rand() % (w - 5);

    while (tree_x <= on_road_right && (tree_x + 5) >= on_road_left ){
      tree_x = rand() % (w - 5);
    }

    if ((sprite_y(tree[i])) > screen_height()-1){

      sprite_move_to(tree[i], tree_x ,-5);

      // if the tree_x value is less than the roadleft but bigger than road right, spawn it otherwise no
    }else if(fromCollision){
      sprite_move_to(tree[i], tree_x ,sprite_y(tree[i]));
    }
  }
}


// Obstacles
// Function that creates the debris objects
sprite_id create_debris( void ){

  int debris_x = rand() % ((on_road_right - 6) + 1 - (on_road_left + 2)) + (on_road_left + 2);
  int debris_y = rand() % ((h/4 * 2) + 1 - 2) + 2 ;

  sprite_id debris = sprite_create(debris_x, debris_y, 5, 2, debris_img);

  return debris;
}

// Function which dictates when the debris object will start moving and the
// speed at which the debris object will move in relativity to the speed of them
// car
void move_debris(sprite_id debris){
  if (round(sprite_y(race_car)) == (h - (h-4)/4)){
    sprite_move(debris, 0, +0.02 * speed);
  }
}

// Function which determines the random spawn location of the debris after its
// scrolled off the bottom of the screen
void respawn_debris(void){
  for (int i = 0; i < debris_count; i++){
    int debris_x = rand() % ((on_road_right - 6) + 1 - (on_road_left + 2)) + (on_road_left + 2);
    if (round(sprite_y(debris[i])) > screen_height()-1){
      sprite_move_to(debris[i], debris_x ,-5);
    }
  }
}
// End of part 6

// // creating finish line
sprite_id create_finish_line(void){
  finish_line = sprite_create((on_road_left+1), -2, (on_road_right - on_road_left), 2, finish_line_img);
  return finish_line;
}

// The function that dictates which conditions must be met for the tree to start
// scrolling down the screen and the speed at which the trees scroll down in
// relativity to the current speed of the car
void move_finish_line(sprite_id finish_line){
  if (round(sprite_y(race_car)) == (h - (h-4)/4)){
    sprite_move(finish_line, 0, 0.02*speed);
  }
}


// Part 7 - fuel depot
// Function that creates the fuel depot
sprite_id create_fuel_depot (void){

  int FD_x;
  int FD_y = rand() % (h + 1 - 1) + 1;

  int FD_left_or_right = rand() % (2 + 1 -1) + 1;

  if(FD_left_or_right == 1){ // 1 = left
    FD_x = on_road_left - 15; //
  }

  if(FD_left_or_right == 2){ // 2 = right
    FD_x = on_road_right;
  }

  fuel_depot = sprite_create(FD_x, FD_y, 16, 9, fuel_depot_img);

  return fuel_depot;
}

// Function states which conditions must be met for the fuel depot to start
// moving and the speed at which the fuel depot will move relative to the car
void move_fuel_depot(sprite_id fuel_depot){
  if (round(sprite_y(race_car)) == (h - (h-4)/4)){
    sprite_move(fuel_depot, 0, +0.02 * speed);
  }
}

void respawn_fuel_depot(void){
  // Drawing the fuel depot at randomised position
  int FD_x;

  int FD_left_or_right = rand() % (2 + 1 -1) + 1;

  if(FD_left_or_right == 1){ // 1 = left
    FD_x = on_road_left - 15; //
  }

  if(FD_left_or_right == 2){ // 2 = right
    FD_x = on_road_right;
  }

  int FD_y = rand() % (100 + 1 - 50) + 50;

  if (round(sprite_y(fuel_depot)) > screen_height()-1){
    if (FD_left_or_right == 1){
      sprite_move_to(fuel_depot, FD_x ,-FD_y);
    }
    if (FD_left_or_right == 2){
      sprite_move_to(fuel_depot, FD_x , -FD_y);
    }
  }
}

// End of part 7

// Part B zombie //
bool zombie_collision_check(sprite_id zombie){
  bool hit_something = false;

  if(collision( fuel_depot, zombie)) {
    hit_something = true;
  }
  // //preventing overlap of zombie with tree
  for ( int j = 0; j < tree_count; j++ ) {
    if(collision( tree[j], zombie)) {
      hit_something = true;
    }
  }
  // // //preventing overlap of zombie with debris
  for ( int j = 0; j < debris_count; j++ ) {
    if(collision(debris[j], zombie)) { //
      hit_something = true;
    }
  }
  return hit_something;
}

sprite_id create_zombie( void ){
  z_x = rand() % ((w - 1 -10) + 1 - 1) + 1;
  int z_y = rand() % (0 + 1 - -h) + -h;

  sprite_id zombie = sprite_create(z_x, z_y, 9, 4, zombie_img);

  int count = 0;

  while(zombie_collision_check(zombie)){
    count++;
    z_x = rand() % ((w - 1 -10) + 1 - 1) + 1;
    sprite_move_to(zombie, z_x , z_y);
  }

  double z_dx = 2.0;
  sprite_turn_to(zombie, 0.05 * z_dx, 0);

  return zombie;
}


void move_zombie(sprite_id zombie){
  double old_zx = sprite_x(zombie);
  double old_zy = sprite_y(zombie);

  bool bounced = false;

  sprite_step( zombie );

  if (round(sprite_y(race_car)) == (h - (h-4)/4)){
    sprite_move(zombie, 0, +0.02 * speed);
  }

  if ( round(sprite_x(zombie)) < 1 || round(sprite_x(zombie)) > screen_width() - 10 ) {
    double zdx = sprite_dx ( zombie);
    double zdy = sprite_dy( zombie);
    sprite_turn_to( zombie, -zdx, zdy  );
    bounced = true;
  }

  if(collision(zombie, fuel_depot)){
    double zdx = sprite_dx ( zombie);
    double zdy = sprite_dy( zombie);
    sprite_turn_to( zombie, -zdx, zdy  );
    bounced = true;
  }

  // collision detection for the on road debris
  for( int i = 0; i < debris_count; i++){
    if(collision(zombie, debris[i])){
      double zdx = sprite_dx ( zombie);
      double zdy = sprite_dy( zombie);
      sprite_turn_to( zombie, -zdx, zdy  );
      bounced = true;
    }
  }

  // collision detection for the tree
  for( int i = 0; i < tree_count; i++){
    if(collision(zombie, tree[i])){
      double zdx = sprite_dx ( zombie);
      double zdy = sprite_dy( zombie);
      sprite_turn_to( zombie, -zdx, zdy  );
      bounced = true;
    }
  }

  if ( bounced ) {
    sprite_move_to(zombie, old_zx, old_zy);
  }
}


void respawn_zombie(sprite_id zombie){
  if ((sprite_y(zombie)) > screen_height()-1){
    do{
      z_x = rand() % ((w - 1 -10) + 1 - 1) + 1;
      sprite_move_to(zombie, z_x ,-4);
    }while(zombie_collision_check(zombie));
  }
}

void increase_zombies(void){
  if (distance_travelled > 200){
    zombie_count = 10;
  }
  if (distance_travelled > 400){
    zombie_count = 15;
  }
  if (distance_travelled > 600){
    zombie_count = 20;
  }
  if (distance_travelled > 800){
    zombie_count = 25;
  }



  if (game_over){
    zombie_count = 5;
  }
}

// Part B zombie end
void drawing_sprites(void){
  sprite_draw(finish_line);

  for(int i = 0; i < tree_count; i++){
    sprite_draw(tree[i]);
  }
  for (int i = 0; i < debris_count; i++){
    sprite_draw(debris[i]);
  }
  for (int i = 0; i < zombie_count; i++){
    sprite_draw(zombie[i]);
  }

  sprite_draw(fuel_depot);
}

// ----------------------------End of object functions------------------------//

// Part 4 & 5 - Race car, horizontal movement, acceleration and speed.
// function which sets up the initial screen and the race_car sprite
void setup_game(void){
  setup_screen();

  on_road_left = (screen_width() - 1) / 3 + 1;
  on_road_right = 2 * on_road_left;

  w = screen_width() -1;
  h = screen_height() -1;

  race_car = sprite_create( (w-5)/2, h - 4 , 5 , 4 , race_car_image ); //unsure if it draws up or down
  sprite_draw(race_car);

  time_t now = time(NULL);
  srand( now );

  for( int i = 0; i < debris_count; i++){
    debris[i] = create_debris();
  }

  for( int i = 0; i < tree_count; i++){
    tree[i] = create_tree();
  }

  create_fuel_depot();
  create_finish_line();

  for( int i = 0; i < Max_zombies; i++){
    zombie[i] = create_zombie();
  }

  draw_border( );
  dashboard( );
  show_screen();
}


// function which determines the horizontal movement, speed and slowing down of
// the car
void move_car( int key ){
  if (key == 'w'){
    if (sprite_y(race_car) > (h - (h-4)/4)){
      sprite_move(race_car, 0, -1);
    }
    if (sprite_x(race_car) >= on_road_left-1 && sprite_x(race_car) <= on_road_right){
      if (speed >= 0 && speed < 10){
        speed = speed + 1;
      }
    }
    if (sprite_x(race_car) < on_road_left || sprite_x(race_car) > on_road_right){
      if (speed >= 0 && speed < 3){
        speed = speed + 1;
      }
    }
  }
  if (key == 's'){
    if (speed > 0 && speed <= 10){
      speed = speed - 1;
    }
  }
  if (key == 'a'){
    if (sprite_x(race_car) > 1 && speed > 0) {
      sprite_move( race_car, -1, 0);
    }
    if (sprite_x(race_car) < on_road_left){
      if (speed > 3){
        speed = 3;
      }
    }
  }
  if ( key == 'd' ) {
    if ( sprite_x(race_car) < screen_width() -1 -sprite_width(race_car) && speed > 0) {
      sprite_move( race_car, +1, 0);
    }
    if (sprite_x(race_car) > on_road_right){
      if (speed > 3){
        speed = 3;
      }
    }
  }
  // quit game function moved to here due to ease of seeing all controls in one function
  if (key == 'q'){
    clear_screen();
    setup_screen();

    draw_string(midx - 2, midy - 6, "GAME OVER");
    draw_string(midx - 2, midy - 4, "YOU QUIT");
    draw_string(midx - 4, midy - 0, "Time elapsed:");
    draw_double(midx - 4, midy + 2, elapsed_time);
    draw_string(midx + 1, midy + 2, "seconds");
    draw_string(midx - 9, midy + 10, "press 'y' to restart...");
    draw_string(midx - 9, midy + 12, "press 'n' to exit...");

    show_screen();
    game_over = true;

  }
}
// End of part 4 & 5


// Part 8 - Fuel
// Fuel consumed - explain number choices
void fuel(void){
  if (fuel_total > 1){
    fuel_total = fuel_total - 0.002 * speed;
  }
}

// refuelling function
void refuel(void){

  if(sprite_y(race_car) < sprite_y(fuel_depot) || sprite_y(race_car) > sprite_y(fuel_depot) + 8){
    refuel_start = 0;
    refuel_end = 0;}


    if ((sprite_y(race_car) >= sprite_y(fuel_depot) && sprite_y(race_car) <= sprite_y(fuel_depot)+8)
    && (((sprite_x(race_car) == on_road_left + 1) || (round(sprite_x(race_car)) + 4) == sprite_x(fuel_depot)-1)) ){// change screen_width term to sprite
      if(speed == 0){
        refuel_end = get_current_time();
      }
    }

    if ((sprite_y(race_car) >= sprite_y(fuel_depot) && sprite_y(race_car) <= sprite_y(fuel_depot)+8)
    && (((sprite_x(race_car) == on_road_left + 1) || (round(sprite_x(race_car)) + 4) == sprite_x(fuel_depot) -1)) ){// change screen_width term to sprite
      if(speed > 0){
        refuel_start = get_current_time();
      }
    }

    double refuel_time = refuel_end - refuel_start;

    if(refuel_time >=3){
      fuel_total = 100;
    }
  }
  // End of part 8


  // Part 9 - Distance travelled
  void distance(void){
    distance_travelled =  0.01 * speed + distance_travelled;

    if (distance_travelled >= 1000){
      move_finish_line(finish_line);
    }
  }
  // End of part 9


  // Part 10 - Collision
  // function to check collision between objects
  void if_collided() {

    if ( collision( fuel_depot, race_car ) ) {
      health = 0;
      speed = 0;
    }

    // prevent overlapping of trees
    for ( int i = 0; i < tree_count; i++ ) {
      for ( int j = 0; j < tree_count; j++ ) {
        int d_x = sprite_x(tree[i]);
        int d_y = sprite_y(tree[i]);
        if(i != j){
          while ( collision( tree[i], tree[j] ) ) {
            sprite_move_to(tree[i], d_x, d_y - sprite_height(tree[i]));
          }
        }
      }
    }

    for ( int i = 0; i < tree_count; i++ ) {
      while ( collision( fuel_depot, tree[i] ) ) {
        respawn_trees(true);
      }
    }

    for ( int i = 0; i < debris_count; i++ ) {
      if ( collision( race_car, debris[i] ) ) {
        health = health - 20;
        speed = 0; // to account for left, right, top and bottom collision and the two layers of the roadblock
        sprite_move_to(race_car, (w-5)/2, screen_height() - 5);
      }
    }
    for ( int i = 0; i < tree_count; i++ ) {
      if ( collision( race_car, tree[i] ) ) {
        health = health - 20;
        speed = 0; // to account for left, right, top and bottom collision and the two layers of the roadblock
        sprite_move_to(race_car, (w-5)/2, screen_height() - 5);
      }
    }

    // prevent overlapping of debris
    for ( int i = 0; i < debris_count; i++ ) {
      for ( int j = 0; j < debris_count; j++ ) {
        int debris_x = rand() % ((on_road_right - 6) + 1 - (on_road_left + 2)) + (on_road_left + 2);
        int debris_y = -2 ;
        if(i != j){
          while ( collision( debris[i], debris[j] ) ) {
            sprite_move_to(debris[i], debris_x, debris_y);
          }
        }
      }
    }
    // PART B
    //collision with car
    for ( int i = 0; i < zombie_count; i++ ) {
      if ( collision( race_car, zombie[i] ) ) {
        health = health - 20;
        speed = 0; // to account for left, right, top and bottom collision and the two layers of the roadblock
        sprite_move_to(race_car, (w-5)/2, screen_height() - 5);
        //Pushes zombie off the bottom of the screen, so it gets caught by the respawn function
        sprite_move_to(zombie[i], 0, h + 50);
        respawn_zombie(zombie[i]);
      }
    }
  }
  // collision between race car and debris - end

  // End of part 10

  // Part 11 - Function to check if the game is over
  void lose_screen(){
    clear_screen();
    setup_screen();

    draw_string(midx - 2, midy - 6, "GAME OVER");
    draw_string(midx - 2, midy - 4, "YOU LOSE");
    draw_string(midx - 4, midy - 0, "Time elapsed:");
    draw_double(midx - 4, midy + 2, elapsed_time);
    draw_string(midx + 1, midy + 2, "seconds");
    draw_string(midx - 9, midy + 10, "press 'y' to restart...");
    draw_string(midx - 9, midy + 12, "press 'n' to exit...");

    show_screen();
  }

  void win_screen(){
    clear_screen();
    setup_screen();

    draw_string(midx - 2, midy - 6, "GAME OVER");
    draw_string(midx - 2, midy - 4, "YOU WIN");
    draw_string(midx - 4, midy - 0, "Time elapsed:");
    draw_double(midx - 4, midy + 2, elapsed_time);
    draw_string(midx + 1, midy + 2, "seconds");
    draw_string(midx - 9, midy + 10, "press 'y' to restart...");
    draw_string(midx - 9, midy + 12, "press 'n' to exit...");

    show_screen();
  }

  void if_game_over() {
    // if lost
    if(health <= 0){
      game_over = true;
      lose_screen();
    }
    if(fuel_total < 1){
      game_over = true;
      lose_screen();
    }
    // if won
    if (collision(race_car, finish_line)){
      game_over = true;
      win_screen();
    }

    if ( game_over ) {
      char ans = wait_char();

      while(ans != 'y' && ans != 'n'){
        ans = wait_char();
      }

      if(ans ==  'y'){
        game_over = false;
        //call reset function here
        clear_screen();
        splash_screen();
        setup_game ( );
        speed = 0;
        distance_travelled = 0;
        fuel_total = 100;
        health = 100;
        zombie_count = 5;
        Time_before = get_current_time();
      }
    }
  }
  // end of part 11


  // Primary process of the game
  void process(void){
    clear_screen();

    int key = get_char();

    move_car( key );

    for (int i = 0; i < zombie_count; i++){
      move_zombie(zombie[i]);
      respawn_zombie(zombie[i]);
    }

    for (int i = 0; i < debris_count; i++){
      move_debris(debris[i]);
    }

    for (int i = 0; i < tree_count; i++){
      move_tree(tree[i]);
    }
    move_fuel_depot(fuel_depot);
    clear_screen();
    sprite_draw(race_car);
    increase_zombies();
    respawn_fuel_depot();
    respawn_debris();
    respawn_trees(false);
    if_collided();
    drawing_sprites();
    draw_border();
    clear_dashboard_area(); //clear area is called after the trees move and draw so that no overlap occurs with the dashboard and the trees.
    dashboard( );
    distance();
    fuel();
    refuel();
    if_game_over();
    Time_after = get_current_time();
    show_screen();
  }


  int main(void) {
    splash_screen();
    setup_game ( );
    Time_before = get_current_time();
    while (!game_over){
      process();
      timer_pause(10);
    }
    return 0;
  }
