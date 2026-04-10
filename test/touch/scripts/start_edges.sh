#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EDGE_BINARY="$SCRIPT_DIR/../touch_edge/touch_edge-linux"
DST_HOST="47.100.49.48"
DST_PORT="54321"

# Check if binary exists, compile if not
if [ ! -f "$EDGE_BINARY" ]; then
    echo "Edge binary not found: $EDGE_BINARY"
    EDGE_DIR="$SCRIPT_DIR/../touch_edge"
    if [ -f "$EDGE_DIR/Makefile" ]; then
        echo "Compiling touch_edge..."
        cd "$EDGE_DIR" && make clean && make
        cd - > /dev/null
    fi
fi

# Check again after compilation
if [ ! -f "$EDGE_BINARY" ]; then
    echo "Error: edge binary still not found after compilation attempt"
    exit 1
fi

# Default to 3 devices, can be specified via argument
COUNT=${1:-3}

# Show help
if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
    echo "Usage: $0 [COUNT]"
    echo ""
    echo "Start specified number of touch_edge devices"
    echo ""
    echo "Arguments:"
    echo "  COUNT    Number of devices to start (default: 3)"
    echo ""
    echo "Examples:"
    echo "  $0          # Start 3 devices"
    echo "  $0 10       # Start 10 devices"
    echo "  $0 100      # Start 100 devices"
    exit 0
fi

# Validate argument
if ! [[ "$COUNT" =~ ^[0-9]+$ ]]; then
    echo "Error: COUNT must be a positive integer"
    exit 1
fi

echo "Starting $COUNT edge device(s)..."

STARTED=0
SKIPPED=0

for i in $(seq 1 $COUNT); do
    DEVICE_ID="edge-$(printf '%03d' $i)"
    DEVICE_KEY="key$(printf '%03d' $i)"
    LOCAL_IP="192.168.9.$((93 + i % 256))"

    # Check if process with same parameters is already running
    # Use pgrep -f but exclude the pgrep process itself
    if pgrep -f "touch_edge-linux.*-i $DEVICE_ID.*-L $LOCAL_IP" | grep -v "^$$\$" > /dev/null 2>&1; then
        echo "Skip: device $DEVICE_ID is already running"
        ((SKIPPED++))
        continue
    fi

    $EDGE_BINARY -H $DST_HOST -P $DST_PORT -i "$DEVICE_ID" -k "$DEVICE_KEY" -t touch_edge -L "$LOCAL_IP" &
    ((STARTED++))
done
echo "Started $STARTED edge device(s), skipped $SKIPPED already running device(s)"
