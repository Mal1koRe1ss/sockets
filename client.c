#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEFAULT_PORT 20015

int main(int argc, char const *argv[])
{
    int port = DEFAULT_PORT;
    char mesaj[255];
    int soket = 0;
    struct sockaddr_in server_adres;
    char buffer[1024] = {0}; // ? Burada bufferı veri giriş çıkışları için kullanıyoruz.

    if (argc > 1) port = atoi(argv[1]); // ? atoi fonksiyonu ile konsoldan gelen inputu Integer bir değere çevirip porta atıyoruz.

    // ? Soketimizi oluşturmaya başlayalım.

    if ((soket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

        // ? 0, -1, -2 ...

        // ? AF_INET = IpV4
        // ? AF_INET6 = IpV6

        perror("Soket hatasi!");
        exit(EXIT_FAILURE);
    } 

    server_adres.sin_family = AF_INET;
    server_adres.sin_port = htons(port);

    // ? IP adresini binary'e çevirme.
    if (inet_pton(AF_INET, "127.0.0.1", &server_adres.sin_addr) <= 0) {
        perror("IP adresi binary'e cevirilemedi!");
        exit(EXIT_FAILURE);
    }

    // ? Sunucuya bağlanma.

    if (connect(soket, (struct sockaddr *)&server_adres, sizeof(server_adres)) < 0) {

        // ? Soket üzerinden, server_adres'ine point eden bir soket adresi oluşturup sunucuya bağlantı kuruyoruz.

        perror("Baglanti hatasi!");
        exit(EXIT_FAILURE);
    }

    printf("Mesajiniz : ");
    scanf("%254s", mesaj);
    send(soket, mesaj, strlen(mesaj), 0);
    read(soket, buffer, 1024);
    printf("Sunucudan cevap geldi : %s\n",buffer);

    close(soket);
    return 0;
}
