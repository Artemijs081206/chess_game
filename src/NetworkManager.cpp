#include "NetworkManager.h"           // Подключаем заголовочный файл класса NetworkManager
#include <iostream>                   // Подключаем стандартную библиотеку ввода-вывода для вывода сообщений об ошибках и статуса
#include <SDL2/SDL.h>                 // Подключаем SDL для работы с базовыми функциями, необходимыми для SDL_net

//
// Конструктор класса NetworkManager
//
NetworkManager::NetworkManager()
    : networkMode(false),            // Инициализируем флаг сетевого режима как false (по умолчанию не используется)
      isServer(false),               // Инициализируем флаг сервера как false (по умолчанию не сервер)
      tcpSocket(nullptr),            // Устанавливаем указатель на TCP сокет в nullptr (не создан)
      serverSocket(nullptr),         // Устанавливаем указатель на серверный сокет в nullptr (не создан)
      socketSet(nullptr)             // Устанавливаем указатель на набор сокетов в nullptr (не создан)
{
    // Конструктор не содержит дополнительной логики, инициализация происходит через список инициализации
}

//
// Деструктор класса NetworkManager
//
NetworkManager::~NetworkManager() {
    close();                         // Вызываем метод close() для корректного закрытия соединений и освобождения ресурсов
}

//
// Метод для инициализации сервера
//
bool NetworkManager::initServer() {
    networkMode = true;              // Включаем сетевой режим
    isServer = true;                 // Устанавливаем, что данный экземпляр является сервером
    if (SDLNet_Init() < 0) {          // Инициализируем библиотеку SDL_net; если инициализация не удалась
        std::cout << "SDLNet ошибка: " << SDLNet_GetError() << std::endl;  // Выводим сообщение об ошибке
        return false;                // Возвращаем false, сигнализируя об ошибке инициализации
    }
    IPaddress ip;                    // Объявляем структуру IPaddress для хранения IP-адреса и порта сервера
    if (SDLNet_ResolveHost(&ip, NULL, 12345) < 0) { // Разрешаем хост; NULL означает, что сервер слушает на всех интерфейсах, порт 12345
        std::cout << "Ошибка разрешения хоста: " << SDLNet_GetError() << std::endl; // Выводим сообщение об ошибке разрешения хоста
        return false;                // Возвращаем false, если произошла ошибка
    }
    serverSocket = SDLNet_TCP_Open(&ip); // Открываем TCP-сокет для сервера с заданным IP-адресом и портом
    if (!serverSocket) {             // Если серверный сокет не создан успешно
        std::cout << "Не удалось открыть серверный сокет: " << SDLNet_GetError() << std::endl; // Выводим сообщение об ошибке
        return false;                // Возвращаем false, сигнализируя об ошибке открытия сокета
    }
    std::cout << "Ожидание подключения клиента..." << std::endl; // Выводим сообщение о том, что сервер ожидает подключения клиента
    while (!(tcpSocket = SDLNet_TCP_Accept(serverSocket))) { // Ждем подключения клиента, пытаемся принять TCP-соединение
        SDL_Delay(100);              // Задержка в 100 миллисекунд, чтобы не перегружать процессор в цикле ожидания
    }
    std::cout << "Клиент подключен!" << std::endl; // Выводим сообщение о том, что клиент успешно подключился
    socketSet = SDLNet_AllocSocketSet(1); // Выделяем набор сокетов для одного сокета (tcpSocket)
    SDLNet_TCP_AddSocket(socketSet, tcpSocket); // Добавляем TCP-сокет в набор для отслеживания событий
    return true;                     // Возвращаем true, сигнализируя об успешной инициализации сервера
}

//
// Метод для инициализации клиента
//
bool NetworkManager::initClient(const std::string& serverIP) {
    networkMode = true;              // Включаем сетевой режим
    isServer = false;                // Устанавливаем, что данный экземпляр является клиентом
    if (SDLNet_Init() < 0) {          // Инициализируем библиотеку SDL_net; если инициализация не удалась
        std::cout << "SDLNet ошибка: " << SDLNet_GetError() << std::endl; // Выводим сообщение об ошибке
        return false;                // Возвращаем false, сигнализируя об ошибке инициализации
    }
    IPaddress ip;                    // Объявляем структуру IPaddress для хранения IP-адреса сервера и порта
    if (SDLNet_ResolveHost(&ip, serverIP.c_str(), 12345) < 0) { // Разрешаем имя сервера (переданное как строка) для порта 12345
        std::cout << "Ошибка разрешения имени сервера: " << SDLNet_GetError() << std::endl; // Выводим сообщение об ошибке
        return false;                // Возвращаем false, если произошла ошибка
    }
    tcpSocket = SDLNet_TCP_Open(&ip); // Открываем TCP-сокет для подключения к серверу с указанным IP-адресом и портом
    if (!tcpSocket) {                // Если не удалось подключиться к серверу
        std::cout << "Не удалось подключиться к серверу: " << SDLNet_GetError() << std::endl; // Выводим сообщение об ошибке
        return false;                // Возвращаем false, сигнализируя об ошибке подключения
    }
    std::cout << "Подключение к серверу успешно!" << std::endl; // Выводим сообщение об успешном подключении к серверу
    socketSet = SDLNet_AllocSocketSet(1); // Выделяем набор сокетов для одного сокета (tcpSocket)
    SDLNet_TCP_AddSocket(socketSet, tcpSocket); // Добавляем TCP-сокет в набор для отслеживания событий
    return true;                     // Возвращаем true, сигнализируя об успешной инициализации клиента
}

//
// Метод для отправки хода по сети
//
void NetworkManager::sendMove(int fromX, int fromY, int toX, int toY, uint8_t continuation) {
    if (!networkMode) return;       // Если сетевой режим не включен, ничего не отправляем и выходим из функции
    uint8_t packet[5];              // Создаем массив из 5 байтов для хранения данных хода
    packet[0] = static_cast<uint8_t>(fromX); // Преобразуем координату fromX в байт и сохраняем в packet[0]
    packet[1] = static_cast<uint8_t>(fromY); // Преобразуем координату fromY в байт и сохраняем в packet[1]
    packet[2] = static_cast<uint8_t>(toX);   // Преобразуем координату toX в байт и сохраняем в packet[2]
    packet[3] = static_cast<uint8_t>(toY);   // Преобразуем координату toY в байт и сохраняем в packet[3]
    packet[4] = continuation;                // Сохраняем флаг продолжения (например, для множественного захвата) в packet[4]
    SDLNet_TCP_Send(tcpSocket, packet, 5);    // Отправляем пакет данных через TCP-сокет
}

//
// Метод для получения хода по сети
//
bool NetworkManager::receiveMove(int& fromX, int& fromY, int& toX, int& toY, uint8_t& continuation) {
    if (!networkMode) return false;  // Если сетевой режим не включен, возвращаем false, так как ход не может быть получен
    int numReady = SDLNet_CheckSockets(socketSet, 0); // Проверяем набор сокетов на наличие готовых к чтению данных, таймаут 0 мс
    if (numReady > 0) {              // Если есть сокеты с готовыми данными
        uint8_t packet[5];         // Создаем массив для приема 5 байтов данных
        int received = SDLNet_TCP_Recv(tcpSocket, packet, 5); // Получаем данные из TCP-сокета
        if (received == 5) {       // Если получено ровно 5 байтов (полный пакет)
            fromX = packet[0];     // Извлекаем координату fromX из первого байта пакета
            fromY = packet[1];     // Извлекаем координату fromY из второго байта пакета
            toX = packet[2];       // Извлекаем координату toX из третьего байта пакета
            toY = packet[3];       // Извлекаем координату toY из четвертого байта пакета
            continuation = packet[4]; // Извлекаем флаг продолжения из пятого байта пакета
            return true;           // Возвращаем true, сигнализируя, что ход успешно получен
        }
    }
    return false;                  // Если данные не получены или пакет неполный, возвращаем false
}

//
// Метод для закрытия соединения и освобождения ресурсов
//
void NetworkManager::close() {
    if (tcpSocket) {               // Если TCP-сокет существует
        SDLNet_TCP_Close(tcpSocket); // Закрываем TCP-сокет
        tcpSocket = nullptr;       // Обнуляем указатель на TCP-сокет
    }
    if (serverSocket) {            // Если серверный сокет существует
        SDLNet_TCP_Close(serverSocket); // Закрываем серверный сокет
        serverSocket = nullptr;    // Обнуляем указатель на серверный сокет
    }
    if (socketSet) {               // Если набор сокетов существует
        SDLNet_FreeSocketSet(socketSet); // Освобождаем выделенный набор сокетов
        socketSet = nullptr;       // Обнуляем указатель на набор сокетов
    } // это скобка
    SDLNet_Quit();                 // Завершаем работу SDL_net, освобождая все выделенные ресурсы
}
