# 🔐 bootauth


<div align="center">

[![Platform](https://img.shields.io/badge/platform-BIOS%20%7C%20UEFI-blue?logo=data%3Aimage%2Fsvg%2Bxml%3Bbase64%2CPHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCI%2BPHBhdGggZmlsbD0id2hpdGUiIGQ9Ik0xMiAyQzYuNDggMiAyIDYuNDggMiAxMnM0LjQ4IDEwIDEwIDEwIDEwLTQuNDggMTAtMTBTMTcuNTIgMiAxMiAyem0wIDE4Yy00LjQxIDAtOC0zLjU5LTgtOHMzLjU5LTggOC04IDggMy41OSA4IDgtMy41OSA4LTggOHoiLz48L3N2Zz4%3D)](https://en.wikipedia.org/wiki/BIOS)
[![Language](https://img.shields.io/badge/language-C%20%7C%20Assembly-00599C)](https://en.wikipedia.org/wiki/Assembly_language)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)

*Pre-Boot Authentication System for BIOS and UEFI*

</div>

---

## ⚠️ Legal Disclaimer

**This software is intended for educational purposes and personal device protection only.**

- ✅ **Allowed use**: Protecting your own devices with pre-boot authentication, educational research
- ❌ **Prohibited use**: Locking devices without authorization, any illegal activities

**The author assumes no responsibility for misuse of this software.**

---

## 📖 Table of Contents | Оглавление

- [English](#english)
  - [📋 Overview](#-overview)
  - [✨ Features](#-features)
  - [🏗️ Architecture](#️-architecture)
  - [🔐 Authentication Flow](#-authentication-flow)
  - [🚀 Quick Start](#-quick-start)
  - [📁 Project Structure](#-project-structure)
  - [🔑 Password Generation](#-password-generation)
  - [⚙️ Technical Details](#️-technical-details)
  - [⚠️ Warnings](#️-warnings)

- [Русский](#русский)
  - [📋 Обзор](#-обзор)
  - [✨ Возможности](#-возможности)
  - [🏗️ Архитектура](#️-архитектура)
  - [🔐 Процесс аутентификации](#-процесс-аутентификации)
  - [🚀 Быстрый старт](#-быстрый-старт)
  - [📁 Структура проекта](#-структура-проекта)
  - [🔑 Генерация паролей](#-генерация-паролей)
  - [⚙️ Технические детали](#️-технические-детали)
  - [⚠️ Предупреждения](#️-предупреждения)

---

# English

## 📋 Overview

**BootAuth** is a **pre-boot authentication system** that protects your computer before the operating system loads. It supports both legacy **BIOS (MBR)** and modern **UEFI** systems.

### What it does

| Component | Platform | Description |
|-----------|----------|-------------|
| **MBR Bootloader** | BIOS | 512-byte MBR sector with password protection |
| **UEFI Application** | UEFI | EFI executable with cryptographic hash verification |
| **Password Generator** | Any | Python script to generate BIOS/UEFI passwords |

### How it works

```
┌─────────────────────────────────────────────────────────────────┐
│                      POWER ON / RESET                           │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                    BIOS / UEFI FIRMWARE                         │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                    BootAuth Loads                               │
│              (MBR sector or EFI application)                    │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                 PASSWORD PROMPT                                 │
│            ┌─────────────────────────────┐                      │
│            │ [ AUTH SYSTEM ]             │                      │
│            │ STATUS:PROTECTED            │                      │
│            │                             │                      │
│            │ Authentication required     │                      │
│            │ Enter password to proceed   │                      │
│            │                             │                      │
│            │ PASSWORD: _                 │                      │
│            └─────────────────────────────┘                      │
└─────────────────────────────────────────────────────────────────┘
                              ↓
                    ┌─────────┴─────────┐
                    ↓                   ↓
            ┌───────────────┐   ┌───────────────┐
            │ ACCESS GRANTED │   │ ACCESS DENIED │
            └───────────────┘   └───────────────┘
                    ↓                   ↓
            ┌───────────────┐   ┌───────────────┐
            │ Self-Destruct │   │   Retry Loop  │
            │ Boot Original │   │   (3 attempts)│
            │     OS        │   └───────────────┘
            └───────────────┘
```

## ✨ Features

### MBR (BIOS) Version

| Feature | Description |
|---------|-------------|
| 🔐 **XOR Password** | Simple XOR-based password obfuscation |
| 📟 **Real Mode** | 16-bit real mode assembly |
| 🎨 **Color Theme** | Red (denied) / Green (granted) |
| 📏 **512 Bytes** | Fits in single MBR sector |
| 🔄 **Self-Destruct** | Overwrites MBR on successful auth |
| ⌨️ **Backspace Support** | Full password editing |

### UEFI Version

| Feature | Description |
|---------|-------------|
| 🔐 **TwisterHash** | Custom 256-bit cryptographic hash |
| 📟 **GNU-EFI** | UEFI application framework |
| 🎨 **Color Theme** | Red (denied) / Green (granted) |
| 🔄 **Self-Destruct** | Deletes EFI file on successful auth |
| ⌨️ **Backspace Support** | Full password editing |
| ⏱️ **Watchdog Disable** | Prevents automatic reboot |

### Password Generator

| Feature | Description |
|---------|-------------|
| 🔑 **BIOS Mode** | Generates XOR-obfuscated password array |
| 🔐 **UEFI Mode** | Generates TwisterHash 32-byte array |
| 📋 **Copy-Paste Ready** | Outputs C/ASM compatible array format |

## 🏗️ Architecture

### MBR Bootloader Flow

```
┌─────────────────────────────────────────────────────────────────┐
│ 1. Disable interrupts (cli)                                     │
│ 2. Setup segments (DS=ES=SS=0, SP=0x7C00)                       │
│ 3. Clear screen (INT 0x10, AH=0x00, AL=0x03)                    │
│ 4. Set color theme (red)                                        │
│ 5. Display authentication message                               │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ 6. Read password (INT 0x16)                                     │
│    • Support Backspace (0x08)                                   │
│    • Max length: 30 chars                                       │
│    • Enter (0x0D) to submit                                     │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ 7. XOR verify password                                          │
│    • Each char XORed with 0x3F                                  │
│    • Compare with stored hash                                   │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ 8. IF MATCH: Self-destruct                                      │
│    • Overwrite MBR with zeros (INT 0x13, AH=0x03)               │
│    • Display "ACCESS GRANTED"                                   │
│    • Halt system                                                │
│                                                                 │
│ 9. IF NO MATCH: Retry                                           │
│    • Display "ACCESS DENIED"                                    │
│    • Wait 3 seconds                                             │
│    • Return to step 3                                           │
└─────────────────────────────────────────────────────────────────┘
```

### UEFI Application Flow

```
┌─────────────────────────────────────────────────────────────────┐
│ 1. Initialize GNU-EFI library                                   │
│ 2. Disable watchdog timer                                       │
│ 3. Disable cursor                                               │
│ 4. Set color theme (red)                                        │
│ 5. Display authentication message                               │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ 6. Read password (EFI_SIMPLE_TEXT_INPUT_PROTOCOL)               │
│    • Support Backspace                                          │
│    • Max length: 254 chars                                      │
│    • Enter to submit                                            │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ 7. TwisterHash password                                         │
│    • 256-bit output                                             │
│    • Compare with stored hash                                   │
│    • Constant-time comparison (XOR all bytes)                   │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ 8. IF MATCH: Self-destruct                                      │
│    • Get EFI file path via DevicePath                           │
│    • Delete EFI file                                            │
│    • Display "ACCESS GRANTED"                                   │
│    • Shutdown system                                            │
│                                                                 │
│ 9. IF NO MATCH: Retry                                           │
│    • Display "ACCESS DENIED"                                    │
│    • Wait 3 seconds                                             │
│    • Return to step 5                                           │
└─────────────────────────────────────────────────────────────────┘
```

## 🔐 Authentication Flow

### MBR (BIOS) - XOR Verification

```asm
.pass_equal:
    lodsb                   ; Load next char from input
    mov bl, al              ; Save original
    xor al, XOR_KEY         ; XOR with 0x3F
    cmp al, [di]            ; Compare with stored hash
    jne .PRINT_DENIED_pre   ; Mismatch → deny
    test bl, bl             ; Check for null terminator
    jz .ACCESS_GRANTED      ; Match complete → grant
    inc di                  ; Next stored byte
    jmp .pass_equal         ; Continue
```

### UEFI - TwisterHash Verification

```c
// Constant-time comparison (prevents timing attacks)
static inline BOOLEAN TwisterEqual(const UINT8 a[32], const UINT8 b[32]) {
    volatile UINT8 r = 0;
    for (UINT8 i = 0; i < 32; i++) {
        r |= a[i] ^ b[i];
    }
    return r == 0;
}
```

### TwisterHash Algorithm

```python
# Custom cryptographic hash
state[0] = 0x6C69

for i in range(1, 16):
    state[i] = ((state[i-1] * 0x9E37) + i) & 0xFFFF

for char in password:
    for j in range(16):
        x = (state[j] ^ char) & 0xFFFF
        x ^= (((x >> 4) ^ (x << 3)) ^ (x << 7)) ^ (x >> 8)
        state[j] = ((x << j) | (x >> (16-j))) ^ state[(j+7) & 15]
    
    # Mix columns
    for j in range(0, 16, 4):
        t = (state[j] ^ state[j+1]) + (state[j+2] * state[j+3])
        state[j] = t & 0xFFFF
        state[j+3] = ((state[j+3] << 5) | (state[j+3] >> 11)) & 0xFFFF

# Finalize to 32 bytes
for i in range(8):
    h = ((state[i] << 16) | state[i+8]) & 0xFFFFFFFF
    h = ((h << 5) | (h >> 27)) & 0xFFFFFFFF
    out[i*4 : i*4+4] = h.to_bytes(4, 'big')
```

## 🚀 Quick Start

### 📋 Prerequisites

| Component | Requirements |
|-----------|-------------|
| **MBR** | NASM, QEMU (for testing), DD (for flashing) |
| **UEFI** | GNU-EFI, GCC, QEMU+OVMF (for testing) |
| **Password Generator** | Python 3.x |

### 🔨 Building MBR (BIOS)

```bash
# Assemble
nasm -f bin mbr.asm -o mbr.bin

# Test in QEMU
qemu-system-i386 -drive file=mbr.bin,format=raw

# Flash to disk (DANGER!)
sudo dd if=mbr.bin of=/dev/sdX bs=512 count=1
```

### 🔨 Building UEFI

```bash
# Install GNU-EFI
sudo apt install gnu-efi

# Compile
gcc -I/usr/include/efi -I/usr/include/efi/x86_64 \
    -fno-stack-protector -fpic -fshort-wchar -mno-red-zone \
    -c efi.c -o efi.o

ld -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic \
    -L/usr/lib -L/usr/lib/gnuefi \
    /usr/lib/crt0-efi-x86_64.o \
    efi.o -o efi.so -lefi -lgnuefi

objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym \
    -j .rel -j .rela -j .reloc -j .relaoc \
    --target=efi-app-x86_64 efi.so efi.efi

# Test in QEMU with OVMF
qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd \
    -drive file=fat:rw:./,format=raw
```

### 🔑 Generate Password

```bash
# For BIOS (MBR)
python3 genpass.py BIOS "MySecretPassword"

# Output:
# BIOS PASSWORD: (  0x2C, 0x3E, 0x2C, 0x3A, 0x2B, 0x3E, 0x2F, ...  )

# For UEFI
python3 genpass.py UEFI "MySecretPassword"

# Output:
# UEFI PASSWORD: (  0xA1, 0x3F, 0x7B, 0xE2, 0x11, 0x44, 0x89, 0xCC, ...  )
```

### 📝 Embed Password

**MBR (`mbr.asm`):**
```asm
%define PASSWORD 0x2C, 0x3E, 0x2C, 0x3A, 0x2B, 0x3E, 0x2F, ...
```

**UEFI (`efi.c`):**
```c
static const UINT8 PASSWORD[32] = { 0xA1, 0x3F, 0x7B, 0xE2, 0x11, ... };
```

## 📁 Project Structure

```
bootauth/
├── mbr.asm              # MBR bootloader (BIOS)
├── efi.c                # UEFI application
├── genpass.py           # Password generator
├── Makefile             # Build automation
└── README.md            # Documentation
```

## 🔑 Password Generation

### BIOS Mode (XOR)

```python
XOR_KEY = 0x3F

def xor_password(password):
    return [ord(c) ^ XOR_KEY for c in password]
```

**Example:**
- Password: `"test"`
- ASCII: `[0x74, 0x65, 0x73, 0x74]`
- XOR 0x3F: `[0x4B, 0x5A, 0x4C, 0x4B]`
- Output: `0x4B, 0x5A, 0x4C, 0x4B`

### UEFI Mode (TwisterHash)

```python
def twister_hash(password):
    # 256-bit cryptographic hash
    # Returns 32-byte array
```

**Example:**
- Password: `"test"`
- Hash: 32 bytes
- Output: `0xA1, 0x3F, 0x7B, 0xE2, ...`

## ⚙️ Technical Details

### MBR Memory Layout

```
0x0000:0x7C00 ────────────────────────────────┐
                                              │
  ┌───────────────────────────────────────────┘
  │  Code Section
  │  (authentication logic)
  │
  ├── Data Section
  │   • MSG          (authentication banner)
  │   • prompt       ("PASSWORD: ")
  │   • msg_denied   ("ACCESS DENIED")
  │   • msg_granted  ("ACCESS GRANTED")
  │   • xp           (stored XOR password)
  │   • buffer       (30+1 bytes input)
  │
  ├── Boot Signature
  │   0x55 0xAA at offset 510
  │
0x0000:0x7E00 ─────────────────────────────────
```

### UEFI Self-Destruct Mechanism

```c
// Get EFI file path
CHAR16 *FilePathStr = DevicePathToStr(LoadedImage->FilePath);

// Open file for read/write
Status = Root->Open(Root, &File, FilePathStr,
                    EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);

// Delete file
if (!EFI_ERROR(Status)) {
    uefi_call_wrapper(File->Delete, 1, File);
}
```

### MBR Self-Destruct Mechanism

```asm
; Overwrite MBR with zeros
mov ax, 0x0301      ; Write 1 sector
mov cx, 0x0001      ; Cylinder 0, Sector 1
mov dx, 0x0080      ; First hard disk
mov bx, 0x0500      ; Buffer with zeros
int 0x13            ; Disk interrupt
```

## ⚠️ Warnings

### ⚡ Critical Warnings

| Warning | Description |
|---------|-------------|
| **Data Loss** | Failed authentication destroys bootloader |
| **No Recovery** | Once flashed, only hardware reset can remove |
| **BIOS/UEFI Brick** | Incorrect flashing may brick motherboard |
| **Test First** | Always test in QEMU before real hardware |
| **Backup MBR** | `dd if=/dev/sdX of=mbr_backup.bin bs=512 count=1` |

### 🔧 Recovery Options

**If locked out:**

| System | Recovery Method |
|--------|-----------------|
| **BIOS** | Boot from USB, run `dd if=/dev/zero of=/dev/sdX bs=512 count=1` |
| **UEFI** | Enter UEFI shell, delete EFI file, or reinstall OS |
| **Both** | Remove drive, connect to another PC, wipe boot sector |

---

# Русский

## 📋 Обзор

**BootAuth** — это **система предзагрузочной аутентификации**, которая защищает компьютер до загрузки операционной системы. Поддерживает как устаревшие **BIOS (MBR)**, так и современные **UEFI** системы.

### Что делает

| Компонент | Платформа | Описание |
|-----------|-----------|----------|
| **MBR загрузчик** | BIOS | 512-байтный MBR сектор с защитой паролем |
| **UEFI приложение** | UEFI | EFI исполняемый файл с криптографической верификацией |
| **Генератор паролей** | Любая | Python скрипт для генерации паролей BIOS/UEFI |

## ✨ Возможности

### MBR (BIOS) версия

| Возможность | Описание |
|-------------|----------|
| 🔐 **XOR пароль** | Простая XOR-обфускация пароля |
| 📟 **Реальный режим** | 16-битный ассемблер |
| 🎨 **Цветовая тема** | Красный (отказ) / Зелёный (доступ) |
| 📏 **512 байт** | Помещается в один MBR сектор |
| 🔄 **Самоуничтожение** | Перезаписывает MBR при успешной аутентификации |
| ⌨️ **Поддержка Backspace** | Полное редактирование пароля |

### UEFI версия

| Возможность | Описание |
|-------------|----------|
| 🔐 **TwisterHash** | Собственный 256-битный криптографический хеш |
| 📟 **GNU-EFI** | Фреймворк для UEFI приложений |
| 🎨 **Цветовая тема** | Красный (отказ) / Зелёный (доступ) |
| 🔄 **Самоуничтожение** | Удаляет EFI файл при успешной аутентификации |
| ⏱️ **Отключение Watchdog** | Предотвращает автоматическую перезагрузку |

## 🏗️ Архитектура

*(См. английскую версию для диаграмм)*

## 🔐 Процесс аутентификации

### MBR (BIOS) - XOR верификация

```asm
.pass_equal:
    lodsb                   ; Загрузить следующий символ
    mov bl, al              ; Сохранить оригинал
    xor al, XOR_KEY         ; XOR с 0x3F
    cmp al, [di]            ; Сравнить с сохранённым хешем
    jne .PRINT_DENIED_pre   ; Несовпадение → отказ
    test bl, bl             ; Проверка на конец строки
    jz .ACCESS_GRANTED      ; Полное совпадение → доступ
```

### UEFI - TwisterHash верификация

```c
// Сравнение за константное время (защита от timing-атак)
static inline BOOLEAN TwisterEqual(const UINT8 a[32], const UINT8 b[32]) {
    volatile UINT8 r = 0;
    for (UINT8 i = 0; i < 32; i++) {
        r |= a[i] ^ b[i];
    }
    return r == 0;
}
```

## 🚀 Быстрый старт

### 📋 Требования

| Компонент | Требования |
|-----------|------------|
| **MBR** | NASM, QEMU (для тестов), DD (для прошивки) |
| **UEFI** | GNU-EFI, GCC, QEMU+OVMF (для тестов) |
| **Генератор паролей** | Python 3.x |

### 🔨 Сборка MBR (BIOS)

```bash
# Ассемблирование
nasm -f bin mbr.asm -o mbr.bin

# Тест в QEMU
qemu-system-i386 -drive file=mbr.bin,format=raw

# Прошивка на диск (ОПАСНО!)
sudo dd if=mbr.bin of=/dev/sdX bs=512 count=1
```

### 🔨 Сборка UEFI

```bash
# Установка GNU-EFI
sudo apt install gnu-efi

# Компиляция
gcc -I/usr/include/efi -I/usr/include/efi/x86_64 \
    -fno-stack-protector -fpic -fshort-wchar -mno-red-zone \
    -c efi.c -o efi.o

ld -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic \
    -L/usr/lib -L/usr/lib/gnuefi \
    /usr/lib/crt0-efi-x86_64.o \
    efi.o -o efi.so -lefi -lgnuefi

objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym \
    -j .rel -j .rela -j .reloc -j .relaoc \
    --target=efi-app-x86_64 efi.so efi.efi
```

### 🔑 Генерация пароля

```bash
# Для BIOS (MBR)
python3 genpass.py BIOS "МойСекретныйПароль"

# Для UEFI
python3 genpass.py UEFI "МойСекретныйПароль"
```

### 📝 Встраивание пароля

**MBR (`mbr.asm`):**
```asm
%define PASSWORD 0x2C, 0x3E, 0x2C, 0x3A, ...
```

**UEFI (`efi.c`):**
```c
static const UINT8 PASSWORD[32] = { 0xA1, 0x3F, 0x7B, 0xE2, ... };
```

## ⚠️ Предупреждения

### ⚡ Критические предупреждения

| Предупреждение | Описание |
|----------------|----------|
| **Потеря данных** | Неудачная аутентификация уничтожает загрузчик |
| **Нет восстановления** | После прошивки только сброс оборудования может удалить |
| **Окирпичивание BIOS/UEFI** | Неправильная прошивка может сломать материнскую плату |
| **Тестируй сначала** | Всегда тестируй в QEMU перед реальным железом |
| **Бэкап MBR** | `dd if=/dev/sdX of=mbr_backup.bin bs=512 count=1` |

### 🔧 Способы восстановления

**Если заблокирован:**

| Система | Способ восстановления |
|---------|----------------------|
| **BIOS** | Загрузиться с USB, выполнить `dd if=/dev/zero of=/dev/sdX bs=512 count=1` |
| **UEFI** | Войти в UEFI Shell, удалить EFI файл, или переустановить ОС |
| **Обе** | Извлечь диск, подключить к другому ПК, стереть загрузочный сектор |

---

<div align="center">

**[⬆ Back to Top](#-bootauth)**

*Pre-Boot Authentication — Protect Your System Before It Boots*

</div>
