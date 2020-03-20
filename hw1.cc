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
    // initial State
    State state;
    string tmp;
    while(getline(file, tmp)) {
        state.map.push_back(tmp);
    };
    for(int height_iter = 0; height_iter < state.map.size(); height_iter++) {
        for(int width_iter = 0; width_iter < state.map[height_iter].length(); width_iter++) {
            cout << state.map[height_iter][width_iter];
        }
        cout << endl;
    }
}