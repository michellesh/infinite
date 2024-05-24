// By XTronical, www.xtronical.com, use as you wish
// Based on work and on the audio library of schreibfaul1
// See github page : https://github.com/schreibfaul1/ESP32-audioI2S
// Also has volume control via a potentiometer attached to pin 13
#include "Arduino.h"
#include "Audio.h"
#include "FS.h"
#include "SD.h"

#include "Timer.h"

// Digital I/O used
#define SD_CS 5
#define SPI_MOSI 23 // SD Card
#define SPI_MISO 19
#define SPI_SCK 18

#define I2S_DOUT 25
#define I2S_BCLK 27 // I2S
#define I2S_LRC 26

// #define VolPin 13

Audio audio;

uint8_t Volume; // range is 0 to 21

File rootDir;

#define MAX_FILES 100

char *fileNames[MAX_FILES];
int fileCount = 0;

void setup() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  Serial.begin(9600);
  if (!SD.begin(SD_CS)) {
    Serial.println("Error talking to SD card!");
    while (true)
      ; // end program
  }
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  rootDir = SD.open("/");
  PlayNextSong(); // Play next song, which will be the first at this point


  audio.setVolume(
      21); // GetVolume()); // Check volume level and adjust if necassary
}

Timer timer = {1000};

void loop() {
  audio.loop();

  static bool lowVol = false;
  if (timer.complete()) {
    // Serial.println("Play next");
    // audio.connecttoFS(SD, entry.name());
    lowVol = !lowVol;
    //audio.setVolume(lowVol ? 10 : 21);
    timer.reset();
  }
}

void audio_eof_mp3(const char *info) { // end of file
  PlayNextSong();
}

void PlayNextSong() {
  bool SongFound = false;
  bool DirRewound = false;

  while (SongFound == false) {
    File entry = rootDir.openNextFile();
    if (!entry) // no more files
    {
      if (DirRewound == true) // If we've already rewound once then there are no
                              // songs to play in this DIR
      {
        Serial.println("No MP3 files found to play");
        entry.close();
        return;
      }
      // else we've reached the end of all files in this directory, just rewind
      // back to beginning
      rootDir.rewindDirectory(); // reset back to beginning
      DirRewound = true;         // Flag that we've rewound
    } else {
      if (!entry.isDirectory()) // only enter this if not a DIR
      {
        if (MusicFile(entry.name())) // Only enter if one of the acceptable
                                     // music files
        {
          Serial.print("Playing ");
          Serial.println(entry.name());
          // audio.connecttoSD(entry.name()); // Play the file
          audio.connecttoFS(SD, entry.name()); // SD
          // audio.connecttoFS(SD_MMC, "/test.wav"); // SD_MMC
          // audio.connecttoFS(SPIFFS, "/test.wav"); // SPIFFS

          SongFound = true;
        }
      }
    }
    entry.close();
  }
}

bool MusicFile(String FileName) {
  // returns true if file is one of the supported file types, i.e. mp3,aac
  String ext;
  ext = FileName.substring(FileName.indexOf('.') + 1);
  if ((ext == "mp3") | (ext == "aac"))
    return true;
  else
    return false;
}

// uint8_t GetVolume() {
//   // looks at the ADC pin that the potentiometer is connected to.
//   // returns the value as a volume setting
//   // The esp32's ADC has linerality problems at top and bottom we will ignore
//   // them and only respond to values in the middle range
//
//   uint16_t VolumeSettingReading;
//
//   VolumeSettingReading = analogRead(VolPin);
//   if (VolumeSettingReading < 25) // because of problems mentioned above,
//                                  // anything below 25 will be 0 volume
//     return 0;
//   if (VolumeSettingReading > 4000) // because of problems mentioned above,
//                                    // anything above 4000 will be 21 volume
//     return 21;
//   // If we get this far we are in the middle range that should be linear
//   // 500-4000
//   return uint8_t(((VolumeSettingReading - 25) /
//                   190)); // this will give the correct 0-21 range
// }

void listFiles() {
  while (true) {
    File entry = rootDir.openNextFile();
    if (!entry) {
      // No more files
      break;
    }

    if (!entry.isDirectory()) {
      // Check if the file has a .mp3 extension
      const char *fileName = entry.name();
      const char *extension =
          strrchr(fileName, '.'); // Find the last occurrence of '.'
      if (extension != nullptr && strcmp(extension, ".mp3") == 0) {
        // Store the file name
        if (fileCount < MAX_FILES) {
          fileNames[fileCount] =
              strdup(fileName); // Allocate memory and copy the name
          fileCount++;
        } else {
          // Handle case where there are more files than the array can hold
          Serial.println("Too many files to store in array");
          break;
        }
      }
    }
    entry.close();
  }
}
