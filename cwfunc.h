#ifndef CWFUNC_H
#define CWFUNC_H

#define BOARD_SIZE 15
#define MAX_WORDS 20
#define MAX_WORD_LEN 16

typedef enum {
    ACROSS,
    DOWN
} Direction;

typedef struct {
    char word[MAX_WORD_LEN];
    char clue[MAX_WORD_LEN];
    int row;
    int col;
    Direction dir;
    int placed;
} WordInfo;

typedef struct {
    WordInfo words[MAX_WORDS];
    int count;
} WordList;

typedef struct {
    char solution[BOARD_SIZE][BOARD_SIZE];
    char puzzle[BOARD_SIZE][BOARD_SIZE];
} Board;

// 单词处理函数
int readWords(WordList *wordList, const char *filename);
void toUpperCase(char *str);
int isValidWord(const char *word);
void sortWordsByLength(WordList *wordList);

// 棋盘操作函数
void initBoard(Board *board);
void printBoard(const Board *board, int isSolution);
int placeWord(Board *board, const char *word, int row, int col, Direction dir);

// 核心算法函数
int placeFirstWord(Board *board, const char *word);
int tryPlaceWord(Board *board, const char *word,int *row ,int *col,Direction *dirs);
void generateCrossword(Board *board, WordList *wordList);

// 线索生成函数
void generateClue(const char *word, char *clue);
void generateAllClues(WordList *wordList);

// 输出函数
void printClues(const WordList *wordList);
void saveOutput(const Board *board, const WordList *wordList, const char *filename);

#endif