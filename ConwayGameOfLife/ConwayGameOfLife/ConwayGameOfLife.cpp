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

    char a[maxrow][maxcol];
    int row, col, frameno, i, j, tempval, livecount;
    int neighbour_live_cell;

public:

    ConwayGame()
    {
        cout << "\nEnter row size: ";
        cin >> row;
        cout << "\nEnter col size: ";
        cin >> col;
        cout << "\nEnter number of living cells: ";
        cin >> livecount;
        cout << "\nEnter number of frames: ";
        cin >> frameno;
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

            }
        }
        //end of array construstion

    }
    ~ConwayGame()
    {

    }

    int getframeno()
    {
        return frameno;
    }

    int count_live_neighbour_cell(char a[maxrow][maxcol], int r, int c)
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

    void ApplyRules()
    {
        for (i = 0; i < row; i++) {
            for (j = 0; j < col; j++) {
                neighbour_live_cell
                    = count_live_neighbour_cell(a, i, j);
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
        if (frameno > 0)
        {
            frameno--;
        }
        
    }


};

int newgame()
{
    char response;
    ConwayGame G1;
    cout << "\n 1.Resume \n 2.Restart \n 3. Back to menu";
    G1.PrintFrame();
    do
    {
        G1.ApplyRules();
        G1.PrintFrame();
        cout << "\n awaiting input : ";
        cin >> response;
        cout << endl;

    } while (G1.getframeno() >= 0 && (response == '1'));

    if (response == '3')
    {
        cout << "\n BACK TO MAIN MENU \n______________" << endl;
        return 0;
    }

    if (response == '2')
    {
        cout << "\n Restarting..." << endl;
        G1.~ConwayGame();
        newgame();
    }

    return 0;

}






int main()
{
    srand(time(0));
    char userinput;
    cout << "\n CONWAY'S GAME OF LIFE";
    cout << "\n_______________" << endl;;
    cout << "\n 1. Start new game  \n 2. Load Game \n 3. Quit Game" << endl;

    do
    {
        cout << "\n Awaiting input : ";
        cin >> userinput;
        switch (userinput) {

        case '1':
            cout << "Hello world" << endl;
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

