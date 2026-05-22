import serial
import time
import spotipy
from spotipy.oauth2 import SpotifyOAuth

CLIENT_ID = '25fbdf8a58724c38986fe45eb69498a8'
CLIENT_SECRET = '87a7852f02974700ae11c8b889eedd4c'
REDIRECT_URI = 'http://127.0.0.1:8000/callback' 
SCOPE = "user-modify-playback-state user-read-playback-state"

print("Spotify Entegration Started\n")

sp = spotipy.Spotify(auth_manager=SpotifyOAuth(
    client_id=CLIENT_ID,
    client_secret=CLIENT_SECRET,
    redirect_uri=REDIRECT_URI,
    scope=SCOPE,
    open_browser=True  
))

try:
    user_info = sp.current_user()
    print(f"Connection success, welcome {user_info['display_name']}\n")
except Exception as e:
    print("Spotify API connection error:", e)
    exit()


PORT = '/dev/rfcomm0'
BAUD_RATE = 115200

try:
    ser = serial.Serial(PORT, BAUD_RATE, timeout=0.5)
    time.sleep(2)
    print("✓ ESP32 Bluetooth connection success - waiting for LDR gestures...")
    
    while True:
        line = ser.readline()
        if line:
            command = line.decode('utf-8', errors='ignore').strip()
            print(f"Command: {command}")
            
            try:
                if command == "NEXT":
                    print(">>> Next track")
                    sp.next_track()
                elif command == "PREV":
                    print("<<< Previous track")
                    sp.previous_track()
            except Exception as spotify_error:
                print("Spotify API Error (should have a song already playing):", spotify_error)

except serial.SerialException as e:
    print(f"Serial port error - is rfcomm0 connected?: {e}")
except KeyboardInterrupt:
    print("\nTerminated.")