#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <cmath>
#include <iostream>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int CELL_SIZE = 100;       // Размер клетки
const int PIECE_RADIUS = 45;     // Радиус шашки

int currentTurn = 1;           // 1 - ход белых, 2 - ход черных
int localPlayer = 1;           // Для сетевой игры: сервер (1) играет белыми, клиент (2) – черными

//-1 недопустимая, тойть чёрная хуйня на поле
//0 - пустая
//1 - чёрные шашки
//2 - белые шашки
//3 - чёрные выделенные шашки
//4 - белые выделеные шашки
//5 - чёрная дамка
//6 - белая дамка

// SDL глобальные переменные
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* boardTexture = nullptr;
SDL_Texture* whitePieceTexture = nullptr;
SDL_Texture* blackPieceTexture = nullptr;
SDL_Texture* selecetedW = nullptr;
SDL_Texture* selecetedB = nullptr;
SDL_Texture* blackKing = nullptr;
SDL_Texture* blackKingS = nullptr;
SDL_Texture* whiteKing = nullptr;
SDL_Texture* whiteKingS = nullptr;

// Сетевые глобальные переменные
bool networkMode = false;
bool isServer = false;
TCPsocket tcpSocket = nullptr;
TCPsocket serverSocket = nullptr;
SDLNet_SocketSet socketSet = nullptr;

// Функция загрузки текстур
SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cout << "Ошибка загрузки: " << path << " | " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Ошибка SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow("Checkers", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;
    
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;
    
    boardTexture     = loadTexture("board.png");
    whitePieceTexture = loadTexture("white_piece.png");
    blackPieceTexture = loadTexture("black_piece.png");
    selecetedW       = loadTexture("white_piece_s.png");
    selecetedB       = loadTexture("black_piece_s.png");
    blackKing        = loadTexture("black_king.png");
    blackKingS       = loadTexture("black_king_s.png");
    whiteKing        = loadTexture("white_king.png");
    whiteKingS       = loadTexture("white_king_s.png");
    
    return boardTexture && whitePieceTexture && blackPieceTexture &&
           selecetedW && selecetedB && blackKing && blackKingS &&
           whiteKing && whiteKingS;
}

void close() {
    SDL_DestroyTexture(boardTexture);
    SDL_DestroyTexture(whitePieceTexture);
    SDL_DestroyTexture(blackPieceTexture);
    SDL_DestroyTexture(selecetedW);
    SDL_DestroyTexture(selecetedB);
    SDL_DestroyTexture(blackKing);
    SDL_DestroyTexture(blackKingS);
    SDL_DestroyTexture(whiteKing);
    SDL_DestroyTexture(whiteKingS);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    if (networkMode) {
        if (tcpSocket) SDLNet_TCP_Close(tcpSocket);
        if (serverSocket) SDLNet_TCP_Close(serverSocket);
        if (socketSet) SDLNet_FreeSocketSet(socketSet);
        SDLNet_Quit();
    }
    
    IMG_Quit();
    SDL_Quit();
}

// Класс игрового поля
class Board {
private:
    int board[8][8];
public:
    Board() {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                if ((x + y) % 2 == 1) {
                    if (y < 3)
                        board[y][x] = 1; // Белые шашки
                    else if (y > 4)
                        board[y][x] = 2; // Черные шашки
                    else
                        board[y][x] = 0; // Пустая клетка
                } else {
                    board[y][x] = -1;  // Недопустимая клетка
                }
            }
        }
    }
    
    int getCell(int x, int y) { return board[y][x]; }
    void setCell(int x, int y, int value) { board[y][x] = value; }
    
    // Функция выделения/снятия выделения шашки
    void Selected(int x, int y, bool select) {
        if (select) {
            if (board[y][x] == 1) board[y][x] = 3;
            else if (board[y][x] == 2) board[y][x] = 4;
            else if (board[y][x] == 5) board[y][x] = 7;
            else if (board[y][x] == 6) board[y][x] = 8;
        } else {
            if (board[y][x] == 3) board[y][x] = 1;
            else if (board[y][x] == 4) board[y][x] = 2;
            else if (board[y][x] == 7) board[y][x] = 5;
            else if (board[y][x] == 8) board[y][x] = 6;
        }
    }
    
    void draw() {
        SDL_RenderCopy(renderer, boardTexture, NULL, NULL);
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                SDL_Texture* pieceTexture = nullptr;
                if (board[y][x] == 1) pieceTexture = whitePieceTexture;
                else if (board[y][x] == 2) pieceTexture = blackPieceTexture;
                else if (board[y][x] == 3) pieceTexture = selecetedW;
                else if (board[y][x] == 4) pieceTexture = selecetedB;
                else if (board[y][x] == 5) pieceTexture = whiteKing;
                else if (board[y][x] == 6) pieceTexture = blackKing;
                else if (board[y][x] == 7) pieceTexture = whiteKingS;
                else if (board[y][x] == 8) pieceTexture = blackKingS;
                
                if (pieceTexture) {
                    SDL_Rect rect = {
                        x * CELL_SIZE + (CELL_SIZE - PIECE_RADIUS * 2) / 2,
                        y * CELL_SIZE + (CELL_SIZE - PIECE_RADIUS * 2) / 2,
                        PIECE_RADIUS * 2,
                        PIECE_RADIUS * 2
                    };
                    SDL_RenderCopy(renderer, pieceTexture, NULL, &rect);
                }
            }
        }
    }
};

Board board;

// Функция, определяющая, является ли клетка с piece дружественной
bool isFriendly(int cell, int piece) {
    bool white = (piece == 1 || piece == 3 || piece == 5 || piece == 7);
    if (white)
        return (cell == 1 || cell == 3 || cell == 5 || cell == 7);
    else
        return (cell == 2 || cell == 4 || cell == 6 || cell == 8);
}

// Проверка возможности взятия дамкой по всем диагоналям
bool canKingCapture(int x, int y, int piece) {
    int directions[4][2] = { {1, 1}, {-1, 1}, {1, -1}, {-1, -1} };
    for (int d = 0; d < 4; d++) {
        int dx = directions[d][0];
        int dy = directions[d][1];
        int curX = x + dx;
        int curY = y + dy;
        bool enemyFound = false;
        while (curX >= 0 && curX < 8 && curY >= 0 && curY < 8) {
            int cellVal = board.getCell(curX, curY);
            if (cellVal == 0) {
                if (enemyFound)
                    return true;
                curX += dx;
                curY += dy;
            } else {
                if (isFriendly(cellVal, piece))
                    break;
                if (!enemyFound) {
                    enemyFound = true;
                    curX += dx;
                    curY += dy;
                } else {
                    break;
                }
            }
        }
    }
    return false;
}

// Проверка возможности взятия обычной шашкой
bool canCapture(int x, int y, int piece) {
    int directions[4][2] = { {-1, -1}, {1, -1}, {-1, 1}, {1, 1} };
    for (int i = 0; i < 4; i++) {
        int midX = x + directions[i][0];
        int midY = y + directions[i][1];
        int endX = x + directions[i][0] * 2;
        int endY = y + directions[i][1] * 2;
        if (midX < 0 || midX >= 8 || midY < 0 || midY >= 8) continue;
        if (endX < 0 || endX >= 8 || endY < 0 || endY >= 8) continue;
        int middlePiece = board.getCell(midX, midY);
        int endCell = board.getCell(endX, endY);
        bool isWhite = (piece == 1 || piece == 3 || piece == 5 || piece == 7);
        bool isBlack = (piece == 2 || piece == 4 || piece == 6 || piece == 8);
        if (endCell == 0) {
            if (isWhite && (middlePiece == 2 || middlePiece == 4 || middlePiece == 6 || middlePiece == 8))
                return true;
            if (isBlack && (middlePiece == 1 || middlePiece == 3 || middlePiece == 5 || middlePiece == 7))
                return true;
        }
    }
    return false;
}

// Проверка превращения шашки в дамку
void checkForKing(int x, int y) {
    int piece = board.getCell(x, y);
    if (piece == 2 && y == 0)
        board.setCell(x, y, 6);
    else if (piece == 1 && y == 7)
        board.setCell(x, y, 5);
}

// Глобальные переменные для выбора шашки
bool selected = false;
int selectedX, selectedY;

// Функция отправки хода по сети (пакет: [fromX, fromY, toX, toY, continuation])
// continuation = 0 – ход завершён, 1 – возможен дополнительный захват
void sendMove(int fromX, int fromY, int toX, int toY, uint8_t continuation) {
    if (!networkMode) return;
    uint8_t packet[5];
    packet[0] = static_cast<uint8_t>(fromX);
    packet[1] = static_cast<uint8_t>(fromY);
    packet[2] = static_cast<uint8_t>(toX);
    packet[3] = static_cast<uint8_t>(toY);
    packet[4] = continuation;
    SDLNet_TCP_Send(tcpSocket, packet, 5);//Этот вызов отправляет 5 байтов данных по TCP-соединению
}

// Функция применения хода, полученного по сети
void applyNetworkMove(int fromX, int fromY, int toX, int toY, uint8_t continuation) {
    int piece = board.getCell(fromX, fromY);
    // Если перемещение более чем на 1 клетку – вероятно, это взятие
    //перепрыгивание через фигуру 
    if (std::abs(toX - fromX) >= 2) {
        int dx = (toX - fromX) / std::abs(toX - fromX);//координаты шашки относительно той что ела чтобы убрать нахуй
        int dy = (toY - fromY) / std::abs(toY - fromY);//
        int enemyX = -1, enemyY = -1;//Если находим фигуру противника, запоминаем ее координаты (enemyX, enemyY).
        //поиск шашки которую убрать нахуй
        for (int i = 1; i < std::abs(toX - fromX); i++) {
            int cx = fromX + i * dx;
            int cy = fromY + i * dy;
            int cellVal = board.getCell(cx, cy);
            if (cellVal != 0 && !isFriendly(cellVal, piece)) {
                enemyX = cx;
                enemyY = cy;
                break;
            }
        }
        if (enemyX != -1 && enemyY != -1)
            board.setCell(enemyX, enemyY, 0);//удаляется с доски (обнуляется в board).
    }
    board.setCell(toX, toY, piece);// Ставим фигуру на новую клетку
    board.setCell(fromX, fromY, 0);// Очищаем старую клетку
    board.Selected(toX, toY, false);
    checkForKing(toX, toY);//проверяет, нужно ли превратить фигуру в короля
    //Проверка продолжения хода
    if (continuation == 0) {
        currentTurn = localPlayer;  // Передаём ход принимающей стороне
        selected = false;
    } else {//типо если 2 раз ходить она должна быть выделена и выбрана сразу
        selected = true;
        selectedX = toX;
        selectedY = toY;
    }
}

void handleMouseClick(int x, int y) {
    // В сетевом режиме обрабатываем клик только если сейчас ход локального игрока
    if (networkMode && currentTurn != localPlayer)
        return;
    
    int cellX = x / CELL_SIZE;
    int cellY = y / CELL_SIZE;
    
    if (!selected) {
        int piece = board.getCell(cellX, cellY);
        if (!networkMode) {
            // Локальная игра: выбор шашки по currentTurn
            if ((currentTurn == 1 && (piece == 1 || piece == 3 || piece == 5 || piece == 7)) ||
                (currentTurn == 2 && (piece == 2 || piece == 4 || piece == 6 || piece == 8))) {
                board.Selected(cellX, cellY, true);
                selected = true;
                selectedX = cellX;
                selectedY = cellY;
            }
        } else {
            // Сетевая игра: выбор по локальному игроку
            if ((localPlayer == 1 && (piece == 1 || piece == 3 || piece == 5 || piece == 7)) ||
                (localPlayer == 2 && (piece == 2 || piece == 4 || piece == 6 || piece == 8))) {
                board.Selected(cellX, cellY, true);
                selected = true;
                selectedX = cellX;
                selectedY = cellY;
            }
        }
    } else {
        //cellX – это текущая координата X, куда игрок хочет переместить фигуру.
        //selectedX – это координата X выделенной фигуры (откуда она перемещается).
        //dx – это разница между ними, то есть на сколько клеток переместилась фигура по X.
        int dx = cellX - selectedX;
        int dy = cellY - selectedY;
        int piece = board.getCell(selectedX, selectedY);
        int fromX = selectedX, fromY = selectedY;
        
        // Ход для дамки: перемещение по диагонали на любое расстояние
        if ((piece == 5 || piece == 6 || piece == 7 || piece == 8) &&
            (std::abs(dx) == std::abs(dy)) && std::abs(dx) >= 1) {
            if (board.getCell(cellX, cellY) != 0) {
                board.Selected(selectedX, selectedY, false);
                selected = false;
                return;
            }
            int stepX = (dx > 0) ? 1 : -1;
            int stepY = (dy > 0) ? 1 : -1;
            int distance = std::abs(dx);
            int enemyCount = 0;
            int enemyX = -1, enemyY = -1;
            for (int i = 1; i < distance; i++) {
                int curX = selectedX + i * stepX;
                int curY = selectedY + i * stepY;
                int cellVal = board.getCell(curX, curY);
                if (cellVal != 0) {
                    if (isFriendly(cellVal, piece)) {
                        enemyCount = 2;
                        break;
                    } else {
                        enemyCount++;
                        enemyX = curX;
                        enemyY = curY;
                    }
                }
            }
            if (enemyCount > 1) {
                board.Selected(selectedX, selectedY, false);
                selected = false;
                return;
            }
            if (enemyCount == 0) {
                board.setCell(cellX, cellY, piece);
                board.setCell(selectedX, selectedY, 0);
                board.Selected(cellX, cellY, false);
                selected = false;
                currentTurn = (currentTurn == 1) ? 2 : 1;
                sendMove(fromX, fromY, cellX, cellY, 0);
                return;
            } else if (enemyCount == 1) {
                board.setCell(cellX, cellY, piece);
                board.setCell(selectedX, selectedY, 0);
                board.setCell(enemyX, enemyY, 0); // Удаляем захваченную шашку
                board.Selected(cellX, cellY, false);
                if (canKingCapture(cellX, cellY, piece)) {
                    selected = true;
                    selectedX = cellX;
                    selectedY = cellY;
                    sendMove(fromX, fromY, cellX, cellY, 1);
                } else {
                    selected = false;
                    currentTurn = (currentTurn == 1) ? 2 : 1;
                    sendMove(fromX, fromY, cellX, cellY, 0);
                }
                return;
            }
        }
        
        // Обычный ход для простой шашки (без захвата)
        if (std::abs(dx) == 1 && 
            (((piece == 1 || piece == 3) && dy == 1) || ((piece == 2 || piece == 4) && dy == -1))) {
            if (board.getCell(cellX, cellY) == 0) {
                board.setCell(cellX, cellY, piece);
                board.setCell(selectedX, selectedY, 0);
                board.Selected(cellX, cellY, false);
                checkForKing(cellX, cellY);
                selected = false;
                currentTurn = (currentTurn == 1) ? 2 : 1;
                sendMove(fromX, fromY, cellX, cellY, 0);
                return;
            }
        }
        
        // Захват обычной шашкой
        if (std::abs(dx) == 2 && std::abs(dy) == 2) {
            int midX = selectedX + dx / 2;
            int midY = selectedY + dy / 2;
            int middlePiece = board.getCell(midX, midY);
            if (((piece == 1 || piece == 3) && (middlePiece == 2 || middlePiece == 4 || middlePiece == 6 || middlePiece == 8)) ||
                ((piece == 2 || piece == 4) && (middlePiece == 1 || middlePiece == 3 || middlePiece == 5 || middlePiece == 7))) {
                if (board.getCell(cellX, cellY) == 0) {
                    board.setCell(cellX, cellY, piece);
                    board.setCell(selectedX, selectedY, 0);
                    board.setCell(midX, midY, 0);
                    board.Selected(cellX, cellY, false);
                    checkForKing(cellX, cellY);
                    if (canCapture(cellX, cellY, piece)) {
                        selected = true;
                        selectedX = cellX;
                        selectedY = cellY;
                        sendMove(fromX, fromY, cellX, cellY, 1);
                    } else {
                        selected = false;
                        currentTurn = (currentTurn == 1) ? 2 : 1;
                        sendMove(fromX, fromY, cellX, cellY, 0);
                    }
                    return;
                }
            }
        }
        
        // Если ход неверный, снимаем выделение
        board.Selected(selectedX, selectedY, false);
        selected = false;
    }
}

int main() {
    std::cout << "Выберите режим:\n1 - Сервер\n2 - Клиент\n3 - Локальная игра\nВаш выбор: ";
    int mode;
    std::cin >> mode;
    
    if (mode == 1) {
        networkMode = true;
        isServer = true;
        localPlayer = 1; // Сервер играет белыми
        if (SDLNet_Init() < 0) {
            std::cout << "SDLNet ошибка: " << SDLNet_GetError() << std::endl;
            return -1;
        }
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, NULL, 12345) < 0) {
            std::cout << "Ошибка разрешения хоста: " << SDLNet_GetError() << std::endl;
            return -1;
        }
        serverSocket = SDLNet_TCP_Open(&ip);
        if (!serverSocket) {
            std::cout << "Не удалось открыть серверный сокет: " << SDLNet_GetError() << std::endl;
            return -1;
        }
        std::cout << "Ожидание подключения клиента..." << std::endl;
        while (!(tcpSocket = SDLNet_TCP_Accept(serverSocket))) {//Если подключения нет, функция возвращает NULL
            SDL_Delay(100);
        }
        std::cout << "Клиент подключен!" << std::endl;
        socketSet = SDLNet_AllocSocketSet(1);//выделяет память для набора сокетов, который может хранить до 1 сокета.
        SDLNet_TCP_AddSocket(socketSet, tcpSocket);//Добавляет tcpSocket в socketSet, чтобы можно было проверять его состояние (данные есть / нет).
    }
    else if (mode == 2) {
        networkMode = true;
        isServer = false;
        localPlayer = 2; // Клиент играет черными
        if (SDLNet_Init() < 0) {
            std::cout << "SDLNet ошибка: " << SDLNet_GetError() << std::endl;
            return -1;
        }
        std::string serverIP;
        std::cout << "Введите IP сервера: ";
        std::cin >> serverIP;
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, serverIP.c_str(), 12345) < 0) {
            std::cout << "Ошибка разрешения имени: " << SDLNet_GetError() << std::endl;
            return -1;
        }
        tcpSocket = SDLNet_TCP_Open(&ip);
        if (!tcpSocket) {
            std::cout << "Не удалось подключиться к серверу: " << SDLNet_GetError() << std::endl;
            return -1;
        }
        std::cout << "Подключение к серверу успешно!" << std::endl;
        socketSet = SDLNet_AllocSocketSet(1);
        SDLNet_TCP_AddSocket(socketSet, tcpSocket);
    }
    else {
        networkMode = false;
    }
    
    if (!init()) return -1;//Этот код вызывает функцию init() и проверяет, успешно ли она выполнилась.
    
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_MOUSEBUTTONDOWN)
                handleMouseClick(event.button.x, event.button.y);
        }
        
        // Если играем по сети и сейчас не наш ход, проверяем входящие данные
        if (networkMode == true && currentTurn != localPlayer) {//Если текущий ход принадлежит другому игроку, значит, нужно ждать данные по сети
            int numReady = SDLNet_CheckSockets(socketSet, 0); //проверяет, есть ли данные в сокете.
            if (numReady > 0) {
                uint8_t packet[5];//читает 5 байтов из сети
                int received = SDLNet_TCP_Recv(tcpSocket, packet, 5);
                if (received == 5) {
                    int fromX = packet[0];//Координаты начальной позиции
                    int fromY = packet[1];//
                    int toX   = packet[2];//Координаты конечной позиции
                    int toY   = packet[3];//
                    uint8_t cont = packet[4];//uint8_t 1 байт, int обычно занимает 4 байта, в нашем случае int 4 байта, но там пиздец и по скольку uint8_t то мы в него инт запишем не 0x96, а 0x00000096 так, тойсть он хорошо войдёт в байт
                    applyNetworkMove(fromX, fromY, toX, toY, cont);//отправляем данные, чтобы видно было куда двигается шашки
                }
            }
        }
        
        SDL_RenderClear(renderer);
        board.draw();
        SDL_RenderPresent(renderer);
    }
    
    close();
    return 0;
}
