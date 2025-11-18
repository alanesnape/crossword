#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cwfunc.h"

// ==================== 单词处理函数 ====================

int readWords(WordList *wordList, const char *filename) {
    FILE *fp = NULL;
    int count = 0;

    if (filename) {
        fp = fopen(filename, "r");
        if (!fp) return -1;
    }

    printf("Anagram Crossword Puzzle Generator\n");
    printf("----------------------------------\n");

    if (!filename) {
        printf("Enter a list of words:\n");
    }

    char buffer[MAX_WORD_LEN];
    while (count < MAX_WORDS) {
        if (filename) {
            if (fscanf(fp, "%15s", buffer) != 1) break;
        } else {
            if (scanf("%15s", buffer) != 1) break;
        }

        if (strcmp(buffer, ".") == 0) break;

        if (!isValidWord(buffer)) {
            printf("Invalid word '%s' - ignored\n", buffer);
            continue;
        }
        toUpperCase(buffer);
        strcpy(wordList->words[count].word, buffer);
        wordList->words[count].placed = 0;
        count++;
    }

    if (fp) fclose(fp);
    wordList->count = count;
    return count;
}

void toUpperCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int isValidWord(const char *word) {
    int len = strlen(word);
    if (len < 2 || len > 15) return 0;

    for (int i = 0; word[i]; i++) {
        if (!isalpha(word[i])) return 0;
    }
    return 1;
}

void sortWordsByLength(WordList *wordList) {
    for (int i = 0; i < wordList->count - 1; i++) {
        for (int j = 0; j < wordList->count - i - 1; j++) {
            int len1 = strlen(wordList->words[j].word);
            int len2 = strlen(wordList->words[j+1].word);
            if (len1 < len2) {
                WordInfo temp = wordList->words[j];
                wordList->words[j] = wordList->words[j+1];
                wordList->words[j+1] = temp;
            }
        }
    }
}

// ==================== 棋盘操作函数 ====================

void initBoard(Board *board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board->solution[i][j] = '.';
            board->puzzle[i][j] = '#';
        }
    }
}

void printBoard(const Board *board, int isSolution) {
    printf("-----------------\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("|");
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c", isSolution ? board->solution[i][j] : board->puzzle[i][j]);
        }
        printf("|\n");
    }
    printf("-----------------\n");
}

int placeWord(Board *board, const char *word, int row, int col, Direction dir) {
    int len = strlen(word);

    if (dir == ACROSS) {
        if (col + len > BOARD_SIZE) return 0;
    } else {
        if (row + len > BOARD_SIZE) return 0;
    }

    if (dir == ACROSS) {
        for (int i = 0; i < len; i++) {
            char existing = board->solution[row][col + i];
            if (existing != '.' && existing != word[i]) {
                return 0;
            }
        }
    } else {
        for (int i = 0; i < len; i++) {
            char existing = board->solution[row + i][col];
            if (existing != '.' && existing != word[i]) {
                return 0;
            }
        }
    }

    if (dir == ACROSS) {
        for (int i = 0; i < len; i++) {
            board->solution[row][col + i] = word[i];
            board->puzzle[row][col + i] = ' ';
        }
    } else {
        for (int i = 0; i < len; i++) {
            board->solution[row + i][col] = word[i];
            board->puzzle[row + i][col] = ' ';
        }
    }

    return 1;
}

// ==================== 核心算法函数 ====================

int checkAdjacentCell(Board *board, int r, int c, int isIntersection) {
    if (isIntersection) return 1;

    if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
        if (board->solution[r][c] != '.') {
            return 0;
        }
    }
    return 1;
}

int checkBoundary(int row, int col, int len, Direction dir) {
    if (dir == ACROSS) {
        return (col + len <= BOARD_SIZE);
    } else {
        return (row + len <= BOARD_SIZE);
    }
}

int checkIntersectionDistribution(Board *board, const char *word,
                                 int row, int col, Direction dir) {
    int len = strlen(word);

    for (int k1 = 0; k1 < len; k1++) {
        int r1 = (dir == ACROSS) ? row : row + k1;
        int c1 = (dir == ACROSS) ? col + k1 : col;

        if (board->solution[r1][c1] != '.' && board->solution[r1][c1] == word[k1]) {
            for (int k2 = k1 + 1; k2 < len; k2++) {
                int r2 = (dir == ACROSS) ? row : row + k2;
                int c2 = (dir == ACROSS) ? col + k2 : col;

                if (board->solution[r2][c2] != '.' && board->solution[r2][c2] == word[k2]) {
                    if (abs(r2 - r1) + abs(c2 - c1) == 1) {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

int tryPlaceWord(Board *board, const char *word,int *row ,int *col,Direction *dirs) {
    int len = strlen(word);

    for (int dir = ACROSS; dir <= DOWN; dir++) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (!checkBoundary(i, j, len, dir)) continue;

                int hasIntersection = 0;
                int validPlacement = 1;

                for (int k = 0; k < len && validPlacement; k++) {
                    int r = (dir == ACROSS) ? i : i + k;
                    int c = (dir == ACROSS) ? j + k : j;

                    char existing = board->solution[r][c];
                    int isIntersection = (existing != '.');

                    if (isIntersection) {
                        if (existing == word[k]) {
                            hasIntersection = 1;
                        } else {
                            validPlacement = 0;
                            break;
                        }
                    }

                    if (validPlacement) {
                        if (dir == ACROSS) {
                            if (!checkAdjacentCell(board, i-1, j+k, isIntersection) ||
                                !checkAdjacentCell(board, i+1, j+k, isIntersection)) {
                                validPlacement = 0;
                                break;
                            }
                            if (k == 0 && !checkAdjacentCell(board, i, j-1, 0)) {
                                validPlacement = 0;
                                break;
                            }
                            if (k == len - 1 && !checkAdjacentCell(board, i, j+len, 0)) {
                                validPlacement = 0;
                                break;
                            }
                        } else {
                            if (!checkAdjacentCell(board, i+k, j-1, isIntersection) ||
                                !checkAdjacentCell(board, i+k, j+1, isIntersection)) {
                                validPlacement = 0;
                                break;
                            }
                            if (k == 0 && !checkAdjacentCell(board, i-1, j, 0)) {
                                validPlacement = 0;
                                break;
                            }
                            if (k == len - 1 && !checkAdjacentCell(board, i+len, j, 0)) {
                                validPlacement = 0;
                                break;
                            }
                        }
                    }
                }

                if (validPlacement && hasIntersection) {
                    if (checkIntersectionDistribution(board, word, i, j, dir)) {
                        if (placeWord(board, word, i, j, dir)) {
                            *row =i;
                            *col = j;
                            *dirs = dir;
                            return 1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int placeFirstWord(Board *board, const char *word) {
    int len = strlen(word);
    int row = BOARD_SIZE / 2;
    int col = (BOARD_SIZE - len) / 2;

    if (placeWord(board, word, row, col, ACROSS)) {
        return 1;
    }
    return 0;
}

void generateCrossword(Board *board, WordList *wordList) {
    if (wordList->count > 0) {
        const char *firstWord = wordList->words[0].word;
        if (placeFirstWord(board, firstWord)) {
            wordList->words[0].placed = 1;
            wordList->words[0].row = BOARD_SIZE / 2;
            wordList->words[0].col = (BOARD_SIZE - strlen(firstWord)) / 2;
            wordList->words[0].dir = ACROSS;
            printf("Placed first word: %s\n", firstWord);
        }
    }

    for (int i = 1; i < wordList->count; i++) {
        int row, col;
        Direction dir;
        if (tryPlaceWord(board, wordList->words[i].word, &row, &col, &dir)) {
            wordList->words[i].placed = 1;
            wordList->words[i].row = row;
            wordList->words[i].col = col;
            wordList->words[i].dir = dir;
            printf("Placed word: %s at (%d,%d) %s\n",
                   wordList->words[i].word, row, col,
                   dir == ACROSS ? "Across" : "Down");
        } else {
            printf("Cannot place word: %s\n", wordList->words[i].word);
            break;
        }
    }
}

// ==================== 线索生成函数 ====================

void generateClue(const char *word, char *clue) {
    int len = strlen(word);
    char temp[MAX_WORD_LEN];
    strcpy(temp, word);

    for (int i = len - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char tmp = temp[i];
        temp[i] = temp[j];
        temp[j] = tmp;
    }

    strcpy(clue, temp);
}

void generateAllClues(WordList *wordList) {

    for (int i = 0; i < wordList->count; i++) {
        if (wordList->words[i].placed) {
            generateClue(wordList->words[i].word, wordList->words[i].clue);
        }
    }
}

// ==================== 输出函数 ====================

void printClues(const WordList *wordList) {
    printf("\nClues:\n\n");

    // 创建一个已放置单词的索引数组
    int placedIndices[MAX_WORDS];
    int placedCount = 0;

    // 收集所有已放置单词的索引
    for (int i = 0; i < wordList->count; i++) {
        if (wordList->words[i].placed) {
            placedIndices[placedCount++] = i;
        }
    }

    // 对索引数组进行随机洗牌
    for (int i = placedCount - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = placedIndices[i];
        placedIndices[i] = placedIndices[j];
        placedIndices[j] = temp;
    }

    // 按随机顺序输出线索
    for (int i = 0; i < placedCount; i++) {
        int idx = placedIndices[i];
        const char *dirStr = (wordList->words[idx].dir == ACROSS) ? "Across" : "Down";
        printf("%2d,%2d %-7s %s\n",
               wordList->words[idx].row,
               wordList->words[idx].col,
               dirStr,
               wordList->words[idx].clue);
    }
}
void printBoardToFile(FILE *fp, const Board *board, int isSolution) {
    fprintf(fp, "-----------------\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        fprintf(fp, "|");
        for (int j = 0; j < BOARD_SIZE; j++) {
            fprintf(fp, "%c", isSolution ? board->solution[i][j] : board->puzzle[i][j]);
        }
        fprintf(fp, "|\n");
    }
    fprintf(fp, "-----------------\n");
}

void saveOutput(const Board *board, const WordList *wordList, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Cannot create output file %s\n", filename);
        return;
    }

    fprintf(fp, "Solution:\n");
    printBoardToFile(fp, board, 1);

    fprintf(fp, "\nCrossword puzzle:\n");
    printBoardToFile(fp, board, 0);

    fprintf(fp, "\nClues:\n\n");

    // 添加随机排序逻辑（与printClues一致）
    int placedIndices[MAX_WORDS];
    int placedCount = 0;

    // 收集所有已放置单词的索引
    for (int i = 0; i < wordList->count; i++) {
        if (wordList->words[i].placed) {
            placedIndices[placedCount++] = i;
        }
    }

    // 对索引数组进行随机洗牌
    for (int i = placedCount - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = placedIndices[i];
        placedIndices[i] = placedIndices[j];
        placedIndices[j] = temp;
    }

    // 按随机顺序输出线索到文件
    for (int i = 0; i < placedCount; i++) {
        int idx = placedIndices[i];
        const char *dirStr = (wordList->words[idx].dir == ACROSS) ? "Across" : "Down";
        fprintf(fp, "%2d,%2d %-7s %s\n",
               wordList->words[idx].row,
               wordList->words[idx].col,
               dirStr,
               wordList->words[idx].clue);
    }

    fclose(fp);
    printf("Output saved to %s\n", filename);
}

