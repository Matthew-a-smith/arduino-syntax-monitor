#!/bin/bash

CONFIG="setup.conf"

# Load config
source <(grep "=" "$CONFIG")

OUTPUT=errors.txt

PORT="$com"

EDITOR="$editor"

BAUD="$baud"

# Setup serial connection
stty -F "$PORT" "$BAUD" cs8 -cstopb -parenb raw -echo

# Convert extensions list into array
IFS=',' read -ra EXTENSIONS <<< "$extensions"
NANO_FLAG=0

while true
do
    # Clear old errors
    > "$OUTPUT"

    # Find files opened by nano
    for PID in $(pgrep $EDITOR)
    do
        if [ "$NANO_FLAG" -eq 0 ]; then
            NANO_FLAG=1
            echo  "3" > "$PORT"
        fi
        for FILE in $(ps aux | grep $EDITOR | grep -v grep | awk '{print $NF}')

        do

            EXT="${FILE##*.}"

            for ALLOWED in "${EXTENSIONS[@]}"
            do
                if [ "$EXT" = "$ALLOWED" ]; then
                    case "$EXT" in

                        cpp)
                            g++ -fsyntax-only "$FILE" 2>> "$OUTPUT"

                            if [ -s "$OUTPUT" ]; then
                                ERROR=$(cat "$OUTPUT")

                                if [ "$ERROR" != "$LAST_ERROR" ]; then
                                    echo "1" > "$PORT"
                                    cat "$OUTPUT" > "$PORT"
                                    LAST_ERROR="$ERROR"
                                fi
                            else
                                    echo "0|$FILE" > "$PORT"
                            fi
                            ;;

                        py)
                            python -m py_compile "$FILE" 2>> "$OUTPUT"

                            if [ -s "$OUTPUT" ]; then
                                ERROR=$(cat "$OUTPUT")

                                if [ "$ERROR" != "$LAST_ERROR" ]; then
                                    echo "1" > "$PORT"
                                    cat "$OUTPUT" > "$PORT"
                                    LAST_ERROR="$ERROR"
                                fi
                            else
                                    echo "0|$FILE" > "$PORT"
                            fi
                            ;;

                    esac


                fi
            done

        done
    done
    if ! pgrep $EDITOR > /dev/null && [ "$NANO_FLAG" -eq 1 ]; then
        NANO_FLAG=0
        > "$OUTPUT"
        echo -n "3" > "$PORT"
    fi

    sleep 3
done

