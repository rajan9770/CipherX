#include <windows.h>
#include "aes_crypto.h"
#include <fstream>
#include <string>
#include <commdlg.h>
#include <gdiplus.h>
#include <cstring>
#include <cstdio>

using namespace Gdiplus;
using namespace std;

// ==============================
// CipherX GUI Resources
// ==============================
ULONG_PTR gdiplusToken;
Image *backgroundImage = nullptr;
HFONT hTitleFont;
HFONT hSubtitleFont;
HFONT hButtonFont;
HFONT hNormalFont;
HWND hStatus = NULL;

HBRUSH hBackgroundBrush;
HBRUSH hButtonBrush;
HBRUSH hEncryptBrush;
HBRUSH hDecryptBrush;
HBRUSH hDeleteBrush;
HBRUSH hExitBrush;

void CreateGUIFonts()
{
    hTitleFont = CreateFontA(
        36, 0, 0, 0, FW_BOLD,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        "Segoe UI");

    hSubtitleFont = CreateFontA(
        20, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        "Segoe UI");

    hButtonFont = CreateFontA(
        17, 0, 0, 0, FW_SEMIBOLD,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        "Segoe UI");

    hNormalFont = CreateFontA(
        14, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        "Segoe UI");

    // CipherX background
    hBackgroundBrush = CreateSolidBrush(
        RGB(245, 247, 250));

    // Button brushes
    hButtonBrush = CreateSolidBrush(
        RGB(230, 235, 242));

    hEncryptBrush = CreateSolidBrush(
        RGB(45, 110, 210));

    hDecryptBrush = CreateSolidBrush(
        RGB(40, 160, 110));

    hDeleteBrush = CreateSolidBrush(
        RGB(220, 75, 75));

    hExitBrush = CreateSolidBrush(
        RGB(90, 95, 105));
}

// ==============================
// Main Window Button IDs
// ==============================
#define ID_CREATE 101
#define ID_ENCRYPT 102
#define ID_DECRYPT 103
#define ID_DELETE 104
#define ID_EXIT 105
#define ID_ABOUT 106

// ==============================
// Create File Dialog IDs
// ==============================
#define ID_FILE_NAME 201
#define ID_CREATE_OK 202
#define ID_CREATE_CANCEL 203

// ==============================
// Encrypt File Dialog IDs
// ==============================
#define ID_ENCRYPT_FILE_NAME 301
#define ID_ENCRYPT_OUTPUT_NAME 302
#define ID_ENCRYPT_PASSWORD 303
#define ID_ENCRYPT_OK 304
#define ID_ENCRYPT_CANCEL 305
#define ID_ENCRYPT_BROWSE 306

// ==============================
// Decrypt File Dialog IDs
// ==============================
#define ID_DECRYPT_FILE_NAME 401
#define ID_DECRYPT_OUTPUT_NAME 402
#define ID_DECRYPT_PASSWORD 403
#define ID_DECRYPT_OK 404
#define ID_DECRYPT_CANCEL 405
#define ID_DECRYPT_BROWSE 406

// ==============================
// Create File Dialog Procedure
// ==============================
LRESULT CALLBACK CreateFileProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        // Label
        CreateWindowA(
            "STATIC",
            "Enter file name:",
            WS_VISIBLE | WS_CHILD,
            25, 25, 250, 25,
            hwnd,
            NULL,
            NULL,
            NULL);

        // File name input box
        CreateWindowA(
            "EDIT",
            "",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            25, 55, 330, 30,
            hwnd,
            (HMENU)ID_FILE_NAME,
            NULL,
            NULL);

        // Create button
        CreateWindowA(
            "BUTTON",
            "Create",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            25, 105, 155, 35,
            hwnd,
            (HMENU)ID_CREATE_OK,
            NULL,
            NULL);

        // Cancel button
        CreateWindowA(
            "BUTTON",
            "Cancel",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            200, 105, 155, 35,
            hwnd,
            (HMENU)ID_CREATE_CANCEL,
            NULL,
            NULL);

        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_CREATE_OK:
        {
            char fileName[260];

            GetWindowTextA(
                GetDlgItem(hwnd, ID_FILE_NAME),
                fileName,
                sizeof(fileName));

            string name = fileName;

            // Check empty name
            if (name.empty())
            {
                UpdateStatus("File created successfully");
                MessageBoxA(
                    hwnd,
                    "Please enter a file name.",
                    "CipherX",
                    MB_OK | MB_ICONWARNING);

                break;
            }

            // Create the file
            ofstream file(name);

            if (!file)
            {
                MessageBoxA(
                    hwnd,
                    "Could not create the file.",
                    "CipherX",
                    MB_OK | MB_ICONERROR);

                break;
            }

            file.close();
            
            MessageBoxA(
                hwnd,
                "File created successfully!",
                "CipherX",
                MB_OK | MB_ICONINFORMATION);

            DestroyWindow(hwnd);

            break;
        }

        case ID_CREATE_CANCEL:
        {
            DestroyWindow(hwnd);
            break;
        }
        }

        break;
    }

    case WM_CLOSE:
    {
        DestroyWindow(hwnd);
        break;
    }

    default:
        return DefWindowProcA(
            hwnd,
            uMsg,
            wParam,
            lParam);
    }

    return 0;
}

// ==============================
// Open Create File Dialog
// ==============================
void OpenCreateFileDialog(HWND parent, HINSTANCE hInstance)
{
    const char CLASS_NAME[] = "CipherXCreateFile";

    static bool registered = false;

    if (!registered)
    {
        WNDCLASSA wc = {};

        wc.lpfnWndProc = CreateFileProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClassA(&wc);

        registered = true;
    }

    HWND dialog = CreateWindowExA(
        WS_EX_DLGMODALFRAME,
        CLASS_NAME,
        "Create File",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        400,
        200,
        parent,
        NULL,
        hInstance,
        NULL);

    if (dialog == NULL)
    {
        MessageBoxA(
            parent,
            "Could not open Create File dialog.",
            "CipherX",
            MB_OK | MB_ICONERROR);

        return;
    }

    // Center dialog relative to main window
    RECT parentRect;
    RECT dialogRect;

    GetWindowRect(parent, &parentRect);
    GetWindowRect(dialog, &dialogRect);

    int dialogWidth =
        dialogRect.right - dialogRect.left;

    int dialogHeight =
        dialogRect.bottom - dialogRect.top;

    int x =
        parentRect.left +
        ((parentRect.right - parentRect.left) - dialogWidth) / 2;

    int y =
        parentRect.top +
        ((parentRect.bottom - parentRect.top) - dialogHeight) / 2;

    SetWindowPos(
        dialog,
        HWND_TOP,
        x,
        y,
        0,
        0,
        SWP_NOSIZE);

    ShowWindow(dialog, SW_SHOW);
    UpdateWindow(dialog);

    // Disable main window while dialog is open
    EnableWindow(parent, FALSE);

    MSG msg;

    while (IsWindow(dialog) &&
           GetMessageA(&msg, NULL, 0, 0) > 0)
    {
        if (!IsDialogMessageA(dialog, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    EnableWindow(parent, TRUE);
    SetForegroundWindow(parent);
}

// ==============================
// Encrypt File Dialog Procedure
// ==============================
LRESULT CALLBACK EncryptFileProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        // Input file label
        CreateWindowA(
            "STATIC",
            "Enter file name:",
            WS_VISIBLE | WS_CHILD,
            25, 20, 250, 25,
            hwnd,
            NULL,
            NULL,
            NULL);

        // Input file
        CreateWindowA(
            "EDIT",
            "",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            25, 50, 260, 30,
            hwnd,
            (HMENU)ID_ENCRYPT_FILE_NAME,
            NULL,
            NULL);

        // Output file label
        CreateWindowA(
            "STATIC",
            "Enter encrypted file name:",
            WS_VISIBLE | WS_CHILD,
            25, 90, 250, 25,
            hwnd,
            NULL,
            NULL,
            NULL);

        CreateWindowA(
            "BUTTON",
            "Browse",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            295, 50, 60, 30,
            hwnd,
            (HMENU)ID_ENCRYPT_BROWSE,
            NULL,
            NULL);

        // Output file
        CreateWindowA(
            "EDIT",
            "",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            25, 120, 330, 30,
            hwnd,
            (HMENU)ID_ENCRYPT_OUTPUT_NAME,
            NULL,
            NULL);

        // Password label
        CreateWindowA(
            "STATIC",
            "Enter the password:",
            WS_VISIBLE | WS_CHILD,
            25, 160, 250, 25,
            hwnd,
            NULL,
            NULL,
            NULL);

        // Password input
        CreateWindowA(
            "EDIT",
            "",
            WS_VISIBLE | WS_CHILD | WS_BORDER |
                ES_PASSWORD | ES_AUTOHSCROLL,
            25, 190, 330, 30,
            hwnd,
            (HMENU)ID_ENCRYPT_PASSWORD,
            NULL,
            NULL);

        // Encrypt button
        CreateWindowA(
            "BUTTON",
            "Encrypt",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            25, 240, 155, 35,
            hwnd,
            (HMENU)ID_ENCRYPT_OK,
            NULL,
            NULL);

        // Cancel button
        CreateWindowA(
            "BUTTON",
            "Cancel",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            200, 240, 155, 35,
            hwnd,
            (HMENU)ID_ENCRYPT_CANCEL,
            NULL,
            NULL);

        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_ENCRYPT_BROWSE:
        {
            char fileName[260] = {};

            OPENFILENAMEA ofn = {};

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = sizeof(fileName);

            ofn.lpstrFilter =
                "All Files (*.*)\0*.*\0"
                "Text Files (*.txt)\0*.txt\0"
                "Encrypted Files (*.enc)\0*.enc\0";

            ofn.nFilterIndex = 1;

            ofn.Flags =
                OFN_PATHMUSTEXIST |
                OFN_FILEMUSTEXIST;

            if (GetOpenFileNameA(&ofn))
            {
                SetWindowTextA(
                    GetDlgItem(
                        hwnd,
                        ID_ENCRYPT_FILE_NAME),
                    fileName);
            }

            break;
        }

        case ID_ENCRYPT_OK:
        {
            char inputFile[260];
            char outputFile[260];
            char password[260];

            GetWindowTextA(
                GetDlgItem(hwnd, ID_ENCRYPT_FILE_NAME),
                inputFile,
                sizeof(inputFile));

            GetWindowTextA(
                GetDlgItem(hwnd, ID_ENCRYPT_OUTPUT_NAME),
                outputFile,
                sizeof(outputFile));

            GetWindowTextA(
                GetDlgItem(hwnd, ID_ENCRYPT_PASSWORD),
                password,
                sizeof(password));

            string input = inputFile;
            string output = outputFile;
            string pass = password;

            // Check input file
            if (input.empty())
            {
                MessageBoxA(
                    hwnd,
                    "Please enter the input file name.",
                    "CipherX",
                    MB_OK | MB_ICONWARNING);

                break;
            }

            // Check output file
            if (output.empty())
            {
                MessageBoxA(
                    hwnd,
                    "Please enter the encrypted file name.",
                    "CipherX",
                    MB_OK | MB_ICONWARNING);

                break;
            }

            // Check password
            if (pass.empty())
            {
                MessageBoxA(
                    hwnd,
                    "Please enter a password.",
                    "CipherX",
                    MB_OK | MB_ICONWARNING);

                break;
            }

            // Check input file exists
            ifstream file(input, ios::binary);

            if (!file)
            {
                MessageBoxA(
                    hwnd,
                    "The specified input file does not exist.",
                    "CipherX",
                    MB_OK | MB_ICONERROR);

                break;
            }

            file.close();

            // Prevent encrypting to the same file
            if (input == output)
            {
                MessageBoxA(
                    hwnd,
                    "Input and encrypted file names must be different.",
                    "CipherX",
                    MB_OK | MB_ICONWARNING);

                break;
            }

            // Perform AES-256 encryption
            if (encryptFile(input, output, pass))
            {
                string message =
                    "Encryption successful!\n\nCreated: " + output;

                MessageBoxA(
                    hwnd,
                    message.c_str(),
                    "CipherX",
                    MB_OK | MB_ICONINFORMATION);

                DestroyWindow(hwnd);
            }
            else
            {
                MessageBoxA(
                    hwnd,
                    "Encryption failed.",
                    "CipherX",
                    MB_OK | MB_ICONERROR);
            }

            break;
        }

        case ID_ENCRYPT_CANCEL:
        {
            DestroyWindow(hwnd);
            break;
        }
        }

        break;
    }

    case WM_CLOSE:
    {
        DestroyWindow(hwnd);
        break;
    }

    default:
        return DefWindowProcA(
            hwnd,
            uMsg,
            wParam,
            lParam);
    }
     
    return 0;
}

// ==============================
// Open Encrypt File Dialog
// ==============================
void OpenEncryptFileDialog(
    HWND parent,
    HINSTANCE hInstance)
{
    const char CLASS_NAME[] = "CipherXEncryptFile";

    static bool registered = false;

    if (!registered)
    {
        WNDCLASSA wc = {};

        wc.lpfnWndProc = EncryptFileProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClassA(&wc);

        registered = true;
    }

    HWND dialog = CreateWindowExA(
        WS_EX_DLGMODALFRAME,
        CLASS_NAME,
        "Encrypt File",
        WS_OVERLAPPED |
            WS_CAPTION |
            WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        400,
        330,
        parent,
        NULL,
        hInstance,
        NULL);

    if (dialog == NULL)
    {
        MessageBoxA(
            parent,
            "Could not open Encrypt File dialog.",
            "CipherX",
            MB_OK | MB_ICONERROR);

        return;
    }

    // Center dialog
    RECT parentRect;
    RECT dialogRect;

    GetWindowRect(parent, &parentRect);
    GetWindowRect(dialog, &dialogRect);

    int dialogWidth =
        dialogRect.right - dialogRect.left;

    int dialogHeight =
        dialogRect.bottom - dialogRect.top;

    int x =
        parentRect.left +
        ((parentRect.right - parentRect.left) - dialogWidth) / 2;

    int y =
        parentRect.top +
        ((parentRect.bottom - parentRect.top) - dialogHeight) / 2;

    SetWindowPos(
        dialog,
        HWND_TOP,
        x,
        y,
        0,
        0,
        SWP_NOSIZE);

    ShowWindow(dialog, SW_SHOW);
    UpdateWindow(dialog);

    // Disable main window
    EnableWindow(parent, FALSE);

    MSG msg;

    while (IsWindow(dialog) &&
           GetMessageA(&msg, NULL, 0, 0) > 0)
    {
        if (!IsDialogMessageA(dialog, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    EnableWindow(parent, TRUE);
    SetForegroundWindow(parent);
}

// ==============================
// Decrypt File Dialog Procedure
// ==============================
LRESULT CALLBACK DecryptFileProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        // Encrypted file label
        CreateWindowA(
            "STATIC",
            "Enter encrypted file name:",
            WS_VISIBLE | WS_CHILD,
            25, 20, 300, 25,
            hwnd,
            NULL,
            NULL,
            NULL);

        // Encrypted file input
        CreateWindowA(
            "EDIT",
            "",
            WS_VISIBLE | WS_CHILD | WS_BORDER |
                ES_AUTOHSCROLL,
            25, 50, 260, 30,
            hwnd,
            (HMENU)ID_DECRYPT_FILE_NAME,
            NULL,
            NULL);

        CreateWindowA(
            "BUTTON",
            "Browse",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            295, 50, 60, 30,
            hwnd,
            (HMENU)ID_DECRYPT_BROWSE,
            NULL,
            NULL);

        // Output file label
        CreateWindowA(
            "STATIC",
            "Enter output file name:",
            WS_VISIBLE | WS_CHILD,
            25, 90, 300, 25,
            hwnd,
            NULL,
            NULL,
            NULL);

        // Output file input
        CreateWindowA(
            "EDIT",
            "",
            WS_VISIBLE | WS_CHILD | WS_BORDER |
                ES_AUTOHSCROLL,
            25, 120, 330, 30,
            hwnd,
            (HMENU)ID_DECRYPT_OUTPUT_NAME,
            NULL,
            NULL);

        // Password label
        CreateWindowA(
            "STATIC",
            "Enter the password:",
            WS_VISIBLE | WS_CHILD,
            25, 160, 250, 25,
            hwnd,
            NULL,
            NULL,
            NULL);

        // Password input
        CreateWindowA(
            "EDIT",
            "",
            WS_VISIBLE | WS_CHILD | WS_BORDER |
                ES_PASSWORD | ES_AUTOHSCROLL,
            25, 190, 330, 30,
            hwnd,
            (HMENU)ID_DECRYPT_PASSWORD,
            NULL,
            NULL);

        // Decrypt button
        CreateWindowA(
            "BUTTON",
            "Decrypt",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            25, 240, 155, 35,
            hwnd,
            (HMENU)ID_DECRYPT_OK,
            NULL,
            NULL);

        // Cancel button
        CreateWindowA(
            "BUTTON",
            "Cancel",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            200, 240, 155, 35,
            hwnd,
            (HMENU)ID_DECRYPT_CANCEL,
            NULL,
            NULL);

        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_DECRYPT_BROWSE:
        {
            char fileName[260] = {};

            OPENFILENAMEA ofn = {};

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = sizeof(fileName);

            ofn.lpstrFilter =
                "Encrypted Files (*.enc)\0*.enc\0"
                "All Files (*.*)\0*.*\0";

            ofn.nFilterIndex = 1;

            ofn.Flags =
                OFN_PATHMUSTEXIST |
                OFN_FILEMUSTEXIST;

            if (GetOpenFileNameA(&ofn))
            {
                SetWindowTextA(
                    GetDlgItem(
                        hwnd,
                        ID_DECRYPT_FILE_NAME),
                    fileName);
            }

            break;
        }

        case ID_DECRYPT_OK:
        {
            char encryptedFile[260];
            char outputFile[260];
            char password[260];

            GetWindowTextA(
                GetDlgItem(
                    hwnd,
                    ID_DECRYPT_FILE_NAME),
                encryptedFile,
                sizeof(encryptedFile));

            GetWindowTextA(
                GetDlgItem(
                    hwnd,
                    ID_DECRYPT_OUTPUT_NAME),
                outputFile,
                sizeof(outputFile));

            GetWindowTextA(
                GetDlgItem(
                    hwnd,
                    ID_DECRYPT_PASSWORD),
                password,
                sizeof(password));

            string input = encryptedFile;
            string output = outputFile;
            string pass = password;

            // Check encrypted file name
            if (input.empty())
            {
                MessageBoxA(
                    hwnd,
                    "Please enter the encrypted file name.",
                    "CipherX",
                    MB_OK | MB_ICONWARNING);

                break;
            }

            // Check output file name
            if (output.empty())
            {
                MessageBoxA(
                    hwnd,
                    "Please enter the output file name.",
                    "CipherX",
                    MB_OK | MB_ICONWARNING);

                break;
            }

            // Check password
            if (pass.empty())
            {
                MessageBoxA(
                    hwnd,
                    "Please enter a password.",
                    "CipherX",
                    MB_OK | MB_ICONWARNING);

                break;
            }

            // Check encrypted file exists
            ifstream file(
                input,
                ios::binary);

            if (!file)
            {
                MessageBoxA(
                    hwnd,
                    "The encrypted file does not exist.",
                    "CipherX",
                    MB_OK | MB_ICONERROR);

                break;
            }

            file.close();

            // Prevent same input/output file
            if (input == output)
            {
                MessageBoxA(
                    hwnd,
                    "Encrypted and output file names "
                    "must be different.",
                    "CipherX",
                    MB_OK | MB_ICONWARNING);

                break;
            }

            // Perform AES-256 decryption
            if (decryptFile(
                    input,
                    output,
                    pass))
            {
                string message =
                    "Decryption successful!\n\nCreated: " + output;

                MessageBoxA(
                    hwnd,
                    message.c_str(),
                    "CipherX",
                    MB_OK | MB_ICONINFORMATION);

                DestroyWindow(hwnd);
            }
            else
            {
                MessageBoxA(
                    hwnd,
                    "Wrong password or corrupted "
                    "encrypted file.",
                    "CipherX",
                    MB_OK | MB_ICONERROR);
            }

            break;
        }

        case ID_DECRYPT_CANCEL:
        {
            DestroyWindow(hwnd);
            break;
        }
        }

        break;
    }

    case WM_CLOSE:
    {
        DestroyWindow(hwnd);
        break;
    }

    default:
        return DefWindowProcA(
            hwnd,
            uMsg,
            wParam,
            lParam);
    }
    return 0;
}

// ==============================
// Open Decrypt File Dialog
// ==============================
void OpenDecryptFileDialog(
    HWND parent,
    HINSTANCE hInstance)
{
    const char CLASS_NAME[] = "CipherXDecryptFile";

    static bool registered = false;

    if (!registered)
    {
        WNDCLASSA wc = {};

        wc.lpfnWndProc = DecryptFileProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground =
            (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClassA(&wc);

        registered = true;
    }

    HWND dialog = CreateWindowExA(
        WS_EX_DLGMODALFRAME,
        CLASS_NAME,
        "Decrypt File",
        WS_OVERLAPPED |
            WS_CAPTION |
            WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        400,
        330,
        parent,
        NULL,
        hInstance,
        NULL);

    if (dialog == NULL)
    {
        MessageBoxA(
            parent,
            "Could not open Decrypt File dialog.",
            "CipherX",
            MB_OK | MB_ICONERROR);

        return;
    }

    // Center dialog
    RECT parentRect;
    RECT dialogRect;

    GetWindowRect(
        parent,
        &parentRect);

    GetWindowRect(
        dialog,
        &dialogRect);

    int dialogWidth =
        dialogRect.right - dialogRect.left;

    int dialogHeight =
        dialogRect.bottom - dialogRect.top;

    int x =
        parentRect.left +
        ((parentRect.right - parentRect.left) - dialogWidth) / 2;

    int y =
        parentRect.top +
        ((parentRect.bottom - parentRect.top) - dialogHeight) / 2;

    SetWindowPos(
        dialog,
        HWND_TOP,
        x,
        y,
        0,
        0,
        SWP_NOSIZE);

    ShowWindow(
        dialog,
        SW_SHOW);

    UpdateWindow(dialog);

    // Disable main window
    EnableWindow(
        parent,
        FALSE);

    MSG msg;

    while (
        IsWindow(dialog) &&
        GetMessageA(
            &msg,
            NULL,
            0,
            0) > 0)
    {
        if (!IsDialogMessageA(
                dialog,
                &msg))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    EnableWindow(
        parent,
        TRUE);

    SetForegroundWindow(
        parent);

}

// ==============================
// About CipherX
// ==============================

void ShowAboutDialog(HWND parent)
{
    MessageBoxA(
        parent,
        "CipherX\n\n"
        "Secure File Encryption\n\n"
        "Version 1.0\n\n"
        "AES-256-CBC\n"
        "PBKDF2-HMAC-SHA256\n"
        "Random Salt & IV\n\n"
        "Built with C++ and OpenSSL.",
        "About CipherX",
        MB_OK | MB_ICONINFORMATION);
}

// ==============================
// Main Window Procedure
// ==============================
void MakeButtonRounded(HWND hButton)
{
    HRGN hRegion = CreateRoundRectRgn(
        0, 0,
        300, 45,
        16, 16);

    SetWindowRgn(
        hButton,
        hRegion,
        TRUE);
}

LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        // ==============================
        // Title
        // ==============================

        HWND hTitle = CreateWindowA(
            "STATIC",
            "CIPHERX",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            150, 30, 500, 50,
            hwnd,
            NULL,
            NULL,
            NULL);

        SendMessageA(
            hTitle,
            WM_SETFONT,
            (WPARAM)hTitleFont,
            TRUE);

        // ==============================
        // Subtitle
        // ==============================

        HWND hSubtitle = CreateWindowA(
            "STATIC",
            "Secure File Encryption",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            150, 82, 500, 30,
            hwnd,
            NULL,
            NULL,
            NULL);

        SendMessageA(
            hSubtitle,
            WM_SETFONT,
            (WPARAM)hSubtitleFont,
            TRUE);

        // ==============================
        // Description
        // ==============================

        HWND hDescription = CreateWindowA(
            "STATIC",
            "AES-256 encryption | Secure | Private | Reliable",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            100, 115, 600, 25,
            hwnd,
            NULL,
            NULL,
            NULL);

        SendMessageA(
            hDescription,
            WM_SETFONT,
            (WPARAM)hNormalFont,
            TRUE);

        // ==============================
        // Create File
        // ==============================

        HWND hCreate = CreateWindowA(
            "BUTTON",
            "Create File",
            WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            150, 175, 240, 50,
            hwnd,
            (HMENU)ID_CREATE,
            NULL,
            NULL);
        MakeButtonRounded(hCreate);

        SendMessageA(
            hCreate,
            WM_SETFONT,
            (WPARAM)hButtonFont,
            TRUE);

        // ==============================
        // Encrypt File
        // ==============================

        HWND hEncrypt = CreateWindowA(
            "BUTTON",
            "Encrypt File",
            WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            410, 175, 240, 50,
            hwnd,
            (HMENU)ID_ENCRYPT,
            NULL,
            NULL);
        MakeButtonRounded(hEncrypt);

        SendMessageA(
            hEncrypt,
            WM_SETFONT,
            (WPARAM)hButtonFont,
            TRUE);

        // ==============================
        // Decrypt File
        // ==============================

        HWND hDecrypt = CreateWindowA(
            "BUTTON",
            "Decrypt File",
            WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            150, 240, 240, 50,
            hwnd,
            (HMENU)ID_DECRYPT,
            NULL,
            NULL);
        MakeButtonRounded(hDecrypt);

        SendMessageA(
            hDecrypt,
            WM_SETFONT,
            (WPARAM)hButtonFont,
            TRUE);

        // ==============================
        // Delete File
        // ==============================

        HWND hDelete = CreateWindowA(
            "BUTTON",
            "Delete File",
            WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            410, 240, 240, 50,
            hwnd,
            (HMENU)ID_DELETE,
            NULL,
            NULL);
        MakeButtonRounded(hDelete);

        SendMessageA(
            hDelete,
            WM_SETFONT,
            (WPARAM)hButtonFont,
            TRUE);

        // ==============================
        // Exit
        // ==============================

        HWND hExit = CreateWindowA(
            "BUTTON",
            "Exit",
            WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            150, 305, 240, 50,
            hwnd,
            (HMENU)ID_EXIT,
            NULL,
            NULL);
        MakeButtonRounded(hExit);

        SendMessageA(
            hExit,
            WM_SETFONT,
            (WPARAM)hButtonFont,
            TRUE);

        HWND hAbout = CreateWindowA(
            "BUTTON",
            "About CipherX",
            WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            410, 305, 240, 50,
            hwnd,
            (HMENU)ID_ABOUT,
            NULL,
            NULL);
        MakeButtonRounded(hAbout);

        SendMessageA(
            hAbout,
            WM_SETFONT,
            (WPARAM)hButtonFont,
            TRUE);

        // ==============================
        // Footer
        // ==============================
        hStatus = CreateWindowA(
            "STATIC",
            "Status: Ready",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            150, 390, 500, 30,
            hwnd,
            NULL,
            NULL,
            NULL);

        SendMessageA(
            hStatus,
            WM_SETFONT,
            (WPARAM)hNormalFont,
            TRUE);

        HWND hFooter = CreateWindowA(
            "STATIC",
            "CipherX v1.0 | AES-256",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            200, 435, 400, 25,
            hwnd,
            NULL,
            NULL,
            NULL);

        SendMessageA(
            hFooter,
            WM_SETFONT,
            (WPARAM)hNormalFont,
            TRUE);

        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);

        Gdiplus::Graphics graphics(hdc);

        // Dark CipherX background
        graphics.Clear(Color(255, 8, 27, 52));

        if (backgroundImage)
        {
            // Make the CipherX artwork very faint
            Gdiplus::ColorMatrix colorMatrix =
                {
                    1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.12f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

            Gdiplus::ImageAttributes imageAttributes;

            imageAttributes.SetColorMatrix(
                &colorMatrix,
                Gdiplus::ColorMatrixFlagsDefault,
                Gdiplus::ColorAdjustTypeBitmap);

            // Keep the logo smaller instead of stretching it
            int imageWidth = 650;
            int imageHeight = 470;

            int x = (rect.right - imageWidth) / 2;
            int y = (rect.bottom - imageHeight) / 2;

            graphics.DrawImage(
                backgroundImage,
                Gdiplus::Rect(
                    x,
                    y,
                    imageWidth,
                    imageHeight),
                0,
                0,
                backgroundImage->GetWidth(),
                backgroundImage->GetHeight(),
                UnitPixel,
                &imageAttributes);
        }

        EndPaint(hwnd, &ps);

        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;

        SetBkMode(hdcStatic, TRANSPARENT);
        SetTextColor(hdcStatic, RGB(240, 245, 255));

        return (LRESULT)GetStockObject(NULL_BRUSH);
    }

    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT dis =
            (LPDRAWITEMSTRUCT)lParam;

        if (dis->CtlType == ODT_BUTTON)
        {
            int id = dis->CtlID;

            COLORREF backgroundColor;

            // CipherX button colors
            backgroundColor = RGB(245, 247, 250);

            // Darken button when pressed
            if (dis->itemState & ODS_SELECTED)
            {
                backgroundColor = RGB(
                    GetRValue(backgroundColor) * 0.80,
                    GetGValue(backgroundColor) * 0.80,
                    GetBValue(backgroundColor) * 0.80);
            }

            // Use the complete button area
            RECT rect = dis->rcItem;

            // Create button brush
            HBRUSH brush =
                CreateSolidBrush(backgroundColor);

            // Create subtle border
            HPEN borderPen =
                CreatePen(
                    PS_SOLID,
                    1,
                    RGB(190, 200, 215));

            // Select brush and pen
            HBRUSH oldBrush =
                (HBRUSH)SelectObject(
                    dis->hDC,
                    brush);

            HPEN oldPen =
                (HPEN)SelectObject(
                    dis->hDC,
                    borderPen);

            // Draw filled rounded button
            RoundRect(
                dis->hDC,
                rect.left,
                rect.top,
                rect.right,
                rect.bottom,
                16,
                16);

            // Restore previous objects
            SelectObject(
                dis->hDC,
                oldBrush);

            SelectObject(
                dis->hDC,
                oldPen);

            // Delete our objects
            DeleteObject(brush);
            DeleteObject(borderPen);
            // Button text
            char text[256] = {};

            GetWindowTextA(
                dis->hwndItem,
                text,
                sizeof(text));

            SetBkMode(
                dis->hDC,
                TRANSPARENT);

            SetTextColor(
                dis->hDC,
                RGB(25, 35, 50));

            SelectObject(
                dis->hDC,
                hButtonFont);

            DrawTextA(
                dis->hDC,
                text,
                -1,
                &rect,
                DT_CENTER |
                    DT_VCENTER |
                    DT_SINGLELINE);

            return TRUE;
        }

        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_ABOUT:
        {
            ShowAboutDialog(hwnd);
            break;
        }
        case ID_CREATE:
        {
            HINSTANCE hInstance =
                (HINSTANCE)GetWindowLongPtrA(
                    hwnd,
                    GWLP_HINSTANCE);

            OpenCreateFileDialog(
                hwnd,
                hInstance);

            break;
        }

        case ID_ENCRYPT:
        {
            HINSTANCE hInstance =
                (HINSTANCE)GetWindowLongPtrA(
                    hwnd,
                    GWLP_HINSTANCE);

            OpenEncryptFileDialog(
                hwnd,
                hInstance);

            break;
        }

        case ID_DECRYPT:
        {
            HINSTANCE hInstance =
                (HINSTANCE)GetWindowLongPtrA(
                    hwnd,
                    GWLP_HINSTANCE);

            OpenDecryptFileDialog(
                hwnd,
                hInstance);

            break;
        }

        case ID_DELETE:
        {
            char fileName[260] = {};

            OPENFILENAMEA ofn = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = sizeof(fileName);
            ofn.lpstrFilter =
                "All Files (*.*)\0*.*\0"
                "Encrypted Files (*.enc)\0*.enc\0"
                "Text Files (*.txt)\0*.txt\0";

            ofn.Flags =
                OFN_FILEMUSTEXIST |
                OFN_PATHMUSTEXIST;

            // Open file selection dialog
            if (GetOpenFileNameA(&ofn))
            {
                int result = MessageBoxA(
                    hwnd,
                    "Are you sure you want to delete this file?",
                    "CipherX - Confirm Delete",
                    MB_YESNO | MB_ICONWARNING);

                if (result == IDYES)
                {
                    if (DeleteFileA(fileName))
                    {
                        MessageBoxA(
                            hwnd,
                            "File deleted successfully!",
                            "CipherX",
                            MB_OK | MB_ICONINFORMATION);
                    }
                    else
                    {
                        MessageBoxA(
                            hwnd,
                            "Could not delete the file.\n"
                            "The file may be in use or you may not have permission.",
                            "CipherX",
                            MB_OK | MB_ICONERROR);
                    }
                }
            }

            break;
        }

        case ID_EXIT:
        {
            int result = MessageBoxA(
                hwnd,
                "Are you sure you want to exit?",
                "Confirmation",
                MB_YESNO | MB_ICONQUESTION);

            if (result == IDYES)
            {
                DestroyWindow(hwnd);
            }

            break;
        }
        }

        break;
    }

    case WM_DESTROY:
    {
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProcA(
            hwnd,
            uMsg,
            wParam,
            lParam);
    }

    return 0;
}

// ==============================
// WinMain
// ==============================
extern "C" int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    const char CLASS_NAME[] = "CipherXWindow";

    WNDCLASSA wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = hBackgroundBrush;

    RegisterClassA(&wc);
    CreateGUIFonts();

    GdiplusStartupInput gdiplusStartupInput;

    if (GdiplusStartup(
            &gdiplusToken,
            &gdiplusStartupInput,
            NULL) != Ok)
    {
        MessageBoxA(
            NULL,
            "Failed to initialize GDI+.",
            "CipherX",
            MB_OK | MB_ICONERROR);

        return 0;
    }

    backgroundImage =
        Image::FromFile(L"CipherX_background.png");

    if (!backgroundImage ||
        backgroundImage->GetLastStatus() != Ok)
    {
        MessageBoxA(
            NULL,
            "Could not load CipherX background image.",
            "CipherX",
            MB_OK | MB_ICONERROR);
    }

    int windowWidth = 800;
    int windowHeight = 580;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "CipherX - Secure File Encryption",
        WS_OVERLAPPED |
            WS_CAPTION |
            WS_SYSMENU |
            WS_MINIMIZEBOX,
        windowX,
        windowY,
        windowWidth,
        windowHeight,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hwnd == NULL)
    {
        MessageBoxA(
            NULL,
            "Failed to create CipherX window.",
            "Error",
            MB_OK | MB_ICONERROR);

        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};

    while (GetMessageA(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}