#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cwfunc.h"

int main(int argc, char *argv[]) {
    srand(time(NULL));
    Board board;
    WordList wordList;
    char *inputFile = NULL;
    char *outputFile = NULL;

    // 处理命令行参数
    if (argc > 3) {
        printf("Error: Too many arguments\n");
        printf("Usage: %s [input_file] [output_file]\n", argv[0]);
        return 1;
    }

    if (argc >= 2) {
        inputFile = argv[1];
    }
    if (argc == 3) {
        outputFile = argv[2];
    }

    // 初始化
    initBoard(&board);
    wordList.count = 0;

    // 读取单词
    if (readWords(&wordList, inputFile) <= 0) {
        printf("Error: No valid words found or cannot read file\n");
        return 1;
    }

    // 排序单词
    sortWordsByLength(&wordList);

    printf("\nSorted word list:\n");
    for (int i = 0; i < wordList.count; i++) {
        printf("%d. %s (length: %zu)\n", i+1, wordList.words[i].word,
               strlen(wordList.words[i].word));
    }

    // 生成填字游戏
    printf("\nGenerating crossword...\n");
    generateCrossword(&board, &wordList);

    // 生成线索
    generateAllClues(&wordList);

    // 输出结果
    if (outputFile) {
        saveOutput(&board, &wordList, outputFile);
        printf("Crossword generated and saved to %s\n", outputFile);
        printf("Successfully placed %d words\n", wordList.count);  // 只显示摘要
    } else {
        // 完整终端输出
        printf("\n=== Final Result ===\n");
        printf("Solution:\n");
        printBoard(&board, 1);
        printf("\nCrossword puzzle:\n");
        printBoard(&board, 0);
        printClues(&wordList);
    }

    return 0;
}