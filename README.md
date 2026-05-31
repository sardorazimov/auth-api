# miransas-agent 🚀

Miransas ekosistemi için geliştirilmiş, harici hiçbir bağımlılığı bulunmayan (**zero-dependency**), saf C dili ile yazılmış düşük seviyeli (low-level) bir **Sistem İzleme Daemon (Micro-Agent)** altyapısıdır.

Özellikle macOS (M1/Apple Silicon) ve Unix mimarileri için optimize edilmiş olup, kernel seviyesindeki çağrılarla en doğru CPU ve bellek verilerini anlık olarak yakalar ve yapılandırılmış JSON formatında UDP protokolü üzerinden uçurur.

## 🛠️ Özellikler

* **Native Performans:** Go veya Rust runtime yükü olmadan, doğrudan makine koduna derlenir. Minimum CPU ve RAM tüketimi.
* **Low-Level Kernel Çağrıları:** macOS `mach/mach.h` ve `sys/sysctl.h` kütüphanelerini kullanarak doğrudan donanım istatistiklerine erişir.
* **Zero-Dependency JSON:** Harici hiçbir JSON kütüphanesi (cJSON vb.) kullanmadan, bellek dostu `snprintf` manipülasyonu ile valid JSON paketleri üretir.
* **UDP Streaming:** Metrikleri engellenemez (non-blocking) ve hızlı bir şekilde ağ üzerinden fırlatır (Örn: `Binboi` veya `Miransas-ID` entegrasyonu için).

## 📂 Proje Yapısı

```text
miransas-agent/
├── bin/          # Derlenmiş çalıştırılabilir binary (Makine kodu)
├── build/        # Geçici nesne (.o) dosyaları
├── include/      # Başlık (.h) dosyaları - Kontratlar
├── src/          # Kaynak (.c) kodları - Çekirdek mantık
├── Makefile      # Derleme otomasyonu
└── README.md     # Şu an okuduğunuz kılavuz