```makefile
# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -Wpedantic -std=c11

# Имя исполняемого файла
TARGET = client

# Исходные файлы
SOURCES = telnet_client.c

# Объектные файлы
OBJECTS = $(SOURCES:.c=.o)

# Цель по умолчанию
all: $(TARGET)

# Правило для сборки исполняемого файла
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Правило для компиляции исходных файлов в объектные
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Цель для очистки
clean:
	rm -f $(OBJECTS) $(TARGET)

# Цель для предотвращения конфликтов с файлами
.PHONY: all clean
```