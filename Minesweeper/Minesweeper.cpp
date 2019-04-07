// Minesweeper.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

/* Minesweeper(2019/04/06)
  author    : saio4016
  language  : c
  IDE/Editor: Visual Studio 2017
  time      : 3h */

/* Guide
  ・w = up, a = left, s = down, d = right
  ・e = flag, f = enter */

#include "pch.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

#define ROW 20 //盤面の縦の長さ
#define COL 30 //盤面の横の長さ
#define BOMB_NUM ROW * COL / 15//爆弾の数

#define OPEND 0 //空いている
#define CLOSED 1 //空いていない
#define MARKED 3 //マーク

#define NONE 0 //何もない
#define BOMB 1 //爆弾あり

//周りの探索
const int mx8[] = { 0,1,1,1,0,-1,-1,-1 };
const int my8[] = { 1,1,0,-1,-1,-1,0,1 };

int board_disp[ROW][COL]; //表示盤面
int board_non_disp[ROW][COL]; //内部盤面
int around_bomb[ROW][COL]; //周りの爆弾の数
int cursorX = COL / 2, cursorY = ROW / 2; //カーソル位置
int rest_bomb = BOMB_NUM; //残り爆弾の数(マークをすると減る)
int rest_none = ROW * COL - BOMB_NUM; //取り除く数

int operate_cursor();
void init_board();
int count_bomb(int x, int y);
void disp_board();
void open_mass(int x, int y);
void end_game(int num);
void disp_end_game();

int main() {
	printf("Press to start "); _getch();
	init_board(); //盤面初期化

	while (1) {
		system("cls"); //画面リセット
		if ((rest_none == ROW * COL - BOMB_NUM)) printf("First open is fixed\n");
		disp_board(); //盤面表示
		if (!operate_cursor()) continue; //カーソルを動かす
		if (board_non_disp[cursorY][cursorX] == BOMB) { end_game(-1); break; } //敗北
		open_mass(cursorX, cursorY); //マスを開ける
		if (rest_none == 0) { end_game(1); break; }; //勝利
	}

	return 0;
}

void init_board() {
	srand(time(NULL));
	//盤面をすべて見えない状態にする
	for (int y = 0; y < ROW; y++) {
		for (int x = 0; x < COL; x++) {
			board_disp[y][x] = CLOSED;
		}
	}
	//初期爆弾配置
	int cnt = BOMB_NUM;
	while (cnt) {
		int x = rand() % COL, y = rand() % ROW;
		if (x == cursorX && y == cursorY) continue; //安置を作る
		if (board_non_disp[y][x] == BOMB) continue;
		board_non_disp[y][x] = BOMB;
		cnt--;
	}
	//周りの爆弾の数を数える
	for (int y = 0; y < ROW; y++) {
		for (int x = 0; x < COL; x++) {
			around_bomb[y][x] = count_bomb(x, y);
		}
	}
}

int count_bomb(int x, int y) {
	//8近傍の爆弾の数を数える
	int ret = 0;
	for (int i = 0; i < 8; i++) {
		int nx = x + mx8[i], ny = y + my8[i];
		if (nx < 0 || COL <= nx || ny < 0 || ROW <= ny) continue;
		if (board_non_disp[ny][nx] == BOMB) ret++;
	}
	return ret;
}

void disp_board() {
	printf("Bombs : %d\n", rest_bomb);
	printf("Rests : %d\n", rest_none);
	for (int y = 0; y < ROW; y++) {
		for (int x = 0; x < COL; x++) {
			if (x == cursorX && y == cursorY) {
				//カーソル位置
				printf("● ");
			}
			else {
				switch (board_disp[y][x]) {
				case OPEND:
					if (around_bomb[y][x] == NONE) printf("・ ");
					else printf(" %d ", around_bomb[y][x]);
					break;
				case CLOSED:
					printf("□ ");
					break;
				case MARKED:
					printf("◎ ");
					break;
				default:
					printf("no");
					break;
				}
			}
		}
		printf("\n");
	}
}

int operate_cursor() {
	//カーソルを動かす(初手は動かせない)
	int ret = 0;
	switch (_getch()) {
	case 'w':
		if (!(rest_none == ROW * COL - BOMB_NUM) && cursorY > 0) cursorY--;
		break;
	case 'a':
		if (!(rest_none == ROW * COL - BOMB_NUM) && cursorX > 0) cursorX--;
		break;
	case 's':
		if (!(rest_none == ROW * COL - BOMB_NUM) && cursorY < ROW - 1) cursorY++;
		break;
	case 'd':
		if (!(rest_none == ROW * COL - BOMB_NUM) && cursorX < COL - 1) cursorX++;
		break;
	case 'f':
		if (board_disp[cursorY][cursorX] == CLOSED) ret = 1;
		break;
	case 'e':
		if (board_disp[cursorY][cursorX] == CLOSED) {
			board_disp[cursorY][cursorX] = MARKED;
			rest_bomb--;
		}
		else if (board_disp[cursorY][cursorX] == MARKED) {
			board_disp[cursorY][cursorX] = CLOSED;
			rest_bomb++;
		}
		break;
	default:
		break;
	}
	return ret; //fで選択したところがCLOSEDではない場合1を返す
}

void open_mass(int x, int y) {
	if (board_disp[cursorY][cursorX] == MARKED) return;
	if (around_bomb[y][x] != NONE) {
		//選択したところの周りに爆弾ある(最初の探索のみ入る可能性あり)
		board_disp[y][x] = OPEND;
		rest_none--;
		return;
	}
	for (int i = 0; i < 8; i++) {
		int nx = x + mx8[i], ny = y + my8[i];
		if (nx < 0 || COL <= nx || ny < 0 || ROW <= ny) continue;
		if (board_disp[ny][nx] == CLOSED && board_non_disp[ny][nx] != BOMB) {
			board_disp[ny][nx] = OPEND;
			rest_none--;
			if (around_bomb[ny][nx] == NONE) open_mass(nx, ny); //さらに探索
		}
	}
	return;
}

void end_game(int num) {
	system("cls");
	disp_end_game();
	switch (num) {
	case -1:
		printf("You died...\n");
		break;
	case 1:
		printf("You win!!\n");
		break;
	default:
		break;
	}
}

void disp_end_game() {
	printf("Bombs num: %d\n", BOMB_NUM);
	for (int y = 0; y < ROW; y++) {
		for (int x = 0; x < COL; x++) {
			if (x == cursorX && y == cursorY && board_non_disp[y][x] == BOMB) {
				printf("☆ "); //アウト
			}
			else {
				switch (board_disp[y][x]) {
				case OPEND:
					if (around_bomb[y][x] == 0) printf("・ ");
					else printf(" %d ", around_bomb[y][x]);
					break;
				case CLOSED:
					if (board_non_disp[y][x] == BOMB) printf(" B ");
					else printf("□ ");
					break;
				case MARKED:
					if (board_non_disp[y][x] == BOMB) printf("◎ ");
					else printf(" x ");
					break;
				default:
					printf("no");
					break;
				}
			}
		}
		printf("\n");
	}
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
