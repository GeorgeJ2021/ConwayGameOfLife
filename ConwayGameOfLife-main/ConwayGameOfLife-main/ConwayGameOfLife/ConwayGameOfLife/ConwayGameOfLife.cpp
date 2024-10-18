#include <iostream>
#include <stdlib.h> 
#include <cstdlib> 
#include <ctime>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

const int maxrow = 50;
const int maxcol = 50;

std::mutex mtx;

class ConwayGame
{
private:

    char a[maxrow][maxcol] , b[maxrow][maxcol], c[maxrow][maxcol];
    int row, col, i, j, tempval, livecount;
    int neighbour_live_cell;

public:
    
    int frameno = 1, maxframe;

    ConwayGame()
    {
        cout << "\nEnter row size: ";
        cin >> row;
        cout << "\nEnter col size: ";
        cin >> col;
        cout << "\nEnter number of living cells: ";
        cin >> livecount;
        cout << "\nEnter number of frames: ";
        cin >> maxframe;
        cout << endl;
        
        for (i = 0; i < row; i++) {
            for (j = 0; j < col; j++) {
                tempval = rand() % 3;
                if (tempval == 1 && (livecount > 0))
                {
                    a[i][j] = 'O';
                    livecount--;

                }
                else
                {
                    a[i][j] = ' ';
                }
                b[i][j] = a[i][j];
            }
        }

    }

    ConwayGame(const string& filename)
    {
        ifstream file(filename);
        if (!file) {
            cerr << "Error: Could not open file for loading." << endl;
            return;
        }

        file >> row >> col >> maxframe;  
        
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                if (a[i][j] == '_')
                {
                    a[i][j] = ' ';
                }           
                file >> a[i][j];
            }
        }

        //cout << "Game state loaded from " << filename << endl;
        file.close();

        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                if (a[i][j] == '_')
                {
                    a[i][j] = ' ';
                }
                b[i][j] = a[i][j];
            }
        }
    }

    ConwayGame(int r, int c, int lc)
    {
        row = r;
        col = c;
        livecount = 0;
        maxframe = 1;

        for (i = 0; i < row; i++) {
            for (j = 0; j < col; j++) {
                tempval = rand() % 4;
                if (tempval == 1 && (livecount < lc))
                {
                    a[i][j] = 'O';
                    livecount++;

                }
                else
                {
                    a[i][j] = ' ';
                }
                b[i][j] = a[i][j];
            }
        }
    }

    template <typename T>
    bool checkEqual(T a, T b) {
        return a == b;
    }
    int CountLiveCells() {
        int lccount = 0;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                if (a[i][j] == 'O') {
                    lccount++;
                }
            }
        }
        return lccount;
    }
    int ApplyRules(int r, int c) 
    {
        int i, j, count = 0;
        for (i = r - 1; i <= r + 1; i++) {
            for (j = c - 1; j <= c + 1; j++) {
                if ((i == r && j == c) || (i < 0 || j < 0)
                    || (i >= maxrow || j >= maxcol)) {
                    continue;
                }
                if (a[i][j] == 'O') {
                    count++;
                }
            }
        }
        return count;
    }

    
    void UpdateGridSection(int startRow, int endRow) {
        for (int i = startRow; i < endRow; i++) {
            for (int j = 0; j < col; j++) {
                int neighbour_live_cell = ApplyRules(i, j);
                if (a[i][j] == 'O' && (neighbour_live_cell == 2 || neighbour_live_cell == 3)) {
                    c[i][j] = 'O'; 
                }
                else if (a[i][j] == ' ' && neighbour_live_cell == 3) {
                    c[i][j] = 'O'; 
                }
                else {
                    c[i][j] = ' '; 
                }
            }
        }
    }

    void ApplyRulesParallel() {
        int numThreads = 4;  
        int chunkSize = row / numThreads;  

        vector<thread> threads;

        for (int i = 0; i < numThreads; i++) {
            int startRow = i * chunkSize;
            int endRow = (i == numThreads - 1) ? row : (i + 1) * chunkSize;

            
            threads.push_back(thread(&ConwayGame::UpdateGridSection, this, startRow, endRow));
        }

        
        for (auto& th : threads) {
            th.join();
        }

        
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                a[i][j] = c[i][j];
            }
        }
    }

    int row_line()
    {
        
        for (int i = 0; i < col+1; i++) {
            cout << ". ";
        }
        cout << endl;
        return 0;
    }

    friend ostream& operator<<(ostream& os, const ConwayGame& game) {
        
        for (int i = 0; i < game.row; i++) {
            os << ".";
            for (int j = 0; j < game.col; j++) {
                os << game.a[i][j] << ".";
            }
            os << endl;
        }
        return os;
    }

    void PrintFrame(ConwayGame& game)
    {
        cout << "\nFrame : "<< frameno << endl;
        row_line();
        cout << game;
        row_line();  
        frameno++;
        
        
    }

    void resetsimulation()
    {
        for (i = 0; i < row; i++) {
            for (j = 0; j < col; j++) {
                a[i][j] = b[i][j];
            }
        }
        frameno = 1;
    }

    void savefile(const string& filename)
    {
        ofstream file(filename);
        if (!file) {
            cerr << "Error: Could not open file for saving." << endl;
            return;
        }

        file << row << " " << col << " " << maxframe << endl;  

        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                if (a[i][j] == ' ')
                {
                    a[i][j] = '_';
                    file << a[i][j];
                }
                else
                {
                    file << a[i][j];
                }
            }
            file << endl;
        }

        cout << "Game state saved to " << filename << endl;
        file.close();
    }

    bool checkForBlock() {
        
        for (int i = 0; i < row - 1; ++i) {
            for (int j = 0; j < col - 1; ++j) {
                if (a[i][j] == 'O' && a[i][j + 1] == 'O' &&
                    a[i + 1][j] == 'O' && a[i + 1][j + 1] == 'O') {
                    if (ApplyRules(i, j) == 3 &&
                        ApplyRules(i, j + 1) == 3 &&
                        ApplyRules(i + 1, j) == 3 &&
                        ApplyRules(i + 1, j + 1) == 3) {
                        cout << "\n Block exists" << endl;
                        return true; 
                    }
                }
            }
        }
        return false;
    }

    bool checkForBeehive() {
        for (int i = 0; i < row - 2; ++i) {
            for (int j = 0; j < col - 3; ++j) {
                if (a[i][j + 1] == 'O' && a[i][j + 2] == 'O' &&
                    a[i + 1][j] == 'O' && a[i + 1][j + 3] == 'O' &&
                    a[i + 2][j + 1] == 'O' && a[i + 2][j + 2] == 'O') {
                    if (ApplyRules(i, j + 1) == 2 &&
                        ApplyRules(i, j + 2) == 2 &&
                        ApplyRules(i + 1, j) == 2 &&
                        ApplyRules(i + 1, j + 3) == 2 &&
                        ApplyRules(i + 2, j + 1) == 2 &&
                        ApplyRules(i + 2, j + 2) == 2) {
                        cout << "\n beehive exists" << endl;
                        return true;  
                    }
                }
            }
        }

        for (int i = 0; i < row - 3; ++i) {
            for (int j = 0; j < col - 2; ++j) {
                if (a[i][j + 1] == 'O' && a[i + 1][j] == 'O' &&
                    a[i + 1][j + 2] == 'O' && a[i + 2][j] == 'O' &&
                    a[i + 2][j + 2] == 'O' && a[i + 3][j + 1] == 'O') {
                    if (ApplyRules(i, j + 1) == 2 &&
                        ApplyRules(i + 1, j) == 2 &&
                        ApplyRules(i + 1, j + 2) == 2 &&
                        ApplyRules(i + 2, j) == 2 &&
                        ApplyRules(i + 2, j + 2) == 2 &&
                        ApplyRules(i + 3, j + 1) == 2) {
                        cout << "\n beehive exists" << endl;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool checkForBlinker() {
        // horizontal blinker
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col - 2; j++) {
                if (a[i][j] == 'O' && a[i][j + 1] == 'O' && a[i][j + 2] == 'O') {
                    if (ApplyRules(i, j) == 1 && ApplyRules(i, j + 1) == 2 && ApplyRules(i, j + 2) == 1) {
                        cout << "\n blinker exists" << endl;
                        return true;
                    }
                }
            }
        }
        // vertical blinker
        for (int i = 0; i < row - 2; i++) {
            for (int j = 0; j < col; j++) {
                if (a[i][j] == 'O' && a[i + 1][j] == 'O' && a[i + 2][j] == 'O') {
                    if (ApplyRules(i, j) == 1 && ApplyRules(i + 1, j) == 2 && ApplyRules(i + 2, j) == 1) {
                        cout << "\n blinker exists" << endl;
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool checkForToad()
    {
        for (int i = 0; i < row - 1; i++) {
            for (int j = 0; j < col - 3; j++) {
                if (a[i][j] == ' ' && a[i][j + 1] == 'O' && a[i][j + 2] == 'O' && a[i][j + 3] == 'O' &&
                    a[i + 1][j] == 'O' && a[i + 1][j + 1] == 'O' && a[i + 1][j + 2] == 'O' && a[i + 1][j + 3] == ' ') {
                    if (ApplyRules(i, j + 1) == 4 && 
                        ApplyRules(i, j + 2) == 4 && 
                        ApplyRules(i, j + 3) == 2 &&
                        ApplyRules(i + 1, j) == 2 &&
                        ApplyRules(i + 1, j + 1) == 4 && 
                        ApplyRules(i + 1, j + 2) == 4) {
                        cout << "\n Toad exists" << endl;
                        return true;
                    }
                }
            }
        }
        for (int i = 0; i < row - 3; i++) {
            for (int j = 0; j < col - 1; j++) {
                if (a[i][j] == ' ' && a[i + 1][j] == 'O' && a[i + 2][j] == 'O' && a[i + 3][j] == 'O' &&
                    a[i][j + 1] == 'O' && a[i + 1][j + 1] == 'O' && a[i + 2][j + 1] == 'O' && a[i + 3][j + 1] == ' ') {
                    if (ApplyRules(i + 1, j) == 4 && 
                        ApplyRules(i + 2, j) == 4 && 
                        ApplyRules(i + 3, j) == 2 &&
                        ApplyRules(i, j + 1) == 2 &&
                        ApplyRules(i + 1, j + 1) == 4 && 
                        ApplyRules(i + 2, j + 1) == 4) {
                        cout << "\n Toad exists" << endl;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool IsGlider(int i, int j)
    {
        return (a[i][j + 1] == 'O' && a[i + 1][j + 2] == 'O' &&
            a[i + 2][j] == 'O' && a[i + 2][j + 1] == 'O' && a[i + 2][j + 2] == 'O') ||

            (a[i][j + 1] == 'O' && a[i +1][j] == 'O' &&
                a[i + 2][j] == 'O' && a[i + 2][j + 1] == 'O' && a[i + 2][j + 2] == 'O') ||

            (a[i][j] == 'O' && a[i][j + 1] == 'O' &&
                a[i][j + 2] == 'O' && a[i + 1][j + 2] == 'O' && a[i + 2][j + 1] == 'O') ||

            (a[i][j] == 'O' && a[i][j + 1] == 'O' &&
                a[i][j + 2] == 'O' && a[i + 1][j] == 'O' && a[i + 2][j + 1] == 'O');
    }

    bool IsolatedGlider(int i, int j) {


        if (ApplyRules(i, j + 1) == 1 &&
            ApplyRules(i + 1, j + 2) == 3 &&
            ApplyRules(i + 2, j) == 1 &&
            ApplyRules(i + 2, j + 1) == 3 &&
            ApplyRules(i + 2, j + 2) == 2) {
            return true;
        }

        if (ApplyRules(i, j + 1) == 1 &&
            ApplyRules(i + 1, j) == 3 &&
            ApplyRules(i + 2, j) == 2 &&
            ApplyRules(i + 2, j + 1) == 3 &&
            ApplyRules(i + 2, j + 2) == 1) {
            return true;
        }

        if (ApplyRules(i, j) == 1 &&
            ApplyRules(i , j + 1) == 3 &&
            ApplyRules(i , j + 2) == 2 &&
            ApplyRules(i + 1, j + 2) == 3 &&
            ApplyRules(i + 2, j + 1) == 1) {
            return true;
        }

        if (ApplyRules(i, j) == 2 &&
            ApplyRules(i, j + 1) == 3 &&
            ApplyRules(i, j + 2) == 1 &&
            ApplyRules(i + 1, j) == 3 &&
            ApplyRules(i + 2, j + 1) == 1) {
            return true;
        }

        return false;
    }

    bool checkForGlider() {
        for (int i = 0; i < row - 2; i++) {
            for (int j = 0; j < col - 2; j++) {
             
                if (IsGlider(i, j)) {

                    if (IsolatedGlider(i, j)) {
                        ConwayGame temp(*this);
                        for (int frame = 1; frame <= 4; frame++) {
                            temp.ApplyRulesParallel();
                        }
                        for (int i = 0; i < row - 2; i++) {
                            for (int j = 0; j < col - 2; j++) {
                                if (temp.IsGlider(i, j)) {
                                    if (temp.IsolatedGlider(i, j))
                                    {
                                        cout << "\n glider exists" << endl;
                                        return true;
                                    }
                                      
                                }
                            }

                        }
                    }
                }
            }
        }
        return false;
    }

    bool ERNGlider()
    {
        for (int i = 0; i < row - 2; i++) {
            for (int j = 0; j < col - 2; j++) {

                if (IsGlider(i, j)) {
                    if (IsolatedGlider(i, j))
                    {
                        cout << "\n Glider exists" << endl;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool IsLWSS(int i , int j)
    {
        return ( a[i][j + 1] == 'O' && a[i][j + 4] == 'O' && a[i + 1][j] == 'O' && a[i + 2][j] == 'O' && a[i + 3][j] == 'O' &&
                a[i + 3][j + 1] == 'O' && a[i + 3][j + 2] == 'O' && a[i + 3][j + 3] == 'O' && a[i + 2][j + 4] == 'O' ) || 
            
           (a[i][j] == 'O' && a[i][j + 3] == 'O' && a[i + 1][j + 4] == 'O' && a[i + 2][j] == 'O' && a[i + 2][j + 4] == 'O' && 
            a[i + 3][j + 1] == 'O' && a[i + 3][j + 2] == 'O' && a[i + 3][j + 3] == 'O' && a[i + 3][j + 4] == 'O' ) ||

           ( a[i][j + 1] == 'O' && a[i][j + 2] == 'O' && a[i][j + 3] == 'O' && a[i][j + 4] == 'O' && a[i + 1][j] == 'O' &&
            a[i + 1][j + 4] == 'O' && a[i + 2][j + 4] == 'O' && a[i + 3][j] == 'O' && a[i + 3][j + 3] == 'O' ) ||

            ( a[i][j] == 'O' && a[i][j + 1] == 'O' && a[i][j + 2] == 'O' && a[i][j + 3] == 'O' && a[i + 1][j + 4] == 'O' &&
            a[i + 2][j] == 'O' && a[i + 3][j + 1] == 'O' && a[i + 3][j + 3] == 'O' && a[i + 3][j + 4] == 'O' );


        

    }

    bool IsolatedLWSS(int i, int j) {


        if (ApplyRules(i, j + 1) == 1 &&
            ApplyRules(i, j + 4) == 0 &&
            ApplyRules(i + 1, j) == 2 &&
            ApplyRules(i + 2, j ) == 3 &&
            ApplyRules(i + 3, j ) == 2 &&
            ApplyRules(i + 3, j + 1) == 3 && 
            ApplyRules(i + 3, j + 2) == 2 && 
            ApplyRules(i + 3, j + 3) == 2 && 
            ApplyRules(i + 2, j + 4) == 1) {
            return true;
        }

        if (ApplyRules(i, j) == 0 &&
            ApplyRules(i, j + 3) == 1 &&
            ApplyRules(i + 1, j + 4) == 2 &&
            ApplyRules(i + 3, j + 4) == 2 &&
            ApplyRules(i + 2, j) == 1 &&
            ApplyRules(i + 2, j + 4) == 3 &&
            ApplyRules(i + 3, j + 1) == 2 &&
            ApplyRules(i + 3, j + 2) == 2 &&
            ApplyRules(i + 3, j + 3) == 3) {
            return true;
        }

        if (ApplyRules(i, j + 1) == 2 &&
            ApplyRules(i, j + 2) == 2 &&
            ApplyRules(i, j + 3) == 3 &&
            ApplyRules(i, j + 4) == 2 &&
            ApplyRules(i + 1, j) == 1 &&
            ApplyRules(i + 1, j + 4) == 3 &&
            ApplyRules(i + 2, j + 4) == 2 &&
            ApplyRules(i + 3, j) == 0 &&
            ApplyRules(i + 3, j + 3) == 1) {
            return true;
        }
        if (ApplyRules(i, j) == 2 &&
            ApplyRules(i, j + 1) == 3 &&
            ApplyRules(i, j + 2) == 2 &&
            ApplyRules(i, j + 3) == 2 &&
            ApplyRules(i + 1, j) == 3 &&
            ApplyRules(i + 1, j + 4) == 1 &&
            ApplyRules(i + 2, j) == 2 &&
            ApplyRules(i + 3, j + 1) == 1 &&
            ApplyRules(i + 3, j + 4) == 0) {
            return true;
        }

        return false;
    }

    bool checkForLWSS()
    {
        for (int i = 0; i < row - 3; i++) {
            for (int j = 0; j < col - 4; j++) {

                if (IsLWSS(i, j)) {

                    

                    if (IsolatedLWSS(i, j)) {
                        
                        ConwayGame temp(*this);
                        for (int frame = 1; frame <= 4; frame++) {
                            temp.ApplyRulesParallel();
                        }
                        for (int i = 0; i < row - 3; i++) {
                            for (int j = 0; j < col - 4; j++) {
                                if (temp.IsLWSS(i, j)) {
                                    if (temp.IsolatedLWSS(i, j))
                                    {
                                        cout << "\n LWSS exists" << endl;
                                        return true;
                                    }

                                }
                            }

                        }
                    }
                }
            }
        }
        return false;

    }

    bool ERNLWSS()
    {
        for (int i = 0; i < row - 3; i++) {
            for (int j = 0; j < col - 4; j++) {

                if (IsLWSS(i, j)) {

                    if (IsolatedLWSS(i, j)) {

                        cout << "\nLWSS exists" << endl;
                        return true;
                    }
                }
            }
        }
        return false;
    }


};

int ERNexp()
{
    int row = 2, col = 2, lc = 4 , total_live_cells, ERN = 0, experiment = 0, iteration = 1;

    ConwayGame G(row,col,lc);
    char response;
    bool foundpattern = false;
    int BlockERN = 0, BeehiveERN = 0, BlinkerERN = 0, toadERN = 0, GliderERN = 0, LWSSERN = 0;

    do {

        cout << "\n Run ERN experiment for: \n 1. Block Current lowest ERN: "<< BlockERN<<" \n 2. Beehive Current lowest ERN: "<< BeehiveERN<<"\n 3. Blinker Current lowest ERN: "<<BlinkerERN<<"\n 4. Toad Current lowest ERN: "<<toadERN<<"\n 5. Glider Current lowest ERN: "<<GliderERN<<"\n 6. Lightweight Spaceship(LWSS) Current lowest ERN: "<<LWSSERN<<"\n 7. reset grid \n 8. Load Lowest ERNs \n 9. Go back" << endl;
        cout << "\n awaiting response: ";
        cin >> response;
        experiment = 0;
        iteration = 1;
        foundpattern = false;

        

        row = 2;
        col = 2;
        lc = 4;

        switch (response)
        {
            case '1':
                
                G.~ConwayGame();
                while (!foundpattern && iteration<10) {
                    experiment++;
                    ConwayGame G(row, col, lc); 
                    total_live_cells = G.CountLiveCells();
                    ERN = row + col + total_live_cells;
                    cout <<"\n Iteration : "<<iteration<< ", Experiment #" << experiment << endl;
                    cout << "\n Grid intialized with ERN : " << ERN << " with grid size " << row << " X " << col << endl;
                    G.PrintFrame(G);  
                    if (G.checkForBlock()) {
                            cout << "\n Pattern detected in experiment #" << experiment << " with ERN: "<<ERN<<endl;
                            if (BlockERN == 0)
                            {
                                BlockERN = ERN;
                                foundpattern = true;
                                break;
                            }
                            else if (ERN < BlockERN)
                                {
                                    BlockERN = ERN;
                                    foundpattern = true;
                                    break;


                             }
                     }
                    else
                    {
                        G.~ConwayGame();
                        if (col < 35)
                        {
                            col++;
                        }
                        else if (row < 35)
                        {
                            row++;
                        }
                        else if (row == 35 && col == 35)
                        {
                            col = 2;
                            row = 2;
                            experiment = 0;
                            iteration++;
                        }
                       
                        lc = (row * col); 
                    }
                }
                if (!foundpattern)
                {
                    cout << "\n Pattern not found :(" << endl;
                }
                break;

            case '2':
                G.~ConwayGame();
                while (!foundpattern && iteration < 20) {
                    experiment++;
                    ConwayGame G(row, col, lc);
                    total_live_cells = G.CountLiveCells();
                    ERN = row + col + total_live_cells;
                    cout << "\n Iteration : " << iteration << ", Experiment #" << experiment << endl;
                    cout << "\n Grid intialized with ERN : " << ERN << " with grid size " << row << " X " << col << endl;
                    G.PrintFrame(G);  
                    if (G.checkForBeehive()) {
                        cout << "\n Pattern detected in experiment #" << experiment << " with ERN: " << ERN << endl;
                        if (BeehiveERN == 0)
                        {
                            BeehiveERN = ERN;
                            foundpattern = true;
                            break;
                        }
                        else if (ERN < BeehiveERN)
                        {
                            BeehiveERN = ERN;
                            foundpattern = true;
                            break;
                        }
                    }
                    else
                    {
                        G.~ConwayGame();
                        if (col < 35)
                        {
                            col++;
                        }
                        else if (row < 35)
                        {
                            row++;
                        }
                        else if (row == 35 && col == 35)
                        {
                            col = 2;
                            row = 2;
                            experiment = 0;
                            iteration++;
                        }

                        lc = (row * col);
                    }
                }
                if (!foundpattern)
                {
                    cout << "\n Pattern not found :(" << endl;
                }
                break;

            case '3':
                G.~ConwayGame();
                while (!foundpattern && iteration < 10) {
                    experiment++;
                    ConwayGame G(row, col, lc);
                    total_live_cells = G.CountLiveCells();
                    ERN = row + col + total_live_cells;
                    cout << "\n Iteration : " << iteration << ", Experiment #" << experiment << endl;
                    cout << "\n Grid intialized with ERN : " << ERN << " with grid size " << row << " X " << col << endl;
                    G.PrintFrame(G);  
                    if (G.checkForBlinker()) {
                        cout << "\n Pattern detected in experiment #" << experiment << " with ERN: " << ERN << endl;
                        if (BlinkerERN == 0)
                        {
                            BlinkerERN = ERN;
                            foundpattern = true;
                            break;
                        }
                        else if (ERN < BlinkerERN)
                        {
                            BlinkerERN = ERN;
                            foundpattern = true;
                            break;
                        }
                    }
                    else
                    {
                        G.~ConwayGame();
                        if (col < 35)
                        {
                            col++;
                        }
                        else if (row < 35)
                        {
                            row++;
                        }
                        else if (row == 35 && col == 35)
                        {
                            col = 2;
                            row = 2;
                            experiment = 0;
                            iteration++;
                        }

                        lc = (row * col) ;
                    }
                }
                if (!foundpattern)
                {
                    cout << "\n Pattern not found :(" << endl;
                }
                break;

            case '4':
                G.~ConwayGame();
                while (!foundpattern && iteration < 20) {
                    experiment++;
                    ConwayGame G(row, col, lc);
                    total_live_cells = G.CountLiveCells();
                    ERN = row + col + total_live_cells;
                    cout << "\n Iteration : " << iteration << ", Experiment #" << experiment << endl;
                    cout << "\n Grid intialized with ERN : " << ERN << " with grid size " << row << " X " << col << endl;
                    G.PrintFrame(G);  
                    if (G.checkForToad()) {
                        cout << "\n Pattern detected in experiment #" << experiment << " with ERN: " << ERN << endl;
                        if (toadERN == 0)
                        {
                            toadERN = ERN;
                            foundpattern = true;
                            break;
                        }
                        else if (ERN < toadERN)
                        {
                            toadERN = ERN;
                            foundpattern = true;
                            break;
                        }
                    }
                    else
                    {
                        G.~ConwayGame();
                        if (col < 35)
                        {
                            col++;
                        }
                        else if (row < 35)
                        {
                            row++;
                        }
                        else if (row == 35 && col == 35)
                        {
                            col = 2;
                            row = 2;
                            experiment = 0;
                            iteration++;
                        }

                        lc = (row * col) ;
                    }
                }
                if (!foundpattern)
                {
                    cout << "\n Pattern not found :(" << endl;
                }
                break;

            case '5':
                G.~ConwayGame();
                while (!foundpattern && iteration < 20) {
                    experiment++;
                    ConwayGame G(row, col, lc);
                    total_live_cells = G.CountLiveCells();
                    ERN = row + col + total_live_cells;
                    cout << "\n Iteration : " << iteration << ", Experiment #" << experiment << endl;
                    cout << "\n Grid intialized with ERN : " << ERN << " with grid size " << row << " X " << col << endl;
                    G.PrintFrame(G);  
                    if (G.ERNGlider()) {
                        cout << "\n Pattern detected in experiment #" << experiment << " with ERN: " << ERN << endl;
                        if (GliderERN == 0)
                        {
                            GliderERN = ERN;
                            foundpattern = true;
                            break;
                        }
                        else if (ERN < GliderERN)
                        {
                            GliderERN = ERN;
                            foundpattern = true;
                            break;
                        }
                    }
                    else
                    {
                        G.~ConwayGame();
                        if (col < 35)
                        {
                            col++;
                        }
                        else if (row < 35)
                        {
                            row++;
                        }
                        else if (row == 35 && col == 35)
                        {
                            col = 2;
                            row = 2;
                            experiment = 0;
                            iteration++;
                        }

                        lc = (row * col) / 2;
                    }
                }
                if (!foundpattern)
                {
                    cout << "\n Pattern not found :(" << endl;
                }
                break;

            case '6':
                G.~ConwayGame();
                while (!foundpattern && iteration < 20) {
                    experiment++;
                    ConwayGame G(row, col, lc);
                    total_live_cells = G.CountLiveCells();
                    ERN = row + col + total_live_cells;
                    cout << "\n Iteration : " << iteration << ", Experiment #" << experiment << endl;
                    cout << "\n Grid intialized with ERN : " << ERN << " with grid size " << row << " X " << col << endl;
                    G.PrintFrame(G);  
                    if (G.ERNLWSS()) {
                        cout << "\n Pattern detected in experiment #" << experiment << " with ERN: " << ERN << endl;
                        if (LWSSERN == 0)
                        {
                            LWSSERN = ERN;
                            foundpattern = true;
                            break;
                        }
                        else if (ERN < LWSSERN)
                        {
                            LWSSERN = ERN;
                            foundpattern = true;
                            break;
                        }
                    }
                    else
                    {
                        G.~ConwayGame();
                        if (col < 35)
                        {
                            col++;
                        }
                        else if (row < 35)
                        {
                            row++;
                        }
                        else if (row == 35 && col == 35)
                        {
                            col = 2;
                            row = 2;
                            experiment = 0;
                            iteration++;
                        }

                        lc = (row * col) / 2;
                    }
                }
                if (!foundpattern)
                {
                    cout << "\n Pattern not found :(" << endl;
                }
                break;

            case '7':
                G.~ConwayGame();
                ERNexp();
                return 0;

            case '8':
                while (ERN != 8 )
                {
                    ConwayGame block(2, 2, 4);
                    if (block.checkForBlock())
                    {
                        ERN = row + col + block.CountLiveCells();
                        block.PrintFrame(block);
                        cout << "\nBlock ERN: " <<ERN<< endl;
                        
                        break;
                    }
                    else
                    {
                        block.~ConwayGame();
                    }
                }
                while (ERN != 13)
                {
                    ConwayGame beehive(3, 4, 6);
                    if (beehive.checkForBeehive())
                    {
                        ERN = row + col + beehive.CountLiveCells();
                        beehive.PrintFrame(beehive);
                        cout << "\nBeehive ERN: " << ERN << endl;
                        break;
                    }
                    else
                    {
                        beehive.~ConwayGame();
                    }
                }
                while (ERN != 9)
                {
                    ConwayGame blinker (1,3, 3);
                    if (blinker.checkForBlinker())
                    {
                        ERN = row + col + blinker.CountLiveCells();
                        blinker.PrintFrame(blinker);
                        cout << "\nBlinker ERN: " << ERN << endl;
                        break;
                    }
                    else
                    {
                        blinker.~ConwayGame();
                    }
                }
                while (ERN != 14)
                {
                    ConwayGame Toad(4, 4, 6);
                    if (Toad.checkForToad())
                    {
                        ERN = row + col + Toad.CountLiveCells();
                        Toad.PrintFrame(Toad);
                        cout << "\nToad ERN: " << ERN << endl;
                        break;
                    }
                    else
                    {
                        Toad.~ConwayGame();
                    }
                }

                while (ERN != 11)
                {
                    ConwayGame Glider(3, 3, 5);
                    if (Glider.ERNGlider())
                    {
                        ERN = row + col + Glider.CountLiveCells();
                        Glider.PrintFrame(Glider);
                        cout << "\nGlider ERN: " << ERN << endl;
                        break;
                    }
                    else
                    {
                        Glider.~ConwayGame();
                    }
                }
                experiment = 0;
                while (ERN != 18 && experiment < 100)
                {
                    ConwayGame LWSS(4, 5, 9);
                    experiment++;
                    if (LWSS.ERNLWSS())
                    {
                        ERN = row + col + LWSS.CountLiveCells();
                        LWSS.PrintFrame(LWSS);
                        cout << "\nGlider ERN: " << ERN << endl;
                        break;
                    }
                    else
                    {
                        LWSS.~ConwayGame();
                    }
                }
                break;

            case '9':

                return 0;
                break;              

        }

    } while (response != '9');
        

} 

void newgame();

 int SimulationOperation (ConwayGame& G1)
{
   
    char response;
    int currentFrame = 0;
    string filename;
    G1.PrintFrame(G1);

    do
    {
        G1.ApplyRulesParallel();
        G1.PrintFrame(G1);


    } while (!(G1.checkEqual(G1.maxframe + 1, G1.frameno)));

    do {
        cout << "\n 1. Resume current simulation \n 2. Reset current simulation \n 3. Restart with new simulation \n 4. save current Frame \n 5. Back to main menu";
        cout << "\n awaiting input : ";
        cin >> response;
        cout << endl;

        switch (response)
        {
            case '1':
                G1.ApplyRulesParallel();
                G1.PrintFrame(G1);
                break;

            case '2':
                cout << "\n resetting simulation" << endl;
                G1.resetsimulation();
                G1.PrintFrame(G1);
                break;

            case '3':
                cout << "\n Loading new simulation..." << endl;
                G1.~ConwayGame();
                newgame();
                return 0;
                break;

            case '4':
                cout << "\n Enter filename to save the current frame: ";
                cin >> filename;
                G1.savefile(filename);
                break;

            case '5':
                cout << "\n BACK TO MAIN MENU \n______________" << endl;
                return 0;

        }

    } while (response != '5');
}

 void newgame()
 {
     ConwayGame Game;
     SimulationOperation(Game);
 }

 void Loadgame()
 {
     string filename;
     cout << "\n Enter filename to load the game from: ";
     cin >> filename;
     ConwayGame Game(filename);
     cout << "Game state loaded from " << filename << endl;
     Game.PrintFrame(Game);
     SimulationOperation(Game);

 }

 int QuestionMenu()
 {
     string filename;
     char response;
     ConwayGame GAME(35, 35, 800);
     bool foundpattern;
     do
     {
         foundpattern = false;
         int experimentCount;
         
         cout << "\n 1. Question 1 \n 2. Question 2 \n 3. Question 3 \n 4. Question 4 \n 5. Question 5 \n 6. Back to main menu" << endl;
         cout << "\n Awaiting response: ";
         cin >> response;

         switch (response) {

         case '1':
             newgame();
             break;
         case '2':
             experimentCount = 0;
             while (!foundpattern && experimentCount<20) {
                 experimentCount++;
                 ConwayGame GAME(35, 35, 800);  
                 system("cls");
                 cout << "\nExperiment #" << experimentCount << endl;
                 GAME.PrintFrame(GAME);  

                 while (GAME.frameno <= 100) {  
                     GAME.ApplyRulesParallel();
                     GAME.PrintFrame(GAME);  

                     if (GAME.checkForBlock() || GAME.checkForBeehive()) {
                         cout << "Block/Beehive Pattern detected in experiment #" << experimentCount << " at frame : " << GAME.frameno - 1 << endl;
                         foundpattern = true;
                         break;
                     }

                 }
             }
             if (foundpattern == false)
             {
                 cout << "Pattern not found :(" << endl;
             }
             cout << "would you like to save ? (y/n) : ";
             cin >> response;
             if (response == 'y')
             {
                 GAME.maxframe = GAME.frameno;
                 cout << "\n Enter filename to save the current frame: ";
                 cin >> filename;
                 GAME.savefile(filename);
             }

            break;

         case '3':
             experimentCount = 0;
             while (!foundpattern && experimentCount < 20) {
                 experimentCount++;
                 ConwayGame GAME(35, 35, 800); 
                 system("cls");
                 cout << "\nExperiment #" << experimentCount << endl;
                 GAME.PrintFrame(GAME);  

                 while (GAME.frameno <= 100) { 
                     GAME.ApplyRulesParallel();
                     GAME.PrintFrame(GAME);  

                     if (GAME.checkForToad() || GAME.checkForBlinker()) {
                         cout << "\n Blinker/Toad Pattern detected in experiment #" << experimentCount << " at frame : " << GAME.frameno - 1 << endl; 
                         foundpattern = true;
                         break;
                     }

                 }
             }
             if (foundpattern == false)
             {
                 cout << "Pattern not found :(" << endl;
             }
             cout << "would you like to save ? (y/n) : ";
             cin >> response;
             if (response == 'y')
             {
                 GAME.maxframe = GAME.frameno;
                 cout << "\n Enter filename to save the current frame: ";
                 cin >> filename;
                 GAME.savefile(filename);
             }
             break;

         case '4':
             experimentCount = 0;
             while (!foundpattern && experimentCount < 20) {
                 experimentCount++;
                 ConwayGame GAME(35, 35, 800);  
                 system("cls");
                 cout << "\nExperiment #" << experimentCount << endl;
                 GAME.PrintFrame(GAME);  

                 while (GAME.frameno <= 100) {  
                     GAME.ApplyRulesParallel();
                     GAME.PrintFrame(GAME);  

                     if (GAME.checkForLWSS() || GAME.checkForGlider()) {
                         cout << "\n LWSS/Glider Pattern detected in experiment #" << experimentCount << " at frame : " << GAME.frameno - 1 << endl;
                         foundpattern = true;
                         break;
                     }
                 }
             }
             if (foundpattern == false)
             {
                 cout << "Pattern not found :(" << endl;
             }
             cout << "\n would you like to save ? (y/n) : ";
             cin >> response;
             if (response == 'y')
             {
                 GAME.maxframe = GAME.frameno;
                 cout << "\n Enter filename to save the current frame: ";
                 cin >> filename;
                 GAME.savefile(filename);
             }
             break;

         case '5':
             ERNexp();
             break;

         case '6':
             return 0;
             break;

         }


     } while (response != '6');

 }

int main()
{
    srand(time(0));
    char userinput;
    string filename;
    
    cout << "\n CONWAY'S GAME OF LIFE";
    cout << "\n_______________" << endl;
    
    do
    {
        cout << "\n 1. Start new game  \n 2. Load Game \n 3. Open Questions menu \n 4. Quit" << endl;
        cout << "\n Awaiting input : ";
        cin >> userinput;

        switch (userinput) {

        case '1':
            cout << "\n Starting Game..." << endl;
            newgame();         
            break;
        
        case '2':
            cout << "\n Loading...." << endl;
            Loadgame();
            break;
            
        case '3':
            cout << "\n Questions Menu \n____________" << endl;
            QuestionMenu();
            break;
           
        case '4':
            break;

        }

    }while (userinput != '4');

    cout << "\n Ending Game ";

    return 0;
}

