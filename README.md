# OpenNextion openHASP

[![English](https://img.shields.io/badge/lang-English-blue)](./README.md)
[![中文](https://img.shields.io/badge/lang-中文-red)](./README.zh-CN.md)

<p align="center"><img src="docs/images/opennextion-openhasp-hero.jpg" alt="OpenNextion openHASP panel in use" width="820"></p>

Turn a supported OpenNextion ESP32-S3 touchscreen into a configurable MQTT control panel for Home Assistant. Flash the matching firmware, connect the panel to Wi-Fi, configure MQTT, then use its web interface and page files to build controls for lights, climate, media, scenes, and other home-automation devices.

> This is an OpenNextion hardware port of [openHASP](https://github.com/HASwitchPlate/openHASP). It is not an official OpenNextion or Home Assistant product.

## Supported hardware

| OpenNextion model | Display | Orientation | PlatformIO environment | Status |
| --- | --- | --- | --- | --- |
| [ONX3248G035][onx3248g035] V1.2 | 3.5-inch capacitive touch, 320 × 480 | Portrait | `onx3248g035` | Verified |
| [ONX2432G028][onx2432g028] V1.3 | 2.8-inch capacitive touch, 240 × 320 | Portrait | `onx2432g028` | Verified |

**Only flash firmware built for the exact device model. Do not interchange firmware between these two panels.**

The current port supports the display, touch input, backlight, Wi-Fi, LittleFS, and PSRAM. Audio, RTC, SD card, camera, and PCF8574A-expanded IO are not currently supported.

## Before you start

- One of the supported OpenNextion panels.
- A USB cable that supports data, not only charging.
- A Windows, macOS, or Linux computer.
- A 2.4 GHz Wi-Fi network. ESP32-S3 does not support 5 GHz-only Wi-Fi.
- An MQTT broker. For Home Assistant, the Mosquitto Broker add-on is a common choice.

## Quick start

### 1. Get the correct firmware

Release firmware will be published on [GitHub Releases](https://github.com/OpenNextion/OpenNextion-Example-openHASP/releases).

| Device | Initial-flash firmware | OTA firmware |
| --- | --- | --- |
| ONX3248G035 V1.2 | To be added | To be added |
| ONX2432G028 V1.3 | To be added | To be added |

There are no release binaries yet. For now, build and upload the firmware from source as described in [Build from source](#build-from-source). When releases are available, use a complete initial-flash image for first setup, and use an OTA package only when it explicitly names your device model.

### 2. Flash the panel

Connect the panel with a data-capable USB cable and upload firmware for its exact model. The current source-build commands are:

```sh
# ONX3248G035 V1.2
pio run -e onx3248g035 -t upload

# ONX2432G028 V1.3
pio run -e onx2432g028 -t upload
```

If no serial port appears, try another USB cable or port first. If uploading cannot connect, put the board into download mode using its BOOT and Reset controls, then retry.

### 3. Connect the panel to Wi-Fi

On first boot, or after Wi-Fi settings are cleared, the panel creates a temporary access point named `HASP-xxxxxx`. The password is `haspadmin`.

1. Scan the QR code on the panel, or connect a phone or computer to the temporary access point.
2. Follow the on-screen Wi-Fi setup prompt. Select your 2.4 GHz network and enter its password.
3. Save the settings and wait for the panel to restart and join your home network.
4. Find its IP address in your router's client list, then open that address in a browser.

The panel's web interface is where you check its status, configure MQTT, edit files, update firmware, and reset the device.

<p align="center"><img src="docs/images/opennextion-openhasp-wifi-setup.jpg" alt="OpenNextion openHASP first-time Wi-Fi setup" width="620"></p>


### 4. Configure MQTT and Home Assistant

In the panel web interface, open **Settings → MQTT Settings** and set:

- **Broker**: IP address or hostname of your MQTT broker.
- **Port**: normally `1883`.
- **Username** and **Password**: if your broker requires authentication.
- **Hostname**: a unique name for this panel, for example `livingroom_panel`.
- **Node Topic**: keep the default unless you use a custom MQTT topic layout.

Save the settings and wait for the panel to reconnect. If Home Assistant's MQTT integration uses the same broker, check **Settings → Devices & services → MQTT** for the panel and its discovered entities. If it does not appear, first confirm Wi-Fi and MQTT are connected on the panel's **Information** page.

<p align="center"><img src="docs/images/opennextion-openhasp-home-assistant.png" alt="OpenNextion openHASP in Home Assistant" width="820"></p>

### 5. Create your first panel page

The default page confirms that the display, touch, and MQTT connection work. Open **File Editor** in the panel web interface to view, upload, or edit page files and images. Download a backup before changing files.

Use Home Assistant automations or MQTT messages to update labels, icons, colors, values, and pages. The [openHASP documentation](https://www.openhasp.com/) describes page objects and MQTT commands.

## Everyday use

| Task | Where to do it |
| --- | --- |
| Check IP address, Wi-Fi, MQTT, and firmware details | **Information** in the panel web interface |
| Change Wi-Fi, MQTT, display, or time settings | **Settings** in the panel web interface |
| Back up or edit pages and images | **File Editor**; back up first |
| Update firmware | **Firmware Update**, using only an OTA file explicitly made for this model |
| Start over | **Factory Reset**; this erases settings and internal files |

## Troubleshooting

### The computer cannot see the panel

Use a data-capable USB cable and try another USB port. Some cables provide power only. If the board still has no serial port, install the USB serial driver required by your operating system.

### Uploading cannot connect

Disconnect and reconnect USB, then retry. If necessary, hold **BOOT**, press and release **Reset**, release **BOOT**, and upload again.

### The build stops at `ft2build.h: No such file or directory`

The FreeType Git submodule is missing. From the project directory, run:

```sh
git submodule update --init --recursive
```

Then build again.

### The panel cannot join Wi-Fi

Check that the selected network provides 2.4 GHz Wi-Fi and that its credentials are correct. If needed, factory-reset the panel and repeat the first-time Wi-Fi setup.

### Home Assistant does not show the panel

Make sure Home Assistant and the panel use the same MQTT broker. Confirm the MQTT connection on the panel's **Information** page, then restart the panel and wait for it to reconnect.

### The display is wrong or the wrong firmware was flashed

Reflash a complete firmware image for the exact panel model over USB. Do not install an OTA package intended for the other model.

## Enclosures and images

<!-- MEDIA TODO — enclosure and installation photos
Suggested paths:
  docs/images/onx3248g035-openhasp-enclosure.jpg
  docs/images/onx2432g028-openhasp-enclosure.jpg
Provide: one well-lit angled photo per model showing the panel fitted in its printed enclosure. Also provide the final public URL for each downloadable 3D model (for example MakerWorld).
-->

3D-printable desktop enclosures and installation photos will be added later.

| Model | 3D enclosure |
| --- | --- |
| ONX3248G035 V1.2 | Link to be added |
| ONX2432G028 V1.3 | Link to be added |

## Build from source

This section is for users who need firmware before release binaries are available, or who want to modify the project.

1. Install [PlatformIO](https://platformio.org/) and clone this repository with its submodules:

   ```sh
   git clone --recurse-submodules https://github.com/OpenNextion/OpenNextion-Example-openHASP.git
   cd OpenNextion-Example-openHASP
   ```

   If you already cloned the repository, initialize the required FreeType submodule:

   ```sh
   git submodule update --init --recursive
   ```

2. Copy `platformio_override-template.ini` to `platformio_override.ini`.

3. In `platformio_override.ini`, uncomment this line under `[platformio]`:

   ```ini
   user_setups/esp32s3/*.ini
   ```

4. Under `[override]`, uncomment one matching target under the **Nextion** section:

   ```ini
   onx3248g035
   ; or
   onx2432g028
   ```

5. Build or upload from the project directory:

   ```sh
   pio run -e onx3248g035
   pio run -e onx3248g035 -t upload
   ```

The board configurations are [onx3248g035.ini](user_setups/esp32s3/onx3248g035.ini) and [onx2432g028.ini](user_setups/esp32s3/onx2432g028.ini).

## Credits, license, and disclaimer

- Upstream project: [openHASP](https://github.com/HASwitchPlate/openHASP)
- Graphics library: [LVGL](https://lvgl.io/)
- OpenNextion: <https://github.com/OpenNextion>

This project follows the repository [LICENSE](LICENSE). Third-party components may have their own license notices.

Flashing third-party firmware carries a risk of device damage or data loss. Use firmware only after confirming that it matches your panel model, and keep backups of panel files before changing them.

[onx3248g035]: https://nextion.tech/wiki/onx3248g035/
[onx2432g028]: https://nextion.tech/wiki/onx2432g028/
