#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ROWS  20
#define COLUMNS 10
#define PocetBloku 7

int GetRandomNumber(int max) //náhodné èíslo na generování blokù
{
	return rand() % max;
}

// Definice blokù
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

// Pole blokù a ukazatelù na nì
int* blocks[PocetBloku] = {
(int*)O,
(int*)L,
(int*)T,
(int*)Lobracene,
(int*)Z,
(int*)I,
(int*)Zobracene };

int vyskabloku[PocetBloku] = { 2, 3, 2, 3, 2, 4, 2 }; // Výška blokù
int sirkabloku[PocetBloku] = { 2, 2, 3, 2, 3, 1, 3 }; // Šíøka blokù

//Vložení bloku do hrací plochy
void vlozBlok(int board[ROWS][COLUMNS], int blokIndex, int x, int y)
{
	int vyska = vyskabloku[blokIndex];
	int sirka = sirkabloku[blokIndex];
	int* blok = blocks[blokIndex];
	for (int i = 0; i < vyska; i++)
	{
		for (int j = 0; j < sirka; j++)
		{
			if (blok[i * sirka + j] == 1) // Pokud je èást bloku
			{
				board[y + i][x + j] = 1; // Vložení do hrací plochy
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

	for (int i = 0; i < vyska; i++) // Kontrola kolize pro každý øádek
	{
		for (int j = 0; j < sirka; j++)
		{
			if (blok[i * sirka + j] == 1) // Když je blok
			{

				if (y + i >= ROWS || x + j >= COLUMNS || x + j < 0 || board[y + i][x + j] == 1)// Kontrola, zda blok pøesahuje hrací plochu nebo koliduje s jiným blokem
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


// Kontrola øádku a mázání plných
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
			for (int k = i; k > 0; k--) //posunout øádky dolù
				for (int j = 0; j < COLUMNS; j++)
					board[k][j] = board[k - 1][j];
			for (int j = 0; j < COLUMNS; j++) //vymazat horní øádek
				board[0][j] = 0;
			score += 100; //pøiètení skóre
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

	// Pole pro aktivní blok, abych s tim mohl otaèet
	int* aktivniData = blocks[aktivniBlok];
	
	int rotovanyBlok[PocetBloku][COLUMNS * ROWS];

	// Výpoèet støedu
	int x = (COLUMNS - sirkabloku[aktivniBlok]) / 2;


	// Souøadnice y pro umístìní bloku
	int y = 0;

	// Šíøka a výška bloku pro rotaci
	int w = 0;
	int h = 0;

	// Zapnutí smyèky
	int running = 1;


	// Magie SDL2
	SDL_Event event;


	// SDL bída
	Uint32 lastTick = SDL_GetTicks();


	// definice skóre
	int score = 0;

	int sirka = sirkabloku[aktivniBlok]; 
	int vyska = vyskabloku[aktivniBlok]; 
	// Herní smyèka
	while (running)
	{

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) running = 0;

			else if (event.type == SDL_KEYDOWN)
			{
				int newX = x;
				int newY = y;

				//posouvání do stran
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
				
					//Otoèení bloku
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
				// uložit trvale
				vlozBlok(board, aktivniBlok, x, y);
				score += smazPlneRadky(board);
				


				//nový blok
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

		// Vykreslení doèasného pole s padajícím blokem

		int doèasnýBoard[ROWS][COLUMNS];
		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLUMNS; j++)
			{
				doèasnýBoard[i][j] = board[i][j];
			}
		}

			vlozBlok(doèasnýBoard, aktivniBlok, x, y); // pøidá blok do doèasného boardu
			
			renderBoard(renderer, doèasnýBoard);
			
			SDL_Delay(16); // 60 FPS

		
	}
	printf("Skore: %d\n", score);
	SDL_DestroyRenderer(renderer); // Znièení rendereru
	SDL_DestroyWindow(window); // Znièení okna
	SDL_Quit();

	return 0;
}
