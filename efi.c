//=================================\\
// [ OWNER ]
//     CREATOR  : Vladislav Khudash
//     AGE      : 17
//     LOCATION : Ukraine
//
// [ PINFO ]
//     DATE     : 09.03.2026
//     PROJECT  : EFI-AUTH
//     PLATFORM : UEFI
//     C        : GNU-EFI 
//=================================\\




#include <efi.h>
#include <efilib.h>




static const UINT8 PASSWORD[32] = { (HERE TWISTER HASH PASSWORD) };


static const UINT8 MAX_LEN_PASSWORD = 254;


static const CHAR16 MSG[] = 
L"[ AUTH SYSTEM ]\r\n"
L"STATUS:PROTECTED\r\n\r\n"
L"Authentication required\r\n"
L"Enter password to proceed\r\n"
L"\r\n\r\n";




#define EFI_THEME_OK    EFI_BACKGROUND_BLACK | EFI_LIGHTGREEN
#define EFI_THEME_NO    EFI_BACKGROUND_BLACK | EFI_LIGHTRED


#define PRINT_MSG()              \
    do {                         \
        EFI_CLEAR_OUTPUT();      \
        Print(MSG);              \
        Print(L"PASSWORD: ");    \
    } while (0)


#define PRINT_DENIED()                      \
    do {                                    \
        EFI_CLEAR_OUTPUT();                 \
        Print(L"[-] ACCESS DENIED\r\n");    \
        EFI_SLEEP(3);                       \
    } while (0)


#define EFI_EXIT()                                        \
    do {                                                  \
        uefi_call_wrapper(                                \
            SystemTable->RuntimeServices->ResetSystem,    \
            4,                                            \
            EfiResetShutdown,                             \
            EFI_SUCCESS,                                  \
            0,                                            \
            NULL                                          \
        );                                                \
        while (1) EFI_SLEEP(1);                           \
    } while (0)


#define EFI_CLEAR_ARRAY(m)                    \
    do {                                      \
        ZeroMem((m), sizeof((m)));            \
        __asm__ volatile("" ::: "memory");    \
    } while (0)


#define EFI_SLEEP(s)                                                          \
    do {                                                                      \
        uefi_call_wrapper(SystemTable->BootServices->Stall, 1, (s << 20));    \
    } while (0)


#define EFI_SET_COLOR(n)                                                                      \
    do {                                                                                      \
        uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 2, SystemTable->ConOut, (n));    \
    } while (0)


#define EFI_CLEAR_OUTPUT()                                                              \
    do {                                                                                \
        uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);    \
    } while (0)


static inline void TwisterHash(const CHAR16 *in, UINT8 len, UINT8 out[32]) {
    UINT16 state[16];
    state[0] = 0x6c69;

    for (UINT8 i = 1; i < 16; i++) {
        state[i] = state[i - 1] * 0x9E37 + i;
    }

    for (UINT8 idx = 0; idx < len; idx++) {
        UINT16 c = in[idx];

        for (UINT8 j = 0; j < 16; j++) {
            UINT16 x = state[j] ^ c;
            x       ^= (((x >> 4) ^ (x << 3)) ^ (x << 7)) ^ (x >> 8);
            state[j] = ((x << j) | (x >> (16 - j))) ^ state[(j + 7) & 15];
        }

        for (UINT8 j = 0; j < 16; j += 4) {
            UINT16 *s = &state[j];
            UINT16 t  = (s[0] ^ s[1]) + (s[2] * s[3]);
            s[0]      = t;
            s[3]      = (s[3] << 5) | (s[3] >> 11);
        }
    }

    for (UINT8 i = 0; i < 8; i++) {
        UINT32 h       = ((UINT32)state[i] << 16) | state[i + 8];
        h              = (h << 5) | (h >> 27);

        out[i * 4]     = (UINT8)(h >> 24);
        out[i * 4 + 1] = (UINT8)(h >> 16);
        out[i * 4 + 2] = (UINT8)(h >> 8);
        out[i * 4 + 3] = (UINT8)h;
    }
}


static inline BOOLEAN TwisterEqual(const UINT8 a[32], const UINT8 b[32]) {
    volatile UINT8 r = 0;

    for (UINT8 i = 0; i < 32; i++) {
        r |= a[i] ^ b[i];
    }

    return r == 0;
}


static inline UINT8 ReadInput(EFI_SYSTEM_TABLE *SystemTable, CHAR16 *buf) {
    EFI_INPUT_KEY key;
    UINT8 pos = 0;

    while (pos < MAX_LEN_PASSWORD) {
        UINT8 idx;

        uefi_call_wrapper(
            SystemTable->BootServices->WaitForEvent, 
            3,
            1, 
            &SystemTable->ConIn->WaitForKey, 
            &idx
        );
        uefi_call_wrapper(
            SystemTable->ConIn->ReadKeyStroke, 
            2,
            SystemTable->ConIn, 
            &key
        );

        if (key.UnicodeChar == CHAR_CARRIAGE_RETURN){
            break;
        }
        else if ((key.UnicodeChar == CHAR_BACKSPACE) && (pos > 0)) {
            pos--;
            Print(L"\b \b");
            continue;
        }
        else if ((key.UnicodeChar >= 0x20) && (key.UnicodeChar <= 0x7E)) {
            buf[pos++] = key.UnicodeChar;
        }
    }
    
    buf[pos] = 0;
    Print(L"\r\n");

    return pos;
}


static inline void Restore(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_LOADED_IMAGE *LoadedImage;
    EFI_FILE_IO_INTERFACE *FileSystem;
    EFI_FILE_HANDLE Root, File;
    EFI_STATUS Status;

    EFI_SET_COLOR(EFI_THEME_OK);
    EFI_CLEAR_OUTPUT();

    Print(L"[+] ACCESS GRANTED\r\n");
    EFI_SLEEP(3);

    Status = uefi_call_wrapper(
        SystemTable->BootServices->HandleProtocol,
        3,
        ImageHandle,
        &LoadedImageProtocol,
        (VOID **)&LoadedImage
    );

    if (EFI_ERROR(Status)) {
        EFI_EXIT();
    }

    Status = uefi_call_wrapper(
        SystemTable->BootServices->HandleProtocol,
        3,
        LoadedImage->DeviceHandle,
        &FileSystemProtocol,
        (VOID **)&FileSystem
    );

    if (!EFI_ERROR(Status)) {
        Status = uefi_call_wrapper(FileSystem->OpenVolume, 2, FileSystem, &Root);

        if (!EFI_ERROR(Status)) {
            CHAR16 *FilePathStr = DevicePathToStr(LoadedImage->FilePath);

            if (FilePathStr) {
                Status = uefi_call_wrapper(
                    Root->Open,
                    5,
                    Root,
                    &File,
                    FilePathStr,
                    EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                    0
                );

                if (!EFI_ERROR(Status)) {
                    uefi_call_wrapper(File->Delete, 1, File);
                }

                FreePool(FilePathStr);
            }

            uefi_call_wrapper(Root->Close, 1, Root);
        }
    }

    EFI_EXIT();
}


EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    SystemTable->BootServices->SetWatchdogTimer(0, 0x0000, 0, NULL);

    EFI_SET_COLOR(EFI_THEME_NO);
    uefi_call_wrapper(SystemTable->ConOut->EnableCursor, 2, SystemTable->ConOut, FALSE);

    
    while (1) { 
        PRINT_MSG();

        UINT8 password[32];
        CHAR16 tmp[MAX_LEN_PASSWORD + 1];

        EFI_CLEAR_ARRAY(password);
        EFI_CLEAR_ARRAY(tmp);

        UINT8 inlen = ReadInput(SystemTable, tmp);

        if (inlen == 0) {
            EFI_CLEAR_ARRAY(tmp);
            PRINT_DENIED();
            continue;
        }

        TwisterHash(tmp, inlen, password);
        EFI_CLEAR_ARRAY(tmp);

        if (TwisterEqual(password, PASSWORD)) {
            EFI_CLEAR_ARRAY(password);
            Restore(ImageHandle, SystemTable); 
        }
        else {
            EFI_CLEAR_ARRAY(password);
            PRINT_DENIED();
        }
    }


    EFI_EXIT();
    return EFI_SUCCESS; 
}
