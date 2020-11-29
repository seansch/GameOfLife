#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define H 30 // Board height
#define W 100 // Board width
#define STASIS_REQ 40 // Number of generations population has to remain stable to consider stasis achieved

void nextGen();
void initBoard();
void render();
void stasisCheck();

int generation[H][W];
int generation_count = 1;
int population = H*W;
int living;
int previous_living;
int stasis_generation = 0;
int pending_stasis_generation = 0;
int stasis_check = 0;

int main() {
    initBoard();
    render();

    while(1) {
        nextGen();
        stasisCheck();
        usleep(100000);
        render();
    }

}

// Generate the next generation
void nextGen() {
    living = 0;
    generation_count++;
    int nextGen[H][W];

    // Indexes of neighboring cells
    int n[8][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,1}, {1,1}, {1,0}, {1,-1}, {0,-1}};

    for (int x = 0; x < H; x++) {
        for (int y = 0; y < W; y++) {
            int neighbors = 0;

            for (int i = 0; i < 8; i++) {
                if ((x + n[i][0] >= 0) && (y + n[i][1] >= 0) && (x + n[i][0] <  H) && (y + n[i][1] <  W)) {
                    neighbors += generation[x+n[i][0]] [y+n[i][1]];
                }
            }

            // Stasis
            if (neighbors == 2) {
                nextGen[x][y] = generation[x][y];
                living += nextGen[x][y];
            }
            // Growth
            if (neighbors == 3) {
                nextGen[x][y] = 1;
                living++;
            }
            // Death
            if ((neighbors < 2) || (neighbors > 3)) {
                nextGen[x][y] = 0;
            }
        }
    }

    memcpy(generation, nextGen, sizeof(nextGen));
}

// Initialize the board with random population
void initBoard() {
    srand(time(NULL));
    for (int x = 0; x < H; x++) {
        for (int y = 0; y < W; y++) {
            generation[x][y] = rand() % 2;
        }
    }
}

// Check if board has achieved stasis
void stasisCheck() {
    if (stasis_generation == 0) {
        // Check if living population within +/-10 of last generation
        if (living < previous_living+10 && living > previous_living-10) {

            // If there isn't already a pending stasis generation record current generation
            if (pending_stasis_generation == 0) {
                pending_stasis_generation = generation_count;
            }

            stasis_check++;
        } else {
            // Reset the stasis check count and pending stasis generation
            stasis_check = 0;
            pending_stasis_generation = 0;
            previous_living = living;
        }

        // If population has remained stable for STASIS_REQ generations consider board to be in stasis
        if (stasis_check == STASIS_REQ) {
            stasis_generation = pending_stasis_generation;
        }
    }
}

// Render the board
void render() {
    char output[H*W+(H*2)] = "";

    for (int x = 0; x < H; x++) {
        for (int y = 0; y < W; y++) {
            if (generation[x][y] == 1) {
                strcat(output, "o");
            } else {
                strcat(output, "-");
            }
        }
        strcat(output, "\n");
    }

    system("clear");
    printf("%s", output);

    printf("Generations: %d\tLiving: %d\tDead: %d\n", generation_count, living, population-living);

    if(stasis_generation == 0) {
        printf("Pending stasis... %d stable generations remaining.\n", STASIS_REQ-stasis_check);
    } else {
        printf("Stasis achieved after %d generations\n", stasis_generation);
    }
}