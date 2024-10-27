# LXDrummer: ESP8266 Drum Machine

LXDrummer is a straightforward drum machine based on the ESP8266, featuring an 8x8 LED matrix display controlled by a rotary encoder. This project allows you to create and play drum patterns with an intuitive interface.

# Demo
<a href="http://www.youtube.com/watch?feature=player_embedded&v=fs34gqKAlCA
" target="_blank"><img src="http://img.youtube.com/vi/fs34gqKAlCA/0.jpg" 
alt="LXDrummer Demo" width="240" height="180" border="10" /></a>

## Modules Used

- **ESP8266**: Should also work with ESP32 but requires code improvements.
- **MAX72xx 8x8 Matrix**: For the LED display.
- **Digital Encoder with Button**: For user input.
- **PCM5102A External DAC**: For audio output.

## Wiring

### ESP8266 to PCM5102A DAC

| PCM5102 Pin | ESP8266 IO  |
|-------------|-------------|
| BCK         | IO 15       |
| DIN         | IO 3        |
| LCK         | IO 2        |
| VIN         | 3.3V        |
| GND         | GND         |

### Encoder Connections

| Encoder Pin | ESP8266 IO       |
|-------------|-----------------|
| +           |            3.3V |
| GND         |            GND  |
| CLK         | GPIO 5          |
| DT          | GPIO 4          |
| SW          | GPIO 0          |

### MAX72xx LED Matrix

| Matrix Pin  | ESP8266 IO      |
|-------------|-----------------|
| VCC         | 3.3V |
| GND         | GND  |
| CLK         | GPIO 14         |
| CS          | GPIO 16         |
| DIN         | GPIO 13         |

## Installation

1. **Clone the Repository**: 
   ```bash
   git clone https://github.com/alextereshyt/LXDrummer.git
   ```
2. **Install Dependencies**:
   - Install the required libraries (ESP8266WiFi, MD_MAX72xx, I2S, SPI, AsyncTimer).
3. **Upload the Code**:
   - Use the Arduino IDE to upload the `LXDrummer.ino` file to your ESP8266.

## Usage

- **Adjust BPM**: Rotate the encoder to change the BPM.
- **Pause/Play**: Press the encoder button to pause or play the drum pattern.
- **Pattern Mode**: Double-click the encoder button to toggle pattern mode.
- **Edit Patterns**: Use the encoder to navigate and edit drum patterns on the LED matrix.

## Contributions

Feel free to submit issues or pull requests. Any improvements or bug fixes are welcome!

## License

This project is licensed under the MIT License. See the LICENSE file for details.

---

This project was based on this interesting article https://www.hackster.io/janost/audio-hacking-on-the-esp8266-fa9464. Thx to author)
Enjoy creating music with LXDrummer! If you have any questions or need assistance, please open an issue on the GitHub repository.

---
