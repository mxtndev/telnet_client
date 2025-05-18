#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define SERVER_HOST "telehack.com"
#define SERVER_PORT 23
#define BUFFER_SIZE 4096
#define MAX_TEXT_LEN 256

// Выводим сообщение об ошибке и завершает программу
void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

// Проверяем корректность аргументов командной строки
void validate_arguments(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <font> <text>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

// Создаем и настраиваем сокет
int create_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error creating socket");
    }
    return sockfd;
}

// Получаем адрес сервера
struct sockaddr_in get_server_address() {
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    struct hostent *server = gethostbyname(SERVER_HOST);
    if (server == NULL) {
        fprintf(stderr, "Error: No such host %s\n", SERVER_HOST);
        exit(EXIT_FAILURE);
    }
    memcpy(&server_addr.sin_addr, server->h_addr_list[0], server->h_length);
    return server_addr;
}

// Устанавливаем соединение с сервером
void connect_to_server(int sockfd, struct sockaddr_in *server_addr) {
    if (connect(sockfd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        error("Error connecting to server");
    }
}

// Формируем команду figlet
void build_figlet_command(char *command, const char *font, const char *text) {
    snprintf(command, MAX_TEXT_LEN, "figlet -f %s %s\r\n", font, text);
}

// Отправляем команду на сервер
void send_command(int sockfd, const char *command) {
    if (send(sockfd, command, strlen(command), 0) < 0) {
        error("Error sending command");
    }
}

// Читаем и выводим ответ от сервера
void receive_response(int sockfd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Пропускаем начальный баннер сервера
    while ((bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        if (strstr(buffer, "Connected to TELEHACK")) {
            break;
        }
    }

    // Читаем ответ на команду figlet
    while ((bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
        // Проверяем конец вывода (сервер может отправить приглашение)
        if (strstr(buffer, "$")) {
            break;
        }
    }

    if (bytes_received < 0) {
        error("Error receiving response");
    }
}

int main(int argc, char *argv[]) {
    validate_arguments(argc, argv);

    const char *font = argv[1];
    const char *text = argv[2];

    // Создаем сокет
    int sockfd = create_socket();

    // Получаем адрес сервера
    struct sockaddr_in server_addr = get_server_address();

    // Устанавливаем соединение
    connect_to_server(sockfd, &server_addr);

    // Формируем команду figlet
    char command[MAX_TEXT_LEN];
    build_figlet_command(command, font, text);

    // Отправляем команду
    send_command(sockfd, command);

    // Получаем и выводим ответ
    receive_response(sockfd);

    // Закрываем сокет
    close(sockfd);
    return 0;
}