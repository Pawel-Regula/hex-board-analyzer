#include<iostream>
#include <string.h>

using namespace std;

#define BASE_BOARD_SIZE 1
#define MAX_BOARD_SIZE 13
#define TOKEN_LEN 4
#define MAX_QUERY_LEN 50

#define BLUE_PAWN 1
#define RED_PAWN 2

#define VISITED_MARK 1

#define MAX_NEIGHBORS 6

#define MIN_BOARD_INDEX 0
#define MAX_BOARD_INDEX 12

#define EMPTY_CELL 0
#define FIRST_PLAYABLE_CELL 1

#define NO_CELL_TO_SKIP 0
#define NOTHING_TO_REMOVE (-1)

const int BLUE_ANCHOR_CELL = 1 * MAX_BOARD_SIZE + 0;
const int RED_ANCHOR_CELL = 0 * MAX_BOARD_SIZE + 1;

struct Node {
    int value;
    Node* next;
    Node* prev;
};

class List
{
public:
    List()
    {
        this->head = nullptr;
        this->tail = nullptr;
    }
    const void pushBack(const int value)
    {
        Node* newNode = new Node;
        newNode->value = value;
        newNode->next = nullptr;
        newNode->prev = nullptr;

        appendNode(newNode);
    }

    const bool isEmpty() const
    {
        if (this->head != nullptr) return false;
        return true;
    }

    ~List()
    {
        while (this->head != nullptr)
        {
            if (this->head->next == nullptr)
            {
                delete this->head;
                this->head = nullptr;
                this->tail = nullptr;
            }
            else
            {
                this->tail = this->tail->prev;
                delete this->tail->next;
                this->tail->next = nullptr;
            }
        }
    }

protected:

    const void printNodeValue(Node* listNode) const
    {
        cout << listNode->value << " ";
    }

    const void appendNode(Node* newNode)
    {
        if (this->tail == nullptr) {
            this->head = newNode;
            this->tail = newNode;
            return;
        }

        newNode->prev = this->tail;
        this->tail = newNode;

        this->tail->prev->next = newNode;
    }

    Node* head;
    Node* tail;

};

class Queue : public virtual List
{
public:
    const void popFront()
    {
        if (this->head == nullptr) return;

        if (this->head->next == nullptr)
        {
            delete this->head;
            this->head = nullptr;
            this->tail = nullptr;
            return;
        }
        
        this->head = this->head->next;
        delete this->head->prev;
        this->head->prev = nullptr;
    }
    const int front() const
    {
        return this->head->value;
    }

};

class Neighbors
{
public:
    Neighbors()
    {
        for (int i = 0; i < MAX_NEIGHBORS; i++) arr[i] = 0;
        count = 0;
    }

    const int& operator[](std::size_t i) const
    {
        return arr[i];
    }

    void pushBack(const int value)
    {
        arr[count] = value;
        count++;
    }

    void popBack()
    {
        if (count > 0)
        {
            arr[count - 1] = 0;
            count--;
        }

    }

    const int back() const
    {
        return arr[count - 1];
    }

    const int size() const
    {
        return count;
    }

    const bool empty() const
    {
        return (count == 0);
    }

private:
    int arr[MAX_NEIGHBORS];
    int count;
};

struct BoardInfo
{
    int redCount = 0;
    int blueCount = 0;
    int boardSize = BASE_BOARD_SIZE;
};

struct QueryResults
{
    int pawnCount = 0;
    bool blueWon = false;
    bool redWon = false;
    bool boardPossible = false;
    bool boardCorrect = false;
};

struct TargetCells
{
    int redTarget = 0, blueTarget = 0;
};

void BFS(Neighbors boardGraph[], int visited[], int root, int visitMark)
{
    Queue Queue;

    Queue.pushBack(root);
    visited[root] = visitMark;

    while (!Queue.isEmpty())
    {
        int vertex = Queue.front();
        Queue.popFront();

        for (int i = 0; i < boardGraph[vertex].size(); i++)
        {
            int neighbor = boardGraph[vertex][i];

            if (visited[neighbor] != visitMark)
            {
                Queue.pushBack(neighbor);
                visited[neighbor] = visitMark;
            }
        }
    }

}

void buildBoardGraph(Neighbors boardGraph[], int board[][MAX_BOARD_SIZE])
{
    int parent;
    for (int j = 0; j < MAX_BOARD_SIZE; j++)
    {
        for (int i = 0; i < MAX_BOARD_SIZE; i++)
        {
            parent = j * MAX_BOARD_SIZE + i;
            // j , i + 1
            if (i != MAX_BOARD_INDEX && (board[j][i] == board[j][i + 1]))
                boardGraph[parent].pushBack(j * MAX_BOARD_SIZE + (i + 1));
            // j , i - 1
            if (i != MIN_BOARD_INDEX && (board[j][i] == board[j][i - 1]))
                boardGraph[parent].pushBack(j * MAX_BOARD_SIZE + (i - 1));
            // j + 1 , i
            if (j != MAX_BOARD_INDEX && (board[j][i] == board[j + 1][i]))
                boardGraph[parent].pushBack((j + 1) * MAX_BOARD_SIZE + i);
            // j - 1 , i
            if (j != MIN_BOARD_INDEX && (board[j][i] == board[j - 1][i]))
                boardGraph[parent].pushBack((j - 1) * MAX_BOARD_SIZE + i);
            // j + 1 , i + 1
            if (j != MAX_BOARD_INDEX && i != MAX_BOARD_INDEX && (board[j][i] == board[j + 1][i + 1]))
                boardGraph[parent].pushBack((j + 1) * MAX_BOARD_SIZE + (i + 1));
            // j - 1 , i - 1
            if (j != MIN_BOARD_INDEX && i != MIN_BOARD_INDEX && (board[j][i] == board[j - 1][i - 1]))
                boardGraph[parent].pushBack((j - 1) * MAX_BOARD_SIZE + (i - 1));
        }
    }

}

void addVertex(Neighbors boardGraph[], int board[][MAX_BOARD_SIZE], const int& x, const int& y)
{
    int parent = y * MAX_BOARD_SIZE + x;
    // j , i + 1
    if (board[y][x] == board[y][x + 1])
    {
        boardGraph[parent].pushBack(y * MAX_BOARD_SIZE + (x + 1));
        boardGraph[y * MAX_BOARD_SIZE + (x + 1)].pushBack(parent);
    }
    // j , i - 1
    if (board[y][x] == board[y][x - 1])
    {
        boardGraph[parent].pushBack(y * MAX_BOARD_SIZE + (x - 1));
        boardGraph[y * MAX_BOARD_SIZE + (x - 1)].pushBack(parent);
    }
    // j + 1 , i
    if (board[y][x] == board[y + 1][x])
    {
        boardGraph[parent].pushBack((y + 1) * MAX_BOARD_SIZE + x);
        boardGraph[(y + 1) * MAX_BOARD_SIZE + x].pushBack(parent);
    }
    // j - 1 , i
    if (board[y][x] == board[y - 1][x])
    {
        boardGraph[parent].pushBack((y - 1) * MAX_BOARD_SIZE + x);
        boardGraph[(y - 1) * MAX_BOARD_SIZE + x].pushBack(parent);
    }
    // j + 1 , i + 1
    if (board[y][x] == board[y + 1][x + 1])
    {
        boardGraph[parent].pushBack((y + 1) * MAX_BOARD_SIZE + (x + 1));
        boardGraph[(y + 1) * MAX_BOARD_SIZE + (x + 1)].pushBack(parent);
    }
    // j - 1 , i - 1
    if (board[y][x] == board[y - 1][x - 1])
    {
        boardGraph[parent].pushBack((y - 1) * MAX_BOARD_SIZE + (x - 1));
        boardGraph[(y - 1) * MAX_BOARD_SIZE + (x - 1)].pushBack(parent);
    }
}

void removeVertex(Neighbors boardGraph[], const int& x, const int& y)
{
    int parent = y * MAX_BOARD_SIZE + x;

    while (!boardGraph[parent].empty())
    {
        if (!boardGraph[boardGraph[parent].back()].empty()) boardGraph[boardGraph[parent].back()].popBack();
        boardGraph[parent].popBack();
    }
}

void clearBoard(int board[][MAX_BOARD_SIZE])
{
    for (int j = 0; j < MAX_BOARD_SIZE; j++)
    {
        for (int i = 0; i < MAX_BOARD_SIZE; i++)
        {
            board[j][i] = EMPTY_CELL;
        }
    }
}

void placePawnAndCount(int& selectedBoardCell, int& pawnCount, const int& pawn)
{
    selectedBoardCell = pawn;
    pawnCount++;
}

void  updateAndSetIndices(int& cellX, int& cellY, int& helperCellX,
    int& helperCellY, const bool& halfRowEndReached)
{
    if (halfRowEndReached) helperCellY++;
    else helperCellX++;

    cellX = helperCellX;
    cellY = helperCellY;
}

void moveToNextCell(int& cellX, int& cellY)
{
    cellX--;
    cellY++;
}

void buildBoardBorder(int board[][MAX_BOARD_SIZE], const int& boardSize)
{
    for (int i = 0; i <= boardSize; i++)
    {
        board[i][boardSize] = BLUE_PAWN;
        board[boardSize][i] = RED_PAWN;
        board[MIN_BOARD_INDEX][i] = RED_PAWN;
        board[i][MIN_BOARD_INDEX] = BLUE_PAWN;
    }
}

void readBoard(int board[][MAX_BOARD_SIZE], BoardInfo& boardInfo,
    int& emptyCellCount, pair<int, int> emptyCells[MAX_BOARD_SIZE * MAX_BOARD_SIZE])
{
    char boardTokens[TOKEN_LEN];
    int helperCellX = BASE_BOARD_SIZE, helperCellY = BASE_BOARD_SIZE;
    int cellX = BASE_BOARD_SIZE, cellY = BASE_BOARD_SIZE;
    bool halfRowStartReached = false, halfRowEndReached = false;

    while (cin >> boardTokens)
    {
        if (!strcmp(boardTokens, "b"))
        {
            placePawnAndCount(board[cellY][cellX], boardInfo.blueCount, BLUE_PAWN);
        }
        else if (!strcmp(boardTokens, "r"))
        {
            placePawnAndCount(board[cellY][cellX], boardInfo.redCount, RED_PAWN);
        }
        else if (!strcmp(boardTokens, "<")) halfRowStartReached = true;
        else if (!strcmp(boardTokens, ">"))
        {
            halfRowEndReached = true;

            if (board[cellY][cellX] == EMPTY_CELL)
            {
                emptyCells[emptyCellCount] = { cellY, cellX };
                emptyCellCount++;
            }

            updateAndSetIndices(cellX, cellY, helperCellX,
                helperCellY, halfRowEndReached);
        }
        else if (!halfRowStartReached && !strcmp(boardTokens, "--<")) boardInfo.boardSize++;
        else if (!strcmp(boardTokens, ">-<"))
        {
            if (board[cellY][cellX] == EMPTY_CELL)
            {
                emptyCells[emptyCellCount] = { cellY, cellX };
                emptyCellCount++;
            }

            moveToNextCell(cellX, cellY);
        }
        else if (!strcmp(boardTokens, ">--"))
        {
            if (board[cellY][cellX] == EMPTY_CELL)
            {
                emptyCells[emptyCellCount] = { cellY, cellX };
                emptyCellCount++;
            }

            updateAndSetIndices(cellX, cellY, helperCellX,
                helperCellY, halfRowEndReached);
        }
        else if (!strcmp(boardTokens, "---")) return;
    }
}

void buildBoard(int board[][MAX_BOARD_SIZE], Neighbors boardGraph[], BoardInfo& boardInfo,
    int& emptyCellCount, pair<int, int> emptyCells[MAX_BOARD_SIZE * MAX_BOARD_SIZE])
{
    clearBoard(board);
    readBoard(board, boardInfo, emptyCellCount, emptyCells);
    buildBoardBorder(board, boardInfo.boardSize + 1);
    buildBoardGraph(boardGraph, board);
}

bool isBoardCorrect(const BoardInfo boardInfo)
{
    return (boardInfo.redCount == boardInfo.blueCount
        || boardInfo.redCount == boardInfo.blueCount + 1);
}

int countPawns(const BoardInfo boardInfo)
{
    return boardInfo.redCount + boardInfo.blueCount;
}

bool hasWon(Neighbors boardGraph[], const int& boardSize, const int& fixedAnchor,
    const int& targetAnchor, const int& cellToSkip)
{
    int visited[MAX_BOARD_SIZE * MAX_BOARD_SIZE] = { EMPTY_CELL };
    if (cellToSkip) visited[cellToSkip] = VISITED_MARK;

    BFS(boardGraph, visited, fixedAnchor, VISITED_MARK);

    if (visited[targetAnchor] == visited[fixedAnchor]) return true;
    return false;
}

bool existsCriticalPawnToRemove(Neighbors boardGraph[], int board[][MAX_BOARD_SIZE],
    const int& boardSize, const int& fixedAnchor, const int& targetAnchor, const int& checkedPlayer)
{
    for (int j = FIRST_PLAYABLE_CELL; j <= boardSize; j++)
    {
        for (int i = FIRST_PLAYABLE_CELL; i <= boardSize; i++)
        {
            if (board[j][i] == checkedPlayer)
            {
                if (!hasWon(boardGraph, boardSize, fixedAnchor,
                    targetAnchor, j * MAX_BOARD_SIZE + i)) return true;
            }
        }
    }
    return false;
}

bool isBoardPossible(Neighbors boardGraph[], int board[][MAX_BOARD_SIZE], const QueryResults queryResults,
    const BoardInfo boardInfo, const TargetCells targetCells)
{
    if (!queryResults.blueWon && !queryResults.redWon) return true;
    else if (queryResults.blueWon && !queryResults.redWon &&
        boardInfo.redCount == boardInfo.blueCount)
    {
        return existsCriticalPawnToRemove(boardGraph, board, boardInfo.boardSize, BLUE_ANCHOR_CELL,
            targetCells.blueTarget, BLUE_PAWN);
    }
    else if (!queryResults.blueWon && queryResults.redWon && boardInfo.redCount ==
        boardInfo.blueCount + 1)
    {
        return existsCriticalPawnToRemove(boardGraph, board, boardInfo.boardSize, RED_ANCHOR_CELL,
            targetCells.redTarget, RED_PAWN);
    }

    return false;
}

bool canWinIn1MoveNaive(Neighbors boardGraph[], int board[][MAX_BOARD_SIZE],
    const pair<int, int> candidateCells[MAX_BOARD_SIZE * MAX_BOARD_SIZE], const int& candidateCellCount,
    const int& boardSize, const int& fixedAnchor, const int& targetAnchor, const int& checkedPlayer,
    pair<int, int> toRemove)
{
    bool hasWonFlag = false;

    for (int i = 0; i < candidateCellCount; i++)
    {
        if (board[candidateCells[i].second][candidateCells[i].first] == EMPTY_CELL)
        {
            board[candidateCells[i].second][candidateCells[i].first] = checkedPlayer;

            addVertex(boardGraph, board, candidateCells[i].first, candidateCells[i].second);

            if (boardGraph[candidateCells[i].second * MAX_BOARD_SIZE + candidateCells[i].first].size() > 1)
            {
                hasWonFlag = hasWon(boardGraph, boardSize, fixedAnchor,
                    targetAnchor, NO_CELL_TO_SKIP);
            }

            board[candidateCells[i].second][candidateCells[i].first] = EMPTY_CELL;

            if (hasWonFlag)
            {
                if (toRemove.first != NOTHING_TO_REMOVE)
                {
                    removeVertex(boardGraph, toRemove.first, toRemove.second);
                }
                removeVertex(boardGraph, candidateCells[i].first, candidateCells[i].second);
                return true;
            }

            removeVertex(boardGraph, candidateCells[i].first, candidateCells[i].second);
        }
    }
    return false;
}

bool canWinIn2MovesNaive(Neighbors boardGraph[], int board[][MAX_BOARD_SIZE],
    const pair<int, int> candidateCells[MAX_BOARD_SIZE * MAX_BOARD_SIZE], const int& candidateCellCount,
    const int& boardSize, const int& fixedAnchor, const int& targetAnchor, const int& checkedPlayer)
{
    bool hasWonFlag = false;
    for (int i = 0; i < candidateCellCount; i++)
    {
        board[candidateCells[i].second][candidateCells[i].first] = checkedPlayer;

        addVertex(boardGraph, board, candidateCells[i].first, candidateCells[i].second);

        if (!hasWon(boardGraph, boardSize, fixedAnchor,
            targetAnchor, NO_CELL_TO_SKIP))
        {

            hasWonFlag = canWinIn1MoveNaive(boardGraph, board, candidateCells, candidateCellCount,
                boardSize, fixedAnchor, targetAnchor, checkedPlayer, candidateCells[i]);

            if (hasWonFlag)
            {
                board[candidateCells[i].second][candidateCells[i].first] = EMPTY_CELL;
                return true;
            }
        }
        removeVertex(boardGraph, candidateCells[i].first, candidateCells[i].second);

        board[candidateCells[i].second][candidateCells[i].first] = EMPTY_CELL;

    }

    return false;
}

bool hasEnoughEmptyCells(const int& player, const int& leadingPlayer, const int& otherPlayer,
    const int& movesCount, const int& emptyCells)
{
    return ((player == otherPlayer && ((movesCount == 1 && emptyCells >= 2) || (movesCount == 2 && emptyCells >= 4)))
        || (player == leadingPlayer && ((movesCount == 1 && emptyCells >= 1) || (movesCount == 2 && emptyCells >= 3))));
}

void addCandidateCell(pair<int, int> candidateCells[MAX_BOARD_SIZE * MAX_BOARD_SIZE], int& candidateCellCount,
    pair<int, int> candidateCell)
{
    candidateCells[candidateCellCount] = candidateCell;
    candidateCellCount++;
}

void buildCandidateCellsWithNeighbors(int board[][MAX_BOARD_SIZE], const pair<int, int> emptyCells[MAX_BOARD_SIZE * MAX_BOARD_SIZE],
    const int& emptyCellCount, pair<int, int> candidateCells[MAX_BOARD_SIZE * MAX_BOARD_SIZE],
    int& candidateCellCount, const int& player, const int& boardSize)
{
    for (int i = 0; i < emptyCellCount; i++)
    {
        if ( player == board[emptyCells[i].first][emptyCells[i].second + 1]
            || player == board[emptyCells[i].first][emptyCells[i].second - 1]
            || player == board[emptyCells[i].first + 1][emptyCells[i].second]
            || player == board[emptyCells[i].first - 1][emptyCells[i].second]
            || player == board[emptyCells[i].first + 1][emptyCells[i].second + 1]
            || player == board[emptyCells[i].first - 1][emptyCells[i].second - 1] )
        {
            addCandidateCell(candidateCells, candidateCellCount, { emptyCells[i].second , emptyCells[i].first });
        }
    }
}

bool canWinInNMovesNaive(Neighbors boardGraph[], int board[][MAX_BOARD_SIZE], pair<int, int> candidateCells[],
    const int& candidateCellCount, const int player, const int movesCount, const QueryResults queryResults,
    const BoardInfo boardInfo, const TargetCells targetCells)
{

    int emptyCells = boardInfo.boardSize * boardInfo.boardSize
        - boardInfo.redCount - boardInfo.blueCount;

    if ((boardInfo.redCount == boardInfo.blueCount &&
        hasEnoughEmptyCells(player, RED_PAWN, BLUE_PAWN, movesCount, emptyCells))
        || (boardInfo.redCount == boardInfo.blueCount + 1 &&
            hasEnoughEmptyCells(player, BLUE_PAWN, RED_PAWN, movesCount, emptyCells)))
    {

        if (player == RED_PAWN && movesCount == 1) return canWinIn1MoveNaive(boardGraph, board,
            candidateCells, candidateCellCount, boardInfo.boardSize, RED_ANCHOR_CELL, targetCells.redTarget,
            RED_PAWN, { NOTHING_TO_REMOVE, NOTHING_TO_REMOVE });

        else if (player == RED_PAWN && movesCount == 2) return canWinIn2MovesNaive(boardGraph, board,
            candidateCells, candidateCellCount, boardInfo.boardSize, RED_ANCHOR_CELL, targetCells.redTarget,
            RED_PAWN);

        else if (player == BLUE_PAWN && movesCount == 1) return canWinIn1MoveNaive(boardGraph, board,
            candidateCells, candidateCellCount, boardInfo.boardSize, BLUE_ANCHOR_CELL, targetCells.blueTarget,
            BLUE_PAWN, { NOTHING_TO_REMOVE, NOTHING_TO_REMOVE });

        else return canWinIn2MovesNaive(boardGraph, board,
            candidateCells, candidateCellCount, boardInfo.boardSize, BLUE_ANCHOR_CELL, targetCells.blueTarget,
            BLUE_PAWN);
    }

    return false;
}

void computeTargetCells(TargetCells& targetCells, const int& boardSize)
{
    targetCells.redTarget = RED_ANCHOR_CELL + (boardSize + 1) * MAX_BOARD_SIZE;
    targetCells.blueTarget = BLUE_ANCHOR_CELL + (boardSize + 1);
}

void computeQueryResults(Neighbors boardGraph[], int board[][MAX_BOARD_SIZE], QueryResults& queryResults,
    TargetCells& targetCells, const BoardInfo boardInfo)
{
    computeTargetCells(targetCells, boardInfo.boardSize);

    queryResults.pawnCount = countPawns(boardInfo);

    queryResults.boardCorrect = isBoardCorrect(boardInfo);
    if (queryResults.boardCorrect)
    {
        queryResults.blueWon = hasWon(boardGraph, boardInfo.boardSize, BLUE_ANCHOR_CELL,
            targetCells.blueTarget, NO_CELL_TO_SKIP);

        queryResults.redWon = hasWon(boardGraph, boardInfo.boardSize, RED_ANCHOR_CELL,
            targetCells.redTarget, NO_CELL_TO_SKIP);

        queryResults.boardPossible = isBoardPossible(boardGraph, board, queryResults,
            boardInfo, targetCells);
    }
}

void answerQueries(Neighbors boardGraph[], int board[][MAX_BOARD_SIZE], const QueryResults queryResults,
    const BoardInfo boardInfo, const TargetCells targetCells, const int& emptyCellCount,
    const pair<int, int> emptyCells[MAX_BOARD_SIZE * MAX_BOARD_SIZE])
{
    char query[MAX_QUERY_LEN];

    cin >> query;

    if (!strcmp(query, "BOARD_SIZE")) printf("%d\n", boardInfo.boardSize);
    else if (!strcmp(query, "PAWNS_NUMBER")) printf("%d\n", queryResults.pawnCount);
    else if (!strcmp(query, "IS_BOARD_CORRECT")) printf("%s\n", queryResults.boardCorrect ? "YES" : "NO");
    else if (!strcmp(query, "IS_GAME_OVER"))
    {
        if (queryResults.blueWon) printf("YES BLUE\n");
        else if (queryResults.redWon) printf("YES RED\n");
        else printf("NO\n");
    }
    else if (!strcmp(query, "IS_BOARD_POSSIBLE")) printf("%s\n", queryResults.boardPossible ? "YES" : "NO");
    else if (!strcmp(query, "CAN_RED_WIN_IN_1_MOVE_WITH_NAIVE_OPPONENT"))
    {
        cin >> query >> query >> query;
        if (!queryResults.boardCorrect || queryResults.blueWon
            || queryResults.redWon || boardInfo.redCount + 2 < boardInfo.boardSize)
        {
            printf("NO\nNO\nNO\nNO\n");
            return;
        }

        int candidateCellCount_n = 0;
        int candidateCellCount_c = 0;
        pair<int, int> candidateCells_n[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
        pair<int, int> candidateCells_c[MAX_BOARD_SIZE * MAX_BOARD_SIZE];

        buildCandidateCellsWithNeighbors(board, emptyCells, emptyCellCount, candidateCells_c, candidateCellCount_c,
            RED_PAWN, boardInfo.boardSize);
        buildCandidateCellsWithNeighbors(board, emptyCells, emptyCellCount, candidateCells_n, candidateCellCount_n,
            BLUE_PAWN, boardInfo.boardSize);

        printf("%s\n", canWinInNMovesNaive(boardGraph, board, candidateCells_c, candidateCellCount_c,
            RED_PAWN, 1, queryResults, boardInfo, targetCells) ? "YES" : "NO");

        printf("%s\n", canWinInNMovesNaive(boardGraph, board, candidateCells_n, candidateCellCount_n, BLUE_PAWN, 1,
            queryResults, boardInfo, targetCells) ? "YES" : "NO");

        printf("%s\n", canWinInNMovesNaive(boardGraph, board, candidateCells_c, candidateCellCount_c,
            RED_PAWN, 2, queryResults, boardInfo, targetCells) ? "YES" : "NO");
        printf("%s\n", canWinInNMovesNaive(boardGraph, board, candidateCells_n, candidateCellCount_n, BLUE_PAWN, 2,
            queryResults, boardInfo, targetCells) ? "YES" : "NO");
    }
}

void hex()
{
    Neighbors* boardGraph = new Neighbors[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
    int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];

    pair<int, int> emptyCells[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
    int emptyCellCount = 0;

    BoardInfo boardInfo;

    QueryResults queryResults;

    TargetCells targetCells;

    buildBoard(board, boardGraph, boardInfo, emptyCellCount, emptyCells);

    computeQueryResults(boardGraph, board, queryResults, targetCells, boardInfo);

    answerQueries(boardGraph, board, queryResults, boardInfo, targetCells, emptyCellCount, emptyCells);

    delete[] boardGraph;
}

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    char boardTokens[TOKEN_LEN];

    while (cin >> boardTokens) hex();

    return 0;
}