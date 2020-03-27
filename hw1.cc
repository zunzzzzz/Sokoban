#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <omp.h>
#include <unordered_map>
#include <map>
#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_unordered_map.h"
#include <boost/functional/hash.hpp>


#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3

using namespace std;
class Goal {
public:
    int x;
    int y;
};
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
    vector<Goal> goal;
    bool is_legal = true;
    bool is_deadlock = false;
    string output;
    bool CheckSolved() {
        int correct_count = 0;
        for(vector<Obstacle>::iterator it = obstacle.begin(); it < obstacle.end(); it++) {
            // right position -> "X"
            if((*it).corret_position) {
                correct_count++;
            }
        } 
        if(correct_count == obstacle.size()) {
            cout << output << endl;
            exit(0);
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
    void ShowMap() {
        // cout << "Player position : " << player.y << " " << player.x << endl;
        for(vector<string>::iterator it = map.begin(); it < map.end(); it++) {
            for(int index = 0; index < (*it).length(); index++) {
                cout << (*it)[index];
            }
            cout << endl;
        }
        // cout << "Obstacle position : " << endl;
        // for(vector<Obstacle>::iterator it = obstacle.begin(); it < obstacle.end(); it++) {
            
        //     cout << (*it).y << " " << (*it).x << " " << (*it).corret_position << endl;
        // }
        // cout << "Goal position : " << endl;
        // for(vector<Goal>::iterator it = goal.begin(); it < goal.end(); it++) {
        //     cout << (*it).y << " " << (*it).x << " " << endl;
        // }
    }
    bool BlockHorizontal(vector<string> map_deadlock, vector<string> tmp_map, int x, int y) {
        // cout << "H " << y << " " << x << endl;
        // ShowMap();
        tmp_map[y][x] = '#';
        if(tmp_map[y][x-1] == '#' || tmp_map[y][x+1] == '#') return true;
        else if(map_deadlock[y][x-1] == '-' && map_deadlock[y][x+1] == '-') return true;
        else if(tmp_map[y][x-1] == 'x' || tmp_map[y][x-1] == 'X') {
            if(BlockVertical(map_deadlock, tmp_map, x-1, y)) return true;
            return false;
        }
        else if(tmp_map[y][x+1] == 'x' || tmp_map[y][x+1] == 'X') {
            if(BlockVertical(map_deadlock, tmp_map, x+1, y)) return true;
            return false;
        }
        else return false;
    }
    bool BlockVertical(vector<string> map_deadlock, vector<string> tmp_map, int x, int y) {
        // cout << "V " << y << " " << x << endl;
        // ShowMap();
        tmp_map[y][x] = '#';
        if(tmp_map[y-1][x] == '#' || tmp_map[y+1][x] == '#') return true;
        else if(map_deadlock[y-1][x] == '-' && map_deadlock[y+1][x] == '-') return true;
        else if(tmp_map[y+1][x] == 'x' || tmp_map[y+1][x] == 'X') {
            if(BlockHorizontal(map_deadlock, tmp_map, x, y+1)) return true;
            return false;
        }
        else if(tmp_map[y-1][x] == 'x' || map[y-1][x] == 'X') {
            if(BlockHorizontal(map_deadlock, tmp_map, x, y-1)) return true;
            return false;
        }
        else return false;
    }
    void CheckDeadlock(int direction, vector<string> map_deadlock) {

        int x = player.x;
        int y = player.y;
        if(direction == UP) y -= 1;
        if(direction == DOWN) y += 1;
        if(direction == LEFT) x -= 1;
        if(direction == RIGHT) x += 1;
        // cout << y << " " << x << endl;
        if(map[y][x] == 'x') {
            // type 1 deadlock
            if(map_deadlock[y][x] == '-') {
                is_deadlock = true;
                return;
            }
            // type 2 deadlock
            if(BlockVertical(map_deadlock, map, x, y) && BlockHorizontal(map_deadlock, map, x, y)) {
                is_deadlock = true;
                return;
            }
            //upright
            if(WallOrObstacle(y-1, x) && WallOrObstacle(y-1, x+1) && WallOrObstacle(y, x+1)) {
                is_deadlock = true;
                return;
            }
            //downright
            if(WallOrObstacle(y+1, x) && WallOrObstacle(y+1, x+1) && WallOrObstacle(y, x+1)) {
                is_deadlock = true;
                return;
            }
            //downleft
            if(WallOrObstacle(y, x-1) && WallOrObstacle(y+1, x) && WallOrObstacle(y+1, x-1)) {
                is_deadlock = true;
                return;
            }
            //upleft
            if(WallOrObstacle(y-1, x) && WallOrObstacle(y-1, x-1) && WallOrObstacle(y, x-1)) {
                is_deadlock = true;
                return;
            }
        }
    }
};
class MapDeadlock {
public:
    int x;
    int y;
    vector<string> map;
};
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
            // find goal
            if(init_state.map[height_iter][width_iter] == 'X' || init_state.map[height_iter][width_iter] == 'O' || init_state.map[height_iter][width_iter] == '.') {
                Goal tmp_goal;
                tmp_goal.x = width_iter;
                tmp_goal.y = height_iter;
                init_state.goal.push_back(tmp_goal);
            }
            // cout << init_state.map[height_iter][width_iter];
        }
        // cout << endl;
    }
    return init_state;
}
void Show(vector<string> map) {
    cout << endl;
    for(vector<string>::iterator it = map.begin(); it < map.end(); it++) {
        for(int index = 0; index < (*it).length(); index++) {
            cout << (*it)[index];
        }
        cout << endl;
    }
}
vector<string> FindDeadlockMap(vector<string> map_deadlock, vector<Goal> goal) {
    // remove obstacle and player, goal left
    for(vector<string>::iterator it = map_deadlock.begin(); it < map_deadlock.end(); it++) {
        for(int index = 0; index < (*it).length(); index++) {
            if((*it)[index] == 'O' || (*it)[index] == 'X') {
                (*it)[index] = '.';
            }
            else if((*it)[index] == '#' || (*it)[index] == '.') {
                
            }
            else {
                (*it)[index] = '-';
            }
        }
    }
    for(vector<Goal>::iterator it = goal.begin(); it < goal.end(); it++) {
        int x = (*it).x;
        int y = (*it).y;
        if(map_deadlock[y][x] != '*') {
            map_deadlock[y][x] = '*';


            MapDeadlock init_map_deadlock;
            init_map_deadlock.x = x;
            init_map_deadlock.y = y;
            init_map_deadlock.map = vector<string>(map_deadlock);
            tbb::concurrent_queue<MapDeadlock> todo_map;
            tbb::concurrent_unordered_map<vector<string>, string, boost::hash<vector<string>>> visited_map;


            todo_map.push(init_map_deadlock);
            visited_map.insert(pair<vector<string>, string>(map_deadlock, ""));
            while (!todo_map.empty())
            {
                MapDeadlock current_map = *(todo_map.unsafe_begin());
                todo_map.try_pop(current_map);
                x = current_map.x;
                y = current_map.y;
                

                for(int dir_iter = 0; dir_iter < 4; dir_iter++) {
                    MapDeadlock new_map_deadlock;
                    new_map_deadlock = current_map;
                    int one_step_x = x;
                    int two_step_x = x;
                    int one_step_y = y;
                    int two_step_y = y;
                    if(dir_iter == UP) {
                        one_step_y = y - 1;
                        two_step_y = y - 2;
                    }
                    else if(dir_iter == DOWN) {
                        one_step_y = y + 1;
                        two_step_y = y + 2;
                    }
                    else if(dir_iter == RIGHT) {
                        one_step_x = x + 1;
                        two_step_x = x + 2;
                    }
                    else if(dir_iter == LEFT) {
                        one_step_x = x - 1;
                        two_step_x = x - 2;
                    }
                    bool can_move = false;
                    if(map_deadlock[one_step_y][one_step_x] == '#' || map_deadlock[two_step_y][two_step_x] == '#') {
                        can_move = false;
                    }
                    else {
                        map_deadlock[one_step_y][one_step_x] = '*';
                        new_map_deadlock.map = vector<string>(map_deadlock);
                        new_map_deadlock.x = one_step_x;
                        new_map_deadlock.y = one_step_y;
                        can_move = true;
                    }
                    if(can_move) {
                        tbb::concurrent_unordered_map<vector<string>, string, boost::hash<vector<string>>>::iterator it = visited_map.find(new_map_deadlock.map);
                        if(it == visited_map.end()) {
                            visited_map.insert(pair<vector<string>, string>(map_deadlock, ""));
                            todo_map.push(new_map_deadlock);
                        }
                    }
                }
            }
        }
        
    }
    return map_deadlock;
}
int main(int argc, char** argv) {
    vector <State> all_state;
    tbb::concurrent_queue<State> todo_queue;
    tbb::concurrent_unordered_map<vector<string>, string, boost::hash<vector<string>>> visited;
    State init_state;
    bool is_solve = false;

    init_state = ReadInput(argv);
    vector<string> map_deadlock(init_state.map);

    // find simple deadlock map
    map_deadlock = FindDeadlockMap(map_deadlock, init_state.goal);
    // Show(map_deadlock);
    // store init state
    todo_queue.push(init_state);
    visited.insert(pair<vector<string>, string>(init_state.map, init_state.output));
    while(!is_solve) {
        // take and pop the first element
        // State current_state = todo_queue.front();
        // todo_queue.pop();
        State current_state = *(todo_queue.unsafe_begin());
        todo_queue.try_pop(current_state);
        // ShowMap(current_state);

        // check whether question is solved
        is_solve = current_state.CheckSolved();
        // #pragma omp parallel for
        for(int dir_iter = 0; dir_iter < 4; dir_iter++) {
            State new_state;
            new_state = current_state.Move(dir_iter);
            if(new_state.is_legal) {
                new_state.CheckDeadlock(dir_iter, map_deadlock);
            }
            if(new_state.is_legal && !new_state.is_deadlock) {
                tbb::concurrent_unordered_map<vector<string>, string, boost::hash<vector<string>>>::iterator it = visited.find(new_state.map);
                // #pragma omp critical
                if(it == visited.end()) {
                    todo_queue.push(new_state);
                    visited.insert(pair<vector<string>, string>(new_state.map, new_state.output));
                }
            }
        }
    }
    return 0;
}