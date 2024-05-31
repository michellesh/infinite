#include "Arduino.h"
#include "Audio.h" // https://github.com/schreibfaul1/ESP32-audioI2S
#include "FS.h"
#include "SD.h"

// SD Card
#define SD_CS 5
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18

// MAX98357 audio amplifier
// GAIN pin tied to ground
// SD pin floating
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

#define MAX_VOLUME 21
#define MAX_FILES 100

Audio audio;

File rootDir;

char *fileNames[MAX_FILES];
int fileCount = 0;
int trackNumber = 0;

void setup() {
  Serial.begin(9600);

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed!");
    return;
  }

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(MAX_VOLUME);

  rootDir = SD.open("/");
  if (!rootDir) {
    Serial.println("Failed to open root directory");
    return;
  }
  // Ensure the directory is a valid directory
  if (!rootDir.isDirectory()) {
    Serial.println("Root is not a directory");
    return;
  }

  listFiles(rootDir);

  // Print the file names
  Serial.println("MP3 files found on SD card:");
  for (int i = 0; i < fileCount; i++) {
    Serial.println(fileNames[i]);
  }

  playTrack(0);
}

void loop() { audio.loop(); }

void audio_eof_mp3(const char *info) { // end of file
  trackNumber++;
  if (trackNumber < fileCount) {
    playTrack(trackNumber);
  }
}

void playTrack(int index) {
  Serial.print("playing ");
  Serial.println(fileNames[index]);
  audio.connecttoFS(SD, fileNames[index]);
}

void listFiles(File dir) {
  while (true) {
    File entry = dir.openNextFile();
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
