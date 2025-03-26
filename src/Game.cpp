#include "Game.h"                       // Подключаем заголовочный файл Game.h, содержащий объявление класса Game
#include <SDL2/SDL_image.h>              // Подключаем библиотеку SDL_image для загрузки изображений
#include <iostream>                      // Подключаем библиотеку для ввода/вывода (std::cout, std::cin)
#include <cmath>                         // Подключаем математическую библиотеку (для функции std::abs и др.)

// Размеры экрана
const int SCREEN_WIDTH = 800;            // Определяем ширину окна приложения (800 пикселей)
const int SCREEN_HEIGHT = 800;           // Определяем высоту окна приложения (800 пикселей)

Game::Game()
    : window(nullptr), renderer(nullptr),
      boardTexture(nullptr), whitePieceTexture(nullptr), blackPieceTexture(nullptr),
      selectedW(nullptr), selectedB(nullptr),  //пусто
      blackKing(nullptr), blackKingS(nullptr), whiteKing(nullptr), whiteKingS(nullptr),
      board(nullptr), networkManager(nullptr),
      currentTurn(0), localPlayer(0), networkMode(false),
      selected(false), selectedX(0), selectedY(0)
{
    // Конструктор класса Game: инициализирует все указатели и переменные начальными значениями
}

Game::~Game() {
    close();                            // Деструктор класса Game: освобождает ресурсы, вызывая метод close()
}

SDL_Texture* Game::loadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);  // Загружаем изображение по указанному пути в поверхность SDL_Surface
    if (!surface) {                          // Если загрузка изображения не удалась
        std::cout << "Ошибка загрузки текстуры: " << path << " | " << IMG_GetError() << std::endl; // Выводим сообщение об ошибке
        return nullptr;                      // Возвращаем nullptr, чтобы сигнализировать об ошибке загрузки
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface); // Создаем текстуру из загруженной поверхности
    SDL_FreeSurface(surface);                // Освобождаем память, занятую поверхностью, так как она больше не нужна
    return texture;                          // Возвращаем созданную текстуру
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {       // Инициализируем подсистему видео SDL; если возникла ошибка
        std::cout << "Ошибка SDL: " << SDL_GetError() << std::endl;  // Выводим сообщение об ошибке
        return false;                       // Завершаем инициализацию, возвращая false
    }
    
    window = SDL_CreateWindow("Checkers", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);  // Создаем окно с заголовком "Checkers", расположенное по центру экрана
    if (!window) return false;               // Если окно не создано, возвращаем false
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);  // Создаем рендерер с аппаратным ускорением для окна
    if (!renderer) return false;             // Если рендерер не создан, возвращаем false
    
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;  // Инициализируем SDL_image для работы с PNG; если не удалось — возвращаем false
    
    // Загружаем текстуры для элементов игры
    boardTexture = loadTexture("board.png");           // Загружаем текстуру игрового поля
    whitePieceTexture = loadTexture("white_piece.png");  // Загружаем текстуру белой шашки
    blackPieceTexture = loadTexture("black_piece.png");  // Загружаем текстуру черной шашки
    selectedW = loadTexture("white_piece_s.png");        // Загружаем текстуру выделенной белой шашки
    selectedB = loadTexture("black_piece_s.png");        // Загружаем текстуру выделенной черной шашки
    blackKing = loadTexture("black_king.png");           // Загружаем текстуру черной дамки
    blackKingS = loadTexture("black_king_s.png");        // Загружаем текстуру выделенной черной дамки
    whiteKing = loadTexture("white_king.png");           // Загружаем текстуру белой дамки
    whiteKingS = loadTexture("white_king_s.png");        // Загружаем текстуру выделенной белой дамки
    
    if (!boardTexture || !whitePieceTexture || !blackPieceTexture ||
        !selectedW || !selectedB || !blackKing || !blackKingS ||
        !whiteKing || !whiteKingS) {        // Проверяем, что все текстуры успешно загружены
        return false;                      // Если какая-либо текстура не загружена, возвращаем false
    }
    
    // Выбор режима игры (сетевая игра или локальная игра)
    std::cout << "Выберите режим:\n1 - Сервер\n2 - Клиент\n3 - Локальная игра\nВаш выбор: ";
    int mode;
    std::cin >> mode;                      // Считываем выбор режима из консоли
    
    if (mode == 1) {                       // Если выбран режим сервера
        networkMode = true;                // Устанавливаем, что игра в сетевом режиме
        localPlayer = white_checker;       // Сервер играет белыми (используем константу white_checker)
        networkManager = new NetworkManager(); // Создаем объект сетевого менеджера
        if (!networkManager->initServer()) return false; // Инициализируем сервер; если не удалось — возвращаем false
    } else if (mode == 2) {                // Если выбран режим клиента
        networkMode = true;                // Устанавливаем, что игра в сетевом режиме
        localPlayer = black_checker;       // Клиент играет черными (используем константу black_checker)
        networkManager = new NetworkManager(); // Создаем объект сетевого менеджера
        std::string serverIP;
        std::cout << "Введите IP сервера: "; // Просим пользователя ввести IP адрес сервера
        std::cin >> serverIP;              // Считываем IP адрес сервера
        if (!networkManager->initClient(serverIP)) return false; // Инициализируем клиент; если не удалось — возвращаем false
    } else {                               // Если выбран локальный режим игры
        networkMode = false;               // Устанавливаем, что сетевой режим не используется
    }
    
    // Устанавливаем, что первым ходят белые шашки
    currentTurn = white_checker;           // Инициализируем текущий ход, используя константу white_checker
    
    // Создаем объект игрового поля, передавая необходимые текстуры
    board = new Board(boardTexture, whitePieceTexture, blackPieceTexture,
                      selectedW, selectedB, blackKing, blackKingS, whiteKing, whiteKingS);
    
    return true;                           // Возвращаем true, сигнализируя об успешной инициализации игры
}

void Game::handleMouseClick(int x, int y) {
    // Если игра в сетевом режиме и сейчас не наш ход, клик игнорируется
    if (networkMode && currentTurn != localPlayer)
        return;
    
    int cellX = x / CELL_SIZE;           // Определяем индекс клетки по оси X, исходя из координаты клика и размера клетки
    int cellY = y / CELL_SIZE;           // Определяем индекс клетки по оси Y
    
    if (!selected) {                     // Если шашка еще не выбрана
        int piece = board->getCell(cellX, cellY); // Получаем содержимое клетки (тип шашки или пустая клетка)
        if (!networkMode) {              // Если игра локальная
            if ((currentTurn == white_checker &&
                (piece == white_checker || piece == black_selected || piece == black_king || piece == black_king_selected)) ||
                (currentTurn == black_checker &&
                (piece == black_checker || piece == white_selected || piece == white_king || piece == white_king_selected))) {
                board->selectCell(cellX, cellY, true); // Выделяем выбранную шашку
                selected = true;         // Устанавливаем флаг, что шашка выбрана
                selectedX = cellX;       // Запоминаем индекс X выбранной клетки
                selectedY = cellY;       // Запоминаем индекс Y выбранной клетки
            }
        } else {                         // Если игра в сетевом режиме
            if ((localPlayer == white_checker &&
                (piece == white_checker || piece == black_selected || piece == black_king || piece == black_king_selected)) ||
                (localPlayer == black_checker &&
                (piece == black_checker || piece == white_selected || piece == white_king || piece == white_king_selected))) {
                board->selectCell(cellX, cellY, true); // Выделяем шашку, если она принадлежит локальному игроку
                selected = true;         // Устанавливаем флаг выбора
                selectedX = cellX;       // Запоминаем координату X выбранной шашки
                selectedY = cellY;       // Запоминаем координату Y выбранной шашки
            }
        }
    } else {                             // Если шашка уже выбрана — обрабатываем попытку хода
        int dx = cellX - selectedX;      // Вычисляем смещение по оси X от исходной позиции
        int dy = cellY - selectedY;      // Вычисляем смещение по оси Y от исходной позиции
        int piece = board->getCell(selectedX, selectedY); // Получаем тип шашки, которую пытаемся переместить
        int fromX = selectedX, fromY = selectedY; // Запоминаем начальные координаты выбранной шашки
        
        // Ход для дамки (шашки, ставшие дамками)
        if ((piece == black_king || piece == white_king ||
            piece == black_king_selected || piece == white_king_selected) &&
           (std::abs(dx) == std::abs(dy)) && std::abs(dx) >= 1) { // Проверяем, что ход по диагонали и длина хода не меньше 1
            
           if (board->getCell(cellX, cellY) != empty) { // Если целевая клетка не пуста
               board->selectCell(selectedX, selectedY, false); // Снимаем выделение с исходной клетки
               selected = false;         // Сбрасываем флаг выбора шашки
               return;                   // Прерываем выполнение функции, так как ход невозможен
           }
           
           int stepX = (dx > 0) ? 1 : -1;  // Определяем направление перемещения по оси X (вправо или влево)
           int stepY = (dy > 0) ? 1 : -1;  // Определяем направление перемещения по оси Y (вниз или вверх)
           int distance = std::abs(dx);    // Вычисляем расстояние хода (количество клеток)
           int enemyCount = 0;             // Инициализируем счетчик противников, встреченных на пути
           int enemyX = -1, enemyY = -1;   // Инициализируем координаты противника (-1 означает, что противник не найден)
           
           for (int i = 1; i < distance; i++) {  // Проходим по всем клеткам между начальной и целевой позицией
               int curX = selectedX + i * stepX;   // Вычисляем текущую клетку по оси X
               int curY = selectedY + i * stepY;   // Вычисляем текущую клетку по оси Y
               int cellVal = board->getCell(curX, curY); // Получаем содержимое текущей клетки
               if (cellVal != empty) {         // Если клетка не пуста
                   if (Board::isFriendly(cellVal, piece)) { // Если шашка в клетке принадлежит тому же игроку
                       enemyCount = 2;       // Устанавливаем enemyCount в 2, что означает невозможность хода
                       break;                // Прерываем цикл
                   } else {
                       enemyCount++;         // Увеличиваем счетчик противников
                       enemyX = curX;        // Запоминаем координату X противника
                       enemyY = curY;        // Запоминаем координату Y противника
                   }
               }
           }
           
           if (enemyCount > 1) {            // Если на пути встречено более одного противника
               board->selectCell(selectedX, selectedY, false); // Снимаем выделение с выбранной шашки
               selected = false;            // Сбрасываем флаг выбора
               return;                      // Ход невозможен, выходим из функции
           }
           
           if (enemyCount == 0) {           // Если противников не найдено — выполняем обычный ход дамкой
               board->setCell(cellX, cellY, piece); // Устанавливаем шашку на целевую клетку
               board->setCell(selectedX, selectedY, empty); // Очищаем исходную клетку
               board->selectCell(cellX, cellY, false); // Снимаем выделение с новой позиции шашки
               selected = false;            // Сбрасываем флаг выбора
               currentTurn = (currentTurn == white_checker) ? black_checker : white_checker; // Меняем ход игрока
               if (networkMode)
                   networkManager->sendMove(fromX, fromY, cellX, cellY, 0); // Если включен сетевой режим, отправляем ход по сети
               return;                      // Завершаем выполнение функции
           } else if (enemyCount == 1) {    // Если найден ровно один противник — выполняем ход с захватом
               board->setCell(cellX, cellY, piece); // Перемещаем шашку на целевую клетку
               board->setCell(selectedX, selectedY, empty); // Очищаем исходную клетку
               board->setCell(enemyX, enemyY, empty); // Удаляем захваченную шашку противника
               board->selectCell(cellX, cellY, false); // Снимаем выделение с шашки после хода
               if (board->canKingCapture(cellX, cellY, piece)) { // Проверяем, может ли дамка продолжить захват
                   selected = true;       // Если да, оставляем шашку выделенной
                   selectedX = cellX;       // Обновляем координату X выбранной шашки
                   selectedY = cellY;       // Обновляем координату Y выбранной шашки
                   if (networkMode)
                       networkManager->sendMove(fromX, fromY, cellX, cellY, 1); // Отправляем ход по сети с флагом продолжения захвата
               } else {
                   selected = false;      // Если захват завершён, снимаем выделение
                   currentTurn = (currentTurn == white_checker) ? black_checker : white_checker; // Меняем ход игрока
                   if (networkMode)
                       networkManager->sendMove(fromX, fromY, cellX, cellY, 0); // Отправляем ход по сети
               }
               return;                      // Выходим из функции
           }
        }
        
        // Обычный ход для простой шашки (не дамки)
        if (std::abs(dx) == 1 &&
            (((piece == white_checker || piece == black_selected) && dy == 1) ||
             ((piece == black_checker || piece == white_selected) && dy == -1))) {
             
            if (board->getCell(cellX, cellY) == empty) { // Если целевая клетка пуста
                board->setCell(cellX, cellY, piece);       // Перемещаем шашку на целевую клетку
                board->setCell(selectedX, selectedY, empty); // Очищаем исходную клетку
                board->selectCell(cellX, cellY, false);      // Снимаем выделение с шашки
                board->checkForKing(cellX, cellY);           // Проверяем, нужно ли превратить шашку в дамку
                selected = false;            // Сбрасываем флаг выбора
                currentTurn = (currentTurn == white_checker) ? black_checker : white_checker; // Меняем ход игрока
                if (networkMode)
                    networkManager->sendMove(fromX, fromY, cellX, cellY, 0); // Если игра в сетевом режиме, отправляем ход
                return;                      // Выходим из функции
            }
        }
        
        // Захват простой шашкой (прыжок через противника)
        if (std::abs(dx) == 2 && std::abs(dy) == 2) {
            int midX = selectedX + dx / 2;  // Вычисляем координату X средней клетки между началом и концом хода
            int midY = selectedY + dy / 2;  // Вычисляем координату Y средней клетки
            int middlePiece = board->getCell(midX, midY); // Получаем содержимое средней клетки
            
            bool whiteAttack = (piece == white_checker || piece == black_selected); // Определяем, атакует ли белая шашка
            bool blackAttack = (piece == black_checker || piece == white_selected); // Определяем, атакует ли черная шашка
            
            if ((whiteAttack && (middlePiece == black_checker || middlePiece == white_selected ||
                                 middlePiece == white_king || middlePiece == white_king_selected)) ||
                (blackAttack && (middlePiece == white_checker || middlePiece == black_selected ||
                                 middlePiece == black_king || middlePiece == black_king_selected))) {
                if (board->getCell(cellX, cellY) == empty) { // Если целевая клетка пуста
                    board->setCell(cellX, cellY, piece);   // Перемещаем шашку на целевую клетку
                    board->setCell(selectedX, selectedY, empty); // Очищаем исходную клетку
                    board->setCell(midX, midY, empty);      // Удаляем шашку противника (захват)
                    board->selectCell(cellX, cellY, false);  // Снимаем выделение с шашки после хода
                    board->checkForKing(cellX, cellY);       // Проверяем, нужно ли превратить шашку в дамку
                    if (board->canCapture(cellX, cellY, piece)) { // Проверяем, может ли шашка продолжить захват
                        selected = true;       // Если да, оставляем шашку выделенной
                        selectedX = cellX;     // Обновляем координату X выбранной шашки
                        selectedY = cellY;     // Обновляем координату Y выбранной шашки
                        if (networkMode)
                            networkManager->sendMove(fromX, fromY, cellX, cellY, 1); // Отправляем ход с флагом продолжения захвата
                    } else {
                        selected = false;      // Если захват завершен, снимаем выделение
                        currentTurn = (currentTurn == white_checker) ? black_checker : white_checker; // Меняем ход игрока
                        if (networkMode)
                            networkManager->sendMove(fromX, fromY, cellX, cellY, 0); // Отправляем ход по сети
                    }
                    return;                      // Выходим из функции
                }
            }
        }
        
        // Если ход не соответствует ни одному из правил — снимаем выделение с выбранной шашки
        board->selectCell(selectedX, selectedY, false); // Снимаем выделение с исходной клетки
        selected = false;                // Сбрасываем флаг выбора
    }
}

void Game::applyNetworkMove(int fromX, int fromY, int toX, int toY, uint8_t continuation) {
    int piece = board->getCell(fromX, fromY); // Получаем шашку, которая делала ход (по ее исходной позиции)
    if (std::abs(toX - fromX) >= 2) {     // Если ход перемещает шашку более чем на одну клетку (то есть захват)
        int dx = (toX - fromX) / std::abs(toX - fromX); // Определяем направление по оси X (1 или -1)
        int dy = (toY - fromY) / std::abs(toY - fromY); // Определяем направление по оси Y (1 или -1)
        int enemyX = -1, enemyY = -1;     // Инициализируем координаты противника как -1 (не найден)
        for (int i = 1; i < std::abs(toX - fromX); i++) { // Проходим по всем клеткам между начальной и целевой позицией
            int cx = fromX + i * dx;    // Вычисляем текущую клетку по оси X
            int cy = fromY + i * dy;    // Вычисляем текущую клетку по оси Y
            int cellVal = board->getCell(cx, cy); // Получаем содержимое текущей клетки
            if (cellVal != empty && !Board::isFriendly(cellVal, piece)) { // Если клетка занята вражеской шашкой
                enemyX = cx;          // Запоминаем координату X противника
                enemyY = cy;          // Запоминаем координату Y противника
                break;                // Прерываем цикл, так как враг найден
            }
        }
        if (enemyX != -1 && enemyY != -1)          // Если противник найден
            board->setCell(enemyX, enemyY, empty); // Удаляем вражескую шашку (захват)
    }
    board->setCell(toX, toY, piece);      // Перемещаем нашу шашку на целевую клетку
    board->setCell(fromX, fromY, empty);  // Очищаем исходную клетку (где шашка была до хода)
    board->selectCell(toX, toY, false);   // Снимаем выделение с новой позиции шашки
    board->checkForKing(toX, toY);        // Проверяем, должна ли шашка превратиться в дамку после хода
    if (continuation == 0) {              // Если флаг продолжения захвата равен 0 (захват завершён)
        currentTurn = localPlayer;      // Устанавливаем, что следующий ход принадлежит локальному игроку
        selected = false;               // Сбрасываем флаг выбора шашки
    } else {                            // Если ход предусматривает возможность продолжения захвата
        selected = true;                // Оставляем шашку выделенной
        selectedX = toX;                // Обновляем координату X выбранной шашки
        selectedY = toY;                // Обновляем координату Y выбранной шашки
    }
}

void Game::run() {
    bool running = true;                // Флаг, управляющий основным игровым циклом
    SDL_Event event;                    // Переменная для хранения событий SDL
    
    while (running) {                   // Основной игровой цикл
        while (SDL_PollEvent(&event)) { // Извлекаем события из очереди
            if (event.type == SDL_QUIT) // Если получено событие закрытия окна
                running = false;        // Завершаем игровой цикл, устанавливая флаг в false
            if (event.type == SDL_MOUSEBUTTONDOWN) // Если произошел клик мыши
                handleMouseClick(event.button.x, event.button.y); // Обрабатываем клик, передавая координаты
        }
        
        // Если игра в сетевом режиме и сейчас ход противника, ждем его хода
        if (networkMode && currentTurn != localPlayer) {
            int fromX, fromY, toX, toY;  // Объявляем переменные для координат хода, полученного по сети
            uint8_t cont;              // Переменная для флага продолжения захвата
            if (networkManager->receiveMove(fromX, fromY, toX, toY, cont)) { // Если успешно получен ход по сети
                applyNetworkMove(fromX, fromY, toX, toY, cont); // Применяем полученный ход к игровому полю
            }
        }
        
        SDL_RenderClear(renderer);      // Очищаем окно рендерера, готовясь к новому кадру
        board->draw(renderer);            // Отрисовываем игровое поле и все шашки на рендерере
        SDL_RenderPresent(renderer);    // Обновляем окно, отображая отрисованный кадр
    }
}

void Game::close() {
    if (board) {                        // Если объект board существует
        delete board;                   // Освобождаем память, занятую объектом board
        board = nullptr;                // Обнуляем указатель на board
    }
    if (networkManager) {               // Если объект networkManager существует
        networkManager->close();        // Закрываем сетевое соединение и освобождаем связанные ресурсы
        delete networkManager;          // Освобождаем память, занятую объектом networkManager
        networkManager = nullptr;       // Обнуляем указатель на networkManager
    }
    if (boardTexture) SDL_DestroyTexture(boardTexture); // Освобождаем текстуру игрового поля
    if (whitePieceTexture) SDL_DestroyTexture(whitePieceTexture); // Освобождаем текстуру белой шашки
    if (blackPieceTexture) SDL_DestroyTexture(blackPieceTexture); // Освобождаем текстуру черной шашки
    if (selectedW) SDL_DestroyTexture(selectedW);         // Освобождаем текстуру выделенной белой шашки
    if (selectedB) SDL_DestroyTexture(selectedB);         // Освобождаем текстуру выделенной черной шашки
    if (blackKing) SDL_DestroyTexture(blackKing);         // Освобождаем текстуру черной дамки
    if (blackKingS) SDL_DestroyTexture(blackKingS);       // Освобождаем текстуру выделенной черной дамки
    if (whiteKing) SDL_DestroyTexture(whiteKing);         // Освобождаем текстуру белой дамки
    if (whiteKingS) SDL_DestroyTexture(whiteKingS);       // Освобождаем текстуру выделенной белой дамки
    
    if (renderer) SDL_DestroyRenderer(renderer);          // Освобождаем рендерер SDL
    if (window) SDL_DestroyWindow(window);                // Освобождаем окно SDL
    
    IMG_Quit();                        // Завершаем работу с SDL_image и освобождаем связанные ресурсы
    SDL_Quit();                        // Завершаем работу с SDL, освобождая все выделенные ресурсы
}
