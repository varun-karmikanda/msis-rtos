# STM32F103 Development and Flashing from WSL

This document describes the complete setup used to build and flash the
STM32F103 projects in this directory from Ubuntu on WSL2. The toolchain runs
inside WSL. The ST-Link USB debug probe is connected to WSL from Windows with
`usbipd-win`.

## What you need

- Windows 10 or Windows 11 with WSL2 enabled.
- An Ubuntu WSL distribution.
- An STM32F103 board with an ST-Link programmer/debugger.
- A USB data cable. Charge-only cables will not work.
- The repository available inside WSL, for example under `/mnt/c/...`.

The board must be powered while it is being programmed. Connect the board to
the computer before running the USB commands below.

## 1. Install or update WSL

Run these commands in **Windows PowerShell as Administrator** if WSL is not
already installed:

```powershell
wsl --install
```

Restart Windows if prompted. After the restart, open Ubuntu from the Start
menu and create the Linux username and password. Updating an existing WSL
installation is also useful before using USB devices:

```powershell
wsl --update
wsl --shutdown
```

Start Ubuntu again after `wsl --shutdown`.

Check that the distribution is using WSL2:

```powershell
wsl --list --verbose
```

The `VERSION` column should show `2`. Convert a distribution to WSL2 if
necessary, replacing `Ubuntu` with the name shown by the previous command:

```powershell
wsl --set-version Ubuntu 2
```

## 2. Install the ARM toolchain in WSL

Run the following commands in the **Ubuntu/WSL terminal**, not in PowerShell:

```bash
sudo apt update
sudo apt install -y gcc-arm-none-eabi binutils-arm-none-eabi gdb-multiarch build-essential
```

The packages provide:

- `gcc-arm-none-eabi`: ARM Cortex-M C compiler and linker.
- `binutils-arm-none-eabi`: ARM assembler, `objcopy`, and related tools.
- `gdb-multiarch`: debugger capable of debugging ARM targets.
- `build-essential`: `make` and the normal Linux build utilities.

Install the USB listing utility as well. It is used to confirm that the ST-Link
has appeared inside WSL:

```bash
sudo apt install -y usbutils
```

Verify the installation:

```bash
which arm-none-eabi-gcc
arm-none-eabi-gcc --version
which make
make --version
which gdb-multiarch
```

## 3. Install ST-Link tools in WSL

Still in the Ubuntu/WSL terminal, install the ST-Link command-line tools:

```bash
sudo apt update
sudo apt install -y stlink-tools
```

Verify that `st-flash` is installed and show its version:

```bash
which st-flash
st-flash --version
```

Expected output includes a path similar to `/usr/bin/st-flash` and an ST-Link
version such as `v1.8.0`.

## 4. Install USBIPD-WIN in Windows

`usbipd-win` shares a Windows USB device with WSL2. Install it once in an
**Administrator PowerShell** window.

Using Chocolatey:

```powershell
choco install usbipd -y
```

The package name is `usbipd`; do not concatenate it with the command. If
Chocolatey is not installed, use the official Windows package manager instead:

```powershell
winget install --exact --interactive dorssel.usbipd-win
```

Close and reopen PowerShell after installation if the `usbipd` command is not
found. Confirm the installation:

```powershell
usbipd --version
```

## 5. Find the ST-Link bus ID in Windows

Plug in the board, then run this command in PowerShell:

```powershell
usbipd list
```

Find the row corresponding to the ST-Link. It may be identified as `ST-Link`,
`STMicroelectronics`, or a USB debug/programming device. Copy its `BUSID`, for
example `1-4`.

The bus ID is assigned by Windows and is not guaranteed to remain `1-4`. Always
use the value currently shown by `usbipd list`.

## 6. Bind and attach the ST-Link to WSL

Run these commands in **Administrator PowerShell**, replacing `1-4` with the
actual bus ID:

```powershell
usbipd bind --busid 1-4
usbipd attach --wsl --busid 1-4
```

What the commands do:

- `usbipd bind` makes the Windows USB device available for USB/IP sharing.
- `usbipd attach --wsl` connects that device to the currently running WSL2
	distribution.

Check the connection from PowerShell:

```powershell
usbipd list
```

The device should show a state such as `Attached`.

If the command reports that the device is already shared or attached, do not
bind it again unnecessarily. Check the current state with `usbipd list` and
continue with the WSL verification step.

## 7. Verify the ST-Link inside WSL

Run this command in Ubuntu/WSL:

```bash
lsusb
```

The output should contain an ST-Link or STMicroelectronics device. Seeing only
the Linux root hub entries means the ST-Link has not been attached to WSL yet.

You can also ask `st-info` to identify the probe:

```bash
st-info --probe
```

A successful result reports ST-Link information and the connected target. If
`st-info` cannot find a probe, repeat the Windows attach step and check the
troubleshooting section below.

## 8. Build a project

From the WSL terminal, change to one of the project directories:

```bash
cd /mnt/c/Users/<WindowsUser>/<path-to-repository>/lab04/stm32f103/led-blink
```

For a repository stored in the Linux home directory, use its Linux path
instead, for example:

```bash
cd ~/rtos/lab04/stm32f103/led-blink
```

Build the project and inspect its size:

```bash
make clean
make
make size
```

The build creates files under `build/`, including:

- `build/led-blink.elf`: debugger-friendly ELF file.
- `build/led-blink.bin`: raw binary sent to the microcontroller.
- `build/led-blink.hex`: Intel HEX representation.

Build the FreeRTOS example similarly:

```bash
cd ../three-thread-blink
make
```

The FreeRTOS source must first exist at
`lab04/stm32f103/FreeRTOS`. From the `stm32f103` directory, clone it once:

```bash
cd /mnt/c/Users/<WindowsUser>/<path-to-repository>/lab04/stm32f103
git clone --depth 1 https://github.com/FreeRTOS/FreeRTOS-Kernel.git FreeRTOS
```

## 9. Flash the board

Make sure the ST-Link is still attached to WSL, then from the project
directory run:

```bash
make flash
```

This executes the project Makefile rule:

```bash
st-flash write build/led-blink.bin 0x08000000
```

For a different project, use that project's generated binary. For example:

```bash
cd ../three-thread-blink
make flash
```

After a successful write, reset the board if it does not start running the new
firmware automatically. The STM32F103 flash base address is `0x08000000`.

## 10. Do these settings need to be repeated?

### One-time setup

These normally need to be done only once per Windows/WSL installation:

- Installing WSL2 and the Ubuntu distribution.
- Installing the WSL packages (`gcc-arm-none-eabi`, `make`, `gdb-multiarch`,
	and `stlink-tools`).
- Installing `usbipd-win` in Windows.
- Cloning the FreeRTOS repository.

### Usually required after reconnecting or rebooting

USB attachment is a session-level operation. After unplugging and reconnecting
the board, restarting Windows, restarting WSL, or changing the USB port, run:

```powershell
usbipd list
usbipd bind --busid <current-busid>
usbipd attach --wsl --busid <current-busid>
```

Then verify again in WSL:

```bash
lsusb
st-info --probe
```

The `bind` operation is generally retained while the device remains connected,
but `attach --wsl` should be treated as temporary. In practice, checking
`usbipd list` and attaching again is the reliable workflow after a reboot or
reconnect. The bus ID can also change, so do not hard-code it permanently.

## Troubleshooting

### `usbipd` is not recognized

Reopen Administrator PowerShell after installing the package. If it is still
not found, verify the installation and PATH:

```powershell
Get-Command usbipd
usbipd --version
```

### The ST-Link does not appear in `usbipd list`

Check that the board is powered, reconnect the USB cable, try another data
cable or USB port, and run:

```powershell
usbipd list
```

Close programs that may already be using the ST-Link, such as STM32CubeProgrammer
or another debugger. Windows must be able to see the device before it can be
attached to WSL.

### `lsusb` shows only Linux root hubs

The device is still owned by Windows or has not been attached to WSL. From
Administrator PowerShell, use the current bus ID:

```powershell
usbipd list
usbipd attach --wsl --busid <current-busid>
```

Then run `lsusb` again in WSL.

### `st-info --probe` or `st-flash` cannot find the probe

Confirm all of the following:

```bash
which st-flash
st-flash --version
lsusb
st-info --probe
```

If `lsusb` does not show the ST-Link, fix the USBIPD attachment first. If it
does show the ST-Link, disconnect and reattach it, close other ST-Link software,
and retry the command.

### `make flash` reports that the binary is missing

Build before flashing and check the generated file:

```bash
make
ls -l build/*.bin
make flash
```

### Permission errors from `usbipd`

Run `usbipd bind` and `usbipd attach` from Administrator PowerShell. Linux
commands such as `make` and `st-flash` should be run from WSL; do not prefix
them with Windows `sudo` or run them in PowerShell.

## Quick repeatable workflow

After the one-time setup, the normal daily sequence is:

**Windows Administrator PowerShell:**

```powershell
usbipd list
usbipd bind --busid <current-busid>
usbipd attach --wsl --busid <current-busid>
```

**Ubuntu/WSL:**

```bash
cd /mnt/c/Users/<WindowsUser>/<path-to-repository>/lab04/stm32f103/led-blink
lsusb
st-info --probe
make
make flash
```
