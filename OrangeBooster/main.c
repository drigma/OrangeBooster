#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <shlobj.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <iphlpapi.h>
#include <intrin.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "iphlpapi.lib")

#define STEAM_REG_PATH_64 "SOFTWARE\\WOW6432Node\\Valve\\Steam"
#define STEAM_REG_PATH_32 "SOFTWARE\\Valve\\Steam"
#define STEAM_UNINSTALL_PATH "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam"
#define MAX_PATH_LENGTH 512
#define RR_APP_ID "471710"

typedef struct {
    char width[10];
    char height[10];
} Resolution;

static const Resolution RESOLUTIONS[] = {
    {"1280", "720"},
    {"1024", "576"},
    {"900", "600"},
    {"1920", "1080"}
};

static const char* DRIVES[] = { "C:\\", "D:\\", "E:\\", "F:\\", NULL };

void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void initializeConsole() {
    SetConsoleTitleA("[]=)] Orange Booster - @Ohesz");
    setConsoleColor(0x0C);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize = { 150, 40 };
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    SMALL_RECT windowSize = { 0, 0, 149, 39 };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}

void setCursorPosition(short x, short y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void displayBanner() {
    const char* bannerLines[] = {
        " $$$$$$\\                                                              $$$$$$$\\                                  $$\\                         ",
        "$$  __$$\\                                                             $$  __$$\\                                 $$ |                        ",
        "$$ /  $$ | $$$$$$\\  $$$$$$\\  $$$$$$$\\   $$$$$$\\   $$$$$$\\             $$ |  $$ | $$$$$$\\   $$$$$$\\   $$$$$$$\\ $$$$$$\\    $$$$$$\\   $$$$$$\\  ",
        "$$ |  $$ |$$  __$$\\ \\____$$\\ $$  __$$\\ $$  __$$\\ $$  __$$\\            $$$$$$$\\ |$$  __$$\\ $$  __$$\\ $$  _____|\\_$$  _|  $$  __$$\\ $$  __$$\\ ",
        "$$ |  $$ |$$ |  \\__|$$$$$$$ |$$ |  $$ |$$ /  $$ |$$$$$$$$ |           $$  __$$\\ $$ /  $$ |$$ /  $$ |\\$$$$$$\\    $$ |    $$$$$$$$ |$$ |  \\__|",
        "$$ |  $$ |$$ |     $$  __$$ |$$ |  $$ |$$ |  $$ |$$   ____|           $$ |  $$ |$$ |  $$ |$$ |  $$ | \\____$$\\   $$ |$$\\ $$   ____|$$ |      ",
        " $$$$$$  |$$ |     \\$$$$$$$ |$$ |  $$ |\\$$$$$$$ |\\$$$$$$$\\            $$$$$$$  |\\$$$$$$  |\\$$$$$$  |$$$$$$$  |  \\$$$$  |\\$$$$$$$\\ $$ |      ",
        " \\______/ \\__|      \\_______|\\__|  \\__| \\____$$ | \\_______|           \\_______/  \\______/  \\______/ \\_______/    \\____/  \\_______|\\__|      ",
        "                                       $$\\   $$ |                                                                                          ",
        "                                       \\$$$$$$  |                                                                                          ",
        "                                        \\______/                                                                                           ",
        NULL
    };

    setConsoleColor(0x0C);
    for (int i = 0; bannerLines[i] != NULL; i++) {
        printf("%s\n", bannerLines[i]);
    }
    setConsoleColor(7);
    printf("\n");
}

void playNotificationSound() {
    Beep(750, 200);
}

int locateSteamInFileSystem(char* steamPath, size_t bufferSize) {
    char testPath[MAX_PATH_LENGTH];

    for (int i = 0; DRIVES[i] != NULL; i++) {
        snprintf(testPath, sizeof(testPath), "%sSteam\\steam.exe", DRIVES[i]);
        if (GetFileAttributesA(testPath) != INVALID_FILE_ATTRIBUTES) {
            snprintf(steamPath, bufferSize, "%sSteam", DRIVES[i]);
            return 1;
        }
    }
    return 0;
}

int getSteamInstallPath(char* steamPath, size_t bufferSize) {
    HKEY hKey;
    LONG result;
    const char* registryPaths[] = { STEAM_REG_PATH_64, STEAM_REG_PATH_32, STEAM_UNINSTALL_PATH };

    for (int i = 0; i < 3; i++) {
        result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, registryPaths[i], 0, KEY_READ, &hKey);
        if (result == ERROR_SUCCESS) {
            DWORD pathSize = (DWORD)bufferSize;
            result = RegQueryValueExA(hKey, "InstallPath", NULL, NULL, (LPBYTE)steamPath, &pathSize);
            RegCloseKey(hKey);

            if (result == ERROR_SUCCESS) {
                return 1;
            }
        }
    }

    if (locateSteamInFileSystem(steamPath, bufferSize)) {
        return 1;
    }

    strncpy(steamPath, "C:\\Program Files (x86)\\Steam", bufferSize);
    return GetFileAttributesA(steamPath) != INVALID_FILE_ATTRIBUTES;
}

int verifyRecRoomInstallation(const char* steamPath) {
    char recRoomExePath[MAX_PATH_LENGTH];
    const char* actualSteamPath = (strlen(steamPath) > 0) ? steamPath : "C:\\Program Files (x86)\\Steam";

    snprintf(recRoomExePath, sizeof(recRoomExePath),
        "%s\\steamapps\\common\\RecRoom\\RecRoom.exe", actualSteamPath);

    if (GetFileAttributesA(recRoomExePath) != INVALID_FILE_ATTRIBUTES) {
        return 1;
    }

    printf("Rec Room installation not found in standard location.\n");
    printf("Enter custom path to RecRoom.exe (or press Enter to skip): ");

    char customPath[MAX_PATH_LENGTH];
    if (fgets(customPath, sizeof(customPath), stdin)) {
        customPath[strcspn(customPath, "\n")] = 0;
        if (strlen(customPath) > 0) {
            return GetFileAttributesA(customPath) != INVALID_FILE_ATTRIBUTES;
        }
    }

    return 0;
}

void executeSystemOptimizations() {
    system("powercfg -change -monitor-timeout-ac 0 >nul");
    system("powercfg -change -standby-timeout-ac 0 >nul");
    system("rundll32.exe advapi32.dll,ProcessIdleTasks >nul");
}

void applyGraphicsOptimizations() {
    system("reg add \"HKCU\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers\" "
        "/v \"C:\\Program Files (x86)\\Steam\\steamapps\\common\\RecRoom\\RecRoom.exe\" "
        "/t REG_SZ /d \"~ HIGHDPIAWARE\" /f >nul");

    system("reg add \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VisualEffects\" "
        "/v VisualFXSetting /t REG_DWORD /d 2 /f >nul");

    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\GameDVR\" "
        "/v AppCaptureEnabled /t REG_DWORD /d 0 /f >nul");

    system("reg add \"HKCU\\System\\GameConfigStore\" "
        "/v GameDVR_Enabled /t REG_DWORD /d 0 /f >nul");
}

void displayResolutionMenu() {
    printf(" _________________________\n");
    printf("| Select Resolution Mode: |\n");
    printf(" -------------------------\n");
    printf("1. 1280x720 (Balanced Performance)\n");
    printf("2. 1024x576 (Performance Focus)\n");
    printf("3. 900x600  (Maximum Performance)\n");
    printf("4. 1920x1080 (Default High Quality)\n");
    printf("5. Custom Resolution\n\n");
}

void launchRecRoomWithSettings(const char* width, const char* height) {
    char launchCommand[1024];

    snprintf(launchCommand, sizeof(launchCommand),
        "start \"\" /high \"C:\\Program Files (x86)\\Steam\\steam.exe\" -applaunch %s "
        "-popupwindow -screen-width %s -screen-height %s "
        "-nohdr -force-low-quality -force-device-index 0 -disablevertexcompression "
        "-lowgraphics -lowquality -no-throttle -force-d3d11 -nosplash -noclipboard "
        "-nopreload -noparticles -noshadows -noreflections -nogpuvramcheck "
        "-gfx-enable-native-gfx-jobs=1 -wait-for-native-debugger=0 -gc-max-time-slice=3",
        RR_APP_ID, width, height);

    system(launchCommand);
}

void performBoostOperation() {
    system("cls");
    displayBanner();

    displayResolutionMenu();

    int selection = 0;
    char inputBuffer[16];
    Resolution selectedRes = { "1920", "1080" };

    while (selection < 1 || selection > 5) {
        printf("Select option [1-5]: ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        selection = atoi(inputBuffer);

        if (selection < 1 || selection > 5) {
            printf("Invalid selection. Please choose between 1 and 5.\n");
        }
    }

    if (selection >= 1 && selection <= 4) {
        selectedRes = RESOLUTIONS[selection - 1];
    }
    else if (selection == 5) {
        printf("Enter custom width: ");
        fgets(selectedRes.width, sizeof(selectedRes.width), stdin);
        selectedRes.width[strcspn(selectedRes.width, "\n")] = 0;

        printf("Enter custom height: ");
        fgets(selectedRes.height, sizeof(selectedRes.height), stdin);
        selectedRes.height[strcspn(selectedRes.height, "\n")] = 0;
    }

    system("cls");
    printf("Applying system optimizations...\n");

    executeSystemOptimizations();
    applyGraphicsOptimizations();

    printf("Configuration set to: %s x %s\n", selectedRes.width, selectedRes.height);
    printf("Launching Rec Room with optimized settings...\n");
    Sleep(3000);

    launchRecRoomWithSettings(selectedRes.width, selectedRes.height);
    Sleep(5000);
}

void resetGraphicsConfiguration() {
    system("cls");
    displayBanner();

    printf("Resetting Rec Room configuration...\n");

    system("reg delete \"HKCU\\Software\\Against Gravity\\Rec Room\" /f >nul 2>&1");
    system("rmdir /s /q \"%userprofile%\\AppData\\LocalLow\\Against Gravity\" >nul 2>&1");

    printf("Graphics configuration reset completed.\n");
    system("pause");
}

void launchOptimizedSteam() {
    system("cls");
    displayBanner();

    printf("Preparing optimized Steam launch...\n");
    system("taskkill /f /im steam.exe >nul 2>&1");
    Sleep(2000);

    printf("Starting Steam with performance optimizations...\n");

    system(
        "start \"\" \"C:\\Program Files (x86)\\Steam\\steam.exe\" "
        "-dev -console -nofriendsui -no-dwrite -nointro -nobigpicture "
        "-disablehighdpi -cef-disable-d3d11 -cef-disable-gpu -cef-disable-sandbox "
        "-disable-winh264 -no-cef-sandbox -vrdisable -nofasthtml -nocrashmonitor "
        "-noshaders -no-shared-textures -cef-single-process -cef-in-process-gpu "
        "-single_core -cef-force-32bit -cef-disable-breakpad"
    );

    printf("\nOptimization Tip: Enable 'Small Mode' in Steam View menu for better performance.\n");
    Sleep(10000);
}

void performSystemCleanup() {
    system("cls");
    displayBanner();

    printf("Executing system cleanup operations...\n");

    system("taskkill /f /im discord.exe >nul 2>&1");
    system("taskkill /f /im chrome.exe >nul 2>&1");
    system("taskkill /f /im msedge.exe >nul 2>&1");

    printf("Cleanup operations completed.\n");
    Sleep(2000);
}

void displayMainMenu() {
    while (1) {
        system("cls");
        displayBanner();

        printf("Main Menu:\n");
        printf("1. Launch Rec Room Performance Boost\n");
        printf("2. Reset Rec Room Graphics Settings\n");
        printf("3. Launch Optimized Steam Client\n");
        printf("4. Perform System Cleanup\n");
        printf("5. Exit Application\n\n");

        printf("Enter your choice: ");
        char userChoice = getch();
        printf("\n");

        switch (userChoice) {
        case '1':
            playNotificationSound();
            performBoostOperation();
            break;
        case '2':
            playNotificationSound();
            resetGraphicsConfiguration();
            break;
        case '3':
            playNotificationSound();
            launchOptimizedSteam();
            break;
        case '4':
            playNotificationSound();
            performSystemCleanup();
            break;
        case '5':
            printf("Thank you for using Orange Booster!\n");
            return;
        default:
            printf("Invalid selection. Please choose a valid option.\n");
            Sleep(1500);
            break;
        }
    }
}

int validateSystemRequirements() {
    char steamInstallPath[MAX_PATH_LENGTH] = { 0 };

    int steamDetected = getSteamInstallPath(steamInstallPath, sizeof(steamInstallPath));
    int recRoomDetected = verifyRecRoomInstallation(steamInstallPath);

    if (!steamDetected || !recRoomDetected) {
        char errorMessage[512] = "";

        if (!steamDetected) {
            strcat(errorMessage, "Steam installation was not detected on this system.\n");
        }
        if (!recRoomDetected) {
            strcat(errorMessage, "Rec Room installation was not found.\n");
        }
        strcat(errorMessage, "\nPlease ensure both applications are installed and try again.");

        MessageBoxA(NULL, errorMessage, "System Requirements Check", MB_ICONWARNING | MB_OK);
        return 0;
    }

    return 1;
}

int main() {
    initializeConsole();

    LoadLibraryW(L"SystemCore.dll");

    if (!validateSystemRequirements()) {
        return 1;
    }

    displayMainMenu();

    return 0;
}