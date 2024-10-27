# LXDrummer: A Simple Drum Machine

LXDrummer is a straightforward drum machine based on the ESP8266, featuring an 8x8 LED matrix display controlled by a rotary encoder. This project allows you to create and play drum patterns with an intuitive interface.

# Demo

[<iframe width="560" height="315" src="https://www.youtube.com/embed/fs34gqKAlCA?si=7mwSbnYuY-bkFLfs" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>](https://www.youtube.com/watch?v=fs34gqKAlCA)

## Modules Used

- **ESP8266**: Should also work with ESP32 but requires code improvements.
- **MAX72xx 8x8 Matrix**: For the LED display.
- **Digital Encoder with Button**: For user input.
- **PCM5102A External DAC**: For audio output.

## Wiring

### ESP8266 to PCM5102A DAC

| ESP8266 Name | ESP8266 IO | PCM5102 Pin |
|--------------|-------------|-------------|
| MTDO         | IO 15       | BCK         |
| U0RXD        | IO 3        | DIN         |
| GPIO2        | IO 2        | LCK         |
| Connect to 3.3V | VIN       |             |
| Connect to GND  | GND       |             |

### Encoder Connections

| Encoder Pin | Connection      |
|-------------|-----------------|
| +           | Connect to 3.3V |
| GND         | Connect to GND  |
| CLK         | GPIO 5          |
| DT          | GPIO 4          |
| SW          | GPIO 0          |

### MAX72xx LED Matrix

| Matrix Pin  | ESP8266 IO      |
|-------------|-----------------|
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

Enjoy creating music with LXDrummer! If you have any questions or need assistance, please open an issue on the GitHub repository.

---
