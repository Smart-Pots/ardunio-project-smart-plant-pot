// Servo Motorun Kütüphanesi Dahil Edilir
#include <Servo.h>

// Saat Modülü Kütüphanesi Dahil Edilir.
#include <virtuabotixRTC.h>

// LCD ve I2C Modülü Kütüphanesi Dahil Edilir.
#include <LiquidCrystal_I2C.h>

// Servo'dan bir obje üretilir
Servo ServoNesnesi;

// Arduino uno üzerine takılı tüm pinler tanımlanır
#define SU_SEVIYE A0
#define TOPRAK_NEM A1
#define HAVA_NEM 2
#define BUZZER 3
#define SERVO 4
#define MZ80 5
int RGB_R = 6;
int RGB_G = 7;
int RGB_B = 12;
#define SAAT_CLK 8
#define SAAT_DAT 9
#define SAAT_RST 10
#define DOKUNMA 11
#define LDR A2
 
 // LCD üzerinde cisim görüldüğünde yazacak yazı
String SELAMLA = "HOSGELDIN";

// LCD den 'lcd' adında bir nesne üretilmesi
LiquidCrystal_I2C lcd(0x27,16,2);

// saat modülünden Saat adında bir nesne üretilmesi
virtuabotixRTC Saat(8, 9, 10);

// hava nem-sıcaklık sensörünün kütüphanesinin dahil edilmesi.
#include <dht11.h>

//dht11 kütüphanesinden obje oluşturulması
dht11 dht_sensor;

// dokunma değerini kontrol barındıran değişken
int touch_count = 0;
void setup() {
  
  // lcd başlat
  lcd.begin();

  // saat modülüne saat dakika saniye gün ay yıl gibi değerleri ver.
  //Saat.setDS1302Time(00, 36, 13, 3, 15, 06, 2022);

  // servo motorun hazırlanması
  ServoNesnesi.attach(SERVO);

  // bileşenlerin (dolayısıyla pinlerin) girdi veya çıktı şeklinde tanımlanması
  pinMode(SU_SEVIYE,INPUT);
  pinMode(TOPRAK_NEM,INPUT);
  pinMode(BUZZER,OUTPUT);
  pinMode(SERVO,OUTPUT);
  pinMode(MZ80,INPUT);
  pinMode(RGB_R,OUTPUT);
  pinMode(RGB_G,OUTPUT);
  pinMode(RGB_B,OUTPUT);
  pinMode(DOKUNMA, INPUT);
  pinMode(LDR, INPUT);

  // seri monitörden izlemek için Serial monitörün başlatılması
  Serial.begin(9600);
 
}

void loop() {

  // START CODE
  // mz80 sensörü değer okur.
  int MZ80_STATUS = digitalRead(MZ80);
  // su seviyesi ölçülür
  int su_seviyesi_deger = analogRead(SU_SEVIYE);
  // hava ve nem değerleri ölçülür
  int chk = dht_sensor.read(HAVA_NEM);
  // toprak nem değeri ölçülür
  int toprak_deger = analogRead(TOPRAK_NEM); 
  // dokunma sensörüne dokunulup/dokunulmadığı ölçülür.
  int dokunma_check = digitalRead(DOKUNMA);
  // LDR sensöründen ışık değeri ölçülür
  int isik_degeri = analogRead(LDR);
  // 0.5 sn bekleme
  delay(500);
  // MZ80 cisim görüyor mu? Evet ise değer 0 olur.
  if(MZ80_STATUS == 0) {
    // Cisim var ise lcd ekran temizlenir ve SELAMLA değeri olan 'HOŞGELDİN' ekrana yazılır
    lcd.clear();
    lcd.setCursor(0,0); // İlk satırın başlangıç noktası
    lcd.print(SELAMLA);
    // 5 sn bekleme
    delay(5000);
   
  } else {
    // SAAT YAZDIR
    lcd.clear();
    lcd.setCursor(0,0); // İlk satırın başlangıç noktası
    
    // SAat değişkeni yardımıyla saatin ekrana yazılması
    Saat.updateTime();
    lcd.print(Saat.dayofmonth);
    lcd.print("/");
    lcd.print(Saat.month);
    lcd.print("/");
    lcd.print(Saat.year);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print(Saat.hours);
    lcd.print(":");
    lcd.print(Saat.minutes);
    // 3 sn beklenir
    delay(3000);
  }
  // Dokunma sensörüne dokunma var mı kontrol edilir.
  if(dokunma_check == 1) {
    // var ise dokunma değeri bir arttırılır
    touch_count += 1;
    // her zaman mod 4'ü alınır çünkü dokunmaya bağlı olarak 4 farklı bilgi lcd ekrandan verilmektedir.
    // mod 4 çünkü ilk değer sıfır (0-1-2-3-4 her bir değer için farklı bir bilgi lcd ekranda gösterilir.)
    touch_count = touch_count % 4;

    // Touch değerini takip etmek için geliştirme sürecinde kullanılan kodlar
    Serial.print("ICERI : ");
    Serial.println(touch_count);
   
  }
 
// dokunma sensörüne bir kere basıldıysa 
  if(touch_count == 1) {
    // lcd temizle
    lcd.clear();
    lcd.setCursor(0,0); // İlk satırın başlangıç noktası
    lcd.print("SU SEVIYESI: ");
    lcd.setCursor(0,1); // İkinci satırın başlangıç noktası
    lcd.print(su_seviyesi_deger/2); // su seviyesi değerini ekrana yazdır
    lcd.print("ml ");
    // 3 sn bekle
    delay(3000);
    // su seviyesi az ise ve mz80 bir cisim görmüyor ise aciktim işlevi çağrılır
    if (su_seviyesi_deger < 165 && MZ80_STATUS == 1){
        aciktim();
     }
  } else if (touch_count == 2) { // dokunma sensörüne iki kere dokunulmuşsa
        // lcd temizkle
      lcd.clear();
      lcd.setCursor(0,0); // İlk satırın başlangıç noktası
      lcd.print("Nem Orani: ");
      lcd.print((float)dht_sensor.humidity, 2); // nem oranını yazdır
      lcd.setCursor(0,1); // İkinci satırın başlangıç noktası
      lcd.print("Sicaklik: ");
      lcd.print((float)dht_sensor.temperature, 2); // sıcaklık değerini yazdır
      // 3 sn bekle
    delay(3000);
    // su seviyesi az ise ve mz80 bir cisim görmüyor ise aciktim işlevi çağrılır
    if (su_seviyesi_deger < 165 && MZ80_STATUS == 1){
        aciktim();
     }
  } else if (touch_count == 3) { // Dokunma sensörüne üç kere dokunulmuşsa
    // lcd ekranı temizle
    lcd.clear();
    lcd.setCursor(0,0); // İlk satırın başlangıç noktası
    
    Saat.updateTime(); // güncel zamanı al
    lcd.print(Saat.dayofmonth); // günü yazdır
    lcd.print("/");
    lcd.print(Saat.month); // ayı yazdır
    lcd.print("/");
    lcd.print(Saat.year); // yılı yazdır
    lcd.print(" ");
    lcd.setCursor(0,1); // İkinci satırın başlangıç noktası
    lcd.print(Saat.hours); // Saati yazdır
    lcd.print(":");
    lcd.print(Saat.minutes); // dakikayı yazdır
    
    delay(3000);
    // su seviyesi az ise ve mz80 bir cisim görmüyor ise aciktim işlevi çağrılır
    if (su_seviyesi_deger < 165 && MZ80_STATUS == 1){
        aciktim();
     }
  }

  // su seviyesi az ise ve mz80 bir cisim görmüyor ise aciktim işlevi çağrılır
    if (su_seviyesi_deger < 165 && MZ80_STATUS == 1){
        aciktim();
     }
    
    // Toprak nem seviyesi düşük mü ?
  if(toprak_deger< 100){
      ServoNesnesi.write(90);  /* Motorun mili 90. dereceye donuyor ve su akmaya başlar*/
        delay(4000); // dört saniye boyunca su akar
      ServoNesnesi.write(270);   /* Motor mili -90. dereceye donuyor ve su akışı durur*/
        delay(1000); // bir saniye bekle
  }
 

// Işık değeri idealdan az mı ? Hava karanlık mı ?
  if(isik_degeri<200){
    renkAyarla(80, 0, 80); // Renk ayarla işlevi çağrılır
  } else {
    // Hava aydınlık ise RGB led'in her renk değeri için LOW verilir ve ışık söner.
   analogWrite(RGB_R, LOW);
   analogWrite(RGB_G, LOW);
   analogWrite(RGB_B, LOW);
  }
 
   
 }

// veirlen kırmızı yeşik ve mavi oranına göre renk ayarlayan işlev
void renkAyarla(int kirmizi, int yesil, int mavi){
  kirmizi = 255-kirmizi;
  yesil = 255-yesil;
  mavi = 255-mavi;
  analogWrite(RGB_R, kirmizi);
  analogWrite(RGB_G, yesil);
  analogWrite(RGB_B, mavi);
  }

// Çarıldığında buzzer çalıştıran ve ekrana acıktım yazdıran işlev
void aciktim(){
//tone(BUZZER, 1000); // Send 1KHz ses sinyali çıkar
    delay(1000);        // 1 saniye boyunca çak
    lcd.clear();    // lcd temizle 
    lcd.setCursor(0,0); // İlk satırın başlangıç noktası
    lcd.print("ACIKTIM !!"); // acıktım yaz
    noTone(BUZZER);     // Buuzer durdur
    delay(2000); // 2 sn bekle
}
