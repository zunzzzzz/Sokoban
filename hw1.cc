#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
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
private:
};
int main(int argc, char** argv) {
    ifstream file(argv[1]);
    vector<State> state;
    // initial State
    State init_state;
    string tmp;
    while(getline(file, tmp)) {
        init_state.map.push_back(tmp);
    };
    for(int height_iter = 0; height_iter < init_state.map.size(); height_iter++) {
        for(int width_iter = 0; width_iter < init_state.map[height_iter].length(); width_iter++) {
            if(init_state.map[height_iter][width_iter] == 'o' || init_state.map[height_iter][width_iter] == 'O') {
                init_state.player.x = width_iter;
                init_state.player.y = height_iter;
            }
            cout << init_state.map[height_iter][width_iter];
        }
        cout << endl;
    }
    cout << init_state.player.x << " " << init_state.player.y << endl;
    state.push_back(init_state);
}