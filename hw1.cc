#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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
private:
};
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
    cout << &todo_queue << endl;
    cout << &all_state << endl;
}