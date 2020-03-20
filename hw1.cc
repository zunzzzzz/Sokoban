#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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
State Move(int direction, State current_state) {
    State new_state;
    int x = current_state.player.x;
    int y = current_state.player.y;
    int one_step_x;
    int one_step_y;
    int two_step_x;
    int two_step_y;
    // cout << current_state.map[y][x] << endl;
    if(direction == UP) {

    }
    else if(direction == LEFT) {

    }
    else if(direction == DOWN) {

    }
    else if(direction == RIGHT) {

    }
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
    cout << init_state.player.x << " " << init_state.player.y << endl;
    cout << init_state.obstacle.size() << endl;
    all_state.push_back(init_state);
    vector<State> todo_queue(all_state);

    while(true) {
        // take and pop the last element
        State current_state = todo_queue.back();
        todo_queue.pop_back();
        // check whether question is solved
        int correct_count = 0;
        for(int iter_ = 0; iter_ < current_state.obstacle.size(); iter_++) {
            // right position -> "X"
            if(current_state.obstacle[iter_].corret_position) {
                correct_count++;
            }
        } 
        if(correct_count == current_state.obstacle.size()) {
            break;
        }
        for(int dir_iter = 0; dir_iter < 4; dir_iter++) {
            State new_state;
            new_state = Move(dir_iter, current_state);
            if(new_state.is_legal) {
                todo_queue.push_back(new_state);
                all_state.push_back(new_state);
            }
        }
    }
    return 0;
}