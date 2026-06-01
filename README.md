# miransas-agent

`miransas-agent`, macOS uzerinde sistem ve uygulama sagligi izleme fikri icin gelistirilen hafif bir agent prototipidir. Su an CPU/RAM metriklerini okur, UDP ile JSON gonderebilir ve ekranda kucuk transparan bir health paneli gosterebilir.

RAM bilgisi icin `sysctl` ve Mach VM istatistiklerini, CPU kullanimi icin Mach processor tick farklarini kullanir. Health paneli macOS Cocoa ile yazilmistir.

## Derleme

Derleme icin macOS geliştirici araclari gerekir. Makefile C dosyalarini ve Cocoa tabanli `src/hud.m` dosyasini birlikte derler.

```bash
make
```

Binary su adrese uretilir:

```bash
bin/miransas_agent
```

Temizlemek icin:

```bash
make clean
```

## Calistirma

Tek paket gonderip cikmak icin:

```bash
./bin/miransas_agent --once
```

Terminalde surekli calistirmak ve loglari gormek icin:

```bash
./bin/miransas_agent --foreground
```

Kucuk transparan health panelini ekranda gostermek icin:

```bash
./bin/miransas_agent --hud
```

Argumansiz calistirildiginda POSIX daemon olarak arka plana ayrilir:

```bash
./bin/miransas_agent
```

Yardim:

```bash
./bin/miransas_agent --help
```

## UDP hedefi

Varsayilan hedef `include/agent.h` icinde tanimlidir:

```c
#define TARGET_IP "127.0.0.1"
#define TARGET_PORT 9999
```

Giden paket formati:

```json
{"node":"Miransas-Node-01","cpu_usage_percent":0.00,"total_ram_mb":8192,"free_ram_mb":225}
```

## LaunchAgent

`com.miransas.agent.plist` dosyasindaki binary yolunu kendi proje dizinine gore guncelle:

```xml
<string>/Users/KULLANICI_ADIN/PROJE_DIZININ/miransas-agent/bin/miransas_agent</string>
```

Sonra macOS LaunchAgent olarak yukleyebilirsin.

## Notlar

Aktif kaynaklar `src/` ve `include/` altindadir. Siradaki buyuk adim, calisan uygulamalari process bazinda okuyup health panelinde uygulama adi, calisma suresi, CPU ve RAM kullanimi olarak gostermektir.
