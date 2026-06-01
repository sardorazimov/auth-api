# miransas-agent

`miransas-agent`, macOS uzerinde sistem metriklerini okuyup UDP ile JSON olarak gonderen kucuk bir C agent'idir. RAM bilgisi icin `sysctl` ve Mach VM istatistiklerini, CPU kullanimi icin Mach processor tick farklarini kullanir.

## Derleme

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

`auth.c` ayri bir demo dosyasidir ve mevcut Makefile tarafindan agent binary'sine dahil edilmez. Agent'in aktif kaynaklari `src/` ve `include/` altindadir.
