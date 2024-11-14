# SnmpWithRedis
R&D project

#SNMP - Simple Network Management Protocol (http://www.net-snmp.org/)

#REDIS - Real time Data management (https://redis.io/)

#SNMP_REDIS - Implement a demo project to parse data with snmp and store to redis queue


##1. Core Files Overview
#1.1 main.c
Purpose: The main entry point for the application, handling user input and SNMP device monitoring.
Key Functions:
input_thread(): Handles user input in a separate thread to control the application's execution (e.g., stop the application).
device_monitor(): Monitors and processes SNMP OIDs from Redis, executes SNMP operations (GET/WALK), and manages SNMP-related operations.
signal_handler(): Handles segmentation faults (SIGSEGV) and ensures clean shutdown of resources.
Logging: Logs messages to DebugFile.txt for debugging purposes.
1.2 redis.c
Purpose: Manages Redis connections, data retrieval, and data storage.
Key Functions:
connect_redis(): Connects to Redis using a specified IP address, port, and authentication credentials.
get_and_process_oid_from_redis(): Retrieves SNMP OIDs from Redis, parses them, and processes them via SNMP.
set_error_value_in_redis(): Stores error information in Redis in JSON format.
trim_data_from_redis(): Removes processed data from Redis.
1.3 snmp.c
Purpose: Handles SNMP operations like SNMP GET and SNMP WALK using the net-snmp library.
Key Functions:
init_snmp_server(): Initializes SNMP session settings (IP, version, community string).
snmp_get_with_hash_key(): Performs an SNMP GET operation to retrieve data.
snmp_walk_with_hash_key(): Performs an SNMP WALK operation to retrieve data sequentially.
async_callback_with_hash_key(): Callback function that processes SNMP response messages asynchronously.
1.4 json.c
Purpose: Provides functions to create and parse JSON objects using the cJSON library.
Key Functions:
oid_info_to_json(): Creates a JSON object representing SNMP OID data, including type, value, and error message.
create_JSON_Object(): Creates a JSON object representing SNMP configuration, including IP address, SNMP version, community string, and OID.
perse_data_from_json(): Parses and extracts a value from a JSON object using a specified key.
