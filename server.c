#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h> // ? Soketleri kullanmak için lazım.
#include <netinet/in.h>

#define DEFAULT_PORT 20015

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address; // ? Socketin internet adresi. IP Adresi denilebilir.
    int addrlen = sizeof(address);
    char buffer[1024] = {0}; // ? Bufferları I/O(Input/Output) işlemleri için kullanacağız. 
    char *cevap = "Merhaba, istemci";
    int port = DEFAULT_PORT;
    int arka_log = 3;
    int opt = 1;

    if (argc > 1) port = atoi(argv[1]); // ? Konsolda herhangi bir input var ise. Atoi fonksiyonu ile input integer bir değere dönüştürülür ve port'a atanır.

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        // ? AF_INET = IpV4
        // ? AF_INET6 = IpV6
        perror("Socket acilamadi!");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("RE_USEADDR hatasi!");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // ? Kabul edeceğimiz istekler.
    address.sin_port = htons(port); // ? htons() methodu portu sunucunun okuyabileceği bir şekile dönüştürür.

    // ? File descriptora, sunucu adresini verdik.

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind hatasi!");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, arka_log) < 0) {
        // ? Eğerki "arka_log"'dan fazla bağlantı varsa, yeni gelen bağlantıları bir bekleme sırasına atıyoruz.
        // ? Bu sırada ise bekleme sırasında olan, bağlantılar diğer bağlantıların bitmesini bekler.
        perror("Dinleme hatasi!");
        exit(EXIT_FAILURE);
    }

    printf("Sunucu %d portunda baglanti bekliyor.\n", port);

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        // ? "server_fd" üzerinden yeni bir bağlantı bekliyoruz.
        // ? Yeni gelen bağlantıları otomatik olarak kabul ediyoruz ve client ile iletişim kuruyoruz.
        perror("Accept hatasi!");
        exit(EXIT_FAILURE);
    }

    // ? Konsoldan gelen veriyi okuma ve cliente geri veri gönderme.

    read(new_socket, buffer, 1024);
    printf("Data from client : %s\n", buffer);
    send(new_socket, cevap, strlen(cevap), 0);
    close(new_socket);
    close(server_fd);

    return 0;
}
