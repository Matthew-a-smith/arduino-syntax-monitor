# Arduino-Powered Syntax Checker for Nano and Terminal-Based Editors

Many lightweight terminal-based text editors on Linux distributions, such as Nano, do not provide built-in syntax checking or real-time error detection. 
When working directly from the terminal, common mistakes such as missing brackets, incorrect syntax, or other code errors may not be discovered until the file is checked or the program is run.

My project is an Arduino-powered syntax monitoring system designed to work alongside terminal-based editors. 
A Bash script runs in the background and monitors the active editor processes to identify the files currently being edited. 
It then performs language-specific syntax checks on those files and sends the results to an Arduino through a USB serial connection.

The Arduino acts as the physical output interface for the system. 
It provides immediate visual feedback using three LEDs and a 16x2 LCD display. 
The green LED indicates that the current file has passed its syntax check, while the red LED indicates that errors have been detected. 
The yellow LED indicates that the editor is closed. When errors are detected, the LCD displays the error information so that the user can review the problems without having to leave their terminal editor.

The system currently supports syntax checking for C++ and Python files. 
C++ files are checked using g++ -fsyntax-only, while Python files are checked using python -m py_compile. These commands check the source code for syntax errors without compiling the program into an executable.

# watch.sh — Setup Instructions

## Step 01 — Locate the Arduino USB Serial Port

First, find the USB serial connection that your Arduino is using:

```bash
dmesg | grep tty
```

Look for the Arduino's serial device, such as:

```text
/dev/ttyUSB0
```

Once you have found the correct device, update `setup.conf` with the address:

```text
com=/dev/ttyUSB0
```

---

## Step 02 — Give the Serial Device Permission

Give the serial device read/write permissions so the script can communicate with the Arduino:

```bash
sudo chmod 666 /dev/ttyUSB0
```

Replace `/dev/ttyUSB0` with the device you found in Step 01.

---

## Step 03 — Run `watch.sh` in the Background

Start the script in the background:

```bash
bash watch.sh &
```

The `&` allows the script to continue running in the background.

To find the process later:

```bash
ps aux | grep watch.sh
```

or:

```bash
pgrep -af watch.sh
```

To stop it:

```bash
kill <PID>
```

Replace `<PID>` with the process ID shown by `pgrep`.

### Quick Setup

```bash
dmesg | grep tty
sudo chmod 666 /dev/ttyUSB0
bash watch.sh &
pgrep -af watch.sh
```
