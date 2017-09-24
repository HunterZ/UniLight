# UniLight
Synchronize Dell/Alienware LightFX/AlienFX and/or Logitech Gaming LED devices with Windows accent/colorization color

## Background
I have an Alienware 17R3 laptop and have been using AlienFX WinTheme to sync the color my LED's with the Windows 10 accent color. However, I just purchased a Logitech G502 Proteus Spectrum and wanted to have it work the same way. This inspired me to write a C++ application that accomplishes the same thing as AlienFX WinTheme but also adds support for Logitech RGB LED equipped devices.

## Usage
Simply launch UniLight and forget about it. Configure Windows to launch it at startup if you want.

UniLight appears as a system tray icon that looks like a color wheel, and polls the Windows accent color at a rate of 1Hz (to minimize CPU usage). On startup and whenever any changes are detected, it calls the LightFX/AlienFX and Logitech Gaming LED APIs to set all RGB LEDs to the current accent color.

Hovering the mouse cursor over the UniLight tray icon will produce a tooltip containing status information, including the current and last accent colors, and whether they were successfully applied to Alienware and/or Logitech devices.

Right-clicking the tray icon will bring up an about/exit menu, while left-clicking brings up the About popup.

## System Requirements
I'm not 100% sure about these. I tried to implement the Alienware and Logitech API access in such a way that it will fail gracefully if either API is not supported on your system. UniLight will also attempt to re-apply the color on every color change, so that there is some hope of avoiding a restart if relevant peripherals are (re)connected after UniLight has already been launched. I haven't tried to make it any more aggressive because I have to poll both APIs (neither provides a notification callback mechanism) and I don't want UniLight to have a noticeable performance impact on gaming or other tasks.

UniLight should load the AlienFX and/or Logitech Gaming LED DLLs that you have installed as part of Alienware Command Center (minimum version unknown; I'm currently on 4.5.19) and Logitech Gaming Software (version 8.55 or higher required for LED support), respectively. Also, the binary distribution of this program is 32-bit for maximum compatibility (I'm running it on 64-bit systems, so I know it works there).

## Tools used
This project was created with Microsoft Visual Studio Community 2015, Alienware AlienFX 1.0 SDK (formerly Dell LightFX), and Logitech Gaming LED SDK.

## License
All code contained in this repository is covered by the MIT open source license. See the LICENSE file for details. Usage of this code must be attributed to GitHub user HunterZ.
