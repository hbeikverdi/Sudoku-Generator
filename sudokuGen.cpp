#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#define UNASSIGNED 0

using namespace std;

class Sudoku {
private:
  int grid[9][9];
  int guessNum[9];
  int gridPos[81];
  int difficultyLevel;

public:
  Sudoku ();
  void createSeed();
  void printGrid();
  bool solveGrid();
  void countSoln(int &number);
  void genPuzzle();
  void printSVG(string);
  void calculateDifficulty();
  int  branchDifficultyScore();
};


// START: Generate random number
int genRandNum(int maxLimit)
{
  return rand()%maxLimit;
}
// END: Generate random number


// START: Create seed grid
void Sudoku::createSeed()
{
  this->solveGrid();
}
// END: Create seed grid


// START: Intialising
Sudoku::Sudoku()
{

  // initialize difficulty level
  this->difficultyLevel = 0;

  // Randomly shuffling the array of removing grid positions
  for(int i=0;i<81;i++)
  {
    this->gridPos[i] = i;
  }

  random_shuffle(this->gridPos, (this->gridPos) + 81, genRandNum);

  // Randomly shuffling the guessing number array
  for(int i=0;i<9;i++)
  {
    this->guessNum[i]=i+1;
  }

  random_shuffle(this->guessNum, (this->guessNum) + 9, genRandNum);

  // Initialising the grid
  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      this->grid[i][j]=0;
    }
  }

}
// END: Initialising


// START: Printing the grid
void Sudoku::printGrid()
{
  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      if(grid[i][j] == 0)
	cout<<".";
      else
	cout<<grid[i][j];
      cout<<"|";
    }
    cout<<endl;
  }

  cout<<"\nDifficulty of current sudoku(0 being easiest): "<<this->difficultyLevel;
  cout<<endl;
}
// END: Printing the grid


// START: Helper functions for solving grid
bool FindUnassignedLocation(int grid[9][9], int &row, int &col)
{
    for (row = 0; row < 9; row++)
    {
        for (col = 0; col < 9; col++)
        {
            if (grid[row][col] == UNASSIGNED)
                return true;
        }
    }

    return false;
}

bool UsedInRow(int grid[9][9], int row, int num)
{
    for (int col = 0; col < 9; col++)
    {
        if (grid[row][col] == num)
            return true;
    }

    return false;
}

bool UsedInCol(int grid[9][9], int col, int num)
{
    for (int row = 0; row < 9; row++)
    {
        if (grid[row][col] == num)
            return true;
    }

    return false;
}

bool UsedInBox(int grid[9][9], int boxStartRow, int boxStartCol, int num)
{
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            if (grid[row+boxStartRow][col+boxStartCol] == num)
                return true;
        }
    }

    return false;
}

bool isSafe(int grid[9][9], int row, int col, int num)
{
    return !UsedInRow(grid, row, num) && !UsedInCol(grid, col, num) && !UsedInBox(grid, row - row%3 , col - col%3, num);
}

// END: Helper functions for solving grid


// START: Modified and improved Sudoku solver
bool Sudoku::solveGrid()
{
    int row, col;

    // If there is no unassigned location, we are done
    if (!FindUnassignedLocation(this->grid, row, col))
       return true; // success!

    // Consider digits 1 to 9
    for (int num = 0; num < 9; num++)
    {
        // if looks promising
        if (isSafe(this->grid, row, col, this->guessNum[num]))
        {
            // make tentative assignment
            this->grid[row][col] = this->guessNum[num];

            // return, if success, yay!
            if (solveGrid())
                return true;

            // failure, unmake & try again
            this->grid[row][col] = UNASSIGNED;
        }
    }

    return false; // this triggers backtracking

}
// END: Modified and improved Sudoku Solver


// START: Check if the grid is uniquely solvable
void Sudoku::countSoln(int &number)
{
  int row, col;

  if(!FindUnassignedLocation(this->grid, row, col))
  {
    number++;
    return ;
  }


  for(int i=0;i<9 && number<2;i++)
  {
      if( isSafe(this->grid, row, col, this->guessNum[i]) )
      {
        this->grid[row][col] = this->guessNum[i];
        countSoln(number);
      }

      this->grid[row][col] = UNASSIGNED;
  }

}
// END: Check if the grid is uniquely solvable


// START: Gneerate puzzle
void Sudoku::genPuzzle()
{
  for(int i=0;i<81;i++)
  {
    int x = (this->gridPos[i])/9;
    int y = (this->gridPos[i])%9;
    int temp = this->grid[x][y];
    this->grid[x][y] = UNASSIGNED;

    // If now more than 1 solution , replace the removed cell back.
    int check=0;
    countSoln(check);
    if(check!=1)
    {
      this->grid[x][y] = temp;
    }
  }
}
// END: Generate puzzle


// START: Printing into SVG file
void Sudoku::printSVG(string path="")
{
  string fileName = path + "svgHead.txt";
  ifstream file1(fileName.c_str());
  stringstream svgHead;
  svgHead << file1.rdbuf();

  ofstream outFile("puzzle.svg");
  outFile << svgHead.rdbuf();

  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
      if(this->grid[i][j]!=0)
      {
        int x = 50*j + 16;
        int y = 50*i + 35;

        stringstream text;
        text<<"<text x=\""<<x<<"\" y=\""<<y<<"\" style=\"font-weight:bold\" font-size=\"30px\">"<<this->grid[i][j]<<"</text>\n";

        outFile << text.rdbuf();
      }
    }
  }

    outFile << "<text x=\"50\" y=\"500\" style=\"font-weight:bold\" font-size=\"15px\">Difficulty Level (0 being easiest): "             <<this->difficultyLevel<<"</text>\n";
    outFile << "</svg>";

}
// END: Printing into SVG file


// START: Calculate branch difficulty score
int Sudoku::branchDifficultyScore()
{
  return 0;
}
// END: Finish branch difficulty score


// START: Calculate difficulty level of current grid
void Sudoku::calculateDifficulty()
{
  int B = branchDifficultyScore();
  int emptyCells = 0;

  for(int i=0;i<9;i++)
  {
    for(int j=0;j<9;j++)
    {
	if(this->grid[i][j] == 0)
	   emptyCells++;
    }
  } 

  this->difficultyLevel = B*100 + emptyCells;
}
// END: calculating difficulty level


// START: The main function
int main(int argc, char const *argv[])
{
  // Initialising seed for random number generation
  srand(time(NULL));

  // Creating an instance of Sudoku
  Sudoku *puzzle = new Sudoku();

  // Creating a seed for puzzle generation
  puzzle->createSeed();

  // Generating the puzzle
  puzzle->genPuzzle();

  // Calculating difficulty of puzzle
  puzzle->calculateDifficulty();

  // testing by printing the grid
  puzzle->printGrid();

  // Printing the grid into SVG file
  string rem = "sudokuGen";
  string path = argv[0];
  path = path.substr(0,path.size() - rem.size());
  puzzle->printSVG(path);
  cout<<"The above sudoku puzzle has been stored in puzzles.svg in current folder\n";
  // freeing the memory
  delete puzzle;

  return 0;
}
// END: The main function
