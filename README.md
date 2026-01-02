# Hex (A&DS) - Board Analyzer

---

Command-line **C++** application for analyzing **Hex** board positions (size **1–11**) and answering a set of textual queries defined by the assignment.

The program reads a sequence of test cases. Each test case contains:
1) an ASCII drawing of a Hex board (with pawns: `r` for red, `b` for blue, empty otherwise)
2) a list of queries to answer for that board.

### Supported queries
- `BOARD_SIZE` - returns board size `N` (1 – 11)
- `PAWNS_NUMBER` - returns total number of pawns on the board (0 – 121)
- `IS_BOARD_CORRECT` - `YES/NO` (pawn count difference is valid for a game where red starts)
- `IS_GAME_OVER` - `YES RED` / `YES BLUE` / `NO` (whether a player has already connected their target edges)
- `IS_BOARD_POSSIBLE` - `YES/NO`  
  Board is correct **and** could appear in a real game without “skipping” an earlier win.
- `CAN_*_WIN_IN_{1|2}_MOVE_WITH_NAIVE_OPPONENT` - `YES/NO`  
  Can the player win in exactly **N of their own moves** if the opponent plays the **worst possible** defense.

> Note (from assignment): For queries from `IS_BOARD_CORRECT` to all `CAN_*` variants, if the board is incorrect then the answer must be `NO`.


### Game rules (brief)
Players alternate placing their color on an empty cell.  
A player wins by forming a **continuous chain** connecting their two designated board edges:
- **Red**: top <-> bottom
- **Blue**: left <-> right

### Algorithms & techniques
- **Board parsing** from ASCII art into a compact grid representation
- **BFS/DFS connectivity check** (graph search on hex-neighborhood) to detect:
    - `IS_GAME_OVER` (red/blue win paths)
    - intermediate checks used by other queries
- **State validation**
    - pawn-count balance (`IS_BOARD_CORRECT`)
    - plausibility (`IS_BOARD_POSSIBLE`) - detects impossible histories where a win must have happened earlier
- **Search for win-in-N**
    - **Naive opponent**: simulate candidate winning moves + opponent “worst defense”
### Sample Input / Output
**INPUT:**
```
    ---
 --< b >--
<   >-< r >
 --<   >--
    ---
BOARD_SIZE
```
**OUTPUT:**
```
2
```
**INPUT:**
```
    ---
 --< b >--
<   >-< r >
 --<   >--
    ---
PAWNS_NUMBER
```
**OUTPUT:**
```
2
```
**INPUT:**
```
       ---
    --<   >--
 --< b >-<   >--
< b >-< r >-<   >
 --< r >-<   >--
    --< r >--
       ---
IS_BOARD_CORRECT
```
**OUTPUT:**
```
YES
```
**INPUT:**
```
       ---
    --<   >--
 --< b >-<   >--
< b >-< r >-<   >
 --< r >-<   >--
    --< r >--
       ---
IS_GAME_OVER
```
**OUTPUT:**
```
NO
```
**INPUT:**
```
       ---
    --<   >--
 --<   >-< b >--
<   >-< r >-< b >
 --< r >-< b >--
    --< r >--
       ---
IS_BOARD_POSSIBLE
```
**OUTPUT:**
```
YES
```
**INPUT:**
```
       ---
    --<   >--
 --<   >-< b >--
<   >-< r >-< b >
 --< r >-< b >--
    --< r >--
       ---
CAN_RED_WIN_IN_1_MOVE_WITH_NAIVE_OPPONENT
CAN_BLUE_WIN_IN_1_MOVE_WITH_NAIVE_OPPONENT
CAN_RED_WIN_IN_2_MOVES_WITH_NAIVE_OPPONENT
CAN_BLUE_WIN_IN_2_MOVES_WITH_NAIVE_OPPONENT
```
**OUTPUT:**
```
YES
NO
NO
NO
```