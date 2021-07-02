#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "vmmgr.h"
#include "esp_task_wdt.h"
#include "esp_spiffs.h"
#include "viewmgr.h"
#include "memorystats.h"
#include "esp_log.h"
#include "startup.h"
#include "kernlog.h"

//#include "lvmgr.h"
#include "u8g2_esp32_hal.h"

extern "C"
{
    void app_main(void);
}
void neutrino(void* para)
{
    cout << endl << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl << endl;
    klog("Neutrino Core OS");
    // REQUIRED to determine heap base address
	memorystats::init();
    klog("ESP32MemoryManager", "Kernel heap base address: " + to_string((int)memorystats::basePtr));
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/neutrino",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = false
    };
    klog("SPIFFS", "Configuring filesystem driver on partition /neutrino");
    esp_vfs_spiffs_register(&conf);
    klog("SPIFFS", "File system driver loaded");
    NeutrinoStartup();
    vTaskDelete(NULL);
}
void app_main()
{
	//testdisplay();
    // values for legacy scheduler
    //xTaskCreate(neutrino, "NeutrinoVMM", 16384, NULL, 0, NULL);
    xTaskCreate(neutrino, "NeutrinoVMM", 8192, NULL, 0, NULL);
}
