
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <cstring>      // For std::memset()
#include <vector>
#include <bitset>

#include "../include/tdoku.h"
#include "all_solvers.h"
#include "build_info.h"
#include "klib/ketopt.h"
#include "util.h"

#include "TestCase.h"
#include "CPUWarmUp.h"
#include "StopWatch.h"

// Index: [0 - 4]
#define TEST_CASE_INDEX		4

double calc_percent(size_t num_val, size_t num_total) {
    if (num_total != 0)
        return (num_val * 100.0) / num_total;
    else
        return 0.0;
}

template <size_t TotalSize>
struct SudokuBoard {
    const char * board[TotalSize];
};

template <size_t nPalaceRows = 3, size_t nPalaceCols = 3,
          size_t nPalaceCountX = 3, size_t nPalaceCountY = 3,
          size_t nMinNumber = 1, size_t nMaxNumber = 9>
struct BasicSudoku {
    static const size_t PalaceRows = nPalaceRows;       // 3
    static const size_t PalaceCols = nPalaceCols;       // 3
    static const size_t PalaceCountX = nPalaceCountX;   // 3
    static const size_t PalaceCountY = nPalaceCountY;   // 3
    static const size_t MinNumber = nMinNumber;         // 1
    static const size_t MaxNumber = nMaxNumber;         // 9

    static const size_t Rows = PalaceRows * PalaceCountY;
    static const size_t Cols = PalaceCols * PalaceCountX;
    static const size_t Palaces = PalaceCountX * PalaceCountY;
    static const size_t Numbers = (MaxNumber - MinNumber) + 1;

    static const size_t PalaceSize = PalaceRows * PalaceCols;
    static const size_t BoardSize = Rows * Cols;
    static const size_t TotalSize = PalaceSize * Palaces * Numbers;

    static const size_t TotalSize2 = Rows * Cols * Numbers;

    static const size_t TotalConditions0 = 0;
    static const size_t TotalConditions1 = Rows * Cols;
    static const size_t TotalConditions2 = Rows * Numbers;
    static const size_t TotalConditions3 = Cols * Numbers;
    static const size_t TotalConditions4 = Palaces * Numbers;

    static const size_t TotalConditions01 = TotalConditions0  + TotalConditions1;
    static const size_t TotalConditions02 = TotalConditions01 + TotalConditions2;
    static const size_t TotalConditions03 = TotalConditions02 + TotalConditions3;
    static const size_t TotalConditions04 = TotalConditions03 + TotalConditions4;

    static const size_t TotalConditions =
        TotalConditions1 + TotalConditions2 + TotalConditions3 + TotalConditions4;

    static void clear_board(char board[BasicSudoku::BoardSize]) {
        size_t pos = 0;
        for (size_t row = 0; row < Rows; row++) {
            for (size_t col = 0; col < Cols; col++) {
                board[pos++] = '.';
            }
        }
    }

    static void clear_board(std::vector<std::vector<char>> & board) {
        for (size_t row = 0; row < board.size(); row++) {
            std::vector<char> & line = board[row];
            for (size_t col = 0; col < line.size(); col++) {
                line[col] = '.';
            }
        }
    }

    static void display_board(char board[BasicSudoku::BoardSize],
                              bool is_input = false,
                              int idx = -1) {
        int filled = 0;
        size_t pos = 0;
        for (size_t row = 0; row < Rows; row++) {
            for (size_t col = 0; col < Cols; col++) {
                if (board[pos] != '.')
                    filled++;
            }
        }

        if (is_input) {
            printf("The input is: (filled = %d)\n", filled);
        }
        else {
            if (idx == -1)
                printf("The answer is:\n");
            else
                printf("The answer # %d is:\n", idx + 1);
        }
        printf("\n");
        // printf("  ------- ------- -------\n");
        printf(" ");
        for (size_t countX = 0; countX < PalaceCountX; countX++) {
            printf(" -------");
        }
        printf("\n");
        pos = 0;
        for (size_t row = 0; row < Rows; row++) {
            printf(" | ");
            for (size_t col = 0; col < Cols; col++) {
                char val = board[pos++];
                if (val == ' ' || val == '0' || val == '-')
                    printf(". ");
                else
                    printf("%c ", val);
                if ((col % PalaceCols) == (PalaceCols - 1))
                    printf("| ");
            }
            printf("\n");
            if ((row % PalaceRows) == (PalaceRows - 1)) {
                // printf("  ------- ------- -------\n");
                printf(" ");
                for (size_t countX = 0; countX < PalaceCountX; countX++) {
                    printf(" -------");
                }
                printf("\n");
            }
        }
        printf("\n");
    }

    static void display_board(const std::vector<std::vector<char>> & board,
                              bool is_input = false,
                              int idx = -1) {
        int filled = 0;
        for (size_t row = 0; row < board.size(); row++) {
            const std::vector<char> & line = board[row];
            for (size_t col = 0; col < line.size(); col++) {
                if (line[col] != '.')
                    filled++;
            }
        }

        if (is_input) {
            printf("The input is: (filled = %d)\n", filled);
        }
        else {
            if (idx == -1)
                printf("The answer is:\n");
            else
                printf("The answer # %d is:\n", idx + 1);
        }
        printf("\n");
        // printf("  ------- ------- -------\n");
        printf(" ");
        for (size_t countX = 0; countX < PalaceCountX; countX++) {
            printf(" -------");
        }
        printf("\n");
        for (size_t row = 0; row < Rows; row++) {
            assert(board.size() >= Rows);
            printf(" | ");
            for (size_t col = 0; col < Cols; col++) {
                assert(board[row].size() >= Cols);
                char val = board[row][col];
                if (val == ' ' || val == '0' || val == '-')
                    printf(". ");
                else
                    printf("%c ", val);
                if ((col % PalaceCols) == (PalaceCols - 1))
                    printf("| ");
            }
            printf("\n");
            if ((row % PalaceRows) == (PalaceRows - 1)) {
                // printf("  ------- ------- -------\n");
                printf(" ");
                for (size_t countX = 0; countX < PalaceCountX; countX++) {
                    printf(" -------");
                }
                printf("\n");
            }
        }
        printf("\n");
    }

    static void display_boards(std::vector<SudokuBoard<BasicSudoku::TotalSize>> & boards) {
        printf("Total answers: %d\n\n", (int)boards.size());
        int i = 0;
        for (auto board : boards) {
            BasicSudoku::display_board(board, false, i);
            i++;
        }
    }

    static void display_boards(std::vector<std::vector<std::vector<char>>> & boards) {
        printf("Total answers: %d\n\n", (int)boards.size());
        int i = 0;
        for (auto board : boards) {
            BasicSudoku::display_board(board, false, i);
            i++;
        }
    }
};

// Standard sudoku definition
typedef BasicSudoku<3, 3, 3, 3, 1, 9> Sudoku;

void read_sudoku_board(char board[Sudoku::BoardSize], size_t index)
{
    for (size_t row = 0; row < Sudoku::Rows; row++) {
        size_t row_base = row * 9;
        size_t col = 0;
        const char * prows = test_case[index].rows[row];
        char val;
        while ((val = *prows) != '\0') {
            if (val >= '0' && val <= '9') {
                if (val != '0')
                    board[row_base + col] = val;
                else
                    board[row_base + col] = '.';
                col++;
                assert(col <= Sudoku::Cols);
            }
            else if (val == '.') {
                board[row_base + col] = '.';
                col++;
                assert(col <= Sudoku::Cols);
            }
            prows++;  
        }
        assert(col == Sudoku::Cols);
    }
}

size_t read_sudoku_board(char board[Sudoku::BoardSize], char line[256])
{
    char * pline = line;
    // Skip the white spaces
    while (*pline == ' ' || *pline == '\t') {
        pline++;
    }
    // Is a comment ?
    if ((*pline == '#') || ((*pline == '/') && (pline[1] = '/'))) {
        return 0;
    }
    size_t grid_nums = 0;
    for (size_t row = 0; row < Sudoku::Rows; row++) {
        size_t row_base = row * 9;
        size_t col_valid = 0;
        for (size_t col = 0; col < Sudoku::Cols; col++) {
            char val = *pline;
            if (val >= '0' && val <= '9') {
                if (val != '0')
                    board[row_base + col_valid] = val;
                else
                    board[row_base + col_valid] = '.';
                col_valid++;
                assert(col <= Sudoku::Cols);
            }
            else if (val == '.') {
                board[row_base + col_valid] = '.';
                col_valid++;
                assert(col <= Sudoku::Cols);
            }
            else if (val == '\0') {
                break;
            }
            pline++;  
        }
        assert(col_valid == Sudoku::Cols || col_valid == 0);
        grid_nums += col_valid;
    }
    return grid_nums;
}

void run_a_testcase(size_t index)
{
    double elapsed_time = 0.0;
    if (1)
    {
        printf("--------------------------------\n\n");
        printf("tdoku\n\n");

        char board[Sudoku::BoardSize];
        read_sudoku_board(board, index);

        Sudoku::display_board(board, true);

        char solution[Sudoku::BoardSize]{0};

        jtest::StopWatch sw;
        sw.start();

        size_t num_guesses = 0;
        size_t num_solutions = TdokuSolverDpllTriadSimd(board, 1, 0, solution, &num_guesses);

        sw.stop();
        double elapsed_time = sw.getElapsedMillisec();

		Sudoku::display_board(board);

        printf("elapsed time: %0.3f ms, init_counter: %" PRIuPTR ", recur_counter: %" PRIuPTR "\n\n"
               "num_guesses: %" PRIuPTR ", num_impossibles: %" PRIuPTR ", no_guess: %" PRIuPTR "\n"
               "guess %% = %0.1f %%, impossible %% = %0.1f %%, no_guess %% = %0.1f %%\n\n",
               elapsed_time, (size_t)0, num_guesses,
               num_guesses,
               (size_t)0,
               (size_t)0,
               100.0,
               0.0,
               0.0);
    }
}

void run_tdoku_test(const char * filename)
{
    printf("--------------------------------\n\n");
    printf("tdoku\n\n");

    size_t total_num_guesses = 0;
    size_t total_num_no_guess = 0;
    size_t total_num_impossibles = 0;

    size_t puzzleCount = 0;
    double total_time = 0.0;

    std::ifstream ifs;
    try {
        ifs.open(filename, std::ios::in);
        if (ifs.good()) {
            while (!ifs.eof()) {
                char line[256];
                std::memset(line, 0, 16);
                ifs.getline(line, sizeof(line) - 1);

                char board[Sudoku::BoardSize];
                size_t grid_nums = read_sudoku_board(board, line);
                // Sudoku::BoardSize = 81
                if (grid_nums >= Sudoku::BoardSize) {
                    char solution[Sudoku::BoardSize]{0};

        			jtest::StopWatch sw;
        			sw.start();
			        size_t num_guesses = 0;
			        size_t num_solutions = TdokuSolverDpllTriadSimd(board, 1, 0, solution, &num_guesses);

        			sw.stop();
        			double elapsed_time = sw.getElapsedMillisec();

                    total_time += elapsed_time;
                    if (num_solutions != 0) {
                        total_num_guesses += num_guesses;
                        total_num_no_guess += 0;
                        total_num_impossibles += 0;

                        puzzleCount++;
#ifndef NDEBUG
                        if (puzzleCount > 100)
                            break;
#endif
                    }
                }
                else break;
            }
            ifs.close();
        }
    }
    catch (std::exception & ex) {
        std::cout << "Exception info: " << ex.what() << std::endl << std::endl;
    }

    size_t recur_counter = total_num_guesses + total_num_no_guess + total_num_impossibles;
    double no_guess_percent = calc_percent(total_num_no_guess, recur_counter);
    double impossibles_percent = calc_percent(total_num_impossibles, recur_counter);
    double guesses_percent = calc_percent(total_num_guesses, recur_counter);

    printf("Total puzzle count = %u\n\n", (uint32_t)puzzleCount);
    printf("Total elapsed time: %0.3f ms\n\n", total_time);
    printf("recur_counter: %" PRIuPTR "\n\n"
           "num_guesses: %" PRIuPTR ", num_impossibles: %" PRIuPTR ", no_guess: %" PRIuPTR "\n"
           "guess %% = %0.1f %%, impossible %% = %0.1f %%, no_guess %% = %0.1f %%\n\n",
           recur_counter,
           total_num_guesses, total_num_impossibles, total_num_no_guess,
           guesses_percent, impossibles_percent, no_guess_percent);

    if (puzzleCount != 0) {
        printf("%0.1f usec/puzzle, %0.2f guesses/puzzle, %0.1f puzzles/sec\n\n",
               total_time * 1000.0 / puzzleCount,
               (double)total_num_guesses / puzzleCount,
               puzzleCount / (total_time / 1000.0));
    }
}

int main(int argc, char * argv[])
{
    const char * filename = nullptr;
    const char * out_file = nullptr;
    if (argc > 2) {
        filename = argv[1];
        out_file = argv[2];
    }
    else if (argc > 1) {
        filename = argv[1];
    }

    jtest::CPU::warmup(1000);

    if (1)
    {
        if (filename == nullptr) {
            run_a_testcase(TEST_CASE_INDEX);
        }
    }

    if (1)
    {
        if (filename != nullptr) {
            run_tdoku_test(filename);
        }
    }

#if !defined(NDEBUG) && defined(_MSC_VER)
    ::system("pause");
#endif

    return 0;
}
