@echo off
chcp 65001 >nul
title FBDaw - Build automatico Windows
echo ============================================
echo   FBDaw - Compilazione automatica (Windows)
echo ============================================
echo.

REM --- 1. Controllo Visual Studio ---
set "VS_WHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VS_WHERE%" set "VS_WHERE=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VS_WHERE%" (
  echo [ERRORE] Visual Studio 2022 Community non trovato.
  echo 1) Scaricalo gratis da https://visualstudio.microsoft.com/it/
  echo 2) Durante l'installazione spunta "Sviluppo per desktop con C++"
  pause
  exit /b 1
)

REM --- 2. Controllo CMake ---
where cmake >nul 2>nul
if errorlevel 1 (
  echo [ERRORE] CMake non trovato nel PATH.
  echo 1) Scaricalo da https://cmake.org/download/ (installer Windows x64)
  echo 2) Durante l'installazione spunta "Add CMake to the system PATH"
  pause
  exit /b 1
)

REM --- 3. Controllo JUCE ---
if not exist "%~dp0JUCE\CMakeLists.txt" (
  echo [ERRORE] Cartella JUCE non trovata.
  echo 1) Vai su https://juce.com/ e crea un account gratuito
  echo 2) Scarica JUCE (versione 8.x) e scompattala QUI dentro la cartella FBDaw
  echo    in modo che esista: FBDaw\JUCE\CMakeLists.txt
  pause
  exit /b 1
)

REM --- 4. Configurazione e compilazione ---
echo Configurazione progetto...
cmake -B build -G "Visual Studio 17 2022" -A x64
if errorlevel 1 ( echo [ERRORE] Configurazione fallita. & pause & exit /b 1 )

echo Compilazione (puo' richiedere 5-15 minuti la prima volta)...
cmake --build build --config Release
if errorlevel 1 ( echo [ERRORE] Compilazione fallita. Copia gli errori e incollali a Federica. & pause & exit /b 1 )

echo.
echo ============================================
echo   COMPLETATO!
echo   Eseguibile: build\FBDaw_artefacts\Release\FBDaw.exe
echo ============================================
pause
