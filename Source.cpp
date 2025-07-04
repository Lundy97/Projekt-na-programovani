#define SDL_MAIN_HANDLED //wow
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS  20
#define COLUMNS 10
#define PocetBloku 7
#define BLOCK_SIZE 30 // Velikost bloku v pixelech

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

// Pole blok� a ukazatel� na n�
int* blocks[PocetBloku] = {
	(int*)O,
	(int*)L,
	(int*)T,
	(int*)Lobracene,
	(int*)Z,
	(int*)I,
	(int*)Zobracene
};

int vyskabloku[PocetBloku] = { 2, 3, 2, 3, 2, 4, 2 }; // V��ka blok�
int sirkabloku[PocetBloku] = { 2, 2, 3, 2, 3, 1, 3 }; // ���ka blok�

//Vlo�en� bloku do hrac� plochy (nov� s parametrem typBloku pro barvy)
void vlozBlok(int board[ROWS][COLUMNS], int* data, int sirka, int vyska, int x, int y, int typBloku)
{
	for (int i = 0; i < vyska; i++)
	{
		for (int j = 0; j < sirka; j++)
		{
			if (data[i * sirka + j] == 1)
			{
				if (y + i >= 0 && y + i < ROWS && x + j >= 0 && x + j < COLUMNS)
					board[y + i][x + j] = typBloku + 1; // Ulo��me ��slo bloku (1 a� 7)
			}
		}
	}
}

// vykreslen� hrac� plochy
void renderBoard(SDL_Renderer* renderer, int board[ROWS][COLUMNS])
{
	SDL_Color barvy[] = {
		{255, 255,   0}, // O - �lut�
		{255, 165,   0}, // L - oran�ov�
		{128,   0, 128}, // T - fialov�
		{  0,   0, 255}, // L obr�cen� - modr�
		{255,   0,   0}, // Z - �erven�
		{  0, 255, 255}, // I - tyrkys
		{  0, 255,   0}, // Z obr�cen� - zelen�
	};

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // vy�istit �ern�
	SDL_RenderClear(renderer);

	for (int y = 0; y < ROWS; y++)
	{
		for (int x = 0; x < COLUMNS; x++)
		{
			if (board[y][x])
			{
				int blok = board[y][x] - 1;
				SDL_Rect rect = { x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1 };
				SDL_SetRenderDrawColor(renderer, barvy[blok].r, barvy[blok].g, barvy[blok].b, 255);
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}

	SDL_RenderPresent(renderer);
}

// Kontrola kolize
int koliduje(int board[ROWS][COLUMNS], int* data, int sirka, int vyska, int x, int y)
{
	for (int i = 0; i < vyska; i++)
	{
		for (int j = 0; j < sirka; j++)
		{
			if (data[i * sirka + j] == 1)
			{
				if (y + i >= ROWS || x + j >= COLUMNS || x + j < 0 || (y + i >= 0 && board[y + i][x + j]))
					return 1;
			}
		}
	}
	return 0;
}

// Rotace bloku doprava
void rotacebloku(int* src, int* dest, int w, int h)
{
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			dest[j * h + (h - i - 1)] = src[i * w + j];
		}
	}
}

// Rotace doleva
void rotaceblokuDoleva(int* src, int* dest, int w, int h)
{
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			dest[(w - j - 1) * h + i] = src[i * w + j];
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
		if (plny)
		{
			for (int k = i; k > 0; k--)
			{
				for (int j = 0; j < COLUMNS; j++)
					board[k][j] = board[k - 1][j];
			}
			for (int j = 0; j < COLUMNS; j++)
				board[0][j] = 0;

			score += 250;
		}
	}
	return score;
}

int aktivniBuffer[16]; // buffer na aktivn� blok
int rotovanyBlok[16];  // pole pro rotovan� blok

// Funkce pro kop�rov�n� bloku
void zkopirujBlok(int* src, int* dest, int sirkaSrc, int vyska)
{
	for (int i = 0; i < vyska; i++)
	{
		for (int j = 0; j < sirkaSrc; j++)
		{
			dest[i * sirkaSrc + j] = src[i * sirkaSrc + j];
		}
	}
}

int main()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Tetris - Lundy verze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, COLUMNS * BLOCK_SIZE, ROWS * BLOCK_SIZE, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	srand(time(NULL));

	int board[ROWS][COLUMNS] = { 0 };

	int aktivniBlok = GetRandomNumber(PocetBloku);
	int* aktivniData = blocks[aktivniBlok];

	int sirka = sirkabloku[aktivniBlok];
	int vyska = vyskabloku[aktivniBlok];
	int x = (COLUMNS - sirka) / 2;
	int y = 0;

	zkopirujBlok(aktivniData, aktivniBuffer, sirka, vyska);
	aktivniData = aktivniBuffer;

	int running = 1;
	SDL_Event event;
	Uint32 lastTick = SDL_GetTicks();

	int score = 0;

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

				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					printf("Ode�el jsi ze hry.\n");
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
					SDL_Quit();
					return 0;
				}
				if (event.key.keysym.sym == SDLK_a) newX--;
				if (event.key.keysym.sym == SDLK_d) newX++;
				if (event.key.keysym.sym == SDLK_s) newY++;
				
				if (event.key.keysym.sym == SDLK_SPACE)
				{
					while (!koliduje(board, aktivniData, sirka, vyska, x, y + 1)) y++;
					vlozBlok(board, aktivniData, sirka, vyska, x, y, aktivniBlok);
					score += smazPlneRadky(board);

					// GENEROV�N� NOV�HO BLOKU
					aktivniBlok = GetRandomNumber(PocetBloku);
					
					// Jestli je blok v poli
					aktivniData = blocks[aktivniBlok];
					
					sirka = sirkabloku[aktivniBlok];
					vyska = vyskabloku[aktivniBlok];
					
					x = (COLUMNS - sirka) / 2;
					y = 0;
					
					newX = x; // resetov�n� pozice
					newY = y; // resetov�n� pozice
					
					zkopirujBlok(aktivniData, aktivniBuffer, sirka, vyska);
					aktivniData = aktivniBuffer;

					if (koliduje(board, aktivniData, sirka, vyska, x, y))
					{
						SDL_Delay(2000);
						running = 0;
					}

					
				}
				

				if (!koliduje(board, aktivniData, sirka, vyska, newX, newY))
				{
					x = newX;
					y = newY;
				}

				if (event.key.keysym.sym == SDLK_q)
				{
					for (int i = 0; i < 16; i++) rotovanyBlok[i] = 0;
					rotaceblokuDoleva(aktivniData, rotovanyBlok, sirka, vyska);
					if (!koliduje(board, rotovanyBlok, vyska, sirka, x, y))
					{
						int tmp = sirka;
						sirka = vyska;
						vyska = tmp;
						zkopirujBlok(rotovanyBlok, aktivniBuffer, sirka, vyska);
					}
				}

				if (event.key.keysym.sym == SDLK_e)
				{
					for (int i = 0; i < 16; i++) rotovanyBlok[i] = 0;
					rotacebloku(aktivniData, rotovanyBlok, sirka, vyska);
					if (!koliduje(board, rotovanyBlok, vyska, sirka, x, y))
					{
						int tmp = sirka;
						sirka = vyska;
						vyska = tmp;
						zkopirujBlok(rotovanyBlok, aktivniBuffer, sirka, vyska);
					}
				}
			}
		}

		if (SDL_GetTicks() - lastTick > 500)
		{
			if (!koliduje(board, aktivniData, sirka, vyska, x, y + 1))
				y++;
			else
			{
				vlozBlok(board, aktivniData, sirka, vyska, x, y, aktivniBlok);
				score += smazPlneRadky(board);

				aktivniBlok = GetRandomNumber(PocetBloku);
				aktivniData = blocks[aktivniBlok];
				sirka = sirkabloku[aktivniBlok];
				vyska = vyskabloku[aktivniBlok];
				x = (COLUMNS - sirka) / 2;
				y = 0;

				if (koliduje(board, aktivniData, sirka, vyska, x, y))
				{
					SDL_Delay(2000);
					running = 0;
				}
				else
				{
					zkopirujBlok(aktivniData, aktivniBuffer, sirka, vyska);
					aktivniData = aktivniBuffer;
				}
			}
			lastTick = SDL_GetTicks();
		}

		int docasnyBoard[ROWS][COLUMNS];
		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLUMNS; j++)
				docasnyBoard[i][j] = board[i][j];

		vlozBlok(docasnyBoard, aktivniData, sirka, vyska, x, y, aktivniBlok);
		renderBoard(renderer, docasnyBoard);

		SDL_Delay(16);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	printf("Game Over! Skore: %d\n", score);
	return 0;
}