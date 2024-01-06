#include <vector>
#include <set>
#include <unordered_set>
#include <random>
#include <iterator>
#include <iostream>
#include <numeric>
#include <algorithm>
using namespace std;

class Solution {
public:
    bool solveSudoku(vector<vector<char>>& board) {
        vector<unordered_set<unsigned>> rowDigits(9), colDigits(9);
        vector<vector<unordered_set<unsigned>>> boxDigits(3, vector<unordered_set<unsigned>>(3));
        const size_t n = board.size();
        for (size_t row = 0; row < n; row++) {
            for (size_t col = 0; col < n; col++) {
                unsigned digit = unsigned(board[row][col]);
                if (digit != '.') {
                    rowDigits[row].insert(digit - unsigned('0'));
                    colDigits[col].insert(digit - unsigned('0'));
                    boxDigits[row / 3][col / 3].insert(digit - unsigned('0'));
                }
            }
        }
        return backtrack(board, rowDigits, colDigits, boxDigits);
    }

    bool backtrack(vector<vector<char>>& board, vector<unordered_set<unsigned>>& rowDigits, vector<unordered_set<unsigned>>& colDigits, vector<vector<unordered_set<unsigned>>>& boxDigits, size_t startRow = 0, size_t startCol = 0) {
        const size_t n = board.size();
        for (size_t row = startRow; row < n; row++) {
            for (size_t col = startCol; col < n; col++) {
                if (board[row][col] == '.') {
                    for (unsigned digit = 1; digit <= 9; digit++) {
                        if (!isValid(digit, row, col, rowDigits, colDigits, boxDigits)) {
                            continue;
                        }

                        board[row][col] = char('0' + digit);
                        rowDigits[row].insert(digit);
                        colDigits[col].insert(digit);
                        boxDigits[row / 3][col / 3].insert(digit);

                        if (backtrack(board, rowDigits, colDigits, boxDigits, row, col)) {
                            return true;
                        }

                        // Backtrack
                        board[row][col] = '.';
                        rowDigits[row].erase(digit);
                        colDigits[col].erase(digit);
                        boxDigits[row / 3][col / 3].erase(digit);
                    }
                    return false;
                }
            }
            startCol = 0;
        }
        return true;
    }

    bool isValid(unsigned digit, size_t row, size_t col, vector<unordered_set<unsigned>>& rowDigits, vector<unordered_set<unsigned>>& colDigits, vector<vector<unordered_set<unsigned>>>& boxDigits) {
        return rowDigits[row].count(digit) == 0 &&
               colDigits[col].count(digit) == 0 &&
               boxDigits[row / 3][col / 3].count(digit) == 0;
    }
};


void printBoard(const vector<vector<char>>& board) {
    for (const auto& row : board) {
        for (char c : row) {
            cout << c << " ";
        }
        cout << endl;
    }
}

class Generator {
    public:
    vector<vector<char>> generateSudoku() {
        const size_t n = 9;
        vector<vector<char>> board = vector<vector<char>>(n, vector<char>(n, '.'));
        vector<unordered_set<unsigned>> rowDigits(9), colDigits(9);
        vector<vector<unordered_set<unsigned>>> boxDigits(3, vector<unordered_set<unsigned>>(3));
        backtrack(board, rowDigits, colDigits, boxDigits);
        return board;
    }

    vector<vector<char>> generatePuzzle() {
        resetCaches();
        vector<vector<char>> board = generateSudoku();
        while (removeRandomSquare(board)) {
            continue;
        }
        return board;
    }

    void generatePuzzle(vector<vector<char>> &board) {
        resetCaches();
        while (removeRandomSquare(board)) {
            continue;
        }
    }

private:
    set<tuple<size_t, size_t>> cantRemove;
    unordered_set<size_t> solvableBoards; // Use BoardHash here


    void resetCaches() {
        cantRemove.clear();
        solvableBoards.clear();
    }

    bool backtrack(vector<vector<char>>& board, vector<unordered_set<unsigned>>& rowDigits, vector<unordered_set<unsigned>>& colDigits, vector<vector<unordered_set<unsigned>>>& boxDigits, size_t startRow = 0, size_t startCol = 0) {
        const size_t n = board.size();
        for (size_t row = startRow; row < n; row++) {
            for (size_t col = startCol; col < n; col++) {
                startCol = 0;
                if (board[row][col] == '.') {
                    set<unsigned> digits = {1, 2, 3, 4, 5, 6, 7, 8, 9};
                    // get valid digits
                    for (unsigned digit = 1; digit <= 9; digit++) {
                        if (rowDigits[row].count(digit)) {
                            digits.erase(digit);
                        } else if (colDigits[col].count(digit)) {
                            digits.erase(digit);
                        } else if (boxDigits[row / 3][col / 3].count(digit)) {
                            digits.erase(digit);
                        }
                    }
                    while (!digits.empty()) {
                        unsigned digit = getRandomDigit(digits);
                        digits.erase(digit);

                        board[row][col] = char('0' + digit);
                        rowDigits[row].insert(digit);
                        colDigits[col].insert(digit);
                        boxDigits[row / 3][col / 3].insert(digit);

                        if (backtrack(board, rowDigits, colDigits, boxDigits, row, col)) {
                            return true;
                        }

                        // Backtrack
                        board[row][col] = '.';
                        rowDigits[row].erase(digit);
                        colDigits[col].erase(digit);
                        boxDigits[row / 3][col / 3].erase(digit);
                    }
                    return false;
                }
            }
            startCol = 0;
        }
        return true;
    }

    bool backtrackCountSolutions(vector<vector<char>>& board, vector<unordered_set<unsigned>>& rowDigits, vector<unordered_set<unsigned>>& colDigits, vector<vector<unordered_set<unsigned>>>& boxDigits, unsigned& solutions, size_t startRow = 0, size_t startCol = 0) {
        BoardHash hasher;
        size_t currentHash = hasher(board);
        if (solvableBoards.count(currentHash)) {
            solutions++;
            return true;
        }
        const size_t n = board.size();
        for (size_t row = startRow; row < n; row++) {
            for (size_t col = startCol; col < n; col++) {
                if (board[row][col] == '.') {
                    for (unsigned digit = 1; digit <= 9; digit++) {
                        if (!isValid(digit, row, col, rowDigits, colDigits, boxDigits)) {
                            continue;
                        }
                        board[row][col] = char('0' + digit);
                        rowDigits[row].insert(digit);
                        colDigits[col].insert(digit);
                        boxDigits[row / 3][col / 3].insert(digit);
                        if (backtrackCountSolutions(board, rowDigits, colDigits, boxDigits, solutions, row, col)) {
                            solvableBoards.insert(currentHash);
                        }

                        // Backtrack
                        board[row][col] = '.';
                        rowDigits[row].erase(digit);
                        colDigits[col].erase(digit);
                        boxDigits[row / 3][col / 3].erase(digit);
                    }
                    return false;
                }
            }
            startCol = 0;
        }
        solutions++;
        return true;
    }

    bool isValid(unsigned digit, size_t row, size_t col, vector<unordered_set<unsigned>>& rowDigits, vector<unordered_set<unsigned>>& colDigits, vector<vector<unordered_set<unsigned>>>& boxDigits) {
        return rowDigits[row].count(digit) == 0 &&
               colDigits[col].count(digit) == 0 &&
               boxDigits[row / 3][col / 3].count(digit) == 0;
    }

    unsigned getRandomDigit(const set<unsigned>& digits) const {
        random_device rd;  // Obtain a random number from hardware
        mt19937 gen(rd()); // Seed the generator
        uniform_int_distribution<size_t> distr(0u, digits.size() - 1); // Define the range

        auto it = digits.begin(); // Get iterator to the start of the set
        std::advance(it, distr(gen)); // Advance the iterator by a random number of steps
        return unsigned(*it);
    }

    bool removeRandomSquare(vector<vector<char>>& board) {
        const size_t n = board.size();
        random_device rd;  // Obtain a random number from hardware
        mt19937 gen(rd()); // Seed the generator
        uniform_int_distribution<size_t> distr(0u, n * n - 1); // Define the range
        
        vector<size_t> indices(n * n);
        iota(indices.begin(), indices.end(), 0); // Fill indices from 0 to n*n-1
        shuffle(indices.begin(), indices.end(), gen); // Shuffle the indices
        for (size_t idx : indices) {
            size_t row = idx / n;
            size_t col = idx % n;
            if (cantRemove.find(tuple(row, col)) != cantRemove.end()) {
                continue;
            }
            if (board[row][col] != '.') { // Check if the cell is not already empty
                char temp = board[row][col];
                board[row][col] = '.';
                if (countSolutions(board) == 1) {
                    return true; // Successfully removed a square without creating multiple solutions
                } else {
                    cantRemove.insert(tuple(row, col));
                }
                board[row][col] = temp; // Restore the original number, as removing it leads to multiple solutions
            }
        }
        return false; // No square could be removed without creating multiple solutions
    }

    unsigned countSolutions(vector<vector<char>>& board) {
        const size_t n = board.size();
        vector<unordered_set<unsigned>> rowDigits(n), colDigits(n);
        vector<vector<unordered_set<unsigned>>> boxDigits(n / 3, vector<unordered_set<unsigned>>(n / 3));
        for (size_t row = 0; row < n; row++) {
            for (size_t col = 0; col < n; col++) {
                unsigned digit = unsigned(board[row][col]);
                if (digit != '.') {
                    rowDigits[row].insert(digit - unsigned('0'));
                    colDigits[col].insert(digit - unsigned('0'));
                    boxDigits[row / 3][col / 3].insert(digit - unsigned('0'));
                }
            }
        }
        unsigned solutions = 0;
        backtrackCountSolutions(board, rowDigits, colDigits, boxDigits, solutions);
        return solutions;
    }

    struct BoardHash {
        size_t operator()(const vector<vector<char>>& board) const {
            std::string boardString;
            for (const auto& row : board) {
                for (char c : row) {
                    boardString.push_back(c);
                }
            }
            return std::hash<std::string>()(boardString);
        }
    };

};

int main() {
    vector<vector<char>> board = {
        {'.', '.', '9', '7', '4', '8', '.', '.', '.'},
        {'7', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '2', '.', '1', '.', '9', '.', '.', '.'},
        {'.', '.', '7', '.', '.', '.', '2', '4', '.'},
        {'.', '6', '4', '.', '1', '.', '5', '9', '.'},
        {'9', '8', '.', '.', '.', '.', '3', '.', '.'},
        {'.', '.', '.', '8', '.', '3', '.', '2', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '6'},
        {'.', '.', '.', '2', '7', '5', '9', '.', '.'}
    };

    Solution solution;
    solution.solveSudoku(board);

    cout << "Solved Sudoku:" << endl;
    printBoard(board);
    
    Generator generator;
    vector<vector<char>> board_new = generator.generateSudoku();
    cout << "\n\nGenerated Sudoku:" << endl;
    printBoard(board_new);
    cout << "\n\nGenerated Puzzle:" << endl;
    generator.generatePuzzle(board_new);
    printBoard(board_new);

    cout << "\n\nSolved Puzzle:" << endl;
    solution.solveSudoku(board_new);
    printBoard(board_new);
    return 0;
}
