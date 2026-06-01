# =============================================================================
#          BrewNBites Windows Native Build Script (PowerShell)
# =============================================================================
# This script automates native compilation of BrewNBites on Windows using CMake.
# Since the precompiled Windows libraries in vendor/qt6 are built for MinGW,
# this script enforces the use of the MinGW C++ compiler.
# =============================================================================

$ErrorActionPreference = "Stop"

# Clear screen and show a sleek header
Clear-Host
Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "            BrewNBites Windows Build System               " -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host ""

# -----------------------------------------------------------------------------
# STEP 1: Check if CMake is installed
# -----------------------------------------------------------------------------
Write-Host "[1/4] Checking CMake installation..." -ForegroundColor Yellow
$CMakeCmd = Get-Command "cmake" -ErrorAction SilentlyContinue

if (-not $CMakeCmd) {
    Write-Host "[-] Error: CMake was not found in your system PATH!" -ForegroundColor Red
    Write-Host "    Please download and install CMake from: https://cmake.org/download/" -ForegroundColor Gray
    Write-Host "    Make sure to check the option 'Add CMake to the system PATH' during installation." -ForegroundColor Gray
    Write-Host ""
    Read-Host "Press Enter to exit..."
    Exit 1
}
Write-Host "    Found: $($CMakeCmd.Source)" -ForegroundColor Green

# -----------------------------------------------------------------------------
# STEP 2: Check if MinGW C++ Compiler (g++) is installed
# -----------------------------------------------------------------------------
Write-Host "[2/4] Checking MinGW C++ Compiler (g++)..." -ForegroundColor Yellow
$GppCmd = Get-Command "g++" -ErrorAction SilentlyContinue

if (-not $GppCmd) {
    Write-Host "[-] Error: MinGW C++ compiler (g++) was not found in your system PATH!" -ForegroundColor Red
    Write-Host "    Since the downloaded Qt6 libraries under 'vendor/qt6' are compiled with MinGW," -ForegroundColor Gray
    Write-Host "    you MUST use a MinGW-w64 compiler to compile the project on Windows." -ForegroundColor Gray
    Write-Host ""
    Write-Host "    How to install MinGW:" -ForegroundColor Gray
    Write-Host "    1. Download MSYS2 from: https://www.msys2.org/" -ForegroundColor Gray
    Write-Host "    2. Run the MSYS2 UCRT64 terminal and install the toolchain:" -ForegroundColor Gray
    Write-Host "       pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make" -ForegroundColor Gray
    Write-Host "    3. Add 'C:\msys64\ucrt64\bin' to your Windows User Environment PATH." -ForegroundColor Gray
    Write-Host ""
    Read-Host "Press Enter to exit..."
    Exit 1
}
Write-Host "    Found: $($GppCmd.Source)" -ForegroundColor Green

# -----------------------------------------------------------------------------
# STEP 2.5: Check if make/mingw32-make is installed
# -----------------------------------------------------------------------------
Write-Host "      Checking Make build utility (mingw32-make)..." -ForegroundColor Yellow
$MakeCmd = Get-Command "mingw32-make" -ErrorAction SilentlyContinue
if (-not $MakeCmd) {
    $MakeCmd = Get-Command "make" -ErrorAction SilentlyContinue
}

if (-not $MakeCmd) {
    Write-Host "[-] Error: No make utility (mingw32-make.exe or make.exe) was found in your system PATH!" -ForegroundColor Red
    Write-Host "    Even though g++ is installed, CMake needs a make utility to orchestrate the build." -ForegroundColor Gray
    Write-Host ""
    Write-Host "    How to fix:" -ForegroundColor Gray
    Write-Host "    Open your MSYS2 terminal and install the make package:" -ForegroundColor Gray
    Write-Host "        pacman -S mingw-w64-ucrt-x86_64-make" -ForegroundColor Green
    Write-Host "    After the installation completes, please close and reopen this terminal window" -ForegroundColor Gray
    Write-Host "    so the new path configurations take effect, then re-run the build." -ForegroundColor Gray
    Write-Host ""
    Read-Host "Press Enter to exit..."
    Exit 1
}
Write-Host "    Found Make Tool: $($MakeCmd.Source)" -ForegroundColor Green

# -----------------------------------------------------------------------------
# STEP 3: Verify and download Qt6 local installation
# -----------------------------------------------------------------------------
Write-Host "[3/4] Verifying Qt6 local installation..." -ForegroundColor Yellow
$Qt6Folder = Resolve-Path "$PSScriptRoot\vendor\qt6\*\mingw_64" -ErrorAction SilentlyContinue

if (-not $Qt6Folder) {
    Write-Host "[!] Local Qt6 binaries not found in 'vendor/qt6'!" -ForegroundColor Magenta
    Write-Host "    Since 'vendor/' is ignored in Git, this script can automatically download" -ForegroundColor Gray
    Write-Host "    the matching Windows Qt 6.10.2 (MinGW) libraries to compile the project." -ForegroundColor Gray
    Write-Host "    (This requires Python to be installed on your Windows system)." -ForegroundColor Gray
    Write-Host ""
    
    $DownloadChoice = ""
    while ($DownloadChoice -notin @("Y", "N")) {
        $DownloadChoice = (Read-Host "Download and install matching Qt 6.10.2? [Y/N]").ToUpper()
    }

    if ($DownloadChoice -eq "Y") {
        # Check if Python is installed
        $PythonCmd = Get-Command "python" -ErrorAction SilentlyContinue
        if (-not $PythonCmd) {
            Write-Host "[-] Error: Python was not found in your system PATH!" -ForegroundColor Red
            Write-Host "    Python is required to run the automated Qt6 downloader tool (aqtinstall)." -ForegroundColor Gray
            Write-Host "    Please install Python from: https://www.python.org/downloads/" -ForegroundColor Gray
            Write-Host "    Alternatively, you can manually copy/download Qt 6.10.2 MinGW into 'vendor/qt6/6.10.2/mingw_64'." -ForegroundColor Gray
            Write-Host ""
            Read-Host "Press Enter to exit..."
            Exit 1
        }
        
        Write-Host "    Setting up Python virtual environment..." -ForegroundColor Yellow
        if (-not (Test-Path "$PSScriptRoot\.venv")) {
            python -m venv "$PSScriptRoot\.venv"
        }
        
        Write-Host "    Installing downloader tools (aqtinstall & py7zr)..." -ForegroundColor Yellow
        & "$PSScriptRoot\.venv\Scripts\pip.exe" install -U aqtinstall py7zr
        
        Write-Host "    Downloading Qt 6.10.2 for Windows Desktop MinGW..." -ForegroundColor Yellow
        Write-Host "    (This will download around 200MB and extract it. Please wait...)" -ForegroundColor Gray
        New-Item -ItemType Directory -Path "$PSScriptRoot\vendor\qt6" -Force | Out-Null
        
        & "$PSScriptRoot\.venv\Scripts\python.exe" -m aqt install-qt -O "$PSScriptRoot\vendor\qt6" windows desktop 6.10.2 win64_mingw
        
        Write-Host "[+] Qt6 Windows MinGW binaries downloaded successfully!" -ForegroundColor Green
        
        # Re-resolve the folder path
        $Qt6Folder = Resolve-Path "$PSScriptRoot\vendor\qt6\*\mingw_64" -ErrorAction SilentlyContinue
    } else {
        Write-Host "[-] Cancelled. CMake will attempt to locate global system Qt6 libraries." -ForegroundColor Red
    }
} else {
    Write-Host "    Found Local Qt6: $($Qt6Folder[0].Path)" -ForegroundColor Green
}

# -------------------------------------------------------------
# STEP 4: Configure & Build using CMake and MinGW
# -----------------------------------------------------------------------------
Write-Host "[4/4] Configuring & Building BrewNBites..." -ForegroundColor Yellow

$BuildDir = "$PSScriptRoot\build"

# Clean old cache to prevent compiler conflicts
if (Test-Path "$BuildDir\CMakeCache.txt") {
    Write-Host "    Clearing old build cache..." -ForegroundColor Gray
    Remove-Item "$BuildDir\CMakeCache.txt" -Force
}

# 1. Run CMake configuration
Write-Host "    Running CMake Configuration..." -ForegroundColor Yellow
try {
    # Force MinGW Makefiles generator to use g++ instead of MSVC
    cmake -B "$BuildDir" -S "$PSScriptRoot" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
}
catch {
    Write-Host "[-] Error: CMake configuration failed!" -ForegroundColor Red
    Write-Host ""
    Read-Host "Press Enter to exit..."
    Exit 1
}

# 2. Run CMake compilation
Write-Host "    Compiling codebase..." -ForegroundColor Yellow
try {
    cmake --build "$BuildDir" --config Release -j $env:NUMBER_OF_PROCESSORS
}
catch {
    Write-Host "[-] Error: Compilation failed!" -ForegroundColor Red
    Write-Host ""
    Read-Host "Press Enter to exit..."
    Exit 1
}

# -----------------------------------------------------------------------------
# Compilation Complete!
# -----------------------------------------------------------------------------
Write-Host ""
Write-Host "==========================================================" -ForegroundColor Green
Write-Host "         COMPILATION COMPLETED SUCCESSFULLY!              " -ForegroundColor Green
Write-Host "==========================================================" -ForegroundColor Green
Write-Host ""
Write-Host "Your Windows executable is ready!" -ForegroundColor Green
Write-Host "Location: $BuildDir\BrewNBites.exe" -ForegroundColor Green
Write-Host "Dependencies (DLLs) have been copied alongside the executable automatically." -ForegroundColor Gray
Write-Host ""

Read-Host "Press Enter to finish..."