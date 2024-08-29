#include <wx/wx.h>
#include <vector>
#include <sstream>
#include <list>
#include <cstring>

using std::cerr; using std::endl; using std::ifstream; using std::ofstream; using std::string; using std::cout;
using std::vector; using std::sort; using std::list; using std::stringstream; using std::getline;

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);

private:
    void OnRun(wxCommandEvent& event); // event handler

    // input fields
    wxTextCtrl* m_widthInput; wxTextCtrl* m_heightInput;
    wxTextCtrl* m_includeWords; wxTextCtrl* m_excludeWords;
    wxRadioBox* m_solutionType; wxTextCtrl* m_outputDisplay;

    // helper functions
    vector<string> ParseWords(wxTextCtrl* input);
    string RunInverseWordSearch(int width, int height, vector<string>& include_words, 
        vector<string>& exclude_words, bool find_all);
};

enum {
    ID_Run = 1
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame("Inverse Word Search GUI");
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 700)) {
    wxPanel* panel = new wxPanel(this, wxID_ANY); // panel to hold all controls
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    // horizontal box, board dimensions input
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* st1 = new wxStaticText(panel, wxID_ANY, "Board Dimensions (Width x Height):");
    hbox1->Add(st1, 0, wxRIGHT, 8);
    m_widthInput = new wxTextCtrl(panel, wxID_ANY); // input field
    m_heightInput = new wxTextCtrl(panel, wxID_ANY);
    hbox1->Add(m_widthInput, 1);
    hbox1->Add(m_heightInput, 1, wxLEFT, 5);
    vbox->Add(hbox1, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    // include words input
    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* st2 = new wxStaticText(panel, wxID_ANY, "Include Words (one per line):");
    hbox2->Add(st2, 0, wxRIGHT, 8);
    m_includeWords = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    hbox2->Add(m_includeWords, 1, wxEXPAND);
    vbox->Add(hbox2, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    // exclude words input
    wxBoxSizer* hbox3 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* st3 = new wxStaticText(panel, wxID_ANY, "Exclude Words (one per line):");
    hbox3->Add(st3, 0, wxRIGHT, 8);
    m_excludeWords = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    hbox3->Add(m_excludeWords, 1, wxEXPAND);
    vbox->Add(hbox3, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    wxString solutionChoices[] = { "One Solution", "All Solutions" };
    m_solutionType = new wxRadioBox(panel, wxID_ANY, "Solution Type",
        wxDefaultPosition, wxDefaultSize, WXSIZEOF(solutionChoices), solutionChoices,
        1, wxRA_SPECIFY_ROWS);
    vbox->Add(m_solutionType, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    wxButton* btnRun = new wxButton(panel, ID_Run, "Run");
    vbox->Add(btnRun, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    m_outputDisplay = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    vbox->Add(m_outputDisplay, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    panel->SetSizer(vbox);

    Bind(wxEVT_BUTTON, &MyFrame::OnRun, this, ID_Run);
}

vector<string> MyFrame::ParseWords(wxTextCtrl* input) {
    vector<string> words;
    stringstream ss(input->GetValue().ToStdString());
    string line;
    while (getline(ss, line))
    {
        if (!line.empty())
            words.push_back(line);
    }
    return words;
}

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
                 all_directions){
    
    if(word_index == include_words.size()){
        all_solutions.push_back(board);
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
                        if(valid_insertion(board, include_words[word_index], col, row, all_directions[i]) ||
                           insert_along(board, include_words[word_index], col, row, all_directions[i])){
                            
                            vector<vector<char> > ins_board = new_board(board,
                                    include_words[word_index], col, row, all_directions[i]);
                            
                            if(no_excludes(ins_board, exclude_words, all_directions)){
                                // word has been inserted and is now checking the next word
                                // now checking insertions along empty spaces for next word
                                int next = word_index + 1;
                                insert_word(all_solutions, ins_board, include_words,
                                            next, exclude_words, all_directions);
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

// helper function to execute algorithm when Run is clicked
string MyFrame::RunInverseWordSearch(int width, int height, vector<string>& include_words, 
    vector<string>& exclude_words, bool find_all) {
    vector<vector<char>> board(height, vector<char>(width, '*'));
    list<vector<vector<char>>> all_solutions;
    
    vector<vector<int>> directions = { {1, 0}, { -1, 0 }, { 0, 1 }, { 0, -1 },
        {1, 1}, {1, -1}, { -1, 1 }, { -1, -1 } };

    insert_word(all_solutions, board, include_words, 0, exclude_words, directions);
    fill_empty(all_solutions, exclude_words, directions);
    remove_empty(all_solutions);
    all_solutions.sort();
    all_solutions.unique();

    stringstream result;
    int count = 1;
    if(all_solutions.size() == 0){
        result << "No possible boards" << endl;
    }
    else if (find_all) {
        result << "Total solutions: " << all_solutions.size() << endl << endl;
    }
    for (const auto& solution : all_solutions) {
        if(find_all){
            result << "Board: " << count << endl;
            count++;
        }
        for (const auto& row : solution) {
            for (char c : row) {
                result << c << " ";
            }
            result << "\n";
        }
        result << "\n";
        if(!find_all){
            break;
        }
    }

    return result.str();
}

// function when Run is clicked
void MyFrame::OnRun(wxCommandEvent& event) {
    long width, height;
    if (!m_widthInput->GetValue().ToLong(&width) || !m_heightInput->GetValue().ToLong(&height)) {
        wxMessageBox("Please enter valid numbers for the board dimensions.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    vector<string> include_words = ParseWords(m_includeWords);
    vector<string> exclude_words = ParseWords(m_excludeWords);
    bool find_all = m_solutionType->GetSelection() == 1;

    string result = RunInverseWordSearch(width, height, include_words, exclude_words, find_all);
    m_outputDisplay->SetValue(result);
}
