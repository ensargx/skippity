#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INVALID_PIECE 0

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

typedef enum _Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct _Move {
    int PieceX;
    int PieceY;
    Direction direction;
    struct _Move *next;
} Move;

typedef enum _PlayerType {
    HUMAN,
    COMPUTER
} PlayerType;

typedef struct _Player {
    PlayerType type;
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
 * - INVALID_PIECE if the move is invalid
 * - Piece: the piece that is taken by the move
*/
Piece isMoveValid(Board *board, Move *move)
{
    int toRight = move->direction == RIGHT ? 1 : move->direction == LEFT ? -1 : 0;
    int toTop = move->direction == UP ? -1 : move->direction == DOWN ? 1 : 0;

    Piece p = board->cells[move->PieceX + toRight][move->PieceY + toTop];
    if (p == EMPTY)
    {
        return INVALID_PIECE;
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

Move *createMove(int x, int y, Direction direction)
{
    Move *move = (Move *)malloc(sizeof(Move));
    move->PieceX = x;
    move->PieceY = y;
    move->direction = direction;
    move->next = NULL;
    return move;
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

void movePiece(Board *board, Move *move)
{
    if (move == NULL)
        return;

    int toBottom = move->direction == DOWN ? +2 : move->direction == UP ? -2 : 0;
    int toRight = move->direction == RIGHT ? +2 : move->direction == LEFT ? -2 : 0;
    printf("Direction: %d\n", move->direction);

    printf("Moving piece from (%d, %d) to (%d, %d)\n", move->PieceX, move->PieceY, move->PieceX + toBottom, move->PieceY + toRight);

    board->cells[move->PieceX + toBottom][move->PieceY + toRight] = board->cells[move->PieceX][move->PieceY];
    board->cells[move->PieceX][move->PieceY] = EMPTY;

    toRight /= 2;
    toBottom /= 2;

    board->cells[move->PieceX + toBottom][move->PieceY + toRight] = EMPTY;
    return movePiece(board, move->next);
}

int isNextMoveAvailable(Board *board, Move *move)
{
    printf("1\n");
    /* check if can move right */ 
    if (board->cells[move->PieceX + 1][move->PieceY] != EMPTY && board->cells[move->PieceX + 2][move->PieceY] == EMPTY)
    {
        return 1;
    }

    printf("2\n");
    /* check if can move left */
    if (
            board->size > move->PieceX - 2 &&
            move->PieceX - 2 >= 0 &&
            board->cells[move->PieceX - 1][move->PieceY] != EMPTY && 
            board->cells[move->PieceX - 2][move->PieceY] == EMPTY
        )
    {
        return 1;
    }

    printf("3\n");
    /* check if can move up */
    if (board->cells[move->PieceX][move->PieceY - 1] != EMPTY && board->cells[move->PieceX][move->PieceY - 2] == EMPTY)
    {
        return 1;
    }

    printf("4\n");
    /* check if can move down */ 
    if (
            board->size > move->PieceY + 2 &&
            move->PieceY + 2 >= 0 &&
            board->cells[move->PieceX][move->PieceY + 1] != EMPTY &&
            board->cells[move->PieceX][move->PieceY + 2] == EMPTY
        )
    {
        return 1;
    }

    return 0;
}

Move *humanMakeMove(Board *board)
{
    int x, y;
    char direction;
    Move *move;
    Move *firstMove;
    int nextMoveAvailable = 1;
    printf("Enter the coordinates of the piece to move\n");
    printf("X Y: ");
    scanf("%d %d", &x, &y);
    move = createMove(x - 1, y - 1, 0);
    firstMove = move;

    while (nextMoveAvailable)
    {
        printf("Enter the direction to move (W, A, S, D): ");
        scanf(" %c", &direction);
        if (direction > 'A')
        {
            direction -= 'a' - 'A';
        }
        switch (direction)
        {
        case 'W':
            move->direction = UP;
            break;
        case 'S':
            move->direction = DOWN;
            break;
        case 'A':
            move->direction = LEFT;
            break;
        case 'D':
            move->direction = RIGHT;
            break;
        default:
            printf("Invalid direction\n");
            printf("BURAYI İMPLEMENT ET BOZUK BURA\n");
        }
        Piece c = isMoveValid(board, move);
        if (c == INVALID_PIECE)
        {
            printf("Invalid move\n");
            printf("BURAYI İMPLEMENT ET BOZUK BURA\n");
            continue;
        }
        movePiece(board, move);
        move->PieceY += (move->direction == LEFT ? -1 : move->direction == RIGHT ? 1 : 0);
        move->PieceX += (move->direction == UP ? -1 : move->direction == DOWN ? 1 : 0);
        printBoard(board);

        printf("Piece taken: %c\n", c);

        nextMoveAvailable = isNextMoveAvailable(board, move);
        if (nextMoveAvailable)
        {
            printf("Next move available\n");
            Move *nextMove = createMove(move->PieceX, move->PieceY, move->direction);
            move->next = nextMove;
            move = nextMove;
            move->PieceX += (move->direction == UP ? -2 : move->direction == DOWN ? 2 : 0);
            move->PieceY += (move->direction == LEFT ? -2 : move->direction == RIGHT ? 2 : 0);  
        }
        else 
        {
            move->next = NULL;
            nextMoveAvailable = 0;
        }
    }

    return firstMove;
}

Move *computerMakeMove(Board *board)
{
    Move *move = createMove(0, 0, 0);
    move->PieceX = rand() % board->size;
    move->PieceY = rand() % board->size;
    move->direction = rand() % 4;

    return move;
}

/* Make a move for the player
 *
 * Parameters:
 * - board: the game board
 * - player: the player who will make the move
 * Returns:
 * - the move made by the player
*/
Move *playerMakeMove(Board *board, Player player)
{
    if (player.type == HUMAN)
        return humanMakeMove(board);
    else
        return computerMakeMove(board);
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

    Player player1 = {HUMAN, "Ensar", 0, {0, 0, 0, 0, 0}};
    Player player2 = {HUMAN, "Orçun", 0, {0, 0, 0, 0, 0}};

    while (!isGameOver)
    {
        printf("p1 move: ");
        Move *move;
        int x, y;
        Piece p;

        move = playerMakeMove(board, player1);
        printf("=====================================\n");
    }


    freeBoard(board);

    return 0;
}

