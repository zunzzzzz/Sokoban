#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <omp.h>
#include <unordered_map>
#include <map>
#include <boost/functional/hash.hpp>
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
    bool is_deadlock = false;
    string output;
    bool CheckSolved() {
        int correct_count = 0;
        for(int iter_ = 0; iter_ < obstacle.size(); iter_++) {
            // right position -> "X"
            if(obstacle[iter_].corret_position) {
                correct_count++;
            }
        } 
        if(correct_count == obstacle.size()) {
            // ShowMap(current_state);
            cout << output << endl;
            // cout << ((double) (timer2 - timer1)) / CLOCKS_PER_SEC << endl;
            return true;
        }
        else {
            return false;
        }
    }
    State Move(int direction) {
        State new_state = *this;
        int x = player.x;
        int y = player.y;
        int one_step_x = x;
        int one_step_y = y;
        int two_step_x = x;
        int two_step_y = y;
        if(direction == UP) {
            one_step_y = y - 1;
            two_step_y = y - 2;
            new_state.output += "W";
        }
        else if(direction == LEFT) {
            one_step_x = x - 1;
            two_step_x = x - 2;
            new_state.output += "A";
        }
        else if(direction == DOWN) {
            one_step_y = y + 1;
            two_step_y = y + 2;
            new_state.output += "S";
        }
        else if(direction == RIGHT) {
            one_step_x = x + 1;
            two_step_x = x + 2;
            new_state.output += "D";
        }
        // cout << y << " " << x << endl;
        // cout << one_step_y << " " << one_step_x << endl;
        // cout << two_step_y << " " << two_step_x << endl;
        if(map[one_step_y][one_step_x] == ' ') {
            new_state.map[one_step_y][one_step_x] = 'o';
            new_state.is_legal = true;
        }
        else if(map[one_step_y][one_step_x] == '.') {
            new_state.map[one_step_y][one_step_x] = 'O';
            new_state.is_legal = true;
        }
        else if((map[one_step_y][one_step_x] == 'X' || map[one_step_y][one_step_x] == 'x') && (map[two_step_y][two_step_x] == ' ' || map[two_step_y][two_step_x] == '.')) {
            if(map[one_step_y][one_step_x] == 'x') {
                new_state.map[one_step_y][one_step_x] = 'o';
            }
            else {
                new_state.map[one_step_y][one_step_x] = 'O';
            }
            if(map[two_step_y][two_step_x] == ' ') {
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
            return new_state;
        }
        if(map[y][x] == 'o') {
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
    bool WallOrObstacle(int y, int x) {
        if(map[y][x] == 'x' || map[y][x] == '#' || map[y][x] == 'X') return true;
        else return false;
    }
    void CheckDeadlock(int direction) {
        vector<Obstacle>::iterator it;
        int x = player.x;
        int y = player.y;
        int one_step_x = x;
        int one_step_y = y;
        int two_step_x = x;
        int two_step_y = y;
        if(direction == UP) {
            one_step_y = y - 1;
            two_step_y = y - 2;
        }
        else if(direction == LEFT) {
            one_step_x = x - 1;
            two_step_x = x - 2;
        }
        else if(direction == DOWN) {
            one_step_y = y + 1;
            two_step_y = y + 2;
        }
        else if(direction == RIGHT) {
            one_step_x = x + 1;
            two_step_x = x + 2;
        }
        // on the corner
        for(it = obstacle.begin(); it < obstacle.end(); it++) {
            int x = it->x;
            int y = it->y;
            // cout << y << " " << x << endl;
            
            if(map[y][x] == 'x') {
                //upright
                if(WallOrObstacle(y-1, x) && WallOrObstacle(y-1, x+1) && WallOrObstacle(y, x+1)) {
                    is_deadlock = true;
                    break;
                }
                //downright
                if(WallOrObstacle(y+1, x) && WallOrObstacle(y+1, x+1) && WallOrObstacle(y, x+1)) {
                    is_deadlock = true;
                    break;
                }
                //downleft
                if(WallOrObstacle(y, x-1) && WallOrObstacle(y+1, x) && WallOrObstacle(y+1, x-1)) {
                    is_deadlock = true;
                    break;
                }
                //upleft
                if(WallOrObstacle(y-1, x) && WallOrObstacle(y-1, x-1) && WallOrObstacle(y, x-1)) {
                    is_deadlock = true;
                    break;
                }
            }
        }
        // on the wall without answer
        int count_wall = 0;
        int iter_;
        bool have_answer;
        // cout << player.y << " " << player.x << endl;
        // cout << one_step_y << " " << one_step_x << endl;
        // cout << two_step_y << " " << two_step_x << endl;
        if(map[one_step_y][one_step_x] == 'x' && map[two_step_y][two_step_x] == '#') {
            if(direction == UP || direction == DOWN) {
                iter_ = 1;
                have_answer = false;
                while(!have_answer) {
                    if(map[one_step_y][one_step_x-iter_] == '.') {
                        have_answer = true;
                    }
                    else if(map[one_step_y][one_step_x-iter_] == '#') {
                        count_wall += 1;
                        break;
                    }
                    else {
                        if(map[two_step_y][two_step_x-iter_] == '#') {
                            iter_++;
                        }
                        else break;
                    }
                }
                iter_ = 1;
                while(!have_answer) {
                    if(map[one_step_y][one_step_x+iter_] == '.') {
                        have_answer = true;
                    }
                    else if(map[one_step_y][one_step_x+iter_] == '#') {
                        count_wall += 1;
                        break;
                    }
                    else {
                        if(map[two_step_y][two_step_x+iter_] == '#') {
                            iter_++;
                        }
                        else break;
                    }
                }
                if(count_wall == 2) is_deadlock = true;
            }
            else if(direction == RIGHT || direction == LEFT) {
                iter_ = 1;
                have_answer = false;
                while(!have_answer) {
                    if(map[one_step_y-iter_][one_step_x] == '.') {
                        have_answer = true;
                    }
                    else if(map[one_step_y-iter_][one_step_x] == '#') {
                        count_wall += 1;
                        break;
                    }
                    else {
                        if(map[two_step_y-iter_][two_step_x] == '#') {
                            iter_++;
                        }
                        else break;
                    }
                }
                iter_ = 1;
                while(!have_answer) {
                    if(map[one_step_y+iter_][one_step_x] == '.') {
                        have_answer = true;
                    }
                    else if(map[one_step_y+iter_][one_step_x] == '#') {
                        count_wall += 1;
                        break;
                    }
                    else {
                        if(map[two_step_y+iter_][two_step_x] == '#') {
                            iter_++;
                        }
                        else break;
                    }
                }
                if(count_wall == 2) is_deadlock = true;
            }
        }
    }
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

State ReadInput(char** argv) {
    // initial State
    State init_state;
    string tmp;
    ifstream file(argv[1]);
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
            // cout << init_state.map[height_iter][width_iter];
        }
        // cout << endl;
    }
    return init_state;
}
int main(int argc, char** argv) {
    vector<State> all_state;
    queue<State> todo_queue;
    unordered_map<vector<string>, string, boost::hash<vector<string>>> visited;
    State init_state;
    bool is_solve = false;

    init_state = ReadInput(argv);
    
    // store init state
    todo_queue.push(init_state);
    visited.insert(pair<vector<string>, string>(init_state.map, init_state.output));
    // cout << init_state.player.y << " " << init_state.player.x << endl;
    while(!is_solve) {
        // take and pop the first element
        State current_state = todo_queue.front();
        todo_queue.pop();
        // ShowMap(current_state);

        // check whether question is solved
        is_solve = current_state.CheckSolved();
        // cout << current_state.player.y << " " << current_state.player.x << endl;
        #pragma omp parallel for
        for(int dir_iter = 0; dir_iter < 4; dir_iter++) {
            State new_state;
            new_state = current_state.Move(dir_iter);
            if(new_state.is_legal) {
                new_state.CheckDeadlock(dir_iter);
            }
            if(new_state.is_legal && !new_state.is_deadlock) {
                unordered_map<vector<string>, string, boost::hash<vector<string>>>::iterator it = visited.find(new_state.map);
                #pragma omp critical
                if(it == visited.end()) {
                    todo_queue.push(new_state);
                    visited.insert(pair<vector<string>, string>(new_state.map, new_state.output));
                }
            }
        }
    }
    return 0;
}