#include <iostream>
#include <cstdlib> 
#include <ctime>
using namespace std;

// change row and column value to set the canvas size
const int maxrow = 50;
const int maxcol = 50;

class ConwayGame
{
private:

    char a[maxrow][maxcol] , b[maxrow][maxcol];
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
    ~ConwayGame()
    {

    }

    template <typename T>
    bool checkFrameEqual(T maxframe, T frameno) {
        return maxframe == frameno;
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

    int ApplyRules()
    {
        for (i = 0; i < row; i++) {
            for (j = 0; j < col; j++) {
                neighbour_live_cell
                    = ApplyRules(a, i, j);
                if (a[i][j] == 'O'
                    && (neighbour_live_cell == 2
                        || neighbour_live_cell == 3)) {
                    a[i][j] = 'O';
                }

                else if (a[i][j] == ' '
                    && neighbour_live_cell == 3) {
                    a[i][j] = 'O';
                }

                else {
                    a[i][j] = ' ';
                }
            }
        }
        return 0;
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
};

int newgame()
{
    char response;
    int currentFrame = 0;
    ConwayGame G1;
    G1.PrintFrame();
    do
    {
        G1.ApplyRules();
        G1.PrintFrame();


    } while (!(G1.checkFrameEqual(G1.maxframe+1,G1.frameno)));

    do {
        cout << "\n 1. Resume current simulation \n 2. Reset current simulation \n 3. Restart with new simulation \n 4. Back to menu";
        cout << "\n awaiting input : ";
        cin >> response;
        cout << endl;

        if (response == '1')
        {
                G1.ApplyRules();
                G1.PrintFrame();
        }

        if (response == '2')
        {
            cout << "\n resetting simulation" << endl;
            G1.resetsimulation();
            G1.PrintFrame();
        }

        if (response == '4')
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

    } while (response != '4');

}



int main()
{
    srand(time(0));
    char userinput;
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
            // Load existing file
        case '3':
            break;
            // Quit

        }

    }while (userinput != '3');

    cout << "\n Ending Game ";

    return 0;
}

