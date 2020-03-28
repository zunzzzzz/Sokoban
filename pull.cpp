#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <omp.h>
#include <unordered_map>
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
    vector<Goal> goal;
    bool is_legal = true;
    string output;
    void ShowOutput() {
        for(int index = output.length() - 1; index >= 0; index--) {
            if(output[index] == 'W') cout << 'S';
            if(output[index] == 'S') cout << 'W';
            if(output[index] == 'A') cout << 'D';
            if(output[index] == 'D') cout << 'A';
        }
        cout << endl;
        exit(0);
    }
    State Move(int direction, bool can_pull) {
        State new_state = *this;
        int x = player.x;
        int y = player.y;
        int one_step_x = x;
        int one_step_y = y;
        int opposite_x = x;
        int opposite_y = y;
        if(direction == UP) {
            one_step_y = y - 1;
            opposite_y = y + 1;
            new_state.output += "W";
        }
        else if(direction == LEFT) {
            one_step_x = x - 1;
            opposite_x = x + 1;
            new_state.output += "A";
        }
        else if(direction == DOWN) {
            one_step_y = y + 1;
            opposite_y = y - 1;
            new_state.output += "S";
        }
        else if(direction == RIGHT) {
            one_step_x = x + 1;
            opposite_x = x - 1;
            new_state.output += "D";
        }
        // try to move
        if(map[y][x] == 'o') {
            if(map[one_step_y][one_step_x] == '#' || map[one_step_y][one_step_x] == 'X' || map[one_step_y][one_step_x] == 'x') new_state.is_legal = false;
            else if(map[one_step_y][one_step_x] == '.') {
                if((map[opposite_y][opposite_x] == 'x' || map[opposite_y][opposite_x] == 'X') && can_pull) {
                    if(map[opposite_y][opposite_x] == 'x') new_state.map[opposite_y][opposite_x] = ' ';
                    else if(map[opposite_y][opposite_x] == 'X') new_state.map[opposite_y][opposite_x] = '.';
                    new_state.map[y][x] = 'x';
                }
                else new_state.map[y][x] = ' ';
                new_state.is_legal = true;
                new_state.map[one_step_y][one_step_x] = 'O';
            }
            else if(map[one_step_y][one_step_x] == ' ') {
                if((map[opposite_y][opposite_x] == 'x' || map[opposite_y][opposite_x] == 'X') && can_pull) {
                    if(map[opposite_y][opposite_x] == 'x') new_state.map[opposite_y][opposite_x] = ' ';
                    else if(map[opposite_y][opposite_x] == 'X') new_state.map[opposite_y][opposite_x] = '.';
                    new_state.map[y][x] = 'x';
                }
                else new_state.map[y][x] = ' ';
                new_state.is_legal = true;
                new_state.map[one_step_y][one_step_x] = 'o';
            }
        }
        else if(map[y][x] == 'O') {
            if(map[one_step_y][one_step_x] == '#' || map[one_step_y][one_step_x] == 'X' || map[one_step_y][one_step_x] == 'x') new_state.is_legal = false;
            else if(map[one_step_y][one_step_x] == '.') {
                if((map[opposite_y][opposite_x] == 'x' || map[opposite_y][opposite_x] == 'X') && can_pull) {
                    if(map[opposite_y][opposite_x] == 'x') new_state.map[opposite_y][opposite_x] = ' ';
                    else if(map[opposite_y][opposite_x] == 'X') new_state.map[opposite_y][opposite_x] = '.';
                    new_state.map[y][x] = 'X';
                }
                else new_state.map[y][x] = '.';
                new_state.is_legal = true;
                new_state.map[one_step_y][one_step_x] = 'O';
            }
            else if(map[one_step_y][one_step_x] == ' ') {
                if((map[opposite_y][opposite_x] == 'x' || map[opposite_y][opposite_x] == 'X') && can_pull) {
                    if(map[opposite_y][opposite_x] == 'x') new_state.map[opposite_y][opposite_x] = ' ';
                    else if(map[opposite_y][opposite_x] == 'X') new_state.map[opposite_y][opposite_x] = '.';
                    new_state.map[y][x] = 'X';
                }
                else new_state.map[y][x] = '.';
                new_state.is_legal = true;
                new_state.map[one_step_y][one_step_x] = 'o';
            }
        }
        // update player position
        new_state.player.x = one_step_x;
        new_state.player.y = one_step_y;
        return new_state;
    }
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
            // find goal
            if(init_state.map[height_iter][width_iter] == 'X' || init_state.map[height_iter][width_iter] == 'O' || init_state.map[height_iter][width_iter] == '.') {
                Goal tmp_goal;
                tmp_goal.x = width_iter;
                tmp_goal.y = height_iter;
                init_state.goal.push_back(tmp_goal);
            }
        }
    }
    return init_state;
}
int main(int argc, char** argv) {
    vector<Player> player_possible;
    tbb::concurrent_unordered_map<vector<string>, string, boost::hash<vector<string>>> visited;
    State init_state;

    init_state = ReadInput(argv);
    vector<string> map_goal(init_state.map);
    
    // put answer to goal
    for(vector<string>::iterator it = map_goal.begin(); it < map_goal.end(); it++) {
        for(int index = 0; index < (*it).length(); index++) {
            if((*it)[index] == 'O' || (*it)[index] == 'X' || (*it)[index] == '.') (*it)[index] = 'X';
            else if((*it)[index] == '#') ;
            else (*it)[index] = ' ';
        }
    }
    // list all possible player position
    vector<string> tmp_map_goal(map_goal);
    for(int height_iter = 0; height_iter < tmp_map_goal.size(); height_iter++) {
        for(int width_iter = 0; width_iter < tmp_map_goal[height_iter].length(); width_iter++) {
            if(tmp_map_goal[height_iter][width_iter] == 'X') {
                Player tmp_player;
                if(tmp_map_goal[height_iter-1][width_iter] == ' ') {
                    tmp_player.y = height_iter - 1;
                    tmp_player.x = width_iter;
                    player_possible.push_back(tmp_player);
                    tmp_map_goal[height_iter-1][width_iter] = '*';
                }
                if(tmp_map_goal[height_iter+1][width_iter] == ' ') {
                    tmp_player.y = height_iter + 1;
                    tmp_player.x = width_iter;
                    player_possible.push_back(tmp_player);
                    tmp_map_goal[height_iter+1][width_iter] = '*';
                }
                if(tmp_map_goal[height_iter][width_iter-1] == ' ') {
                    tmp_player.y = height_iter;
                    tmp_player.x = width_iter - 1;
                    player_possible.push_back(tmp_player);
                    tmp_map_goal[height_iter][width_iter-1] = '*';
                }
                if(tmp_map_goal[height_iter][width_iter+1] == ' ') {
                    tmp_player.y = height_iter;
                    tmp_player.x = width_iter + 1;
                    player_possible.push_back(tmp_player);
                    tmp_map_goal[height_iter][width_iter+1] = '*';
                }
            }
        }
    }
    // store init state
    visited.insert(pair<vector<string>, string>(init_state.map, ""));
    // cout << player_possible.size() << endl;
    for(vector<Player>::iterator it = player_possible.begin(); it < player_possible.end(); it++) {
        tbb::concurrent_queue<State> todo_queue;
        // store first state
        map_goal[(*it).y][(*it).x] = 'o';
        State first_state;
        first_state.map = map_goal;
        first_state.player = *it;
        todo_queue.push(first_state);

        while(!todo_queue.empty()) {
            State current_state = *(todo_queue.unsafe_begin());
            todo_queue.try_pop(current_state);
            // four direction WASD
            #pragma omp parallel for
            for(int dir_iter = 0; dir_iter < 4; dir_iter++) {
                // pull or not pull
                for(int i = 0; i < 2; i++) {
                    State new_state;
                    if(i == 0) new_state = current_state.Move(dir_iter, true);
                    else if(i == 1) new_state = current_state.Move(dir_iter, false);
                    if(new_state.is_legal) {
                        tbb::concurrent_unordered_map<vector<string>, string, boost::hash<vector<string>>>::iterator iter;
                        iter = visited.find(new_state.map);
                        if(iter == visited.end()) {
                            todo_queue.push(new_state);
                            visited.insert(pair<vector<string>, string>(new_state.map, new_state.output));
                        }
                        else {
                            if(iter-> second == "") new_state.ShowOutput();
                        }
                    }
                }
            }
        }
        map_goal[(*it).y][(*it).x] = ' ';
    }
    return 0;
}