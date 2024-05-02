#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum _Piece {
    EMPTY = ' ',
    BLUE = 'A',
    GREEN = 'B',
    YELLOW = 'C',
    ORANGE = 'D',
    RED = 'E',
} Piece;

typedef struct _Board {
    int size;
    Piece **cells;
} Board;

typedef struct _Move {
    int PieceX;
    int PieceY;

    int TargetX;
    int TargetY;
} Move;

typedef struct _Player {
    char name[50];
    int score;
    int pieces[5];
} Player;

/* Check if the move is valid
 *
 * Parameters:
 * - board: the game board
 * - move: the move to be checked
 *
 * Returns:
*/
Piece isMoveValid(Board *board, Move move)
{
    int toRight = 0;
    int toTop = 0;

    if (board->cells[move.PieceX][move.PieceY] == EMPTY)
    {
        return 0;
    }

    if (move.PieceX == move.TargetX && move.PieceY == move.TargetY)
    {
        return 0;
    }

    toRight = move.TargetX - move.PieceX;
    toTop = move.TargetY - move.PieceY;

    if (toRight != 0 && toTop != 0)
    {
        return 0;
    }

    toRight = toRight > 0 ? 1 : -1;
    toTop = toTop > 0 ? 1 : -1;

    Piece p = board->cells[move.PieceX + toRight][move.PieceY + toTop];
    if (p == EMPTY)
    {
        return 0;
    }

    return p;
}

/* Initialize the board with random pieces */
Board *initBoard(int N)
{
    int i, j, random;
    Piece p;
    Board *board;
    if (N % 2 != 0) {
        printf("Board size must be an even number\n");
        return NULL;
    }

    board = (Board *)malloc(sizeof(Board));
    board->size = N;
    board->cells = (Piece **)malloc(N * sizeof(Piece *));
    for (i = 0; i < N; i++)
    {
        board->cells[i] = (Piece *)malloc(N * sizeof(Piece));
        for (j = 0; j < N; j++)
        {
            /* check if the cell is in the middle */
            if ( (i == N/2-1 || i == N/2) && (j == N/2-1 || j == N/2) )
            {
                board->cells[i][j] = EMPTY;
            }
            else 
            {
                random = rand() % 5;
                switch (random)
                {
                case 0:
                    p = BLUE;
                    break;
                case 1:
                    p = GREEN;
                    break;
                case 2:
                    p = YELLOW;
                    break;
                case 3:
                    p = ORANGE;
                    break;
                case 4:
                    p = RED;
                    break;
                }
                board->cells[i][j] = p;
            }
        }
    }

    return board;
}

/* Free the memory allocated for the board */
void freeBoard(Board *board)
{
    int i;
    for (i = 0; i < board->size; i++)
    {
        free(board->cells[i]);
    }
    free(board->cells);
    free(board);
}

/* Print the board */
void printBoard(Board *board)
{
    int i, j;
    for (i = 0; i < board->size; i++)
    {
        for (j = 0; j < board->size; j++)
        {
            printf("%c ", board->cells[i][j]);
        }
        printf("\n");
    }
}

void movePiece(Board *board, Move move)
{
    board->cells[move.TargetX][move.TargetY] = board->cells[move.PieceX][move.PieceY];
    board->cells[move.PieceX][move.PieceY] = EMPTY;
}

int main()
{
    int N;
    int isGameOver = 0;

    srand(time(NULL));

    printf("Enter the board size (n): ");
    scanf("%d", &N);

    Board *board = initBoard(N);
    if (board == NULL)
    {
        return 1;
    }
    printBoard(board);

    Player player1 = {"Ensar", 0, {0, 0, 0, 0, 0}};
    Player player2 = {"Orçun", 0, {0, 0, 0, 0, 0}};


    while (!isGameOver)
    {
        printf("p1 move: ");
        Move move;
        int x, y;
        Piece p;

        printf("Enter the coordinates of the piece to move\n");
        printf("X Y: ");
        scanf("%d %d", &move.PieceX, &move.PieceY);

        printf("Enter the coordinates of the target location\n");
        printf("X Y: ");
        scanf("%d %d", &x, &y);
        move.PieceX = x;
        move.PieceY = y;

        p = isMoveValid(board, move);
        if (p == 0)
        {
            printf("Invalid move\n");
            continue;
        }

        printf("Piece takan: %c\n", p);

        movePiece(board, move);
        printBoard(board);

        printf("=====================================\n");



    }


    freeBoard(board);

    return 0;
}

