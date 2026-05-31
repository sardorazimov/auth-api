#include <stdio.h>
#include <string.h>

// Şifreleri maskelemek için gizli bir anahtar (Kriptografi temeli)
#define GIZLI_ANAHTAR 0x5A 

// Şifreyi basitleştirilmiş bir şekilde şifreleyen (Hash) fonksiyon
void sifrele(char *girdi, char *cikti) {
    int i = 0;
    while (girdi[i] != '\0') {
        // Her karakteri XOR işlemine sokarak tanınmaz hale getiriyoruz
        cikti[i] = girdi[i] ^ GIZLI_ANAHTAR; 
        i++;
    }
    cikti[i] = '\0'; // Metnin bittiğini C diline bildiriyoruz
}

// Yeni kullanıcı kaydı ve Dosya Sistemi (File Writing)
void kullanici_kaydet(char *username, char *password) {
    FILE *dosya = fopen("veritabani.txt", "w"); // Dosyayı yazma modunda aç
    
    if (dosya == NULL) {
        printf("Hata: Dosya olusturulamadi!\n");
        return;
    }

    char hashli_sifre[100];
    sifrele(password, hashli_sifre); // Şifreyi maskele

    // Kullanıcı adı ve şifrelenmiş veriyi dosyaya yaz
    fprintf(dosya, "%s %s\n", username, hashli_sifre);
    fclose(dosya); // Dosyayı kapat (Bellek yönetimi)
    
    printf("[SaaS] Kullanici '%s' basariyla veritabanina kaydedildi.\n", username);
}

// Kullanıcı Giriş Kontrolü (File Reading & Verification)
int giris_kontrol(char *username, char *password) {
    FILE *dosya = fopen("veritabani.txt", "r"); // Dosyayı okuma modunda aç
    
    if (dosya == NULL) {
        printf("Hata: Veritabanı bulunamadi! Once kayit olun.\n");
        return 0;
    }

    char dosyadaki_user[100];
    char dosyadaki_hash[100];
    
    // Dosyanın içindeki veriyi oku
    fscanf(dosya, "%s %s", dosyadaki_user, dosyadaki_hash);
    fclose(dosya);

    // Gelen şifreyi de aynı yöntemle şifrele ki karşılaştırabilelim
    char girilen_hash[100];
    sifrele(password, girilen_hash);

    // Hem kullanıcı adı hem de şifrelenmiş halleri eşleşiyor mu?
    if (strcmp(username, dosyadaki_user) == 0 && strcmp(girilen_hash, dosyadaki_hash) == 0) {
        return 1; // Başarılı
    }
    
    return 0; // Hatalı
}

int main() {
    printf("=== C AUTH MOTORU CALISIYOR ===\n\n");

    // 1. Senaryo: Yeni Bir Kullanıcı Kaydediyoruz
    char test_user[] = "sardor";
    char test_pass[] = "superSifre123";
    kullanici_kaydet(test_user, test_pass);

    printf("\n-----------------------------------\n\n");

    // 2. Senaryo: Giriş Yapmayı Deniyoruz (Doğru Şifre)
    printf("[SaaS] Giris deneniyor: %s / %s\n", test_user, test_pass);
    if (giris_kontrol(test_user, test_pass)) {
        printf("[SONUC] ERISIM ONAYLANDI! Token uretildi.\n");
    } else {
        printf("[SONUC] HATA: Kullanici adi veya sifre yanlis!\n");
    }

    // 3. Senaryo: Giriş Yapmayı Deniyoruz (Yanlış Şifre)
    char yanlis_pass[] = "yanlisSifre";
    printf("\n[SaaS] Giris deneniyor: %s / %s\n", test_user, yanlis_pass);
    if (giris_kontrol(test_user, yanlis_pass)) {
        printf("[SONUC] ERISIM ONAYLANDI!\n");
    } else {
        printf("[SONUC] HATA: Kullanici adi veya sifre yanlis!\n");
    }

    return 0;
}
