#include <iostream>
#include <conio.h>
#include <windows.h>

void PrintMenu() {
    std::cout << "1. Подключиться к каналу" << std::endl;
    std::cout << "2. Передать сообщение" << std::endl;
    std::cout << "3. Отключиться от канала" << std::endl;
    std::cout << "0. Выход" << std::endl;
}

bool ConnectToPipe(HANDLE& hPipe, HANDLE& hEvent) {
    hPipe = CreateNamedPipe("\\\\.\\pipe\\dpipe", PIPE_ACCESS_OUTBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, 512, 512, 0, NULL);
    hEvent = CreateEvent(NULL, false, false, NULL);
    OVERLAPPED lpOverlapped = OVERLAPPED();
    bool isConnected = false;

    if (hPipe != INVALID_HANDLE_VALUE && hEvent != INVALID_HANDLE_VALUE) {
        lpOverlapped.hEvent = hEvent;
        isConnected = ConnectNamedPipe(hPipe, &lpOverlapped);
        WaitForSingleObject(hEvent, INFINITE);
    }

    return isConnected;
}

bool SendNewMessage(HANDLE hPipe, HANDLE& hEvent) {
    char message[512];
    OVERLAPPED lpOverlapped = OVERLAPPED();

    std::cout << std::endl << "Введите сообщение: ";
    std::cin.getline(message, 512);

    lpOverlapped.hEvent = hEvent;
    return WriteFile(hPipe, message, 512, NULL, &lpOverlapped);
}

void CloseHandles(HANDLE& hPipe, HANDLE& hEvent) {
    if (hPipe != INVALID_HANDLE_VALUE)
        CloseHandle(hPipe);

    if (hEvent != INVALID_HANDLE_VALUE)
        CloseHandle(hEvent);
}

int main() {
    system("chcp 1251");

    char choice;
    HANDLE pipeHandle = nullptr;
    HANDLE eventHandle = nullptr;
    bool isConnected = false;

    do {
        system("cls");
        PrintMenu();
        choice = _getch();
        switch (choice)
        {
        case '1':
            if (isConnected) CloseHandles(pipeHandle, eventHandle);
            isConnected = ConnectToPipe(pipeHandle, eventHandle);

            if (isConnected) {
                std::cout << std::endl << "Канал успешно подключен" << std::endl;
                system("pause");
            }

            else {
                std::cout << std::endl << "Ошибка создания канала. Экстренное завершение работы";
                choice = '0';
            }
            break;
        case '2':
            if (isConnected) {
                isConnected = SendNewMessage(pipeHandle, eventHandle);
                if (isConnected)
                    std::cout << std::endl << "Запись успешна!" << std::endl;
                else
                    std::cout << std::endl << "Запись не была выполнена" << std::endl;
            }
            else
                std::cout << std::endl << "Канал не подключен" << std::endl;
            system("pause");
            break;
        case '3':
            if (isConnected) {
                if (DisconnectNamedPipe(pipeHandle)) {
                    std::cout << std::endl << "Отключение прошло успешно" << std::endl;
                    isConnected = false;
                }
                else
                    std::cout << std::endl << "Отключение не было выполнено" << std::endl;
            }
            else
                std::cout << std::endl << "Подключения нет" << std::endl;
            system("pause");
            break;
        case '0':
            break;
        default:
            std::cout << std::endl << "Такого пункта не существует" << std::endl;
            system("pause");
            break;
        }
    } while (choice != '0');

    if (isConnected) DisconnectNamedPipe(pipeHandle);
    CloseHandles(pipeHandle, eventHandle);
}