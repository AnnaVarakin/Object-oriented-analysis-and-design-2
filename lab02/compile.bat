@echo off
echo Компиляция C++ сервера...
g++ -std=c++11 -o star_server.exe server.cpp -lws2_32 -lpthread
if %errorlevel% equ 0 (
    echo ✅ Компиляция успешна!
    echo Запусти программу: star_server.exe
) else (
    echo ❌ Ошибка компиляции!
)
pause