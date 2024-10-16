#include <iostream>
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
                tempval = rand() % 2;
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

        file >> row >> col >> maxframe;  // Load dimensions and remaining frames

        // Load the grid
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                if (a[i][j] == '_')
                {
                    a[i][j] = ' ';
                }           
                file >> a[i][j];
            }
        }

        cout << "Game state loaded from " << filename << endl;
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

    // Function to update a portion of the grid concurrently
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
        // Check for a 2x2 block pattern
        for (int i = 0; i < row - 1; ++i) {
            for (int j = 0; j < col - 1; ++j) {
                if (a[i][j] == 'O' && a[i][j + 1] == 'O' &&
                    a[i + 1][j] == 'O' && a[i + 1][j + 1] == 'O') {
                    if (ApplyRules(i, j) == 3 &&
                        ApplyRules(i, j + 1) == 3 &&
                        ApplyRules(i + 1, j) == 3 &&
                        ApplyRules(i + 1, j + 1) == 3) {
                        return true; 
                    }
                }
            }
        }
        return false;
    }

    bool checkForBeehive() {
        // Check for a 3x4 beehive pattern
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
                    if (ApplyRules(i, j) == 2 && ApplyRules(i, j + 1) == 2 && ApplyRules(i, j + 2) == 2) {
                        return true;
                    }
                }
            }
        }
        // vertical blinker
        for (int i = 0; i < row - 2; i++) {
            for (int j = 0; j < col; j++) {
                if (a[i][j] == 'O' && a[i + 1][j] == 'O' && a[i + 2][j] == 'O') {
                    if (ApplyRules(i, j) == 2 && ApplyRules(i + 1, j) == 2 && ApplyRules(i + 2, j) == 2) {
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

    bool IsLWSS(int i , int j)
    {
        return a[i][j + 1] == 'O' && a[i][j + 4] == 'O' && a[i + 1][j] == 'O' && a[i + 2][j] == 'O' && a[i + 3][j] == 'O' &&
                a[i + 3][j + 1] == 'O' && a[i + 3][j + 2] == 'O' && a[i + 3][j + 3] == 'O' && a[i + 2][j + 4] == 'O';

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
        return false;
    }

    bool checkForLWSS()
    {
        for (int i = 0; i < row - 3; i++) {
            for (int j = 0; j < col - 4; j++) {

                if (IsLWSS(i, j)) {

                    //cout << "\n LWSS pattern detected, isolating:" << endl;

                    if (IsolatedLWSS(i, j)) {
                        //cout << "\n glider isolated, checking frame" << endl;
                        ConwayGame temp(*this);
                        for (int frame = 1; frame <= 4; frame++) {
                            temp.ApplyRulesParallel();
                        }
                        for (int i = 0; i < row - 2; i++) {
                            for (int j = 0; j < col - 2; j++) {
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



};

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
     Game.PrintFrame(Game);
     if (Game.checkForLWSS())
     {
         cout << "\n LWSS detected" << endl;
     }

     //SimulationOperation(Game);

 }

 int QuestionMenu()
 {
     string filename;
     char response;
     bool foundpattern;
     do
     {
         foundpattern = false;
         ConwayGame GAME(35, 35, 800);
         cout << "\n 1. Question 1 \n 2. Question 2 \n 3. Question 3 \n 4. Question 4 \n 5. Question 5 \n 6. Back to main menu" << endl;
         cout << "\n Awaiting response: ";
         cin >> response;

         switch (response) {

         case '1':
             newgame();
             break;
         case '2':
             GAME.PrintFrame(GAME);
             do
             {
                 GAME.ApplyRulesParallel();
                 GAME.PrintFrame(GAME);
                 if ( GAME.checkForBlock() || GAME.checkForBeehive())
                 {
                     foundpattern = true;
                     cout << "Block or Beehive detected at Frame : " << GAME.frameno -1 << endl;
                 }
             } while (!foundpattern && GAME.frameno < 100);
             break;

         case '3':
             GAME.PrintFrame(GAME);
             do
             {
                 GAME.ApplyRulesParallel();
                 GAME.PrintFrame(GAME);
                 if ( GAME.checkForBlinker() || GAME.checkForToad() )
                 {
                     foundpattern = true;
                     cout << "Blinker or Toad detected at Frame : " << GAME.frameno - 1 << endl;
                 }
             } while (!foundpattern && GAME.frameno < 100);
             break;

         case '4':

             GAME.PrintFrame(GAME);
             do
             {
                 GAME.ApplyRulesParallel();
                 GAME.PrintFrame(GAME);
                 if ( GAME.checkForLWSS() || GAME.checkForGlider() )
                 {
                     foundpattern = true;
                     cout << "\n Glider or LWSS detected at Frame : " << GAME.frameno - 1 << endl;
                 }
             } while (!foundpattern && GAME.frameno < 105);
             break;

         case '6':
             return 0;
             break;

         }

         if (foundpattern == false && response != '6')
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

