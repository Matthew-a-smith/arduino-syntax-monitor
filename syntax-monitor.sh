
#!/bin/bash

# Title: Syntax-monitor.sh
# Author: Matthew A. Smith
# StudentID: n/a
# Date: August 10th, 2026
# Version: 1.6
#
# Description:
# Background syntax monitoring script for terminal-based editors.
# Checks supported files and sends the results to an Arduino
# through a USB serial connection.
#
# Copyright: 2026 Matthew A. Smith

# DOCUMENTATION
#
# Program Purpose:
# Monitors files opened in the configured terminal editor and
# performs syntax checks using language-specific checkers.
#
# Supported Languages:
# cpp - g++ -fsyntax-only
# py  - python -m py_compile
#
# Serial Status:
# 1          - Errors found
# 0|filename - No errors
# 3          - Editor closed
#
# Configuration:
# setup.conf contains the serial port, editor, baud rate,
# and supported file extensions.
#
# Variables:
# CONFIG     - Configuration file
# OUTPUT     - Temporary error output file
# PORT       - Arduino serial port
# EDITOR     - Terminal editor being monitored
# BAUD       - Serial communication speed
# EXTENSIONS - Supported file extensions
#
# Serial Connection:
# The script communicates with the Arduino through the USB
# serial port defined in setup.conf.
#
# Notes:
# - The script runs continuously in the background.
# - Errors are sent to the Arduino when they change.
# - Supported file extensions are defined in setup.conf.

# Configuration file path
CONFIG="setup.conf"

# Load configuration settings
source "$CONFIG"

# File used to store syntax checker output
OUTPUT=errors.txt

# Arduino serial device path
PORT="$com"

# Serial communication speed
BAUD="$baud"

# Open a persistent serial connection to the Arduino
exec 3>"$PORT"

# Configure the serial port settings
stty -F "$PORT" "$BAUD" cs8 -cstopb -parenb raw -echo

# Allow time for the serial connection to initialize
sleep 2

# Tracks whether an editor is currently open
NANO_FLAG=0

# Tracks whether any configured editor is running
EDITOR_RUNNING=0

# Stores the hash of the previously reported error output
LAST_HASH=""

# Main monitoring loop
while true
do

    # Assume no editor is running until one is found
    EDITOR_RUNNING=0

    # Check each configured editor
    for EDITOR in "${editors[@]}"
    do

        # Find running processes for the current editor
        for PID in $(pgrep "$EDITOR")
        do

            # Notify the Arduino when an editor is first opened
            if [ "$NANO_FLAG" -eq 0 ]; then
                NANO_FLAG=1
                echo "3" >&3
            fi

            # Find files currently opened by the editor
            for FILE in $(ps aux | grep "$EDITOR" | grep -v grep | awk '{print $NF}')
            do

                # Extract the file extension
                EXT="${FILE##*.}"

                # Search for a matching configured extension
                for i in "${!extensions[@]}"
                do

                    if [ "$EXT" = "${extensions[$i]}" ]; then

                        # Retrieve the matching syntax-checking command
                        COMMAND="${commands[$i]}"

                        # Run the syntax checker and save errors
                        $COMMAND "$FILE" 2> "$OUTPUT"

                        # Check if any syntax errors were found
                        if [ -s "$OUTPUT" ]; then

                            # Generate a hash of the error output
                            CURRENT_HASH=$(md5sum "$OUTPUT" | awk '{print $1}')

                            # Only send updates if the errors changed
                            if [ "$CURRENT_HASH" != "$LAST_HASH" ]; then

                                # Notify Arduino that errors exist
                                echo "1" >&3

                                # Extract lines containing the filename
                                # and remove unsupported characters
                                ERROR=$(grep "$FILE" "$OUTPUT" \
                                    | sed 's/[^a-zA-Z0-9 .:_()\/-]//g')

                                # Send the cleaned error message
                                echo "$ERROR" >&3

                                # Signal that all errors have been sent
                                echo "END_ERRORS" >&3

                                # Save the new hash for comparison
                                LAST_HASH="$CURRENT_HASH"

                            fi

                        else

                            # Notify Arduino that the file contains no errors
                            echo "0|$FILE" >&3

                        fi

                    fi

                done

            done

        done

    done

    # Determine whether any configured editor is still running
    for EDITOR in "${editors[@]}"
    do
        if pgrep "$EDITOR" > /dev/null; then
            EDITOR_RUNNING=1
            break
        fi
    done

    # Notify the Arduino when all editors are closed
    if [ "$EDITOR_RUNNING" -eq 0 ] && [ "$NANO_FLAG" -eq 1 ]; then

        NANO_FLAG=0

        # Send editor closed status
        echo -n "3" >&3
    fi

    # Delay before the next scan cycle
    sleep 2

done