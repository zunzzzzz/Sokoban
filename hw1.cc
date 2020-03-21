#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>

#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3

using namespace std;
class Obstacle {
public:
    int x;
    int y;
    bool corret_position;
private:
};
class Player {
public:
    int x;
    int y;
private:
};
class State {
public:
    Player player;
    vector<string> map;
    vector<Obstacle> obstacle;
    bool is_legal = true;
private:
};
void ShowMap(State state) {
    cout << "Player position : " << state.player.y << " " << state.player.x << endl;
    for(int height_iter = 0; height_iter < state.map.size(); height_iter++) {
        for(int width_iter = 0; width_iter < state.map[height_iter].length(); width_iter++) {
            cout << state.map[height_iter][width_iter];
        }
        cout << endl;
    }
    cout << "Obstacle position : " << endl;
    for(int iter_ = 0; iter_ < state.obstacle.size(); iter_++) {
        cout << state.obstacle[iter_].y << " " << state.obstacle[iter_].x << " " << state.obstacle[iter_].corret_position << endl;
    }
}
State Move(int direction, State current_state) {
    State new_state = current_state;
    int x = current_state.player.x;
    int y = current_state.player.y;
    int one_step_x;
    int one_step_y;
    int two_step_x;
    int two_step_y;
    // cout << current_state.map[y][x] << endl;
    if(direction == UP) {
        one_step_x = x;
        two_step_x = x;
        one_step_y = y - 1;
        two_step_y = y - 2;
    }
    else if(direction == LEFT) {
        one_step_x = x - 1;
        two_step_x = x - 2;
        one_step_y = y;
        two_step_y = y;
    }
    else if(direction == DOWN) {
        one_step_x = x;
        two_step_x = x;
        one_step_y = y + 1;
        two_step_y = y + 2;
    }
    else if(direction == RIGHT) {
        one_step_x = x + 1;
        two_step_x = x + 2;
        one_step_y = y;
        two_step_y = y;
    }
    if(current_state.map[one_step_y][one_step_x] == ' ') {
        new_state.map[one_step_y][one_step_x] = 'o';
        new_state.is_legal = true;
    }
    else if(current_state.map[one_step_y][one_step_x] == '.') {
        new_state.map[one_step_y][one_step_x] = 'O';
        new_state.is_legal = true;
    }
    else if((current_state.map[one_step_y][one_step_x] == 'X' || current_state.map[one_step_y][one_step_x] == 'x') && (current_state.map[two_step_y][two_step_x] == ' ' || current_state.map[two_step_y][two_step_x] == '.')) {
        if(current_state.map[one_step_y][one_step_x] == 'x') {
            new_state.map[one_step_y][one_step_x] = 'o';
        }
        else {
            new_state.map[one_step_y][one_step_x] = 'O';
        }
        if(current_state.map[two_step_y][two_step_x] == ' ') {
            new_state.map[two_step_y][two_step_x] = 'x';
            // update obstacle position
            for(int iter_ = 0; iter_ < new_state.obstacle.size(); iter_++) {
                if(one_step_y == new_state.obstacle[iter_].y && one_step_x == new_state.obstacle[iter_].x) {
                    new_state.obstacle[iter_].x = two_step_x;
                    new_state.obstacle[iter_].y = two_step_y;
                    new_state.obstacle[iter_].corret_position = false;
                    break;
                }
            }
        }
        else {
            new_state.map[two_step_y][two_step_x] = 'X';
            // update obstacle position
            for(int iter_ = 0; iter_ < new_state.obstacle.size(); iter_++) {
                if(one_step_y == new_state.obstacle[iter_].y && one_step_x == new_state.obstacle[iter_].x) {
                    new_state.obstacle[iter_].x = two_step_x;
                    new_state.obstacle[iter_].y = two_step_y;
                    new_state.obstacle[iter_].corret_position = true;
                    break;
                }
            }
        }
        new_state.is_legal = true;
    }
    else {
        new_state.is_legal = false;
    }

    if(current_state.map[y][x] == 'o') {
        new_state.map[y][x] = ' ';
    }
    else {
        new_state.map[y][x] = '.';
    }
    // update player position
    new_state.player.x = one_step_x;
    new_state.player.y = one_step_y;
    // ShowMap(new_state);
    return new_state;
}
int main(int argc, char** argv) {
    ifstream file(argv[1]);
    vector<State> all_state;
    // initial State
    State init_state;
    string tmp;
    while(getline(file, tmp)) {
        init_state.map.push_back(tmp);
    };
    for(int height_iter = 0; height_iter < init_state.map.size(); height_iter++) {
        for(int width_iter = 0; width_iter < init_state.map[height_iter].length(); width_iter++) {
            // find player
            if(init_state.map[height_iter][width_iter] == 'o' || init_state.map[height_iter][width_iter] == 'O') {
                init_state.player.x = width_iter;
                init_state.player.y = height_iter;
            }
            // find obstacle
            if(init_state.map[height_iter][width_iter] == 'x' || init_state.map[height_iter][width_iter] == 'X') {
                Obstacle tmp_obstacle;
                tmp_obstacle.x = width_iter;
                tmp_obstacle.y = height_iter;
                if(init_state.map[height_iter][width_iter] == 'X') {
                    tmp_obstacle.corret_position = true;
                }
                else if(init_state.map[height_iter][width_iter] == 'x') {
                    tmp_obstacle.corret_position = false;
                }
                init_state.obstacle.push_back(tmp_obstacle);
            }
            cout << init_state.map[height_iter][width_iter];
        }
        cout << endl;
    }
    all_state.push_back(init_state);
    vector<State> todo_queue(all_state);
    clock_t timer1 = clock();
    while(true) {
        // cout << "FIND " << all_state.size() << endl;
        // take and pop the last element
        State current_state = todo_queue.back();
        todo_queue.pop_back();
        // ShowMap(current_state);
        // check whether question is solved
        int correct_count = 0;
        for(int iter_ = 0; iter_ < current_state.obstacle.size(); iter_++) {
            // right position -> "X"
            if(current_state.obstacle[iter_].corret_position) {
                correct_count++;
            }
        } 
        if(correct_count == current_state.obstacle.size()) {
            ShowMap(current_state);
            clock_t timer2 = clock();
            cout << ((double) (timer2 - timer1)) / CLOCKS_PER_SEC << endl;
            break;
        }
        for(int dir_iter = 0; dir_iter < 4; dir_iter++) {
            State new_state;
            new_state = Move(dir_iter, current_state);
            // cout << "X" << endl;
            // new_state = Move(RIGHT, current_state);
            if(new_state.is_legal) {
                bool is_new_state = true;
                for(int state_iter = 0; state_iter < all_state.size(); state_iter++) {
                    if(all_state[state_iter].player.x == new_state.player.x && all_state[state_iter].player.y == new_state.player.y) {
                        //same player position
                    }
                    else {
                        continue;
                    }
                    int same_count = 0;
                    for(int newstate_obstacle_iter = 0; newstate_obstacle_iter < new_state.obstacle.size(); newstate_obstacle_iter++) {
                        for(int obstacle_iter = 0; obstacle_iter < all_state[state_iter].obstacle.size(); obstacle_iter++) {
                            if(all_state[state_iter].obstacle[obstacle_iter].x == new_state.obstacle[newstate_obstacle_iter].x && all_state[state_iter].obstacle[obstacle_iter].y == new_state.obstacle[newstate_obstacle_iter].y) {
                                same_count++;
                                break;
                            }
                        }
                    }
                    if(same_count == new_state.obstacle.size()) {
                        is_new_state = false;
                        
                        break;
                    }
                }
                if(is_new_state) {
                    todo_queue.push_back(new_state);
                    all_state.push_back(new_state);
                }
            }
        }
        // cout << todo_queue.size() << endl;
    }
    return 0;
}