#include <iostream>
#include <cstdlib> 
#include <ctime>
using namespace std;

// change row and column value to set the canvas size
const int row = 10;
const int col = 10;


// creates row boundary
/*int row_line()
{
    cout << endl;
    for (int i = 0; i < col; i++) {
        cout << " -----";
    }
    cout << endl;
    return 0;
}*/

// returns the count of alive neighbours
int count_live_neighbour_cell(char a[row][col], int r, int c)
{
    int i, j, count = 0;
    for (i = r - 1; i <= r + 1; i++) {
        for (j = c - 1; j <= c + 1; j++) {
            if ((i == r && j == c) || (i < 0 || j < 0)
                || (i >= row || j >= col)) {
                continue;
            }
            if (a[i][j] == 'O') {
                count++;
            }
        }
    }
    return count;
}

int main()
{
    char a[row][col], b[row][col];
    int i, j, tempval, livecount=20;
    int neighbour_live_cell;
    srand(time(0));

    // generate matrix canvas with random values (live and
    // dead cells)
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            tempval = rand() % 7;
            if (tempval == 1 && (livecount>0))
            {
                a[i][j] = 'O';
                livecount--;

            }
            else 
            {
                a[i][j] = '.';
            }
  
        }
    }

    // print array matrix
    cout << "Initial Stage:" << endl;
    //row_line();
    for (i = 0; i < row; i++) {
        cout << " ";
        for (j = 0; j < col; j++) {
            cout << "  " << a[i][j] << " ";
        }
        cout << endl;
        //row_line();
    }

    // next canvas values based on live neighbour count
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            neighbour_live_cell
                = count_live_neighbour_cell(a, i, j);
            if (a[i][j] == 'O'
                && (neighbour_live_cell == 2
                    || neighbour_live_cell == 3)) {
                a[i][j] = 'O';
            }

            else if (a[i][j] == '.'
                && neighbour_live_cell == 3) {
                a[i][j] = 'O';
            }

            else {
                a[i][j] = '.';
            }
        }
    }

    // print next generation
    cout << "\nNext Generation:" << endl;
    //row_line();
    for (i = 0; i < row; i++) {
        cout << " ";
        for (j = 0; j < col; j++) {
            cout << "  " << a[i][j] << " ";
        }
        cout << endl;
        //row_line();
    }


    ////////////////////////////

    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            neighbour_live_cell
                = count_live_neighbour_cell(a, i, j);
            if (a[i][j] == 'O'
                && (neighbour_live_cell == 2
                    || neighbour_live_cell == 3)) {
                a[i][j] = 'O';
            }

            else if (a[i][j] == '.'
                && neighbour_live_cell == 3) {
                a[i][j] = 'O';
            }

            else {
                a[i][j] = '.';
            }
        }
    }

    // print next generation
    cout << "\nNext Generation:"<<endl;
    //row_line();
    for (i = 0; i < row; i++) {
        cout << " ";
        for (j = 0; j < col; j++) {
            cout << "  " << a[i][j] << " ";
        }
        //row_line();
        cout << endl;
    }

    return 0;
}
