/* Yazar: Ensar Gök
 * Github: https://github.com/ensargx/skippity/ 
 * Youtube: https://youtu.be/khZaJd6rALY 
 * 
 * Yapısal Programlama Dersi Proje Ödevi
 * compile: gcc -ansi game.c
*/ 

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
    int id;
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

void savePlayer(char *filename, Player *player)
{
    FILE *file;
    /* open in append mode */
    file = fopen(filename, "a");
    if (file == NULL)
    {
        printf("File not found\n");
        exit(1);
    }
    fprintf(file, "player: id: %d, type: %d, name: %s\n", player->id, player->type, player->name);
    fclose(file);
}

Player *loadPlayer(char *filename, int playerId)
{
    FILE *file;
    Player *player;
    char name[100];
    int id, type;
    file = fopen(filename , "r");
    char line[100];
    if (file == NULL)
    {
        printf("File not found\n");
        exit(1);
    }
    player = (Player *)malloc(sizeof(Player));

    while (fgets(line, 100, file) != NULL)
    {
        if (sscanf(line, "player: id: %d, type: %d, name: %s", &id, &type, name) == 3)
        {
            if (id == playerId)
            {
                player->id = id;
                player->type = type;
                strncpy(player->name, name, 50);
                break;
            }
        }
    }

    fclose(file);
    return player;
}

void saveMove(char *filename, Move move)
{
    FILE *file;
    file = fopen(filename, "a");
    if (file == NULL)
    {
        printf("File not found\n");
        exit(1);
    }
    fprintf(file, "move: player: %d, x: %d, y: %d, direction: %d\n", move.playerId, move.PieceX, move.PieceY, move.direction);
    fclose(file);
}

Board* loadBoard(char *filename)
{
    FILE *file;
    Board *board;
    int i, j;
    char c;
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("File not found\n");
        exit(1);
    }
    board = (Board *)malloc(sizeof(Board));
    fscanf(file, "size: %d\n", &board->size);
    fscanf(file, "board:\n");
    board->cells = (Piece **)malloc(board->size * sizeof(Piece *));
    for (i = 0; i < board->size; i++)
    {
        board->cells[i] = (Piece *)malloc(board->size * sizeof(Piece));
        for (j = 0; j < board->size; j++)
        {
            fscanf(file, "%c", &c);
            board->cells[i][j] = c;
        }
        fscanf(file, "\n");
    }
    fclose(file);
    return board;
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

/* Save the board to a file */ 
void saveBoard(Board *board, char *filename)
{
    FILE *file;
    int i, j;
    file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("File not found\n");
        exit(1);
    }
    fprintf(file, "size: %d\n", board->size);
    fprintf(file, "board:\n");
    for (i = 0; i < board->size; i++)
    {
        for (j = 0; j < board->size; j++)
        {
            fprintf(file, "%c", board->cells[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
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

void loadScores(Player *p1)
{
    int scoreAvail = 1;
    int i;
    while (scoreAvail)
    {
        for (i = 0; i < 5; i++)
        {
            if (p1->pieces[i] == 0)
            {
                scoreAvail = 0;
            }
        }

        if (scoreAvail)
        {
            p1->score++;
            for (i = 0; i < 5; i++)
            {
                p1->pieces[i]--;
            }
        }
    }

}

void renderBoard(Board *board);
void movePiece(Board *board, Move *move);
void loadMoves(char *filename, Board *board, Player *player1, Player *player2, int* lastPlayerId)
{
    FILE *file;
    Move *move = createMove(0, 0, 0);
    char line[100];
    int x, y, direction, playerId;
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("File not found\n");
        exit(1);
    }
    int lastId = 2;
    *lastPlayerId = 1;
    while (fgets(line, 100, file) != NULL)
    {
        if (sscanf(line, "move: player: %d, x: %d, y: %d, direction: %d", &playerId, &x, &y, &direction) == 4)
        {
            move->PieceX = x;
            move->PieceY = y;
            move->direction = direction;
            move->playerId = playerId;
            move->next = NULL;
            Piece c = isMoveValid(board, move);
            movePiece(board, move);
            if (c == INVALID_PIECE)
            {
                renderBoard(board);
                printError(board, "Invalid move, x: %d, y: %d, direction: %d\n", x, y, direction);
                exit(1);
            }
            /* give point to accourding player */
            if (player1->id == playerId)
            {
                player1->pieces[c - 'A']++;
            }
            else if (player2->id == playerId)
            {
                player2->pieces[c - 'A']++;
            }
            lastId = playerId;
        }
    }
    if (lastId == 2)
        *lastPlayerId = 1;
    else
        *lastPlayerId = 2;
    fclose(file);
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

    for (i = 0; i <= board->size; i++)
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

void undoMove(Board *board, Move *move, Piece taken)
{
    int toBottom = move->direction == DOWN ? +2 : move->direction == UP ? -2 : 0;
    int toRight = move->direction == RIGHT ? +2 : move->direction == LEFT ? -2 : 0;

    board->cells[move->PieceX][move->PieceY] = board->cells[move->PieceX + toBottom][move->PieceY + toRight];
    board->cells[move->PieceX + toBottom][move->PieceY + toRight] = EMPTY;

    toRight /= 2;
    toBottom /= 2;

    board->cells[move->PieceX + toBottom][move->PieceY + toRight] = taken;
}

int humanMakeMove(Board *board, Player *player, Player *Opponent, char *outfile)
{
    char xaxis, yaxis;
    Piece selected;
    int x, y;
    char direction;
    Move *move;
    Move last;
    int nextMoveAvailable = 1;
    int score, i, j;
    int redoAvailable = 1;
    int moveAvailable = 0;

    /* check if any move available */ 
    Move *dummy = createMove(0, 0, 0);
    for (i = 0; i < board->size; i++)
    {
        for (j = 0; j < board->size; j++)
        {
            dummy->PieceX = i;
            dummy->PieceY = j;
            if (isMoveValid(board, dummy) != INVALID_PIECE)
            {
                moveAvailable = 1;
            }
        }
    }
    free(dummy);
    if (moveAvailable == 0)
    {
        printError(board, "No move available\n");
        return 0;
    }

    /* get coordinates */ 
    printControl(board, COLOR_BOLD "%s" COLOR_RESET " make your move: ", player->name);
    scanf(" %c", &xaxis);
    if (xaxis == 'q')
    {
        return 0;
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

    if (x < 0 || x >= board->size || y < 0 || y >= board->size)
    {
        printError(board, "Invalid move\n");
        free(move);
        return humanMakeMove(board, player, Opponent, outfile);
    }

    if (board->cells[x][y] == EMPTY)
    {
        printError(board, "Empty cell\n");
        free(move);
        return humanMakeMove(board, player, Opponent, outfile);
    }

    if (isNextMoveAvailable(board, move) == 0)
    {
        printError(board, "No move available\n");
        free(move);
        return humanMakeMove(board, player, Opponent, outfile);
    }

    whitePiece(board, move->PieceX, move->PieceY);

    while (nextMoveAvailable)
    {
        int dirValid = 0;
        Piece c;
        whitePiece(board, move->PieceX, move->PieceY);
        printControl(board, COLOR_BLUE "Direction: " COLOR_RESET);

        while (dirValid == 0)
        {
            Direction dir = getDirection(board);
            if (dir == -1)
            {
                return 1;
            }
            move->direction = dir;
            last = *move;
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
        renderBoard(board);
        move->playerId = player->id;
        /* ask if redo */ 
        if (redoAvailable)
        {
            printControl(board, "Do you want to undo? " COLOR_RED "(y/n)" COLOR_RESET ": ");
            char redo;
            scanf(" %c", &redo);
            if (redo == 'y' || redo == 'Y')
            {
                redoAvailable = 0;
                /* undo the move */ 
                undoMove(board, move, c);
                renderBoard(board);
                
                /* undo the score */
                if (score)
                {
                    player->score--;
                    for (i = 0; i < 5; i++)
                    {
                        player->pieces[i]++;
                    }
                }
                player->pieces[c - 'A']--;


                printControl(board, "Redo move? " COLOR_RED "(y/n)" COLOR_RESET ": ");
                char redo;
                scanf(" %c", &redo);
                if (redo == 'y' || redo =='Y')
                {
                    /* redo the mov, do the first move again */
                    movePiece(board, &last);
                }
                else
                {
                    dirValid = 0;
                    whitePiece(board, move->PieceX, move->PieceY);
                    while (dirValid == 0)
                    {
                        Direction dir = getDirection(board);
                        if (dir == -1)
                        {
                            return 1;
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
                    movePiece(board, move);
                    renderBoard(board);
                    move->playerId = player->id;
                }
            }
        }
        saveMove(outfile, *move);
        move->PieceX += (move->direction == UP ? -2 : move->direction == DOWN ? 2 : 0);
        move->PieceY += (move->direction == LEFT ? -2 : move->direction == RIGHT ? +2 : 0);
        nextMoveAvailable = isNextMoveAvailable(board, move);
    }

    free(move);
    return 1;
}

int calculateBestScore(int N, int **matrix, int posY, int posX, Direction *direction)
{
    int tmpScore;
    int score = 0;
    Direction tmp_direction = 0;
    int maxScore = 0;
    char dummy;

    /* check if can move up */
    if (
        posX - 2 >= 0 &&
        matrix[posX - 2][posY] == 0 &&
        matrix[posX - 1][posY] != 0
    )
    {
        /* simulate move */
        tmpScore = matrix[posX - 1][posY];
        matrix[posX - 2][posY] = matrix[posX][posY];
        matrix[posX][posY] = 0;
        matrix[posX - 1][posY] = 0;
        tmp_direction = UP;
        score = tmpScore + calculateBestScore(N, matrix, posY, posX - 2, &tmp_direction);
        if (score > maxScore)
        {
            maxScore = score;
            *direction = UP;
        }
        /* undo move */
        matrix[posX][posY] = matrix[posX - 2][posY];
        matrix[posX - 2][posY] = 0;
        matrix[posX - 1][posY] = tmpScore;
    }

    /* check if can move down */
    if (
        posX + 2 < N &&
        matrix[posX + 2][posY] == 0 &&
        matrix[posX + 1][posY] != 0
    )
    {
        /* simulate move */
        tmpScore = matrix[posX + 1][posY];
        matrix[posX + 2][posY] = matrix[posX][posY];
        matrix[posX][posY] = 0;
        matrix[posX + 1][posY] = 0;
        tmp_direction = DOWN;
        score = tmpScore + calculateBestScore(N, matrix, posY, posX + 2, &tmp_direction);
        if (score > maxScore)
        {
            maxScore = score;
            *direction = DOWN;
        }
        /* undo move */
        matrix[posX][posY] = matrix[posX + 2][posY];
        matrix[posX + 2][posY] = 0;
        matrix[posX + 1][posY] = tmpScore;
    }

    /* check if can move left */
    if (
        posY - 2 >= 0 &&
        matrix[posX][posY - 2] == 0 &&
        matrix[posX][posY - 1] != 0
    )
    {
        /* simulate move */
        tmpScore = matrix[posX][posY - 1];
        matrix[posX][posY - 2] = matrix[posX][posY];
        matrix[posX][posY] = 0;
        matrix[posX][posY - 1] = 0;
        tmp_direction = LEFT;
        score = tmpScore + calculateBestScore(N, matrix, posY - 2, posX, &tmp_direction);
        if (score > maxScore)
        {
            maxScore = score;
            *direction = LEFT;
        }
        /* undo move */
        matrix[posX][posY] = matrix[posX][posY - 2];
        matrix[posX][posY - 2] = 0;
        matrix[posX][posY - 1] = tmpScore;
    }
    return maxScore;
}

int computerMakeMove(Board *board, Player *player, Player *opponent, char *outfile)
{
    /*
     * Create a matrix for the board: 
     * 1. If the cell is empty, assign 0 otherwise assign 1
     * 2. If the cell is a piece which player needs, multiply it by 2
     * 3. If the cell is a piece which opponent needs, multiply it by 2
    */

    int i, j, k;
    int x, y;
    int maxScore = 0;
    int canMakeMove = 1;
    int score;

    int **matrix = (int **)malloc(board->size * sizeof(int *));
    for (i = 0; i < board->size; i++)
    {
        matrix[i] = (int *)malloc(board->size * sizeof(int));
        for (j = 0; j < board->size; j++)
        {
            matrix[i][j] = 0;
            if (board->cells[i][j] == EMPTY)
                matrix[i][j] = 0;
            else
                matrix[i][j] = 1;
            
            /* check if opponent wants it */
            for (k = 0; k < 5; k++)
            {
                if (opponent->pieces[k] != 0)
                {
                    if (board->cells[i][j] == 'A' + k)
                    {
                        matrix[i][j] *= 2;
                    }
                }
            }

            /* check if player wants it */
            for (k = 0; k < 5; k++)
            {
                if (player->pieces[k] == 0)
                {
                    if (board->cells[i][j] == 'A' + k)
                    {
                        matrix[i][j] *= 2;
                    }
                }
            }
        }
    }

    /* Find the best move */
    int bestX = 0;
    int bestY = 0;
    Direction bestDirection = UP;
    Direction direction;
    for (i = 0; i < board->size; i++)
    {
        for (j = 0; j < board->size; j++)
        {
            Move dummy;
            dummy.PieceX = i;
            dummy.PieceY = j;
            dummy.direction = 0;
            /* if can move piece, calculate the max score */
            if (matrix[i][j] != 0 && isNextMoveAvailable(board, &dummy))
            {
                score = calculateBestScore(board->size, matrix, j, i, &direction);
                if (score > maxScore)
                {
                    maxScore = score;
                    bestX = i;
                    bestY = j;
                    bestDirection = direction;
                }
            }
        }
    }

    /* make the move */
    Move *move = createMove(bestX, bestY, bestDirection);
    if (maxScore == 0)
    {
        printError(board, "Computer cannot make a move\nGame Over!\n");
        return 0;
    }
    Piece c = isMoveValid(board, move);
    move->playerId = player->id;
    if (c == INVALID_PIECE)
    {
        /* bot give up */
        printError(board, "Computer cannot make a move\nGame Over!\n");
        return 0;
    }

    player->pieces[c - 'A']++;
    movePiece(board, move);

    saveMove(outfile, *move);

    /* free */
    for (i = 0; i < board->size; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
    free(move);
    return 1;
}

/* Make a move for the player
 *
 * Parameters:
 * - board: the game board
 * - player: the player who will make the move
 * Returns:
 * - the move made by the player
*/
int playerMakeMove(Board *board, Player *player, Player *opponent, char *outfile)
{
    if (player->type == HUMAN)
        return humanMakeMove(board, player, opponent, outfile);
    else
        return computerMakeMove(board, player, opponent, outfile);
}

void GameLoop(Board *board, Player *player1, Player *player2, char *outfile, int idx)
{
    int isGameRunning = 1;
    int i = 0;
    Player* currentPlayer;
    Player* nextPlayer;
    Player* tmp;

    if (idx == 1)
    {
        currentPlayer = player1;
        nextPlayer = player2;
    }
    else
    {
        currentPlayer = player2;
        nextPlayer = player1;
    }

    render(board, player1, player2);
    while (isGameRunning)
    {
        if (playerMakeMove(board, currentPlayer, nextPlayer, outfile) == 0)
        {
            isGameRunning = 0;
        }
        tmp = currentPlayer;
        currentPlayer = nextPlayer;
        nextPlayer = tmp;

        render(board, player1, player2);
    }
}

int main()
{
    int N;
    int gameMode;
    int isGameRunning = 1;
    int i;
    char outfile[50];
    Board *board = NULL;
    Player *player1, *player2;

    srand(time(NULL));
    setlocale(LC_ALL, "tr_TR.UTF-8");

    char banner[] = 
">>======================================<<\n"
"||                                      ||\n"
"|| ____  _    _             _ _         ||\n"
"||/ ___|| | _(_)_ __  _ __ (_) |_ _   _ ||\n"
"||\\___ \\| |/ / | '_ \\| '_ \\| | __| | | |||\n"
"|| ___) |   <| | |_) | |_) | | |_| |_| |||\n"
"|||____/|_|\\_\\_| .__/| .__/|_|\\__|\\__, |||\n"
"||             |_|   |_|          |___/ ||\n"
"||                            byEnsarGok||\n"
">>======================================<<\n";

    clearScreen();
    printf("%s\n", banner);

    printf("1- Create a new game\n");
    printf("2- Load a game\n");
    scanf("%d", &i);
    if (i == 1)
    {
        printf("Enter the output file name: ");
        scanf("%s", outfile);

        printf(COLOR_BOLD "Enter the board size: " COLOR_RESET);
        scanf("%d", &N);
        if (N % 2 != 0 || N > 20 || N < 4)
        {
            printf("Invalid board size!\n");
            return 1;
        } 

        board = initBoard(N);
        saveBoard(board, outfile);

        printf(COLOR_BOLD "1-" COLOR_RESET " 1 Player\n" COLOR_BOLD "2-" COLOR_RESET " 2 Players\n" );
        printf(COLOR_WHITE "Game Mode: " COLOR_RESET);
        scanf("%d", &gameMode);

        player1 = malloc(sizeof(Player));
        player2 = malloc(sizeof(Player));
        printf(COLOR_BOLD "Enter the name of the first player: " COLOR_RESET);
        scanf("%s", player1->name);
        player1->type = HUMAN;
        player1->id = 1;
        player1->score = 0;
        for (i = 0; i < 5; i++)
        {
            player1->pieces[i] = 0;
        }

        if (gameMode == 2)
        {
            printf(COLOR_BOLD "Enter the name of the second player: " COLOR_RESET);
            scanf("%s", player2->name);
            player2->type = HUMAN;
        }
        else if (gameMode == 1)
        {
            strncpy(player2->name, "Computer", 50);
            player2->type = COMPUTER;
        }
        player2->score = 0;
        player2->id = 2;
        for (i = 0; i < 5; i++)
        {
            player2->pieces[i] = 0;
        }
        savePlayer(outfile, player1);
        savePlayer(outfile, player2);
        /* start the game */ 
        i = 1;
    }
    else if (i == 2)
    {
        printf("Loading game\n");
        printf("Enter the file name: ");
        scanf("%s", outfile);
        board = loadBoard(outfile);
        player1 = loadPlayer(outfile, 1);
        player2 = loadPlayer(outfile, 2);
        /* load moves for the board */
        loadMoves(outfile, board, player1, player2, &i);
        /* calculate the score */ 
        loadScores(player1);
        loadScores(player2); 
    }
    else
    {
        printf("Invalid option\n");
        return 1;
    }

    GameLoop(board, player1, player2, outfile, i);

    moveCursor(board->size + 5, 0);
    printf(COLOR_RED "Game Over\n" COLOR_RESET);
    
    /* print the winner */ 
    if (player1->score > player2->score)
    {
        printf(COLOR_BOLD "%s" COLOR_RESET " wins\n", player1->name);
    }
    else if (player1->score < player2->score)
    {
        printf(COLOR_BOLD "%s" COLOR_RESET " wins\n", player2->name);
    }
    else
    {
        printf("Draw\n");
    }

    freeBoard(board);
    free(player1);
    free(player2);

    return 0;
}

