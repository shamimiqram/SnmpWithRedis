# Compiler and flags
CC = gcc
CFLAGS = -Iinclude -I/usr/include/net-snmp -I/usr/local/include  # Add include path for custom libraries

# Library paths and flags
LIBPATH = -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu
LDFLAGS = /usr/local/lib/libhiredis.a  /usr/local/lib/libnetsnmp.a $(LIBPATH) -lcjson -lssl -lcrypto -lz -lzstd # Link with static libraries
LDFLAGS += -static
# Output binary
TARGET = myprogram

# Source and object files
SRC_DIR = src
OBJ_DIR = obj

SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/redis_task.c \
          $(SRC_DIR)/snmp_task.c \
          $(SRC_DIR)/json_helper.c \
          $(SRC_DIR)/helper.c \
          $(SRC_DIR)/global.c \
          $(SRC_DIR)/ini.c \
          $(SRC_DIR)/log.c \
          $(SRC_DIR)/configuration.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)  # Map .c files to .o object files in the obj dir

# Default target
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile .c files into .o files (with proper include paths)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)  # Create the obj directory if it doesn't exist
	$(CC) -c $< $(CFLAGS) -o $@

# Debugging target (optional)
gdb: $(TARGET)
	gdb ./$(TARGET)

# Clean up object files and the final binary
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

