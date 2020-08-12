# Leap Motion Controller Emulation - SteamVR Driver
[![Build status](https://ci.appveyor.com/api/projects/status/2pc49d2hpt2hx944?svg=true)](https://ci.appveyor.com/project/SDraw/driver-leap) [![Release](http://img.shields.io/github/release/SDraw/driver_leap.svg)](../../releases/latest)

A Fork with updated vendor libraries and extended features.

[![](./.github/repository_img.png)](https://www.youtube.com/playlist?list=PLiEPsxTlqsDk5GKcgsmeDQNRs7KV8lI-s)

### Contents
* [Installation (for users)](#installation)
* [Driver Control configuration and Inputs](#driver-control-configuration-and-inputs)
* [Controller emulation](#controller-emulation)
  * [HTC Vive](#htc-vive)
  * [Valve Index](#valve-index)
* [Troubleshooting](#troubleshooting)
* [Building (for developers)](#building)

## Installation 
*for users*

```
Default "SteamVR_folder" Location: C:\Program Files (x86)\Steam\steamapps\common\SteamVR
Default "Steam_folder" Location: C:\Program Files (x86)\Steam
```

  * Install [latest Leap Motion Orion Beta](https://developer.leapmotion.com/get-started)
  * You should already have Steam and SteamVR installed.
  * Create `leap` folder in <code>*<SteamVR_folder>*/drivers</code>
  * Grab [latest release archive](https://github.com/SDraw/driver_leap/releases/latest) (.zip) for your platform
  * Extract archive to <code>*<SteamVR_folder>*/drivers/leap</code>
  * Edit steamvr configuration in <code>*<Steam_folder>*/config/steamvr.vrsettings</code> file 
    * If you are **using a HMD** already,
      * Add below line in section 'steamvr'
      ```JSON
      "activateMultipleDrivers": true,
      ```
    * If you want to try **without HMD**,
      * Add following lines in 'steamvr' section, and
      ```JSON
      "activateMultipleDrivers": true,
      "requireHmd" : false,
      "forcedDriver" : "null"
      ```
      * Add another section 'driver_null', if it dosen't exist already (you can change renderHeight/Width and windowHeight/Width below)
      ```JSON
      "driver_null" : {
        "displayFrequency" : 60,
        "enable" : true,
        "modelNumber" : "Null Model Number",
        "renderHeight" : 1080,
        "renderWidth" : 1920,
        "secondsFromVsyncToPhotons" : 0.01111111,
        "serialNumber" : "Null Serial Number",
        "windowHeight" : 1080,
        "windowWidth" : 1920,
        "windowX" : 0,
        "windowY" : 0
      }
      ``` 
      
## Driver Control configuration and Inputs
Driver can emulate HTC Vive controllers and Valve Index controllers with skeletal animation. Works in both desktop and HMD orientations of leap controller. Which controller to emulate and in what orientation leap controller is being used in, can be adjusted by editing `settings.xml` in <code>*<SteamVR_folder>*/drivers/leap/resources</code> folder. 

There are more configurable restrictions, such as global input, trackpad, trigger, grip and etc. in `settings.xml`. Check [wiki page](../../wiki/Offset-settings) for few offset settings contributed by users.
Controls are changed by "Game Profiles" that are enabled automatically when respective game is started from Steam.

Available Hotkeys in NumLock active state:
* **Ctrl-P:** Enable/disable right hand controller.
* **Ctrl-O:** Enable/disable left hand controller.
* **Ctrl-\ (or Ctrl-|)** Reload configuration.

## Controller Emulation
Real VR controllers give button touches, button presses, hand orientation feedbacks to SteamVR, which are then relayed to Game applications for different actions in-game. This driver emulates such feedback via gesture recognition by Leap motion controller and provides it to SteamVR. Preconfigured gestures are specified below for respective controller types and game profiles.

### HTC Vive
<img src="./.github/ViveC.png" height="475px">

Game profiles:
  * **default**  
  Controls list:
    * Application menu - hand with palm directed towards face    
    * Touchpad
      - Touchpad press - thumb press
      - Touchpad circle - index finger of one hand directed towards other hand's palm
    * System menu - T-shape with two hands
    * Trigger - bending of the index finger
    * Grip - grab gesture
  * **vrchat** - profile for VRChat. Control restrictions are ignored.  
  Controls list:
    * Gun - corresponding hand gesture
    * V-shape - corresponding hand gesture
    * Point - corresponding hand gesture
    * Rock out - corresponding hand gesture
    * Thumbs up - corresponding hand gesture
    * Spread hand - corresponding hand gesture. Also corresponds to grip button.
    * Trigger - grab gesture
    * Application menu - T-shape with two hands
    
### Valve Index 
<img src="./.github/IndexC.png" height="500px">

Game profiles:
  * **default**  
  Controls list:  
    * Button A - touching of thumb and middle fingertips
    * Button B - touching of thumb and pinky fingertips
    * Touchpad
      - Touchpad - thumb press
      - Touchpad cycle - index finger of one hand directed towards other hand's palm
    * Thumbstick
      - Thumbstick press - touching of thumb fingertip of one hand to index fingertip of other hand
      - Thumbstick direction - arrow keys for left hand, Num2/8/4/6 keys for right hand; available when NumLock is active    
    * Trigger - bending of the index finger
    * Grip - bending of middle, ring and pinky fingers
    * System button - T-shape with two hands
  * **vrchat** - profile for VRChat. Note: game gestures are not implemented due to finger tracking, grip input profile should be used.  
  Controls list:
    * Trigger - bending of the index finger
    * Grip - grab gesture
    * Game menu - T-shape with two hands

### Runtime Configuration

Sometimes it is possible that the Real Hand is not exactly represented in VR Application, so it is required adjust/offset in x,y,z directions. Dynamic Offsetting for `Left,Right hand spatial and rotational offsets` can be done through special keys, as follows,

<img src="https://di4564baj7skl.cloudfront.net/documentation/images/Leap_Axes.png" >

<kbd>SCL</kbd>  - `Scroll Lock: On`
<kbd>NML</kbd> - `Num Lock: On`
<kbd>Caps</kbd> - `Caps Lock: On`

* **LeftHand Spatial** Offset Increment(+ve)
    - x = <kbd>SCL</kbd> + <kbd>U</kbd>
    - y = <kbd>SCL</kbd> + <kbd>I</kbd>
    - z = <kbd>SCL</kbd> + <kbd>O</kbd>
    
* **LeftHand Rotational** Offset Increment(+ve)
    - x = <kbd>SCL</kbd> + <kbd>NML</kbd> + <kbd>U</kbd>
    - y = <kbd>SCL</kbd> + <kbd>NML</kbd> + <kbd>I</kbd>
    - z = <kbd>SCL</kbd> + <kbd>NML</kbd> + <kbd>O</kbd>
    - w = <kbd>SCL</kbd> + <kbd>NML</kbd> + <kbd>P</kbd>

* **RightHand Spatial** Offset Increment(+ve)
    - x = <kbd>Caps</kbd> + <kbd>SCL</kbd> + <kbd>U</kbd>
    - y = <kbd>Caps</kbd> + <kbd>SCL</kbd> + <kbd>I</kbd>
    - z = <kbd>Caps</kbd> + <kbd>SCL</kbd> + <kbd>O</kbd>
    
* **RightHand Rotational** Offset Increment(+ve)
    - x = <kbd>Caps</kbd> + <kbd>SCL</kbd> + <kbd>NML</kbd> + <kbd>U</kbd>
    - y = <kbd>Caps</kbd> + <kbd>SCL</kbd> + <kbd>NML</kbd> + <kbd>I</kbd>
    - z = <kbd>Caps</kbd> + <kbd>SCL</kbd> + <kbd>NML</kbd> + <kbd>O</kbd>
    - w = <kbd>Caps</kbd> + <kbd>SCL</kbd> + <kbd>NML</kbd> + <kbd>P</kbd>

* For **Negative increments**(decrements(-ve)), <kbd>SHIFT</kbd> key should also be used along with above combinations.

* **Saving offsets** to *Config file* : <kbd>NML</kbd> + <kbd>Caps</kbd> + <kbd>SCL</kbd> + <kbd>SHIFT</kbd> + <kbd>G</kbd>
* **Resetting offsets** to default values : <kbd>NML</kbd> + <kbd>Caps</kbd>+ <kbd>SCL</kbd> + <kbd>R</kbd>

## Troubleshooting
Sometimes installation of [base project driver](https://github.com/cbuchner1/driver_leap) doesn't register driver folder for SteamVR. To manually add it:
  * Open console as administrator in <code>*<SteamVR_folder>*/bin/[win32 or win64]</code>
    and execute command:
    ```
    vrpathreg adddriver "path_to_leap_folder"
    ```
    With default leap folder path, the command should look something like:
    ```
    vrpathreg adddriver "C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers\leap"
    ```
  * Check if driver folder is added by calling `vrpathreg` without any arguments.
  * Open <code>*<Steam_folder>*/config/steamvr.vrsettings</code> file and add line in `steamvr` section:
    ```JSON
    "activateMultipleDrivers": true,
    ```
  * You can also check if the driver emulated controllers are registered with SteamVR by using the command `vrcmd` in the same `bin` folder as above.

## Building 
*for developers*

* Open 'driver_leap.sln' solution in **Visual Studio 2013** and **Visual Studio 2017** (Other versions of Visual Studio have to build their own vendor libraries using CMake and retarget solution)
* Build the solution as per your platform. Output files for:
  * x64 - in `bin/win64`, in solution folder.
  * x86 - in `bin/win32`, in solution folder.
* Copy the following built files to <code>*<SteamVR_folder>*/drivers/leap/bin/<your_platform></code>:
  * `driver_leap.dll`
  * `gesture_checker.exe`
  * `leap_monitor.exe`<br/>
  **Note:** There are post-build events for projects to copy build files directly to SteamVR driver folder that can be enabled manually.
* Copy additional shared vendor libraries from solution folder to <code>*<SteamVR_folder>*/drivers/leap/bin/<your_platform></code>: 
  * vendor/LeapSDK/bin/<your_platform>/LeapC.dll
  * vendor/openvr/bin/<your_platform>/openvr_api.dll
* Copy `resources` folder from solution root to driver's `leap` folder (<code>*<SteamVR_folder>*/drivers/leap</code>).
* For runtime debugging and breakpointing attach MSVS debugger to `vrserver.exe` process.
