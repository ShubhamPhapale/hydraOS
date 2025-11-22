#ifndef __HYDRAOS__DRIVERS__VGA_H
#define __HYDRAOS__DRIVERS__VGA_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>

namespace hydraos
{
    namespace drivers
    {
        // VGA Color codes for text mode
        enum VGAColor {
            VGA_COLOR_BLACK = 0,
            VGA_COLOR_BLUE = 1,
            VGA_COLOR_GREEN = 2,
            VGA_COLOR_CYAN = 3,
            VGA_COLOR_RED = 4,
            VGA_COLOR_MAGENTA = 5,
            VGA_COLOR_BROWN = 6,
            VGA_COLOR_LIGHT_GREY = 7,
            VGA_COLOR_DARK_GREY = 8,
            VGA_COLOR_LIGHT_BLUE = 9,
            VGA_COLOR_LIGHT_GREEN = 10,
            VGA_COLOR_LIGHT_CYAN = 11,
            VGA_COLOR_LIGHT_RED = 12,
            VGA_COLOR_LIGHT_MAGENTA = 13,
            VGA_COLOR_LIGHT_BROWN = 14,
            VGA_COLOR_WHITE = 15,
        };

        class VideoGraphicsArray
        {
            protected:
                hydraos::hardwarecommunication::Port8Bit miscPort;
                hydraos::hardwarecommunication::Port8Bit crtcIndexPort;
                hydraos::hardwarecommunication::Port8Bit crtcDataPort;
                hydraos::hardwarecommunication::Port8Bit sequencerIndexPort;
                hydraos::hardwarecommunication::Port8Bit sequencerDataPort;
                hydraos::hardwarecommunication::Port8Bit graphicsControllerIndexPort;
                hydraos::hardwarecommunication::Port8Bit graphicsControllerDataPort;
                hydraos::hardwarecommunication::Port8Bit attributeControllerIndexPort;
                hydraos::hardwarecommunication::Port8Bit attributeControllerReadPort;
                hydraos::hardwarecommunication::Port8Bit attributeControllerWritePort;
                hydraos::hardwarecommunication::Port8Bit attributeControllerResetPort;

                void WriteRegisters(hydraos::common::uint8_t* registers);
                hydraos::common::uint8_t* GetFrameBufferSegment();

                virtual hydraos::common::uint8_t GetColorIndex(hydraos::common::uint8_t r, hydraos::common::uint8_t g, hydraos::common::uint8_t b);

            public:
                VideoGraphicsArray();
                ~VideoGraphicsArray();

                virtual bool SetMode(hydraos::common::uint32_t width, hydraos::common::uint32_t height, hydraos::common::uint32_t colorDepth);
                virtual bool SupportsMode(hydraos::common::uint32_t width, hydraos::common::uint32_t height, hydraos::common::uint32_t colorDepth);
                virtual void PutPixel(hydraos::common::int32_t x, hydraos::common::int32_t y, hydraos::common::uint8_t colorIndex);
                virtual void PutPixel(hydraos::common::int32_t x, hydraos::common::int32_t y, hydraos::common::uint8_t r, hydraos::common::uint8_t g, hydraos::common::uint8_t b);
        };

        class VGATextMode
        {
            private:
                hydraos::common::uint16_t* videoMemory;
                hydraos::common::uint8_t cursorX;
                hydraos::common::uint8_t cursorY;
                hydraos::common::uint8_t foregroundColor;
                hydraos::common::uint8_t backgroundColor;

                static const hydraos::common::uint8_t SCREEN_WIDTH = 80;
                static const hydraos::common::uint8_t SCREEN_HEIGHT = 25;

                hydraos::hardwarecommunication::Port8Bit crtcCommandPort;
                hydraos::hardwarecommunication::Port8Bit crtcDataPort;

                void UpdateCursor();
                void Scroll();

            public:
                VGATextMode();
                ~VGATextMode();

                void Clear();
                void SetColor(VGAColor foreground, VGAColor background);
                void PutChar(char c);
                void Print(const char* str);
                void PrintHex(hydraos::common::uint8_t value);
                void PrintHex16(hydraos::common::uint16_t value);
                void PrintHex32(hydraos::common::uint32_t value);
                void SetCursorPosition(hydraos::common::uint8_t x, hydraos::common::uint8_t y);
                void GetCursorPosition(hydraos::common::uint8_t* x, hydraos::common::uint8_t* y);
        };
    }
}

#endif
