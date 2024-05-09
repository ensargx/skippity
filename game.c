#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <locale.h>
#include <string.h>

#define INVALID_PIECE 0

#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_ORANGE "\x1B[35m"
#define COLOR_WHITE "\x1B[37m"
#define COLOR_BOLD "\x1B[1m"
#define COLOR_UNDERLINE "\x1B[4m"
#define COLOR_BLINK "\x1B[5m"
#define COLOR_INVERT "\x1B[7m"
#define CURSOR_HIDDEN "\x1B[8m"
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
    int playerId;
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

void clearToEnd()
{
    printf("\033[K");
}

void clearToStart()
{
    printf("\033[1K");
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

void printControl(Board *board, char *format, ...)
{
    moveCursor(PADDING_TOP + 4, PADDING_LEFT + 2 * board->size + 5);
    clearToEnd();
    va_list args;
    va_start(args, format);
    printf(COLOR_RED "[Control] " COLOR_RESET);
    vprintf(format, args);
    va_end(args);
}

void printDebug(Board *board, char *format, ...)
{
    va_list args;
    va_start(args, format);
    moveCursor(PADDING_TOP + board->size + 2, 0);
    clearToEnd();
    printf(COLOR_GREEN "[Debug] " COLOR_RESET);
    vprintf(format, args);
    va_end(args);
}

void printError(Board *board, char *format, ...)
{
    va_list args;
    va_start(args, format);
    moveCursor(PADDING_TOP + board->size + 2, 0);
    clearToEnd();
    printf(COLOR_RED "[Error] " COLOR_RESET);
    vprintf(format, args);
    va_end(args);
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
    move->playerId = 0;
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

void whitePiece(Board *board, int x, int y)
{
    moveCursor(PADDING_TOP + x + 1, PADDING_LEFT + 2 * (y + 1));
    printf(COLOR_WHITE "\033[1m%c " COLOR_RESET, board->cells[x][y]);
}

void renderBoard(Board *board)
{
    int i, j;
    moveCursor(PADDING_TOP, PADDING_LEFT);

    for (int i = 0; i <= board->size; i++)
    {
        if (i < 10)
        {
            printf(COLOR_BOLD "%d " RESET, i);
        }
        else
        {
            printf(COLOR_BOLD "%c " RESET, 'A' + i - 10);
        }
    }

    for (i = 1; i <= board->size; i++)
    {
        moveCursor(PADDING_TOP + i, PADDING_LEFT);
        if (i < 10)
        {
            printf(COLOR_BOLD "%d " RESET, i);
        }
        else
        {
            printf(COLOR_BOLD "%c " RESET, 'A' + i - 10);
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
}

void render(Board *board, Player *player1, Player *player2)
{
    int i, j;
    clearScreen();
    renderBoard(board);

    moveCursor(PADDING_TOP, PADDING_LEFT + 2 * board->size + 5);
    printf("%-10s| Score |", "Player");
    printf(COLOR_BLUE " A " COLOR_RESET);
    printf(COLOR_GREEN " B " COLOR_RESET);
    printf(COLOR_YELLOW " C " COLOR_RESET);
    printf(COLOR_ORANGE " D " COLOR_RESET);
    printf(COLOR_RED " E " COLOR_RESET);

    moveCursor(PADDING_TOP + 1, PADDING_LEFT + 2 * board->size + 5);
    printf("%-10s| %5d | ", player1->name, player1->score);
    for (i = 0; i < 5; i++)
    {
        printf("%-2d ", player1->pieces[i]);
    }

    moveCursor(PADDING_TOP + 2, PADDING_LEFT + 2 * board->size + 5);
    printf("%-10s| %5d | ", player2->name, player2->score);
    for (i = 0; i < 5; i++)
    {
        printf("%-2d ", player2->pieces[i]);
    }
}

void movePiece(Board *board, Move *move)
{
    if (move == NULL)
        return;

    int toBottom = move->direction == DOWN ? +2 : move->direction == UP ? -2 : 0;
    int toRight = move->direction == RIGHT ? +2 : move->direction == LEFT ? -2 : 0;

    board->cells[move->PieceX + toBottom][move->PieceY + toRight] = board->cells[move->PieceX][move->PieceY];
    board->cells[move->PieceX][move->PieceY] = EMPTY;

    toRight /= 2;
    toBottom /= 2;

    board->cells[move->PieceX + toBottom][move->PieceY + toRight] = EMPTY;
    return movePiece(board, move->next);
}

int isNextMoveAvailable(Board *board, Move *move)
{
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

Direction getDirection(Board *board)
{
    char direction;

    scanf(" %c", &direction);
    if (direction == 'x')
    {
        return -1;
    }
    if (direction > 'A')
    {
        direction -= 'a' - 'A';
    }

    switch (direction)
    {
    case 'W':
        return UP;
        break;
    case 'S':
        return DOWN;
        break;
    case 'A':
        return LEFT;
        break;
    case 'D':
        return RIGHT;
        break;
    default:
        printError(board, "Invalid direction\n");
        return getDirection(board);
    }
    return -1;
}

Move *humanMakeMove(Board *board, Player *player, Player *Opponent)
{
    char xaxis, yaxis;
    Piece selected;
    int x, y;
    char direction;
    Move *move;
    Move *firstMove;
    int nextMoveAvailable = 1;
    int score, i;
    printControl(board, COLOR_BLUE "%s" COLOR_RESET " make your move: ", player->name);
    scanf(" %c", &xaxis);
    if (xaxis == 'q')
    {
        return NULL;
    }
    scanf(" %c", &yaxis);
    if (xaxis > '0' && xaxis <= '9')
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

    if (yaxis > '0' && yaxis <= '9')
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
        printError(board, "No move available\n");
        return humanMakeMove(board, player, Opponent);
    }

    selected = board->cells[x][y];

    firstMove = move;

    while (nextMoveAvailable)
    {
        int dirValid = 0;
        whitePiece(board, move->PieceX, move->PieceY);
        printControl(board, COLOR_BLUE "Direction: " COLOR_RESET);

        Piece c;
        while (dirValid == 0)
        {
            Direction dir = getDirection(board);
            if (dir == -1)
            {
                return firstMove;
            }
            move->direction = dir;
            c = isMoveValid(board, move);
            if (c != INVALID_PIECE)
            {
                dirValid = 1;
            }
            else
            {
                printError(board, "Invalid move\n");
            }
        }
        player->pieces[c - 'A']++;

        /* calculate the score, */ 
        score = 1;
        for (i = 0; i < 5; i++)
        {
            if (player->pieces[i] == 0)
            {
                score = 0;
            }
        }
        if (score)
        {
            player->score++;
            for (i = 0; i < 5; i++)
            {
                player->pieces[i]--;
            }
        }

        movePiece(board, move);
        move->PieceX += (move->direction == UP ? -2 : move->direction == DOWN ? 2 : 0);
        move->PieceY += (move->direction == LEFT ? -2 : move->direction == RIGHT ? 2 : 0);

        renderBoard(board);

        nextMoveAvailable = isNextMoveAvailable(board, move);
        if (nextMoveAvailable)
        {
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
Move *playerMakeMove(Board *board, Player *player, Player *opponent)
{
    if (player->type == HUMAN)
        return humanMakeMove(board, player, opponent);
    else
        return computerMakeMove(board);
}

int main()
{
    int N;
    int isGameOver = 0;
    int i;
    Move *move, *firstMove;
    Move *nextMove;

    move = malloc(sizeof(Move));
    nextMove = malloc(sizeof(Move));

    srand(time(NULL));
    setlocale(LC_ALL, "tr_TR.UTF-8");

    printf("Enter the board size (n): ");
    scanf("%d", &N);
    if (N % 2 != 0)
    {
        printf("Board size must be an even number\n");
        return 1;
    } 
    if (N > 20)
    {
        printf("Board size must be less than 20\n");
        return 1;
    }
    if (N < 4)
    {
        printf("Board size must be greater than 4\n");
        return 1;
    }

    Board *board = initBoard(N);
    if (board == NULL)
    {
        return 1;
    }

    Player *player1 = malloc(sizeof(Player));
    Player *player2 = malloc(sizeof(Player));
    strncpy(player1->name, "Ensar", 50);
    player1->type = HUMAN;
    player1->score = 0;
    for (i = 0; i < 5; i++)
    {
        player1->pieces[i] = 0;
    }

    strncpy(player2->name, "Orcun", 50);
    player2->type = HUMAN;
    player2->score = 0;
    for (i = 0; i < 5; i++)
    {
        player2->pieces[i] = 0;
    }

    render(board, player1, player2);

    i = 0;
    while (!isGameOver)
    {
        if (i % 2 == 0)
        {
            nextMove = playerMakeMove(board, player1, player2);
        }
        else
        {
            nextMove = playerMakeMove(board, player2, player1);
        }

        if (nextMove == NULL)
        {
            isGameOver = 1;
        }
        i++;

        render(board, player1, player2);
    }

    moveCursor(board->size + 5, 0);
    printf(COLOR_RED "Game Over\n" COLOR_RESET);

    freeBoard(board);

    return 0;
}

