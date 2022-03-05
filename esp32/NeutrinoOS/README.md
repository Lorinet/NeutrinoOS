# Neutrino Core OS ESP32 port

The ESP32 port is built on top of the FreeRTOS kernel, which takes care of low-level tasks, like CPU thread scheduling, SMP and memory allocation.

## Compile NeutrinoOS for ESP32
- Download ESP-IDF 4.3 and set it up: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/
- Copy this folder to ESP-IDF directory (this is the project folder)
- Copy source, include and components to the `main` folder inside `NeutrinoOS`
- Launch ESP-IDF shell, `cd NeutrinoOS` and run `idf.py build` for build, `idf.py flash` to upload and `idf.py monitor` to see boot log

## ESP32 File System Structure
The ESP32 port of NeutrinoOS uses SPIFFS as a file system on the built-in flash memory. SPIFFS is, however, limited compared to other file systems. Some things to note:
- Maximum file name length is 256 characters (full path included).
- No directories. Directories are simulated by prefixing file name with parent directory path, e.g. `/neutrino/folder/hello.txt`. This works for files, but when listing directory contents, there is no way to get directory names. The workaround for this is, for now, a file named `.neutrino` in every "directory", containing child directory names, one per line. The file system driver automatically manages the `.neutrino` file, but when deploying files, there is currently no automatic way to generate these files, so these will have to be created manually. Given the current development state of the system, this isn't a big problem, because currently the rootfs does not contain so many directories.
- Only 5 files can be opened at the same time. This is not a problem, because NeutrinoOS does not support file streams yet. This number can be modified from main.cpp.
- On ESP32, the Neutrino Core OS binary runs from a separate `factory` partition. The data partition, named `storage`, which is formatted with SPIFFS, is mounted as the `0:` drive, and should contain the Neutrino userspace.

### Adding files to data partition
To add files to this partition, copy them to the `spiffs_image` folder, and run `idf.py flash` again. The ESP-IDF will automatically create a SPIFFS image from these files, and flash the `storage` partition alongside the `factory` partition.

## Configure ESP32 features
- Modifying project configuration is not recommended, as changing these options may break NeutrinoOS compatibility. These are already optimized for NeutrinoOS.
- To modify these options, use `idf.py menuconfig` in the `NeutrinoOS` folder.