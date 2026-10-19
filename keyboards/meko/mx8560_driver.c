#include "quantum.h"

#include "pointing_device_internal.h"

// clang-format off
#define MSB1 0x80 // Read bit
#define MSB0 0x7F // Write bit

// Product ID
#define REG_PID1    (0x00)
#define REG_PID2    (0x01)

// Motion Status
#define REG_STATUS  (0x02)
#define REG_X       (0x03)
#define REG_Y       (0x04)

#define REG_SETUP   (0x06)
#define REG_W_PROT  (0x09)

// CPI values
#define REG_CPI  (REG_SETUP)
#define CPI_800  (0x04)
#define CPI_1000 (0x05)
#define CPI_1200 (0x06)
#define CPI_1600 (0x07)

// Operational Modes
#define REG_SLEEP_MODE  (0x05)
#define DISABLE_SLEEP   (0xA0)
#define SLEEP_MODE_1    (0xB0)
#define SLEEP_MODE_2    (0xB8)
#define FORCE_SLEEP_1   (0xB2)
#define FORCE_SLEEP_2   (0xBC)
#define FORCE_WAKEUP    (0xB1)

// Image Threshold
#define REG_IMG_THRES   (0x0D)

// Image Recogonition Rate
#define REG_IMG_RECG    (0x0E)
#define IMG_REC_HIGHEST (0xE0)
#define IMG_REC_HIGH    (0x07)
#define IMG_REC_MED     (0x05)
#define IMG_REC_LOW     (0x09)
#define IMG_REC_LOWEST  (0x0F)

// clang-format on

#define MX8650_SCLK_PIN (POINTING_DEVICE_SCLK_PIN)
#define MX8650_SDIO_PIN (POINTING_DEVICE_SDIO_PIN)

typedef struct {
    int8_t x;
    int8_t y;
} report_mx8650_t;

/* ----------------------------------------------------- */
/* MX8650 ---------------------------------------------- */
// Used Bitbang SPI to comunicate with chip sensor.
// We can use SPI periph by connect MISO and MOSI through
// 1k resistor (so ploopy nano rev1 for more details), but
// currently not enough pins.

// Timing for Bitbang is tested, but not sure this is bullet proof.
// This need more testing.
uint8_t mx8650_serial_read(void) {
    uint8_t read_data = 0;
    gpio_set_pin_input_high(MX8650_SDIO_PIN);

    for (uint8_t i = 0; i < 8; ++i) {
        gpio_write_pin_low(MX8650_SCLK_PIN);
        wait_us(2);
        gpio_write_pin_high(MX8650_SCLK_PIN);
        read_data = (read_data << 1) | gpio_read_pin(MX8650_SDIO_PIN);
    }

    return read_data;
}

void mx8650_serial_write(uint8_t data) {
    gpio_set_pin_output(MX8650_SDIO_PIN);

    for (int8_t b = 7; b >= 0; b--) {
        gpio_write_pin_low(MX8650_SCLK_PIN);
        gpio_write_pin(MX8650_SDIO_PIN, (data >> b) & 0x01);
        gpio_write_pin_high(MX8650_SCLK_PIN);
        // wait_us(1);
    }
}

void mx8650_write_reg(uint8_t reg_addr, uint8_t data) {
    mx8650_serial_write(MSB1 | reg_addr);
    mx8650_serial_write(data);
}

uint8_t mx8650_read_reg(uint8_t reg_addr) {
    mx8650_serial_write(reg_addr);
    wait_us(5);

    return mx8650_serial_read();
}

bool mx8650_check_pid(void) {
    uint8_t pid1 = mx8650_read_reg(REG_PID1);
    uint8_t pid2 = mx8650_read_reg(REG_PID2);

    return (pid1 == 0x30 && pid2 == 0x5A);
}

report_mx8650_t mx8650_read(void) {
    report_mx8650_t report = {0};
    uint8_t         motion = mx8650_read_reg(REG_STATUS);

    // check for motion only (bit 7 in field)
    if (motion & MSB1) {
        // Motion detected
        report.x = (int8_t)mx8650_read_reg(REG_X);
        report.y = (int8_t)mx8650_read_reg(REG_Y);
        // TODO: check for overflow
    }

    return report;
}

/* ----------------------------------------------------- */
/* QMK INTERNAL ---------------------------------------- */

bool pointing_device_driver_init(void) {
    gpio_set_pin_output(MX8650_SCLK_PIN);
    gpio_set_pin_output(MX8650_SDIO_PIN);

    // reset
    mx8650_write_reg(REG_SETUP, MSB1);
    wait_ms(50);

    if (!mx8650_check_pid()) {
        return false;
    }

    // initialize
    // TODO: check if reg is success written
    mx8650_write_reg(REG_SLEEP_MODE, SLEEP_MODE_1);
    mx8650_write_reg(REG_CPI, CPI_800);
    mx8650_write_reg(REG_W_PROT, 0x5A); // disable write protect
    mx8650_write_reg(REG_IMG_THRES, 0x04);
    mx8650_write_reg(REG_IMG_RECG, IMG_REC_HIGH);
    mx8650_write_reg(REG_W_PROT, 0x00); // enable write protect

    mx8650_read();

    return true;
}
report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    report_mx8650_t data = mx8650_read();

    if (data.x != 0 || data.y != 0) {
        pd_dprintf("Raw ] X: %d, Y: %d\n", data.dx, data.dy);
        mouse_report.x = data.x;
        mouse_report.y = data.y;
    }

    return mouse_report;
}
uint16_t pointing_device_driver_get_cpi(void) {
    switch (mx8650_read_reg(REG_CPI)) {
        case CPI_800:
            return 800;
        case CPI_1000:
            return 1000;
        case CPI_1200:
            return 1200;
        case CPI_1600:
            return 1600;
        default:
            return 0;
    }
}
void pointing_device_driver_set_cpi(uint16_t cpi) {
    uint8_t cpival = CPI_800;
    if (cpi <= 900) {
        cpival = CPI_800;
    } else if (cpi <= 1100) {
        cpival = CPI_1000;
    } else if (cpi <= 1400) {
        cpival = CPI_1200;
    } else {
        cpival = CPI_1600;
    }

    mx8650_write_reg(REG_CPI, cpival);
}