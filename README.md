# SnmpWithRedis
R&D project

#SNMP - Simple Network Management Protocol (http://www.net-snmp.org/)

#REDIS - Real time Data management (https://redis.io/)

#SNMP_REDIS - Implement a demo project to parse data with snmp and store to redis queue


1. Core Files Overview

1.1 main.c

Purpose: The main entry point for the application, handling user input and SNMP device monitoring.
Key Functions:
input_thread(): Handles user input in a separate thread to control the application's execution (e.g., stop the application).
device_monitor(): Monitors and processes SNMP OIDs from Redis, executes SNMP operations (GET/WALK), and manages SNMP-related operations.
signal_handler(): Handles segmentation faults (SIGSEGV) and ensures clean shutdown of resources.
Logging: Logs messages to DebugFile.txt for debugging purposes.

1.2 redis_task.c

Purpose: Manages Redis connections, data retrieval, and data storage.
Key Functions:
connect_redis(): Connects to Redis using a specified IP address, port, and authentication credentials.
get_and_process_oid_from_redis(): Retrieves SNMP OIDs from Redis, parses them, and processes them via SNMP.
set_error_value_in_redis(): Stores error information in Redis in JSON format.
trim_data_from_redis(): Removes processed data from Redis.

1.3 snmp_task.c

Purpose: Handles SNMP operations like SNMP GET and SNMP WALK using the net-snmp library.
Key Functions:
init_snmp_server(): Initializes SNMP session settings (IP, version, community string).
snmp_get_with_hash_key(): Performs an SNMP GET operation to retrieve data.
snmp_walk_with_hash_key(): Performs an SNMP WALK operation to retrieve data sequentially.
async_callback_with_hash_key(): Callback function that processes SNMP response messages asynchronously.

1.4 json_task.c

Purpose: Provides functions to create and parse JSON objects using the cJSON library.
Key Functions:
oid_info_to_json(): Creates a JSON object representing SNMP OID data, including type, value, and error message.
create_JSON_Object(): Creates a JSON object representing SNMP configuration, including IP address, SNMP version, community string, and OID.
perse_data_from_json(): Parses and extracts a value from a JSON object using a specified key.

2. System Workflow
3. 
SNMP Operation Setup:

The system begins by configuring SNMP devices, including device IP, SNMP version, community string, and OID.
Configuration data is encapsulated in JSON format using the create_JSON_Object() function and stored in Redis.
Monitoring and Processing:

The device_monitor() function retrieves SNMP OIDs from Redis and processes them by performing SNMP GET or WALK operations.
For each SNMP operation, the response is handled asynchronously via snmp_get_with_hash_key() or snmp_walk_with_hash_key().
Data Handling with Redis:

SNMP results (and errors) are formatted into JSON objects using oid_info_to_json() and pushed into Redis.
Processed data is trimmed from Redis to maintain efficiency using trim_data_from_redis().
Error Handling and Logging:

The system handles errors by logging them to files (LogFile.txt, DebugFile.txt).
In case of a segmentation fault (SIGSEGV), the system performs a clean shutdown via signal_handler() and logs the error details.

3. Conclusion
   
The project facilitates the monitoring and management of SNMP-enabled devices, using Redis for data storage and asynchronous SNMP operations. The use of JSON for data exchange provides a flexible and structured approach, allowing easy integration and scalability. Key functionalities include:

SNMP data retrieval and processing.
Integration with Redis for persistent storage and queueing of SNMP operations.
Logging and error handling for robust system operation.
This modular design ensures that the system can handle large volumes of SNMP data efficiently while providing useful insights into device status and performance.
