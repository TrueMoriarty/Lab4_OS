#include <iostream>
#include <windows.h>
#include <conio.h>

void WINAPI Callback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    std::cout << std::endl << "Сообщение получено" << std::endl;
}

void PrintMenu() {
    std::cout << "1. Подключиться к каналу" << std::endl;
    std::cout << "2. Получить сообщение" << std::endl;
    std::cout << "3. Отключиться от канала" << std::endl;
    std::cout << "0. Выход" << std::endl;
}

bool ConnectToPipe(HANDLE& hPipe) {
    hPipe = CreateFile("\\\\.\\pipe\\dpipe", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    bool isConnected = false;

    if (hPipe != INVALID_HANDLE_VALUE) {
        isConnected = true;
    }

    return isConnected;
}

bool GetNewMessage(HANDLE& hPipe, char message[]) {
    OVERLAPPED lpOverlapped = OVERLAPPED();
    bool isMesGet = ReadFileEx(hPipe, message, 512, &lpOverlapped, Callback);
    SleepEx(INFINITE, true);
    return isMesGet;
}

int main()
{
    system("chcp 1251");

    char choice;
    bool isConnected = false;
    HANDLE pipeHandle = nullptr;
    char message[512];

    do {
        system("cls");
        PrintMenu();
        choice = _getch();
        switch (choice)
        {
        case '1':
            if (isConnected) CloseHandle(pipeHandle);
            isConnected = ConnectToPipe(pipeHandle);
            if (isConnected) std::cout << std::endl << "Подключение выполнено" << std::endl;
            else std::cout << std::endl << "Подключение не выполнено" << std::endl;
            system("pause");
            break;
        case '2':
            if (isConnected) {
                if (!GetNewMessage(pipeHandle, message))
                    std::cout << std::endl << "Сообщение не получено" << std::endl;
                else std::cout << std::endl << message << std::endl;
            }
            else std::cout << std::endl << "Подключение еще не выполнено" << std::endl;
            system("pause");
            break;
        case '3':
            if (isConnected)
                if (CloseHandle(pipeHandle)) {
                    std::cout << std::endl << "Канал закрыт успешно" << std::endl;
                    isConnected = false;
                }
                else std::cout << std::endl << "Канал не закрыт" << std::endl;
            else std::cout << std::endl << "Канал не был подключен" << std::endl;
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

    if (isConnected) {
        CloseHandle(pipeHandle);
    }
}
