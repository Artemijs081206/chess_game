#ifndef BOARD_H                   // Если макрос BOARD_H не определён, начинаем блок защиты от повторного включения
#define BOARD_H                   // Определяем макрос BOARD_H для защиты от повторного включения

#include <SDL2/SDL.h>             // Подключаем заголовочный файл SDL для работы с графическими примитивами и другими функциями SDL

// Размер клетки и радиус шашки
const int CELL_SIZE = 100;        // Определяем размер клетки игрового поля (100 пикселей)
const int PIECE_RADIUS = 45;      // Определяем радиус шашки (45 пикселей)

// Состояния клеток (шашки, выделение, дамки)
enum CellState {                 
    black_cell = -1,              // -1: клетка, на которой нельзя располагать шашки (обычно тёмная клетка)
    empty = 0,                    //  0: пустая клетка, на которой нет шашки
    white_checker = 1,            //  1: белая шашка
    black_checker = 2,            //  2: черная шашка
    black_selected = 3,           //  3: белая шашка, выделенная для хода (отмеченная для перемещения)
    white_selected = 4,           //  4: черная шашка, выделенная для хода (отмеченная для перемещения)
    black_king = 5,               //  5: белая дамка (превращённая шашка)
    white_king = 6,               //  6: черная дамка (превращённая шашка)
    black_king_selected = 7,      //  7: белая дамка, выделенная для хода
    white_king_selected = 8       //  8: черная дамка, выделенная для хода
};

// Объявление класса Board, который управляет игровым полем и логикой шашек
class Board {
public:
    // Конструктор класса Board, принимает текстуры для игрового поля и шашек
    Board(SDL_Texture* boardTexture, SDL_Texture* whitePieceTexture, SDL_Texture* blackPieceTexture,
          SDL_Texture* selectedW, SDL_Texture* selectedB,
          SDL_Texture* blackKing, SDL_Texture* blackKingS,
          SDL_Texture* whiteKing, SDL_Texture* whiteKingS);
    
    void initBoard();             // Метод для инициализации начального состояния игрового поля
    int getCell(int x, int y) const; // Метод для получения состояния клетки по координатам (x, y)
    void setCell(int x, int y, int value); // Метод для установки состояния клетки по координатам (x, y)
    void selectCell(int x, int y, bool select); // Метод для выделения или снятия выделения с клетки
    void draw(SDL_Renderer* renderer); // Метод для отрисовки игрового поля и шашек на рендерере SDL
    void checkForKing(int x, int y); // Метод для проверки, нужно ли превратить шашку в дамку (если шашка достигла противоположной стороны)
    bool canCapture(int x, int y, int piece); // Метод для проверки возможности захвата шашкой противника (обычный захват)
    bool canKingCapture(int x, int y, int piece); // Метод для проверки возможности захвата дамкой противника
    static bool isFriendly(int cell, int piece); // Статический метод для проверки, принадлежат ли две шашки одному игроку (друзья)

private:
    int board[8][8];              // Двумерный массив, представляющий игровое поле (8x8 клеток), содержащий значения типа CellState
    SDL_Texture* boardTexture;    // Текстура игрового поля
    SDL_Texture* whitePieceTexture; // Текстура белой шашки
    SDL_Texture* blackPieceTexture; // Текстура черной шашки
    SDL_Texture* selectedW;       // Текстура выделенной белой шашки
    SDL_Texture* selectedB;       // Текстура выделенной черной шашки
    SDL_Texture* blackKing;       // Текстура белой дамки
    SDL_Texture* blackKingS;      // Текстура выделенной белой дамки
    SDL_Texture* whiteKing;       // Текстура черной дамки
    SDL_Texture* whiteKingS;      // Текстура выделенной черной дамки
};

#endif // BOARD_H               // Конец блока защиты от повторного включения BOARD_H
