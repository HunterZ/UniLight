# UniLight
Synchronize Corsair CUE, Dell/Alienware LightFX/AlienFX, Logitech LED, and/or Razer Chroma devices with Windows accent/colorization color

## Demo
https://youtu.be/I3xfJtbLqgA

## Background
I have an Alienware 17R3 laptop and have been using AlienFX WinTheme to sync the color my LED's with the Windows 10 accent color. However, I just purchased a Logitech G502 Proteus Spectrum and wanted to have it work the same way. This inspired me to write a C++ application that accomplishes the same thing as AlienFX WinTheme but also adds support for Logitech RGB LED equipped devices.

## Usage
Download latest version from: https://github.com/HunterZ/UniLight/releases

Simply launch UniLight and forget about it. Configure Windows to launch it at startup if you want.

UniLight appears as a system tray icon that looks like a color wheel, and listens for changes to accent color or other conditions that may affect LED color synchronization. On startup and whenever any relevant changes are detected, it calls the various APIs to set all RGB LEDs to the current accent color.

Hovering the mouse cursor over the UniLight tray icon will produce a tooltip containing status information, including the current and last accent colors, and whether they were successfully applied to each of the vendor APIs.

Right-clicking the tray icon will bring up a context menu with self-explanatory selections, while left-clicking performs a manual color synchronization.

## Troubleshooting
UniLight may comes with a Corsair CUE DLL because they make me link a loader library into UniLight that prevents it from working at all when the DLL is not present. If you are a Corsair CUE user and have issues, you may want to try renaming/deleting the included DLL to ensure that the one included with your driver version is used instead.

DLLs for the other APIs are not included. UniLight simply skips over any APIs for which it fails to load/initialize a DLL.

## System Requirements
I'm not 100% sure about these. I tried to implement the API access in such a way that it will fail gracefully if a hardware vendor API is not supported on your system. UniLight will also attempt to re-apply the color on every color change, so that there is some hope of avoiding having to restart UniLight if relevant peripherals are (re)connected after UniLight has already been launched. I haven't tried to make it any more aggressive because I have to poll all APIs (none provide a notification callback mechanism) and I don't want UniLight to have a noticeable performance impact on gaming or other tasks.

UniLight should load the AlienFX, Logitech Gaming LED, and/or Razer Chroma DLLs that you have installed as part of Alienware Command Center (minimum version unknown; I'm currently on 4.5.19), Logitech Gaming Software (version 8.55 or higher required for LED support), and/or Razer Synapse (tested with 3.3.1018.10194 / Chroma SDK 2.21.1), respectively. Also, the binary distribution of this program is 32-bit for maximum compatibility (I'm running it on 64-bit systems, so I know it works there).

As of version 1.1, UniLight is completely event-driven and no longer polls the Windows accent color on a timer. The result should be extremely minimal CPU usage.

## Tools used
This project was created with:
* Microsoft Visual Studio Community 2017: https://www.visualstudio.com/vs/community/
* Alienware AlienFX 1.0 SDK (formerly Dell LightFX): http://www.dell.com/support/home/us/en/19/drivers/driversdetails?driverId=T5GGP
* Corsair CUE SDK (Protocol version 4): http://forum.corsair.com/v3/showthread.php?t=156813
* Logitech LED Illumination SDK: https://www.logitechg.com/en-us/developers
* Razer Chrome SDK: https://developer.razer.com/

## License
All code contained in this repository and binaries built from it are covered by the MIT open source license. See the LICENSE file for details. Usage of this code must be attributed to GitHub user HunterZ.

Included DLLs are owned by the hardware vendor companies (Corsair, Dell/Alienware, Logitech, Razer). I assert no rights or support responsibilities for these.
