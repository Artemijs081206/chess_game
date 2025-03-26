#include "Board.h"             // Подключаем заголовочный файл Board.h, содержащий объявление класса Board и перечисление CellState
#include <iostream>            // Подключаем библиотеку для ввода/вывода (например, для отладки)
#include <cmath>               // Подключаем математическую библиотеку (например, для функции std::abs)

//
// Конструктор класса Board
//
Board::Board(SDL_Texture* boardTexture, SDL_Texture* whitePieceTexture, SDL_Texture* blackPieceTexture,
             SDL_Texture* selectedW, SDL_Texture* selectedB,
             SDL_Texture* blackKing, SDL_Texture* blackKingS,
             SDL_Texture* whiteKing, SDL_Texture* whiteKingS)
    : boardTexture(boardTexture),                   // Инициализируем текстуру игрового поля
      whitePieceTexture(whitePieceTexture),         // Инициализируем текстуру белой шашки
      blackPieceTexture(blackPieceTexture),         // Инициализируем текстуру черной шашки
      selectedW(selectedW),                         // Инициализируем текстуру выделенной белой шашки
      selectedB(selectedB),                         // Инициализируем текстуру выделенной черной шашки
      blackKing(blackKing),                         // Инициализируем текстуру белой дамки (обратите внимание, что названия могут быть перепутаны)
      blackKingS(blackKingS),                       // Инициализируем текстуру выделенной белой дамки
      whiteKing(whiteKing),                         // Инициализируем текстуру черной дамки
      whiteKingS(whiteKingS)                        // Инициализируем текстуру выделенной черной дамки
{
    initBoard();                                    // Инициализируем начальное состояние игрового поля
}

//
// Метод для инициализации игрового поля
//
void Board::initBoard() {
    for (int y = 0; y < 8; y++) {                   // Проходим по всем строкам игрового поля (8 строк)
        for (int x = 0; x < 8; x++) {               // Проходим по всем столбцам игрового поля (8 столбцов)
            if ((x + y) % 2 == 1) {                 // Если сумма координат нечетная – клетка "игровая" (обычно светлая)
                if (y < 3)
                    board[y][x] = white_checker;    // Если клетка находится в верхних 3 строках, ставим белую шашку
                else if (y > 4)
                    board[y][x] = black_checker;    // Если клетка находится в нижних 3 строках, ставим черную шашку
                else
                    board[y][x] = empty;            // В остальных случаях клетка остается пустой
            } else {
                board[y][x] = black_cell;           // Если сумма координат четная – клетка "неигровая" (обычно темная)
            }
        }
    }
}

//
// Метод для получения состояния клетки по координатам (x, y)
//
int Board::getCell(int x, int y) const {
    return board[y][x];                           // Возвращаем значение из массива board по координатам y и x
}

//
// Метод для установки состояния клетки по координатам (x, y)
//
void Board::setCell(int x, int y, int value) {
    board[y][x] = value;                          // Записываем новое значение в массив board по координатам y и x
}

//
// Метод для выделения или снятия выделения с клетки (например, при выборе шашки)
//
void Board::selectCell(int x, int y, bool select) {
    if (select) {                                 // Если нужно выделить клетку
        if (board[y][x] == white_checker) board[y][x] = black_selected;  // Если в клетке белая шашка, меняем на состояние выделенной белой шашки
        else if (board[y][x] == black_checker) board[y][x] = white_selected; // Если в клетке черная шашка, меняем на состояние выделенной черной шашки
        else if (board[y][x] == black_king) board[y][x] = black_king_selected;  // Если в клетке белая дамка, меняем на состояние выделенной белой дамки
        else if (board[y][x] == white_king) board[y][x] = white_king_selected;  // Если в клетке черная дамка, меняем на состояние выделенной черной дамки
    } else {                                      // Если нужно снять выделение с клетки
        if (board[y][x] == black_selected) board[y][x] = white_checker;        // Снимаем выделение с белой шашки
        else if (board[y][x] == white_selected) board[y][x] = black_checker;     // Снимаем выделение с черной шашки
        else if (board[y][x] == black_king_selected) board[y][x] = black_king;   // Снимаем выделение с белой дамки
        else if (board[y][x] == white_king_selected) board[y][x] = white_king;   // Снимаем выделение с черной дамки
    }
}

//
// Метод для отрисовки игрового поля и шашек
//
void Board::draw(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, boardTexture, NULL, NULL);  // Рисуем игровое поле, используя текстуру boardTexture

    for (int y = 0; y < 8; y++) {                    // Проходим по каждой строке игрового поля
        for (int x = 0; x < 8; x++) {                // Проходим по каждому столбцу игрового поля
            SDL_Texture* pieceTexture = nullptr;     // Инициализируем указатель на текстуру шашки как nullptr
            switch (board[y][x]) {                   // Выбираем текстуру в зависимости от состояния клетки
                case white_checker: pieceTexture = whitePieceTexture; break;         // Белая шашка
                case black_checker: pieceTexture = blackPieceTexture; break;         // Черная шашка
                case black_selected: pieceTexture = selectedW; break;                // Выделенная белая шашка
                case white_selected: pieceTexture = selectedB; break;                // Выделенная черная шашка
                case black_king: pieceTexture = whiteKing; break;                    // Белая дамка (обратите внимание на соответствие текстур)
                case white_king: pieceTexture = blackKing; break;                    // Черная дамка
                case black_king_selected: pieceTexture = whiteKingS; break;          // Выделенная белая дамка
                case white_king_selected: pieceTexture = blackKingS; break;          // Выделенная черная дамка
                default: break;                                                    // Если состояние клетки не соответствует ни одному варианту
            }
            if (pieceTexture) {                      // Если текстура для данной клетки определена (не nullptr)
                SDL_Rect rect = {                    // Определяем прямоугольник для отрисовки шашки
                    x * CELL_SIZE + (CELL_SIZE - PIECE_RADIUS * 2) / 2,  // Вычисляем координату X с учетом центра клетки
                    y * CELL_SIZE + (CELL_SIZE - PIECE_RADIUS * 2) / 2,  // Вычисляем координату Y с учетом центра клетки
                    PIECE_RADIUS * 2,               // Ширина прямоугольника равна диаметру шашки
                    PIECE_RADIUS * 2                // Высота прямоугольника равна диаметру шашки
                };
                SDL_RenderCopy(renderer, pieceTexture, NULL, &rect); // Отрисовываем текстуру шашки в заданном прямоугольнике
            }
        }
    }
}

//
// Метод для проверки, нужно ли превратить шашку в дамку
//
void Board::checkForKing(int x, int y) {
    int piece = board[y][x];                     // Получаем текущее состояние клетки (тип шашки)
    if (piece == black_checker && y == 0)          // Если это черная шашка и она достигла верхней строки
        board[y][x] = white_king;                // Превращаем ее в белую дамку (константа white_king)
    else if (piece == white_checker && y == 7)     // Если это белая шашка и она достигла нижней строки
        board[y][x] = black_king;                // Превращаем ее в черную дамку (константа black_king)
}

//
// Статический метод для проверки, принадлежат ли шашки одному игроку (друзья)
//
bool Board::isFriendly(int cell, int piece) {
    bool isWhite = (piece == white_checker || piece == black_selected ||
                    piece == black_king || piece == black_king_selected); // Определяем, является ли шашка белой (включая выделенные и дамки)
    if (isWhite)
        return (cell == white_checker || cell == black_selected ||
                cell == black_king || cell == black_king_selected); // Если шашка белая, проверяем, что клетка содержит белую шашку или дамку
    else
        return (cell == black_checker || cell == white_selected ||
                cell == white_king || cell == white_king_selected); // Если шашка черная, проверяем, что клетка содержит черную шашку или дамку
}

//
// Метод для проверки возможности захвата шашкой противника (для обычной шашки)
//
bool Board::canCapture(int x, int y, int piece) {
    int directions[4][2] = { {-1, -1}, {1, -1}, {-1, 1}, {1, 1} }; // Массив направлений: все 4 диагональных направления
    for (int i = 0; i < 4; i++) {                 // Проходим по каждому направлению
        int midX = x + directions[i][0];         // Вычисляем координату средней клетки по X
        int midY = y + directions[i][1];         // Вычисляем координату средней клетки по Y
        int endX = x + directions[i][0] * 2;       // Вычисляем координату целевой клетки по X (через среднюю)
        int endY = y + directions[i][1] * 2;       // Вычисляем координату целевой клетки по Y (через среднюю)
        if (midX < 0 || midX >= 8 || midY < 0 || midY >= 8) continue; // Если средняя клетка выходит за пределы доски, переходим к следующему направлению
        if (endX < 0 || endX >= 8 || endY < 0 || endY >= 8) continue; // Если целевая клетка выходит за пределы доски, переходим к следующему направлению
        int middlePiece = getCell(midX, midY);     // Получаем состояние средней клетки
        int endCell = getCell(endX, endY);         // Получаем состояние целевой клетки
        bool isWhite = (piece == white_checker || piece == black_selected ||
                        piece == black_king || piece == black_king_selected); // Определяем, является ли текущая шашка белой
        bool isBlack = (piece == black_checker || piece == white_selected ||
                        piece == white_king || piece == white_king_selected); // Определяем, является ли текущая шашка черной
        if (endCell == empty) {                    // Если целевая клетка пуста
            if (isWhite && (middlePiece == black_checker || middlePiece == white_selected ||
                            middlePiece == white_king || middlePiece == white_king_selected))
                return true;                     // Если шашка белая и в средней клетке находится вражеская шашка, захват возможен
            if (isBlack && (middlePiece == white_checker || middlePiece == black_selected ||
                            middlePiece == black_king || middlePiece == black_king_selected))
                return true;                     // Если шашка черная и в средней клетке находится вражеская шашка, захват возможен
        }
    }
    return false;                                // Если ни в одном из направлений захват невозможен, возвращаем false
}

//
// Метод для проверки возможности захвата дамкой противника
//
bool Board::canKingCapture(int x, int y, int piece) {
    int directions[4][2] = { {1, 1}, {-1, 1}, {1, -1}, {-1, -1} }; // Массив направлений для дамки (все диагонали)
    for (int d = 0; d < 4; d++) {                // Проходим по каждому из направлений
        int dx = directions[d][0];              // Определяем смещение по X для данного направления
        int dy = directions[d][1];              // Определяем смещение по Y для данного направления
        int curX = x + dx;                      // Инициализируем текущую координату X
        int curY = y + dy;                      // Инициализируем текущую координату Y
        bool enemyFound = false;                // Флаг, указывающий, найден ли противник на пути дамки
        while (curX >= 0 && curX < 8 && curY >= 0 && curY < 8) { // Пока текущие координаты находятся в пределах доски
            int cellVal = getCell(curX, curY);  // Получаем состояние текущей клетки
            if (cellVal == empty) {             // Если клетка пуста
                if (enemyFound)
                    return true;                // Если ранее был найден противник, дамка может захватывать – возвращаем true
                curX += dx;                     // Переходим к следующей клетке в том же направлении по X
                curY += dy;                     // Переходим к следующей клетке в том же направлении по Y
            } else {
                if (isFriendly(cellVal, piece)) // Если найденная шашка принадлежит тому же игроку
                    break;                      // Захват невозможен, выходим из цикла
                if (!enemyFound) {              // Если противник еще не был найден
                    enemyFound = true;          // Устанавливаем флаг, что противник найден
                    curX += dx;                 // Переходим к следующей клетке по X
                    curY += dy;                 // Переходим к следующей клетке по Y
                } else {                        // Если противник уже был найден ранее
                    break;                      // Захват невозможен, выходим из цикла
                }
            }
        }
    }
    return false;                               // Если ни в одном направлении захват не возможен, возвращаем false
}
