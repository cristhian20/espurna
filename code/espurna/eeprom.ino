/*

EEPROM MODULE

*/

#include <EEPROM_Rotate.h>

// -----------------------------------------------------------------------------

bool eepromBackup() {
    // Backup data to last sector if we are using more sectors than the
    // reserved by the memory layout
    if (EEPROMr.size() > EEPROMr.reserved()) {
        DEBUG_MSG_P(PSTR("[EEPROM] Backing up data to last sector\n"));
        return EEPROMr.backup();
    }
}

String eepromSectors() {
    String response;
    for (uint32_t i = 0; i < EEPROMr.size(); i++) {
        if (i > 0) response = response + String(", ");
        response = response + String(EEPROMr.base() - i);
    }
    return response;
}

#if TERMINAL_SUPPORT

void _eepromInitCommands() {

    settingsRegisterCommand(F("EEPROM.DUMP"), [](Embedis* e) {
        EEPROMr.dump(settingsSerial());
        DEBUG_MSG_P(PSTR("\n+OK\n"));
    });

    settingsRegisterCommand(F("FLASH.DUMP"), [](Embedis* e) {
        if (e->argc < 2) {
            DEBUG_MSG_P(PSTR("-ERROR: Wrong arguments\n"));
            return;
        }
        uint32_t sector = String(e->argv[1]).toInt();
        uint32_t max = ESP.getFlashChipSize() / SPI_FLASH_SEC_SIZE;
        if (sector >= max) {
            DEBUG_MSG_P(PSTR("-ERROR: Sector out of range\n"));
            return;
        }
        EEPROMr.dump(settingsSerial(), sector);
        DEBUG_MSG_P(PSTR("\n+OK\n"));
    });

}

#endif

// -----------------------------------------------------------------------------

void eepromSetup() {

    #ifdef EEPROM_ROTATE_SECTORS
        EEPROMr.size(EEPROM_ROTATE_SECTORS);
    #else
        // If the memory layout has more than one sector reserved use those,
        // otherwise calculate pool size based on memory size.
        if (EEPROMr.size() == 1) {
            if (EEPROMr.last() > 1000) { // 4Mb boards
                EEPROMr.size(4);
            } else if (EEPROMr.last() > 250) { // 1Mb boards
                EEPROMr.size(2);
            }
        }
    #endif

    EEPROMr.offset(EEPROM_ROTATE_DATA);
    EEPROMr.begin(EEPROM_SIZE);

    #if TERMINAL_SUPPORT
        _eepromInitCommands();
    #endif

}
