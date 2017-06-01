// Copyright (c) 2016 Tetsuya Hori
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php

//
//  main.c
//  mosaicDesigner
//
//
//  Created by 堀 哲也 on 2017/04/03.
//  Copyright © 2017年 堀 哲也. All rights reserved.
//
//  プロフィール画像に使用できるような正方形のbmp画像(4bit, OS/2)を出力する
//  (カラーパレット使用で画像データ自体は小さくする)
//
//  次の規定に沿った小さな正方形を敷き詰めて画像を得る
//  1. 模様データを記録してあるdesign.txtを読み込み、配列に展開する
//      (背景を0、アクセントカラー1を1, アクセントカラー2を2として記述)
//  2. シルエットの形を記録してあるpattern.txtを読み込み、配列に展開する
//      (背景を0、シルエットを1として文字で記述されるパターン)
//  3. 1の模様データを2のパターンでマスクする(patternが0なら0を、1ならdesignの値を書き込む)
//  (4. 画像の周囲を背景色で縁取る)
//

#include <stdio.h>
#include <stdlib.h>

#define FILEHEADER 14   // ファイルヘッダの大きさ
#define INFOHEADER 12   // 情報ヘッダの大きさ

#define SQUARESIZE 32   // 小さな正方形の一辺の長さ
#define FRAMETHICKNESS 1    // 縁取りの線の厚さ

#define GRIDSIZE 512    // 配列の数

// この構造体は模様とパターンで兼用
typedef struct _pic{
    char **grid;    // (配列を指すポインタの配列)へのポインタ=二次元配列を持つ
    // データは数値で持つこと。1桁なので、%cで取った後に数値にする
    
    int length;     // 一辺の長さ
} PIC;

int input(PIC *target, const char *name);
void adjust(PIC *design, int length);
void coloring(PIC design, PIC *pattern);
int output();

int main() {
    /* 変数宣言 */
    PIC design, pattern;
    int i, j;
    
    /* パターン入力 */
    if(input(&design, "design.txt"))
        return 1;   // end if file can't be opened
    if(input(&pattern, "pattern.txt"))
        return 1;
    
    /* データ処理 */
    if(design.length != pattern.length) // 大きさが違うなら、合わせる必要がある
        adjust(&design, pattern.length);
    coloring(design, &pattern);
    
    /* 出力 */
    if(output(pattern)){
        return 1;   // end if file can't be made
    }
    
    /* 終了 */
    for(i = 0; i < design.length; i++){
        free(design.grid[i]);
    }
    free(design.grid);
    
    for(i = 0; i < pattern.length; i++){
        free(pattern.grid[i]);
    }
    free(pattern.grid);
    return 0;
}

int input(PIC *target, const char *name){
    char buff;
    int i, j;   // column, row
    FILE *fp;
    
    // ファイルオープン
    if((fp = fopen(name, "r")) == NULL){
        printf("file %s can't be opened\n", name);
        exit(1);
    }
    
    // 行・列数の読み込み
    fscanf(fp, "%d", &target->length);
    if(target->length <= 0){
        // 辺の長さが0以下なのはおかしい
        printf("%s is invalid\n", name);
        exit(1);
    }
    
    // ポインタ配列の宣言
    if((target->grid = (char **)malloc(sizeof(char *) * target->length)) == NULL){
        printf("allocating error in %s\n", name);
        exit(1);
    }
    // 1次元配列を行数分だけ用意し、さっき宣言したポインタ配列がそれらを指すようにする
    for(j = 0; j < target->length; j++){
        if((target->grid[j] = (char *)malloc(sizeof(char) * target->length)) == NULL){
        printf("allocating error in %s\n", name);
        exit(1);
        }
    }
    
    // メインのデータを読み込み
    for(i = 0; i < target->length; i++){
        for(j = 0; j < target->length; j++){
            fscanf(fp, " %c", &buff);
            buff = buff - '0';  // 数値に変換
            if(buff < 0 || 9 < buff){
                // 無効な数値
                printf("%s is invalid\n", name);
                exit(1);
            } else 
                target->grid[i][j] = buff;
        }
    }
    
    // 終了
    fclose(fp);
    return 0;
}

// 模様データがパターンデータより小さい場合は、縦横に繰り返す
void adjust(PIC *design, int patternLen){
    char **tempGrid;
    int originalLen = design->length;
    int i, j;
    design->length = patternLen;
    
    // 退避用配列を、構造体側の2次元配列と同じように宣言
    if((tempGrid = (char **)malloc(sizeof(char *) * originalLen)) == NULL){
        printf("temporary allocating error\n");
        exit(1);
    }
    for(j = 0; j < originalLen; j++){
        if((tempGrid[j] = (char *)malloc(sizeof(char) * originalLen)) == NULL){
        printf("allocating error\n");
        exit(1);
        }
    }
    
    // メイン配列から退避用配列へコピー
    for(i = 0; i < originalLen; i++)
        for(j = 0; j < originalLen; j++)
            tempGrid[i][j] = design->grid[i][j];
    
    // メイン配列の大きさを変更
    if((design->grid = (char **)realloc(design->grid, sizeof(char *) * design->length)) == NULL){
        printf("reallocating error\n");
        exit(1);
    }
    for(j = 0; j < design->length; j++){
        if((design->grid[j] = (char *)realloc(design->grid[j], sizeof(char) * design->length)) == NULL){
        printf("reallocating error\n");
        exit(1);
        }
    }
    
    // 退避用配列から参照してメイン配列に書き込み
    for(i = 0; i < design->length; i++){
        for(j = 0; j < design->length; j++){
            design->grid[i][j] = tempGrid[i % originalLen][j % originalLen];    // originalLenの剰余により元のデータを繰り返す
        }
    }
    
    // 終了
    for(j = 0; j < originalLen; j++)
        free(tempGrid[j]);
    free(tempGrid);
}

// patternの各要素に色をつける。座標(i, j)において、
// patternが1(模様がある)ならdesignで指定された色を、
// patternが0(模様がない)なら0(背景色を意味する)を設定
void coloring(PIC design, PIC *pattern){
    int i, j;
    
    for(i = 0; i < pattern->length; i++){
        for(j = 0; j < pattern->length; j++){
            if(pattern->grid[i][j] == 0)
                // 0が入っているので、そのままで良い
                continue;
            else
                pattern->grid[i][j] = design.grid[i][j];
        }
    }
}

// 出力
int output(PIC picture){
    // ------------------ 画像内容パラメータ -------------------
    const int color[] = {0xFFF3FD, 0xDBD0E6, 0xFDD3FF}; // 背景色、アクセントカラー1,2の順 16進数で
    //----------------再定義---------------------------
    int length = picture.length;
    //----------------変数--------------------------------
    int picLen = SQUARESIZE * length;  // 画像の縦及び横の長さ
    unsigned long buff;
    //------------------ 制御用 -----------------------------
    FILE *fp;
    int i, j, k, l;
    //---------------------------------------------------------
    
    fp = fopen("output.bmp", "wb");
    if(fp == NULL){
        printf("file opening process failed\n");
        return 1;
    }
    
    // ファイルヘッダ
    fwrite("BM", 2, 1, fp);     // bmpであることを示す
    
    // ファイルサイズ
    buff = FILEHEADER + INFOHEADER + 3 * 16 + picLen * picLen / 2;
    fwrite(&buff, 4, 1, fp);    // ファイルヘッダ+情報ヘッダ+パレット+画像データ
    
    buff = 0;
    fwrite(&buff, 4, 1, fp);    // 予約領域
    
    buff = FILEHEADER + INFOHEADER + 3 * 16;    // 先頭から画像データまでのオフセット
    fwrite(&buff, 4, 1, fp);    // ファイルヘッダ + 情報ヘッダ + パレットデータ
    
    // 情報ヘッダ
    buff = INFOHEADER;
    fwrite(&buff, 4, 1, fp);    // 情報ヘッダの大きさ
    
    buff = picLen;              // 縦横の長さ
    fwrite(&buff, 2, 1, fp);    // 正方形なので同じ数値を2つ
    fwrite(&buff, 2, 1, fp);
    
    buff = 1;
    fwrite(&buff, 2, 1, fp);    // bcプレーン数（？）は常に1
    
    buff = 4;
    fwrite(&buff, 2, 1, fp);    // 4bitの画像である
    
    // パレットデータ
    for(i = 0; i < 3; i++){  // 最初の3色にcolor[3]のデータを入れ、残りは0に設定
        buff = 0;   // buffのリセット
        // BGRの順に設定する
        buff = color[i] & 0x0000FF;
        fwrite(&buff, 1, 1, fp);
        
        buff = color[i] & 0x00FF00;
        buff = buff >> 8;
        fwrite(&buff, 1, 1, fp);
        
        buff = color[i] & 0xFF0000;
        buff = buff >> 16;
        fwrite(&buff, 1, 1, fp);
    }   // 使わないので0で埋める
    for(i = 0; i < 13; i++){
        buff = 0;
        fwrite(&buff, 3, 1, fp);
    }
    
    // 画像データ
    // 1Byteに2pixelのデータが入ることに注意
    // 画像データは左下から右上に記録される( [length - i - 1]で下から上に記録する )
    for(i = 0; i < length; i++){                    // 縦のパターンの数だけループ
        for(j = 0; j < SQUARESIZE; j++){            // 小さい正方形の縦の長さだけループ
            for(k = 0; k < length; k++){            // 横のパターンの数だけループ
                for(l = 0; l < SQUARESIZE/2; l++){  // 小さい正方形の横の長さ/2だけループ(1回(byte)に2画素ずつ書くため)
                    buff = (picture.grid[length-i-1][k] << 4) + picture.grid[length-i-1][k];  // 4bitごとに色をセット
                    if(j < FRAMETHICKNESS || j >= SQUARESIZE-FRAMETHICKNESS || l < FRAMETHICKNESS || l >= SQUARESIZE-FRAMETHICKNESS){
                        // 縁取りの対象なら
                        buff = 0b00000000;
                    }
                    fwrite(&buff, 1, 1, fp);
                }
            }
        }
    }
    
    fclose(fp);
    return 0;
}
