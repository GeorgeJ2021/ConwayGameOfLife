#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
using namespace std;

// change row and column value to set the canvas size
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
        
        //setup first frame
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
        //end of array construstion

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
                    //file >> a[i][j];
                    a[i][j] = ' ';
                }           
                file >> a[i][j];
                //b[i][j] = a[i][j];
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

    template <typename T>
    bool checkEqual(T a, T b) {
        return a == b;
    }

    int ApplyRules(char a[maxrow][maxcol], int r, int c) //counts living cell neighbours
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
                int neighbour_live_cell = ApplyRules(a, i, j);
                if (a[i][j] == 'O' && (neighbour_live_cell == 2 || neighbour_live_cell == 3)) {
                    c[i][j] = 'O'; // Remains alive
                }
                else if (a[i][j] == ' ' && neighbour_live_cell == 3) {
                    c[i][j] = 'O'; // Comes to life
                }
                else {
                    c[i][j] = ' '; // Dies
                }
            }
        }
    }

    void ApplyRulesParallel() {
        int numThreads = 4;  // Number of threads to use
        int chunkSize = row / numThreads;  // Divide grid into chunks

        vector<thread> threads;

        for (int i = 0; i < numThreads; i++) {
            int startRow = i * chunkSize;
            int endRow = (i == numThreads - 1) ? row : (i + 1) * chunkSize;

            // Launch threads to update grid sections
            threads.push_back(thread(&ConwayGame::UpdateGridSection, this, startRow, endRow));
        }

        // Join all threads to ensure they complete before proceeding
        for (auto& th : threads) {
            th.join();
        }

        // After parallel computation, update the main grid 'a'
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                a[i][j] = c[i][j];
            }
        }
    }

    int row_line()
    {
        //cout << endl;
        for (int i = 0; i < col+1; i++) {
            cout << ". ";
        }
        cout << endl;
        return 0;
    }

    void PrintFrame()
    {
        cout << "\nFrame : "<< frameno << endl;
        row_line();
        for (i = 0; i < row; i++) {
            cout << ".";
            for (j = 0; j < col; j++) {
                cout<< a[i][j]<<".";
            }
            cout << endl;
            //row_line();
        }
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

        file << row << " " << col << " " << maxframe << endl;  // Save dimensions and remaining frames

        // Save the grid
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

};

void newgame();

 int SimulationOperation (ConwayGame& G1)
{
   
    char response;
    int currentFrame = 0;
    G1.PrintFrame();

    do
    {
        G1.ApplyRulesParallel();
        G1.PrintFrame();


    } while (!(G1.checkEqual(G1.maxframe + 1, G1.frameno)));

    do {
        cout << "\n 1. Resume current simulation \n 2. Reset current simulation \n 3. Restart with new simulation \n 4. save current Frame \n 5. Back to main menu";
        cout << "\n awaiting input : ";
        cin >> response;
        cout << endl;

        if (response == '1')
        {
            G1.ApplyRulesParallel();
            G1.PrintFrame();
        }

        if (response == '2')
        {
            cout << "\n resetting simulation" << endl;
            G1.resetsimulation();
            G1.PrintFrame();
        }

        if (response == '5')
        {
            cout << "\n BACK TO MAIN MENU \n______________" << endl;
            return 0;
        }

        if (response == '3')
        {
            cout << "\n Loading new simulation..." << endl;
            G1.~ConwayGame();
            newgame();
        }

        if (response == '4')
        {
            string filename;
            cout << "\n Enter filename to save the current frame: ";
            cin >> filename;
            G1.savefile(filename);
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
     SimulationOperation(Game);
     //SimulationOperation(Game);
 }

int main()
{
    srand(time(0));
    char userinput;
    string filename;
    //ConwayGame g1;
    cout << "\n CONWAY'S GAME OF LIFE";
    cout << "\n_______________" << endl;
    //cout << "\n 1. Start new game  \n 2. Load Game \n 3. Quit Game" << endl;
    do
    {
        cout << "\n 1. Start new game  \n 2. Load Game \n 3. Quit Game" << endl;
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
            // Load existing file
        case '3':
            break;
            // Quit

        }

    }while (userinput != '3');

    cout << "\n Ending Game ";

    return 0;
}

