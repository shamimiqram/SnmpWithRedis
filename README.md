# SnmpWithRedis: R&D Project

## Overview

**SnmpWithRedis** is a demo project that integrates **SNMP (Simple Network Management Protocol)** with **Redis (Real-time Data Management)**. The project enables the monitoring and management of SNMP-enabled devices by parsing SNMP data, performing SNMP operations (GET/WALK), and storing results in Redis for real-time processing and data management.

**Key Features:**
- SNMP GET and SNMP WALK operations to retrieve device data.
- Redis integration for persistent storage and real-time data handling.
- JSON-based data exchange for structured and flexible communication.
- Logging and error handling for robust system operation.

---

## Project Structure

The project consists of multiple modules, each responsible for a specific functionality. Below is a breakdown of the core files:

### 1. Core Files Overview

#### **`main.c`**
- **Purpose**: Main entry point for the application, handling user input and SNMP device monitoring.
- **Key Functions**:
  - `input_thread()`: Handles user input in a separate thread.
  - `device_monitor()`: Monitors and processes SNMP OIDs from Redis, executes SNMP operations.
  - `signal_handler()`: Handles segmentation faults and ensures a clean shutdown of resources.

#### **`redis_task.c`**
- **Purpose**: Manages Redis connections, data retrieval, and data storage.
- **Key Functions**:
  - `connect_redis()`: Connects to Redis with specified credentials.
  - `get_and_process_oid_from_redis()`: Retrieves SNMP OIDs from Redis and processes them.
  - `set_error_value_in_redis()`: Stores error information in Redis in JSON format.
  - `trim_data_from_redis()`: Removes processed data from Redis to maintain efficiency.

#### **`snmp_task.c`**
- **Purpose**: Handles SNMP operations like GET and WALK using the net-snmp library.
- **Key Functions**:
  - `init_snmp_server()`: Initializes SNMP session with device IP, version, and community string.
  - `snmp_get_with_hash_key()`: Performs SNMP GET operation to retrieve data.
  - `snmp_walk_with_hash_key()`: Performs SNMP WALK operation to retrieve data.
  - `async_callback_with_hash_key()`: Asynchronous callback to handle SNMP responses.

#### **`json_task.c`**
- **Purpose**: Provides functions to create and parse JSON objects using the cJSON library.
- **Key Functions**:
  - `oid_info_to_json()`: Converts SNMP OID data to a JSON object.
  - `create_JSON_Object()`: Creates a JSON object for SNMP configuration (IP, version, OID, etc.).
  - `perse_data_from_json()`: Parses and extracts specific values from JSON objects.

---

### 2. System Workflow

The system works in the following sequence:

1. **SNMP Operation Setup**:
   - The system configures SNMP devices using the device's IP, SNMP version, community string, and OID.
   - This configuration is stored in Redis in JSON format for further processing.

2. **Monitoring and Processing**:
   - The `device_monitor()` function retrieves SNMP OIDs from Redis and performs SNMP operations (GET/WALK).
   - SNMP responses are processed asynchronously using callback functions (`async_callback_with_hash_key()`).

3. **Data Handling with Redis**:
   - SNMP results (or errors) are formatted as JSON objects and stored in Redis.
   - Processed data is trimmed from Redis to ensure efficient memory usage and data flow.

4. **Error Handling and Logging**:
   - Errors are logged to files (`LogFile.txt`, `DebugFile.txt`) for troubleshooting.
   - In case of a segmentation fault (SIGSEGV), the system performs a clean shutdown via the `signal_handler()` function.

---

### 3. Conclusion

The **SnmpWithRedis** project integrates SNMP operations with Redis to create a scalable and efficient solution for monitoring SNMP-enabled devices. It offers:
- Real-time SNMP data retrieval and processing.
- Persistent storage and efficient management of SNMP data via Redis.
- Robust error handling and logging for reliable system operation.

This modular architecture ensures high performance, easy integration, and the ability to handle large volumes of SNMP data efficiently.

---

## Requirements

- **libhiredis**: Redis C client library.
- **net-snmp**: SNMP C library.
- **cJSON**: C library for JSON parsing and creation.

To install dependencies, use the following commands (on Ubuntu/Debian-based systems):

```bash
sudo apt-get install libhiredis-dev libsnmp-dev libjson-c-dev
