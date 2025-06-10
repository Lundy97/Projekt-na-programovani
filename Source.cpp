#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ROWS  20
#define COLUMNS 10
#define PocetBloku 7

int GetRandomNumber(int max) //n�hodn� ��slo na generov�n� blok�
{
	return rand() % max;
}

// Definice blok�
int O[2][2] = {
	{1,1},
	{1,1}
};

int L[3][2] = {
	{1,0},
	{1,0},
	{1,1}
};

int T[2][3] = {
	{1,1,1},
	{0,1,0}
};

int Lobracene[3][2] = {
	{0,1},
	{0,1},
	{1,1}
};

int Z[2][3] = {
	{1,1,0},
	{0,1,1}
};

int I[4][1] = {
	{1},
	{1},
	{1},
	{1}
};

int Zobracene[2][3] = {
	{0,1,1},
	{1,1,0}
};



void vytisknout(int board[ROWS][COLUMNS]) //Timto se to tiskne
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			if (board[i][j] == 0)
			{
				printf(" .");
			}
			else
			{
				printf(" #");
			}
		}
		printf("\n");
	}
}

// Pole blok� a ukazatel� na n�
int* blocks[PocetBloku] = {
(int*)O,
(int*)L,
(int*)T,
(int*)Lobracene,
(int*)Z,
(int*)I,
(int*)Zobracene };

int vyskabloku[PocetBloku] = { 2, 3, 2, 3, 2, 4, 2 }; // V��ka blok�
int sirkabloku[PocetBloku] = { 2, 2, 3, 2, 3, 1, 3 }; // ���ka blok�

//Vlo�en� bloku do hrac� plochy
void vlozBlok(int board[ROWS][COLUMNS], int blokIndex, int x, int y)
{
	int vyska = vyskabloku[blokIndex];
	int sirka = sirkabloku[blokIndex];
	int* blok = blocks[blokIndex];
	for (int i = 0; i < vyska; i++)
	{
		for (int j = 0; j < sirka; j++)
		{
			if (blok[i * sirka + j] == 1) // Pokud je ��st bloku
			{
				board[y + i][x + j] = 1; // Vlo�en� do hrac� plochy
			}
		}
	}
}

#define BLOCK_SIZE 30 // Velikost bloku v pixelech

void renderBoard(SDL_Renderer* renderer, int board[ROWS][COLUMNS]) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black
	SDL_RenderClear(renderer);

	for (int y = 0; y < ROWS; y++) {
		for (int x = 0; x < COLUMNS; x++) {
			if (board[y][x]) {
				SDL_Rect rect = { x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1 };
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white blocks
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}

	SDL_RenderPresent(renderer);
}



int koliduje(int board[ROWS][COLUMNS], int blokIndex, int x, int y) // Kontrola kolize
{
	int vyska = vyskabloku[blokIndex];
	int sirka = sirkabloku[blokIndex];
	int* blok = blocks[blokIndex];

	for (int i = 0; i < vyska; i++) // Kontrola kolize pro ka�d� ��dek
	{
		for (int j = 0; j < sirka; j++)
		{
			if (blok[i * sirka + j] == 1) // Kdy� je blok
			{

				if (y + i >= ROWS || x + j >= COLUMNS || x + j < 0 || board[y + i][x + j] == 1)// Kontrola, zda blok p�esahuje hrac� plochu nebo koliduje s jin�m blokem
				{
					return 1; // Kolize je
				}
			}
		}
	}
	return 0; // Kolize neni
}

//rotatuje s blockem
void rotacebloku(int* src, int* dest, int w, int h) {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			dest[j * h + (h - i - 1)] = src[i * w + j];
		}
	}
}


// Kontrola ��dku a m�z�n� pln�ch
int smazPlneRadky(int board[ROWS][COLUMNS])
{
	int score = 0;
	for (int i = 0; i < ROWS; i++)
	{
		int plny = 1;
		for (int j = 0; j < COLUMNS; j++)
		{
			if (board[i][j] == 0)
			{
				plny = 0;
				break;
			}
		}
		if (plny) {
			for (int k = i; k > 0; k--) //posunout ��dky dol�
				for (int j = 0; j < COLUMNS; j++)
					board[k][j] = board[k - 1][j];
			for (int j = 0; j < COLUMNS; j++) //vymazat horn� ��dek
				board[0][j] = 0;
			score += 100; //p�i�ten� sk�re
		}
	}
	return score;
}




int main()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Tetris - Lundy verze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, COLUMNS * BLOCK_SIZE, ROWS * BLOCK_SIZE, 0);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	srand(time(NULL));

	int board[ROWS][COLUMNS] = { 0 };

	int aktivniBlok = GetRandomNumber(PocetBloku);

	// Pole pro aktivn� blok, abych s tim mohl ota�et
	int* aktivniData = blocks[aktivniBlok];
	
	int rotovanyBlok[PocetBloku][COLUMNS * ROWS];

	// V�po�et st�edu
	int x = (COLUMNS - sirkabloku[aktivniBlok]) / 2;


	// Sou�adnice y pro um�st�n� bloku
	int y = 0;

	// ���ka a v��ka bloku pro rotaci
	int w = 0;
	int h = 0;

	// Zapnut� smy�ky
	int running = 1;


	// Magie SDL2
	SDL_Event event;


	// SDL b�da
	Uint32 lastTick = SDL_GetTicks();


	// definice sk�re
	int score = 0;

	int sirka = sirkabloku[aktivniBlok]; 
	int vyska = vyskabloku[aktivniBlok]; 
	// Hern� smy�ka
	while (running)
	{

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) running = 0;

			else if (event.type == SDL_KEYDOWN)
			{
				int newX = x;
				int newY = y;

				//posouv�n� do stran
				if (event.key.keysym.sym == SDLK_LEFT) newX--;
				if (event.key.keysym.sym == SDLK_RIGHT) newX++;
				if (event.key.keysym.sym == SDLK_DOWN) newY++;

				if (!koliduje(board, aktivniBlok, newX, newY))
				{
					x = newX;
					y = newY;
				}

				if (event.key.keysym.sym == SDLK_UP)
				{
					sirka = sirkabloku[aktivniBlok]; 
					vyska = vyskabloku[aktivniBlok]; 

					rotacebloku(blocks[aktivniBlok], rotovanyBlok[aktivniBlok], w, h);

					aktivniData = rotovanyBlok[aktivniBlok]; 
				
					//Oto�en� bloku
					sirka = h;
					vyska = w;
				}
			}
		}
		// Gravitace
		if (SDL_GetTicks() - lastTick > 500) 
		{
			if (!koliduje(board, aktivniBlok, x, y + 1)) 
			{
				y++;
			}
			else {
				// ulo�it trvale
				vlozBlok(board, aktivniBlok, x, y);
				score += smazPlneRadky(board);
				


				//nov� blok
				aktivniBlok = GetRandomNumber(PocetBloku);
				x = (COLUMNS - sirkabloku[aktivniBlok]) / 2;
				y = 0;

				if (koliduje(board, aktivniBlok, x, y)) 
				{
					printf("Konec!\n");
					SDL_Delay(2000);
					running = 0;
				}
			}
			lastTick = SDL_GetTicks();
		}

		// Vykreslen� do�asn�ho pole s padaj�c�m blokem

		int do�asn�Board[ROWS][COLUMNS];
		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLUMNS; j++)
			{
				do�asn�Board[i][j] = board[i][j];
			}
		}

			vlozBlok(do�asn�Board, aktivniBlok, x, y); // p�id� blok do do�asn�ho boardu
			
			renderBoard(renderer, do�asn�Board);
			
			SDL_Delay(16); // 60 FPS

		
	}
	printf("Skore: %d\n", score);
	SDL_DestroyRenderer(renderer); // Zni�en� rendereru
	SDL_DestroyWindow(window); // Zni�en� okna
	SDL_Quit();

	return 0;
}
