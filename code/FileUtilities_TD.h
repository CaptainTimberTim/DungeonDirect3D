#pragma once

#include "String_TD.h"


struct read_file_result
{
    u32 Size;
    u8 *Data;
};

// General file methods
internal b32 WriteEntireFile(arena_allocator *Arena, u8 *Filename, u32 MemorySize, void *Memory);
internal b32 WriteEntireFile(string_w *Filename, u32 MemorySize, void *Memory);
internal void   FreeFileMemory(arena_allocator *Arena, u8 *Memory);
internal b32 ReadEntireFile(arena_allocator *Arena, read_file_result *FileData, u8 *Filename);
internal b32 ReadEntireFile(arena_allocator *Arena, read_file_result *FileData, string_w *Filename);
internal b32 ReadBeginningOfFile(arena_allocator *Arena, read_file_result *FileData, u8 *Filename, u32 ReadAmount);
internal b32 ReadBeginningOfFile(arena_allocator *Arena, read_file_result *FileData, string_w *Filename, u32 ReadAmount);
internal b32 ReadEndOfFile(arena_allocator *Arena, read_file_result *FileData, u8 *Filename, u32 ReadAmount);
internal b32 ReadEndOfFile(arena_allocator *Arena, read_file_result *FileData, string_w *Filename, u32 ReadAmount);
internal b32 AppendToFile(arena_allocator *Arena, char *FileName, u32 MemorySize, void *Memory);

// .bmp loader methods
enum bitmap_color_format
{
    colorFormat_RGBA,
    colorFormat_RGB,
    colorFormat_BGR,
    colorFormat_Alpha,
};

struct loaded_bitmap
{
    b32 WasLoaded;
    u32 Width;
    u32 Height;
    u32 *Pixels;
    bitmap_color_format ColorFormat;
    u32 Pitch; // Width*sizeof(One color (e.g. u32));
};

struct bit_scan_result
{
    b32 Found;
    u32 Index;
};

inline   bit_scan_result FindLeastSignificantSetBit(u32 Value);
internal loaded_bitmap LoadBMPImage(arena_allocator *Arena, u8 *FileName);

#pragma pack(push, 1)
struct bitmap_header
{
    u16 FileType;
    u32 FileSize;
    u16 Reserved1;
    u16 Reserved2;
    u32 BitmapOffset;
    u32 Size;
    i32 Width;
    i32 Height;
    u16 Planes;
    u16 BitsPerPixel;
    u32 Compression;
    u32 SizeOfBitmap;
    i32 HorzResolution;
    i32 VertResolution;
    u32 ColorsUsed;
    u32 ColorsImportant;
    
    u32 RedMask;
    u32 GreenMask;
    u32 BlueMask;
};
#pragma pack(pop)
