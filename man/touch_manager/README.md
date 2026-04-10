# touch_manager (v0.1 MVP)

Management Service for Touch System - v0.1 MVP implementation.

## Features (v0.1)

- **Ingress Configuration Management**: REST API for managing touch_ingress configurations
  - **自动初始化**：首次启动时自动创建默认 ingress (ingress1: 127.0.0.1:54321)
  - List ingresses (列表查询)
  - Add ingress (添加)
  - Update ingress (修改)
  - Delete ingress (删除)
  - 启动时自动连接所有启用的 ingress

- **Web Pages**: Web UI for viewing device and ingress information
  - `/devices` - Device list page (Edge devices)
  - `/ingresses` - Ingress list page

## Requirements

- Python 3.7+
- See `requirements.txt` for dependencies

## Installation

```bash
pip install -r requirements.txt
```

## Usage

```bash
python app.py
```

The service will start on `http://0.0.0.0:5000` by default.

## Web Pages

- `GET /` - Home page with navigation and quick links
- `GET /devices` - Device list page (shows all registered edge devices)
- `GET /ingresses` - Ingress list page (shows all configured ingresses)

The device and ingress list pages auto-refresh every 30 seconds and support manual refresh.

## API Endpoints

### Ingress Management

- `GET /api/v1/ingresses` - List all ingresses
- `POST /api/v1/ingresses` - Add new ingress
- `PUT /api/v1/ingresses/<ingress_id>` - Update ingress
- `DELETE /api/v1/ingresses/<ingress_id>` - Delete ingress

### Device Registration (for touch_edge)

- `POST /api/v1/devices/register` - Device registration
  - Request: `{"device_id": "...", "device_type": "...", "device_key": "...", "local_ip": "..."}`
  - Response: `{"code": 0, "ingress": {"host": "...", "port": ...}, "token": "...", "device_public_ip": "..."}`
- `POST /api/v1/devices/<device_id>/online` - Device online notification
- `GET /api/v1/devices` - List all devices

## Database

SQLite database file: `touch_manager.db` (created automatically)

## Configuration

### Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `PORT` | 18050 | HTTP server port |
| `DATABASE_PATH` | `touch_manager.db` | SQLite database path |
| `WEBSOCKET_PING_INTERVAL` | 15 | WebSocket ping interval (seconds), should be ~1/2 of server interval |
| `WEBSOCKET_PING_TIMEOUT` | 5 | WebSocket pong timeout (seconds), should be 1/3 of interval |
| `INGRESS_CONNECT_TIMEOUT` | 5 | Ingress connection timeout (seconds) |
| `INGRESS_RECONNECT_INTERVAL` | 10 | Ingress reconnect interval (seconds) |
| `DEVICE_LIST_BATCH_SIZE` | 50 | Device list query batch size |
| `DEVICE_LIST_QUERY_TIMEOUT` | 10 | Device list query timeout (seconds) |

### WebSocket Ping Configuration

**Why timeout=5 seconds is reasonable?**

According to RFC 6455 and industry best practices:
- `ping_timeout` should be **1/3 to 1/2** of `ping_interval`
- Current config: interval=15s, timeout=5s (1/3 ratio)
- Detection time: Connection issues detected within 20 seconds max (15s + 5s)

**Coordination with server config:**
- touch_ingress server: interval=30s, timeout=10s
- touch_manager client: interval=15s, timeout=5s
- Client's more frequent ping serves as backup detection

