#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <locale.h>

#define INVALID_PIECE 0

#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_ORANGE "\x1B[35m"
#define COLOR_WHITE "\x1B[37m"
#define BOLD "\x1B[1m"
#define UNDERLINE "\x1B[4m"
#define BLINK "\x1B[5m"
#define INVERT "\x1B[7m"
#define HIDDEN "\x1B[8m"
#define RESET "\x1B[0m"
#define COLOR_RESET "\x1B[0m"

#define PADDING_TOP 3
#define PADDING_LEFT 5


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

void moveCursor(int x, int y)
{
    printf("\033[%d;%dH", x, y);
}

void printAt(int x, int y, const char *text, ...)
{
    va_list args;
    printf("\033[s");
    va_start(args, text);
    printf("\033[%d;%dH", x, y);
    vprintf(text, args);
    va_end(args);
    printf("\033[u");
}

void clearScreen()
{
    printf("\033[2J");
    printf("\033[H");
}

void clearLine(int line)
{
    printf("\033[%d;0H", line);
    printf("\033[K");
}

void clearLines(int start, int end)
{
    int i;
    for (i = start; i <= end; i++)
    {
        clearLine(i);
    }
}

void moveCursorToBottom()
{
    printf("\033[100;0H");
}

void moveCursorToTop()
{
    printf("\033[0;0H");
}

void moveCursorUp(int n)
{
    printf("\033[%dA", n);
}

void moveCursorDown(int n)
{
    printf("\033[%dB", n);
}

void moveCursorRight(int n)
{
    printf("\033[%dC", n);
}

void moveCursorLeft(int n)
{
    printf("\033[%dD", n);
}

void clearInputBuffer()
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void hideCursor()
{
    printf("\033[?25l");
}

void showCursor()
{
    printf("\033[?25h");
}

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
    int toBottom = move->direction == DOWN ? 1 : move->direction == UP ? -1 : 0;
    Piece c;
    if (board->size <= move->PieceX + 2 * toBottom || move->PieceX + 2 * toBottom < 0)
    {
        return INVALID_PIECE;
    }

    if (board->size <= move->PieceY + 2 * toRight || move->PieceY + 2 * toRight < 0)
    {
        return INVALID_PIECE;
    }

    c = board->cells[move->PieceX + toBottom][move->PieceY + toRight];
    if (c == EMPTY)
    {
        return INVALID_PIECE;
    }

    if (board->cells[move->PieceX + 2 * toBottom][move->PieceY + 2 * toRight] != EMPTY)
    {
        return INVALID_PIECE;
    }

    return c;
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
            switch (board->cells[i][j])
            {
            case BLUE:
                printf(COLOR_BLUE "%c " COLOR_RESET, board->cells[i][j]);
                break;
            case GREEN:
                printf(COLOR_GREEN "%c " COLOR_RESET, board->cells[i][j]);
                break;
            case YELLOW:
                printf(COLOR_YELLOW "%c " COLOR_RESET, board->cells[i][j]);
                break;
            case ORANGE:
                printf(COLOR_ORANGE "%c " COLOR_RESET, board->cells[i][j]);
                break;
            case RED:
                printf(COLOR_RED "%c " COLOR_RESET, board->cells[i][j]);
                break;
            case EMPTY:
                printf("%c ", board->cells[i][j]);
                break;
            }
        }
        printf("\n");
    }
}

void render(Board *board, Player player1, Player player2)
{
    int i, j;
    clearScreen();
    moveCursor(PADDING_TOP, PADDING_LEFT);

    for (int i = 0; i <= board->size; i++)
    {
        if (i < 10)
        {
            printf(BOLD "%d " RESET, i);
        }
        else
        {
            printf(BOLD "%c " RESET, 'A' + i - 10);
        }
    }

    for (i = 1; i <= board->size; i++)
    {
        moveCursor(PADDING_TOP + i, PADDING_LEFT);
        if (i < 10)
        {
            printf(BOLD "%d " RESET, i);
        }
        else
        {
            printf(BOLD "%c " RESET, 'A' + i - 10);
        }
    }

    for (i = 0; i < board->size; i++)
    {
        for (j = 0; j < board->size; j++)
        {
            moveCursor(PADDING_TOP + i + 1, PADDING_LEFT + 2 * (j + 1));
            switch (board->cells[i][j])
            {
            case BLUE:
                printf(COLOR_BLUE "%c " COLOR_RESET, board->cells[i][j]);
                break;
            case GREEN:
                printf(COLOR_GREEN "%c " COLOR_RESET, board->cells[i][j]);
                break;
            case YELLOW:
                printf(COLOR_YELLOW "%c " COLOR_RESET, board->cells[i][j]);
                break;
            case ORANGE:
                printf(COLOR_ORANGE "%c " COLOR_RESET, board->cells[i][j]);
                break;
            case RED:
                printf(COLOR_RED "%c " COLOR_RESET, board->cells[i][j]);
                break;
            case EMPTY:
                printf("%c ", board->cells[i][j]);
                break;
            }
        }
    }

    moveCursor(PADDING_TOP, PADDING_LEFT + 2 * board->size + 5);
    printf("%-10s| Score |", "Player");
    printf(COLOR_BLUE " A " COLOR_RESET);
    printf(COLOR_GREEN " B " COLOR_RESET);
    printf(COLOR_YELLOW " C " COLOR_RESET);
    printf(COLOR_ORANGE " D " COLOR_RESET);
    printf(COLOR_RED " E " COLOR_RESET);

    moveCursor(PADDING_TOP + 1, PADDING_LEFT + 2 * board->size + 5);
    printf("%-10s| %5d | ", player1.name, player1.score);
    for (i = 0; i < 5; i++)
    {
        printf("%-2d ", player1.pieces[i]);
    }

    moveCursor(PADDING_TOP + 2, PADDING_LEFT + 2 * board->size + 5);
    printf("%-10s| %5d | ", player2.name, player2.score);
    for (i = 0; i < 5; i++)
    {
        printf("%-2d ", player2.pieces[i]);
    }

    moveCursor(PADDING_TOP + 2 + board->size, PADDING_LEFT + 2 * board->size + 5);
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
    if (
            board->size > move->PieceX + 2 &&
            move->PieceX + 2 >= 0 &&
            board->cells[move->PieceX + 1][move->PieceY] != EMPTY &&
            board->cells[move->PieceX + 2][move->PieceY] == EMPTY
        )
           
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
    if (
            board->size > move->PieceY - 2 &&
            move->PieceY - 2 >= 0 &&
            board->cells[move->PieceX][move->PieceY - 1] != EMPTY &&
            board->cells[move->PieceX][move->PieceY - 2] == EMPTY
        )

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
    char xaxis, yaxis;
    int x, y;
    char direction;
    Move *move;
    Move *firstMove;
    int nextMoveAvailable = 1;
    printf("Enter the piece to move (x, y): ");
    scanf(" %c %c", &xaxis, &yaxis);
    if (xaxis > '0' && xaxis < '9')
    {
        x = xaxis - '0' - 1;
    }
    else if (xaxis >= 'A' && xaxis <= 'Z')
    {
        x = xaxis - 'A' + 9;
    }
    else if (xaxis >= 'a' && xaxis <= 'z')
    {
        x = xaxis - 'a' + 9;
    }

    if (yaxis > '0' && yaxis < '9')
    {
        y = yaxis - '0' - 1;
    }
    else if (yaxis >= 'A' && yaxis <= 'Z')
    {
        y = yaxis - 'A' + 9;
    }
    else if (yaxis >= 'a' && yaxis <= 'z')
    {
        y = yaxis - 'a' + 9;
    }

    move = createMove(x, y, 0);

    if (isNextMoveAvailable(board, move) == 0)
    {
        printf("Seçeceğin taşın amk git başka taş seç\n");
        return move;
    }

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
        move->PieceX += (move->direction == UP ? -2 : move->direction == DOWN ? 2 : 0);
        move->PieceY += (move->direction == LEFT ? -2 : move->direction == RIGHT ? 2 : 0);

        printBoard(board);

        printf("Piece moved\n");
        printf("x: %d, y: %d\n", move->PieceX, move->PieceY);

        printf("Piece taken: %c\n", c);

        nextMoveAvailable = isNextMoveAvailable(board, move);
        if (nextMoveAvailable)
        {
            printf("Next move available\n");
            Move *nextMove = createMove(move->PieceX, move->PieceY, move->direction);
            move->next = nextMove;
            move = nextMove;
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
    int i;

    srand(time(NULL));
    setlocale(LC_ALL, "tr_TR.UTF-8");

    printf("Enter the board size (n): ");
    scanf("%d", &N);

    Board *board = initBoard(N);
    if (board == NULL)
    {
        return 1;
    }

    Player player1 = {HUMAN, "Ensar", 0, {0, 0, 0, 0, 0}};
    Player player2 = {HUMAN, "Orcun", 0, {0, 0, 0, 0, 0}};

    render(board, player1, player2);

    while (!isGameOver)
    {
        printf("p1 move: ");
        Move *move;
        int x, y;
        Piece p;

        move = playerMakeMove(board, player1);
    }


    freeBoard(board);

    return 0;
}

