
#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <numeric> // needed for accumulate in compute_score

using namespace std;

void write_board_csv(const vector<vector<int>>& board, bool first) {
    ios_base::openmode mode = ios::app;
    if (first) mode = ios::trunc;
    ofstream fout("game_output.csv", mode);
    if (!fout) return;
    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            fout<<board[r][c];
            if (!(r==3 && c==3)) fout<<",";
        }
    }
    fout<<"\n";
}

void print_board(const vector<vector<int>>& board, bool first) {
    for (const auto &row : board) {
        for (auto val : row) {
            if (val == 0) cout << ".\t";
            else cout << val << "\t";
        }
        cout << "\n";
    }
    cout << endl;
    write_board_csv(board, first);
}

// TODO: use algorithms to spawn properly
void spawn_tile(vector<vector<int>>& board){
	vector<pair<int, int>> empty; 
	for(int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (board[i][j] == 0){ 
				empty.push_back(make_pair(i,j));
			}
		}
	}
	random_device rd; 
	mt19937 gen(rd()); 
	uniform_int_distribution<> distrib(0, empty.size() - 1); 
	int rand = distrib(gen); 
	uniform_int_distribution<> prob_distrib(1,10); 
	int val = (prob_distrib(gen) <= 9) ? 2 : 4; 
	board[empty[rand].first][empty[rand].second] = val; 
	return; 
}

//TODO: Compress a row, remove zeroes, and then pad with zeroes at the end
std::vector<int> compress_row(const std::vector<int>& row) {
	vector<int> compressed; 
	copy_if(row.begin(), row.end(), back_inserter(compressed), [](int x) {return x != 0;});

	while (compressed.size() < 4){
		compressed.push_back(0); 
	}
	return compressed; 
}
//TODO: Merge a row (assumes the row is already compressed)
std::vector<int> merge_row(std::vector<int> row){
	for(int i = 0; i < 3; i++){
		if(row[i] != 0 && (row[i] == row[i+1])){
			row[i] = row[i] * 2; 
			row[i+1] = 0; 
		}
	}
	return row; 
}

// TODO: implement using STL algorithms
bool move_left(vector<vector<int>>& board){
	bool changed = false; 
	for(auto& row : board){
		vector<int> old = row; 
		vector<int> ret = compress_row(old); 
		ret = merge_row(ret); 
		ret = compress_row(ret); 
		if(old != ret){
			changed = true; 
		}
		row = ret; 
	}
	return changed; 
}
bool move_right(vector<vector<int>>& board){		
	bool changed = false; 
	for(auto& row : board){
		vector<int> old = row; 
		vector<int> ret = old; 
		reverse(ret.begin(), ret.end());
		ret = compress_row(ret); 
		ret = merge_row(ret); 
		ret = compress_row(ret); 
		reverse(ret.begin(), ret.end()); 
		if(old != ret){
			changed = true; 
		}
		row = ret; 
	}
	return changed; 
}
bool move_up(vector<vector<int>>& board){
	bool changed = false; 
	for(int i = 0; i < 4; i++){
		vector<int> old(4);
		vector<int> ret(4); 
		for(int j = 0; j < 4; j++){
			old[j] = board[j][i];
		}
		ret = old; 
		ret = compress_row(ret); 
		ret = merge_row(ret); 
		ret = compress_row(ret); 
		if(old != ret){
			changed = true; 
		}
	       	for(int j = 0; j < 4; j++){
			board[j][i] = ret[j];
		}
	}
	return changed; 
}

bool move_down(vector<vector<int>>& board){
	bool changed = false; 
	for(int i = 0; i < 4; i++){
		vector<int> old(4);
		vector<int> ret(4); 
		for(int j = 0; j < 4; j++){
			old[j] = board[j][i];
		}
		ret = old;
	        reverse(ret.begin(), ret.end()); 	
		ret = compress_row(ret); 
		ret = merge_row(ret); 
		ret = compress_row(ret); 
		reverse(ret.begin(), ret.end()); 
		if(old != ret){
			changed = true; 
		}
	       	for(int j = 0; j < 4; j++){
			board[j][i] = ret[j];
		}
	}
	return changed; 
}

int compute_score(const vector<vector<int>>& board) {
    int sum = 0; 
    for (auto& row : board){
	    for(auto& value : row){
		    sum += value;
	    }
    }
    return sum; 
}

int main(){
    srand(time(nullptr));
    vector<vector<int>> board(4, vector<int>(4,0));
    spawn_tile(board);
    spawn_tile(board);

    stack<vector<vector<int>>> history;
    bool first=true;

    while(true){
        print_board(board, first);
        first=false;
        cout << "Score: " << compute_score(board) << "\n";
        cout<<"Move (w=up, a=left, s=down, d=right), u=undo, q=quit: ";
        char cmd;
        if (!(cin>>cmd)) break;
        if (cmd=='q') break;

        if (cmd=='u') {
		if(!history.empty()){
			board = history.top(); 
			history.pop(); 
			continue;
		}
        }

        vector<vector<int>> prev = board;
        bool moved=false;
        if (cmd=='a') moved=move_left(board);
        else if (cmd=='d') moved=move_right(board);
        else if (cmd=='w') moved=move_up(board);
        else if (cmd=='s') moved=move_down(board);

        if (moved) {
		history.push(prev);
		spawn_tile(board);
        }
    }
    return 0;
}
