
import serial
import time
import spotipy
from spotipy.oauth2 import SpotifyOAuth
import webbrowser

# --- SPOTIFY API AYARLARI ---
CLIENT_ID = '25fbdf8a58724c38986fe45eb69498a8'
CLIENT_SECRET = '87a7852f02974700ae11c8b889eedd4c' 
REDIRECT_URI = 'http://127.0.0.1:8000/callback'
SCOPE = 'user-modify-playback-state'

# --- BLUETOOTH AYARLARI ---
BLUETOOTH_PORT = '/dev/cu.HC-05' 
BAUD_RATE = 9600

def authenticate_spotify():
    print("Spotify hesabına bağlanılıyor...")
    sp = spotipy.Spotify(auth_manager=SpotifyOAuth(
        client_id=CLIENT_ID,
        client_secret=CLIENT_SECRET,
        redirect_uri=REDIRECT_URI,
        scope=SCOPE
    ))
    return sp

def inatci_sarki_baslatici(sp):
    max_deneme = 5
    for deneme in range(max_deneme):
        try:
            sp.start_playback()
            print("🎵 Müzik başarıyla çalıyor!")
            return True
            
        except Exception as e:
            print(f"Deneme {deneme + 1}/{max_deneme} başarısız. Spotify nazlanıyor...")
            if deneme == 0:
                print("🌐 Aktif cihaz bulunamadı! Tarayıcıda Spotify Web Player açılıyor...")
                webbrowser.open('https://open.spotify.com')
            
            print("Cihazın uyanması bekleniyor (5 saniye)...")
            time.sleep(5) 
            
    print("❌ Tüm denemelere rağmen API müziği başlatamadı. Lütfen manuel kontrol edin.")
    return False

def main():
    sp = authenticate_spotify()
    print("Spotify API yetkilendirmesi başarılı!")

    try:
        # 1: timeout=1 eklendi. Böylece sinyal yarım gelirse Python sonsuza kadar beklemez, döngüye devam eder.
        ser = serial.Serial(BLUETOOTH_PORT, BAUD_RATE, timeout=1) 
        
        # 2: Bilgisayarda Bluetooth portu açıldıktan sonra veri hattının oturması için süre tanıyoruz.
        print(f"{BLUETOOTH_PORT} bağlantısı kuruldu. Sistemin oturması için 3 saniye bekleniyor...")
        time.sleep(3) 
        
        # 3: İçeride daha önceden kalmış çöp/hatalı veriler varsa temizliyoruz.
        ser.reset_input_buffer() 
        print("Sistem hazır. İçeceğin soğuması bekleniyor...")
        
    except Exception as e:
        print(f"Bluetooth portuna bağlanılamadı! Hata: {e}")
        return

    while True:
        try:
            if ser.in_waiting > 0:
                # 4: errors='ignore' eklendi. Gelen veride Bluetooth paraziti varsa kodun çökmesini engeller.
                mesaj = ser.readline().decode('utf-8', errors='ignore').strip()
                
                # Eğer gelen mesaj boş değilse ekrana yazdır
                if mesaj:
                    print(f"BİLGİ: Gelen ham veri: '{mesaj}'")
                
                # 5: '==' yerine 'in' kullanıldı. "PLAY" kelimesinin başında veya sonunda 
                # görünmez boşluklar/karakterler varsa bile yakalamasını sağlar.
                if "PLAY" in mesaj:
                    print("\n❄️ Hedef sıcaklığa ulaşıldı! Arduino'dan sinyal geldi.")
                    print("Spotify başlatılıyor...")
                    
                    inatci_sarki_baslatici(sp)
                    time.sleep(10)
            
            time.sleep(0.1)

        except KeyboardInterrupt:
            print("\nProgram kullanıcı tarafından durduruldu.")
            ser.close()
            break
        except Exception as e:
            print(f"Beklenmeyen bir hata oluştu: {e}")
            break

if __name__ == '__main__':
    main()