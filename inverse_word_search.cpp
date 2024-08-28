#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <list>
#include <cstring>

using std::cerr; using std::endl; using std::ifstream;
using std::ofstream; using std::string; using std::cout;
using std::vector; using std::sort; using std::list;

/* generates a new board each time a word is inserted
otherwise, the recursion will continue to work off of the old
board and no new solutions will be generated */
vector<vector<char> > new_board(vector<vector<char> >& board, string word,
                        unsigned int x, unsigned int y, const vector<int> direction){
    vector<vector<char> > after_ins = board;
    for(unsigned int i = 0; i < word.size(); i++){
        after_ins[y][x] = word[i];
        x += direction[0];
        y += direction[1];
    }
    return after_ins;
}

/* checks if there is enough space for insertion. focuses solely
 on whether or not there is enough space by looking for asteriks.
 there is a separate function to see if the word can
 be inserted along another word
 */
bool valid_insertion(const vector<vector<char> >& board, string word,
                     unsigned int x, unsigned int y, const vector<int>& direction){
    for(unsigned int i = 0; i < word.size(); i++){
        if(x < 0 || y < 0 || x >= board[0].size() || y >= board.size()){
            return false;
        }

        if(board[y][x] != '*'){
            return false;
        }

        x += direction[0];
        y += direction[1];
    }
    return true;
}

/* checks if there is enough space for insertion along another word */
bool insert_along(const vector<vector<char> >& board, string word,
                  unsigned int x, unsigned int y, const vector<int>& direction){
    for(unsigned int i = 0; i < word.size(); i++){
        if(x < 0 || y < 0 || x >= board[0].size() || y >= board.size()){
            return false;
        }
        if(board[y][x] != '*' && board[y][x] != word[i]){ return false; }
        x += direction[0];
        y += direction[1];
    }
    return true;
}

/* helper function for no_excludes.
 return true if the word is along the path if it has made it
 through the entire for loop  and return false the moment it
 encounters a letter that does not coincide with the word it is
 iterating along */
bool word_on_path(const vector<vector<char> >& board, string word,
                  unsigned int x, unsigned int y, const vector<int>& direction){
    for(unsigned int i = 0; i < word.size(); i++){
        if(x < 0 || y < 0 || x >= board[0].size() || y >= board.size())
            { return false; }
        
        if(board[y][x] != word[i]){ return false; }
        x += direction[0];
        y += direction[1];
    }
    return true;
}

/* function that checks if a word that is not supposed to be included has
 been inserted. called after the word has been inserted and the original
 board is allowed to continue recursing or be pushed into the solutions
 vector if there are no invalid words.
 iterates through the list of exclude words, then iterates through
 the board each time to find the first letter of the  word being
 checked and then checks in all eight directions to see if the
 word is present */
bool no_excludes(const vector<vector<char> >& board,
                 const vector<string>& exclude_words,
                 const vector<vector<int> >& all_directions){
    for(unsigned int i = 0; i < exclude_words.size(); i++){
        for(unsigned int row = 0; row < board.size(); row++){
            for(unsigned int col = 0; col < board[row].size(); col++){
                if(board[row][col] == exclude_words[i][0]){
                    for(unsigned int dir = 0; dir < all_directions.size(); dir++){
                        if(word_on_path(board, exclude_words[i], col,
                                        row, all_directions[dir])){
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}


/* RECURSIVE FUNCTION
 base case: if it has reached the end of the vector of words to be included
 otherwise, it searches for empty spaces, checks if there is enoguh
 space to insert the current word along any direction
 also searches for the first letter of the current word, checks
 if it can continue off of that square in any direction
 
 always iterates through all possible insertions at each position
 */
void insert_word(list<vector<vector<char> > >& all_solutions, vector<vector<char> >&
                 board, const vector<string>& include_words, unsigned int word_index,
                 const vector<string>& exclude_words, const vector<vector<int> >&
                 all_directions, bool find_all){
    
    if(word_index == include_words.size()){
        all_solutions.push_back(board);
        if(!find_all){ return; }
    }

    else{
        // iterates through the entire board
        // find possible insertion spot and checks if insertion is valid
        // checks if an exclude words happen to exist because of the insertion
        // if not, the word is inserted, now new board is being checked with next word
        // nested for loop moves onto the next spot where the word can be inserted
        // checks entire board for all possible solutions
        for(unsigned int row = 0; row < board.size(); row++){
            for(unsigned int col = 0; col < board[row].size(); col++){
                if(board[row][col] == '*' || board[row][col] == include_words[word_index][0]){
                    // checks all eight directions
                    for(unsigned int i = 0; i < all_directions.size(); i++){
                        if(valid_insertion(board, include_words[word_index],
                                           col, row, all_directions[i]) ||
                           insert_along(board, include_words[word_index],
                                           col, row, all_directions[i])){
                            
                            vector<vector<char> > ins_board = new_board(board,
                                    include_words[word_index], col, row, all_directions[i]);
                            
                            if(no_excludes(ins_board, exclude_words, all_directions)){
                                // word has been inserted and is now checking the next word
                                // now checking insertions along empty spaces for next word
                                int next = word_index + 1;
                                insert_word(all_solutions, ins_board, include_words,
                                            next, exclude_words, all_directions, find_all);
                            }
                        }
                        
                    }
                }
            }
        }
    }
}

/* fills the empty spaces it finds by checking every letter of the alphabet
 after inserting the letter, it checks if any new instances of exclude
 words have been generated and if not, the board is appended to the list
 of all solutions.
 in the case that there is more than one empty space, it fills the empty space,
 pushes it back into the list to revisit the second time to comes along to insert
 letters at the next empty space and will continue to fill it until all boards
 have been checked and filled */
void fill_empty(list<vector<vector<char> > >& all_solutions, const vector<string>&
                exclude_words, const vector<vector<int> >& all_directions){
    list<vector<vector<char> > >::iterator it;
    
    for(it = all_solutions.begin(); it != all_solutions.end(); it++){
        for(unsigned int row = 0; row < (*it).size(); row++){
            for(unsigned int col = 0; col < (*it)[row].size(); col++){
                if((*it)[row][col] == '*'){
                    // iterates over the alphabet
                    for(char letter = 'a'; letter <= 'z'; letter++){
                        (*it)[row][col] = letter;
                        if(no_excludes((*it), exclude_words, all_directions)){
                            all_solutions.push_back((*it));
                        }
                    }
                    it = all_solutions.erase(it);
                    /* there will be a segfault if --it is called here
                       a clean up function is written for the case of when
                       there is more than one empty space */
                }
            }
        }
    }
}

/* remove_empty cleans up after fill_empty
 removes another boards with any empty spaces after fill_empty
 has been called.  */
void remove_empty(list<vector<vector<char> > >& all_solutions){
    list<vector<vector<char> > >::iterator it;
    
    for(it = all_solutions.begin(); it != all_solutions.end(); it++){
        bool found_empty = false;
        for(unsigned int row = 0; row < (*it).size(); row++){
            for(unsigned int col = 0; col < (*it)[row].size(); col++){
                if((*it)[row][col] == '*'){
                    /* if an empty space has been found
                       needs to break out of the nested
                       for loop that is checking each location
                       to the base for loop to delete the board
                     */
                    found_empty = true;
                    break;
                }
            }
            if(found_empty){ break; }
        }
        // --it makes sure all boards are considered
        if(found_empty){ it = all_solutions.erase(it); --it; }
    }
}

/* helper function to print out the solutions. takes a bool
 to know whether to print one or all solutions. all solutions
 are always found and when one solution is requested, the first
 one is outputted */
void output(list<vector<vector<char> > >& all_solutions,
            ofstream &output_file, bool out_all){
    list<vector<vector<char> > >::iterator it;
    
    if(out_all) { output_file << all_solutions.size() << " solution(s)" << endl; }
    for(it = all_solutions.begin(); it != all_solutions.end(); it++){
        output_file << "Board:" << endl;
        for(unsigned int row = 0; row < (*it).size(); row++){
            output_file << "  ";
            for(unsigned int col = 0; col < (*it)[row].size(); col++){
                output_file << (*it)[row][col];
            }
            output_file << endl;
        }
        if(!out_all){ break; }
    }
}

int main(int argc, const char * argv[]) {
    if(argc != 4){
        cerr << "Usage: " << argv[0] <<
        "puzzle_file output_file num_solutions" << endl;
        exit(1);
    }
    ifstream puzzle_info(argv[1]);
    ofstream out_file(argv[2]);
    if(!puzzle_info.good()){
        cerr << "Can't open " << argv[1] << " to read." << endl;
        exit(1);
    }
    if(!out_file.good()){
        cerr << "Can't open " << argv[2] << " to write." << endl;
        exit(1);
    }
    
    int width, height;
    puzzle_info >> width >> height;
    
    string word; char sign;
    vector<string> include_words;
    vector<string> exclude_words;
    while(puzzle_info >> sign){
        puzzle_info >> word;
        if(sign == '+'){ include_words.push_back(word); }
        else if(sign == '-'){ exclude_words.push_back(word); }
    }
    vector<vector<char> > board(height, vector<char>(width, '*'));
    list<vector<vector<char> > > all_solutions;
    
    // vector of all directions to check, instead of making 8 if statements
    // only the bounds are checked through checking the vector
    vector<vector<int> > directions;
    directions.push_back(vector<int>{1, 0});   // left to right
    directions.push_back(vector<int>{-1, 0});  // right to left
    directions.push_back(vector<int>{0,1});    // bottom to top
    directions.push_back(vector<int>{0, -1});  // top to bottom
    directions.push_back(vector<int>{1, 1});   // towards top right
    directions.push_back(vector<int>{1, -1});  // towards bottom right
    directions.push_back(vector<int>{-1, 1});  // towards top left
    directions.push_back(vector<int>{-1, -1}); // towards bottom left
    bool find_all = strcmp(argv[3], "all_solutions");
    insert_word(all_solutions, board, include_words, 0,
                exclude_words, directions, find_all);
    fill_empty(all_solutions, exclude_words, directions);
    remove_empty(all_solutions);
    all_solutions.sort(); all_solutions.unique();
    if(all_solutions.size() == 0){ out_file << "No solutions"; }
    else if(strcmp(argv[3], "one_solution") == 0)
        { output(all_solutions, out_file, false); }
    else if(strcmp(argv[3], "all_solutions") == 0)
        { output(all_solutions, out_file, true); }
    
    return 0;
}
