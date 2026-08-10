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
