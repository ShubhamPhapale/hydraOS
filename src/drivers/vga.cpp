#include <drivers/vga.h>

using namespace hydraos::common;
using namespace hydraos::drivers;
using namespace hydraos::hardwarecommunication;

// ============================================================================
// VideoGraphicsArray (Base VGA Driver)
// ============================================================================

VideoGraphicsArray::VideoGraphicsArray()
    : miscPort(0x3c2),
      crtcIndexPort(0x3d4),
      crtcDataPort(0x3d5),
      sequencerIndexPort(0x3c4),
      sequencerDataPort(0x3c5),
      graphicsControllerIndexPort(0x3ce),
      graphicsControllerDataPort(0x3cf),
      attributeControllerIndexPort(0x3c0),
      attributeControllerReadPort(0x3c1),
      attributeControllerWritePort(0x3c0),
      attributeControllerResetPort(0x3da)
{
}

VideoGraphicsArray::~VideoGraphicsArray()
{
}

void VideoGraphicsArray::WriteRegisters(uint8_t* registers)
{
    // Write MISCELLANEOUS register
    miscPort.Write(*(registers++));

    // Write SEQUENCER registers
    for(uint8_t i = 0; i < 5; i++)
    {
        sequencerIndexPort.Write(i);
        sequencerDataPort.Write(*(registers++));
    }

    // Unlock CRTC registers
    crtcIndexPort.Write(0x03);
    crtcDataPort.Write(crtcDataPort.Read() | 0x80);
    crtcIndexPort.Write(0x11);
    crtcDataPort.Write(crtcDataPort.Read() & ~0x80);

    // Make sure they remain unlocked
    registers[0x03] = registers[0x03] | 0x80;
    registers[0x11] = registers[0x11] & ~0x80;

    // Write CRTC registers
    for(uint8_t i = 0; i < 25; i++)
    {
        crtcIndexPort.Write(i);
        crtcDataPort.Write(*(registers++));
    }

    // Write GRAPHICS CONTROLLER registers
    for(uint8_t i = 0; i < 9; i++)
    {
        graphicsControllerIndexPort.Write(i);
        graphicsControllerDataPort.Write(*(registers++));
    }

    // Write ATTRIBUTE CONTROLLER registers
    for(uint8_t i = 0; i < 21; i++)
    {
        attributeControllerResetPort.Read();
        attributeControllerIndexPort.Write(i);
        attributeControllerWritePort.Write(*(registers++));
    }

    // Lock 16-color palette and unblank display
    attributeControllerResetPort.Read();
    attributeControllerIndexPort.Write(0x20);
}

bool VideoGraphicsArray::SupportsMode(uint32_t width, uint32_t height, uint32_t colorDepth)
{
    return width == 320 && height == 200 && colorDepth == 8;
}

bool VideoGraphicsArray::SetMode(uint32_t width, uint32_t height, uint32_t colorDepth)
{
    if(!SupportsMode(width, height, colorDepth))
        return false;

    unsigned char g_320x200x256[] =
    {
    /* MISC */
        0x63,
    /* SEQ */
        0x03, 0x01, 0x0F, 0x00, 0x0E,
    /* CRTC */
        0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
        0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
        0xFF,
    /* GC */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
        0xFF,
    /* AC */
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x41, 0x00, 0x0F, 0x00, 0x00
    };

    WriteRegisters(g_320x200x256);
    return true;
}

uint8_t* VideoGraphicsArray::GetFrameBufferSegment()
{
    graphicsControllerIndexPort.Write(0x06);
    uint8_t segmentNumber = graphicsControllerDataPort.Read() & (3<<2);
    switch(segmentNumber)
    {
        default:
        case 0<<2: return (uint8_t*)0x00000;
        case 1<<2: return (uint8_t*)0xA0000;
        case 2<<2: return (uint8_t*)0xB0000;
        case 3<<2: return (uint8_t*)0xB8000;
    }
}

void VideoGraphicsArray::PutPixel(int32_t x, int32_t y, uint8_t colorIndex)
{
    if(x < 0 || x >= 320 || y < 0 || y >= 200)
        return;

    uint8_t* pixelAddress = GetFrameBufferSegment() + 320*y + x;
    *pixelAddress = colorIndex;
}

uint8_t VideoGraphicsArray::GetColorIndex(uint8_t r, uint8_t g, uint8_t b)
{
    if(r == 0x00 && g == 0x00 && b == 0x00) return 0x00; // black
    if(r == 0x00 && g == 0x00 && b == 0xA8) return 0x01; // blue
    if(r == 0x00 && g == 0xA8 && b == 0x00) return 0x02; // green
    if(r == 0xA8 && g == 0x00 && b == 0x00) return 0x04; // red
    if(r == 0xFF && g == 0xFF && b == 0xFF) return 0x3F; // white
    return 0x00;
}

void VideoGraphicsArray::PutPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    PutPixel(x, y, GetColorIndex(r, g, b));
}

// ============================================================================
// VGATextMode (Text Mode Driver)
// ============================================================================

VGATextMode::VGATextMode()
    : crtcCommandPort(0x3D4),
      crtcDataPort(0x3D5)
{
    videoMemory = (uint16_t*)0xb8000;
    cursorX = 0;
    cursorY = 0;
    foregroundColor = VGA_COLOR_WHITE;
    backgroundColor = VGA_COLOR_BLACK;
    Clear();
}

VGATextMode::~VGATextMode()
{
}

void VGATextMode::Clear()
{
    uint16_t blank = ((uint16_t)backgroundColor << 12) | ((uint16_t)foregroundColor << 8) | ' ';
    for(uint8_t y = 0; y < SCREEN_HEIGHT; y++)
    {
        for(uint8_t x = 0; x < SCREEN_WIDTH; x++)
        {
            videoMemory[SCREEN_WIDTH * y + x] = blank;
        }
    }
    cursorX = 0;
    cursorY = 0;
    UpdateCursor();
}

void VGATextMode::SetColor(VGAColor foreground, VGAColor background)
{
    foregroundColor = (uint8_t)foreground;
    backgroundColor = (uint8_t)background;
}

void VGATextMode::UpdateCursor()
{
    uint16_t position = cursorY * SCREEN_WIDTH + cursorX;

    // Send the high byte
    crtcCommandPort.Write(14);
    crtcDataPort.Write((position >> 8) & 0xFF);

    // Send the low byte
    crtcCommandPort.Write(15);
    crtcDataPort.Write(position & 0xFF);
}

void VGATextMode::Scroll()
{
    uint16_t blank = ((uint16_t)backgroundColor << 12) | ((uint16_t)foregroundColor << 8) | ' ';

    // Move all lines up by one
    for(uint8_t y = 0; y < SCREEN_HEIGHT - 1; y++)
    {
        for(uint8_t x = 0; x < SCREEN_WIDTH; x++)
        {
            videoMemory[SCREEN_WIDTH * y + x] = videoMemory[SCREEN_WIDTH * (y + 1) + x];
        }
    }

    // Clear the last line
    for(uint8_t x = 0; x < SCREEN_WIDTH; x++)
    {
        videoMemory[SCREEN_WIDTH * (SCREEN_HEIGHT - 1) + x] = blank;
    }

    cursorY = SCREEN_HEIGHT - 1;
}

void VGATextMode::PutChar(char c)
{
    uint16_t attrib = ((uint16_t)backgroundColor << 12) | ((uint16_t)foregroundColor << 8);

    switch(c)
    {
        case '\n':
            cursorX = 0;
            cursorY++;
            break;

        case '\r':
            cursorX = 0;
            break;

        case '\t':
            cursorX = (cursorX + 4) & ~(4 - 1);
            break;

        case '\b':
            if(cursorX > 0)
            {
                cursorX--;
                videoMemory[SCREEN_WIDTH * cursorY + cursorX] = attrib | ' ';
            }
            break;

        default:
            videoMemory[SCREEN_WIDTH * cursorY + cursorX] = attrib | c;
            cursorX++;
            break;
    }

    if(cursorX >= SCREEN_WIDTH)
    {
        cursorX = 0;
        cursorY++;
    }

    if(cursorY >= SCREEN_HEIGHT)
    {
        Scroll();
    }

    UpdateCursor();
}

void VGATextMode::Print(const char* str)
{
    for(int i = 0; str[i] != '\0'; ++i)
    {
        PutChar(str[i]);
    }
}

void VGATextMode::PrintHex(uint8_t value)
{
    const char* hex = "0123456789ABCDEF";
    PutChar(hex[(value >> 4) & 0x0F]);
    PutChar(hex[value & 0x0F]);
}

void VGATextMode::PrintHex16(uint16_t value)
{
    PrintHex((value >> 8) & 0xFF);
    PrintHex(value & 0xFF);
}

void VGATextMode::PrintHex32(uint32_t value)
{
    PrintHex16((value >> 16) & 0xFFFF);
    PrintHex16(value & 0xFFFF);
}

void VGATextMode::SetCursorPosition(uint8_t x, uint8_t y)
{
    if(x < SCREEN_WIDTH && y < SCREEN_HEIGHT)
    {
        cursorX = x;
        cursorY = y;
        UpdateCursor();
    }
}

void VGATextMode::GetCursorPosition(uint8_t* x, uint8_t* y)
{
    if(x != 0)
        *x = cursorX;
    if(y != 0)
        *y = cursorY;
}
