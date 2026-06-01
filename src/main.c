#include "agent.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

// Süreci gerçek bir Unix Daemon'ına (arka plan hayaletine) çeviren fonksiyon
void daemonize(void) {
    pid_t pid;

    // 1. Adım: İlk fork() çağrısı ile ana süreçten ayrılıyoruz
    pid = fork();

    // Hata oluştuysa çık
    if (pid < 0) exit(EXIT_FAILURE);

    // Eğer ana süreçsek (parent), terminale geri dönmek için kendimizi sonlandırıyoruz
    if (pid > 0) exit(EXIT_SUCCESS);

    // 2. Adım: Çocuk süreç (child) artık yeni bir oturum lideri (session leader) oluyor
    if (setsid() < 0) exit(EXIT_FAILURE);

    // Sinyalleri yakalamak veya göz ardı etmek için (Zombi süreç oluşmasın diye)
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    // 3. Adım: İkinci fork(). Sürecin bir terminale tekrar bağlanmasını kesin olarak engelliyoruz
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    // 4. Adım: Dosya izin maskesini sıfırlıyoruz
    umask(0);

    // 5. Adım: Kök dizine geçiyoruz (Çalıştığı klasör silinirse süreç havada kalmasın diye)
    if (chdir("/") < 0) exit(EXIT_FAILURE);

    // 6. Adım: Standart girdi, çıktı ve hataları (Terminal bağlantılarını) tamamen kapatıyoruz
    // Onları /dev/null (Unix kara deliği) üzerine yönlendiriyoruz
    int fd = open("/dev/null", O_RDWR);
    if (fd != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
}

int main(void) {
    int sock_fd;
    sys_metrics_t metrics;

    // Arka plan hayalet modunu aktif et!
    // Bu satırdan sonra terminal bağlantısı kopacak ve kod arka planda akacak.
    daemonize();

    // Soketi daemon olduktan sonra başlatıyoruz
    sock_fd = init_socket();
    if (sock_fd < 0) {
        return 1;
    }

    // Sonsuz döngü: Sistem açık olduğu sürece arka planda çıt çıkarmadan çalışır
    while (1) {
        collect_metrics(&metrics);
        send_metrics(sock_fd, &metrics);
        sleep(INTERVAL_SEC);
    }

    close(sock_fd);
    return 0;
}