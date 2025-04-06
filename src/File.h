#pragma once
#include "Restyle.h"

enum class ESimpleFileEncodingType
{
    Utf8,
    Wtf16,
};

class CSimpleFile
{
private:
    HANDLE _hFile = INVALID_HANDLE_VALUE;
    ESimpleFileEncodingType _eEncoding = ESimpleFileEncodingType::Utf8;
    
    /**
     * Get the Windows intl code page identifier.
     *
     * Used for MultiByteToWideString and WideStringToMultiByte.
     */
    inline UINT GetWindowsIntlCodePage();
    
public:
    ~CSimpleFile();
    
    bool IsOpen();
    
    void Close();
    
    HRESULT Create(LPCWSTR szPath, ESimpleFileEncodingType eEncoding = ESimpleFileEncodingType::Utf8);
    
    HRESULT Open(LPCWSTR szPath, ESimpleFileEncodingType eEncoding = ESimpleFileEncodingType::Utf8, bool fExclusive = false);
    
    HRESULT Append(LPCWSTR szPath, ESimpleFileEncodingType eEncoding = ESimpleFileEncodingType::Utf8);
    
    DWORD GetFileSize(DWORD *pdwSizeHigh);
    
    /**
     * Reads the contents of the file into the specified buffer.
     */
    HRESULT Read(void *pBuffer, DWORD nBytes, DWORD *pcb);
    
    /**
     * Appends a series of bytes to the file.
     */
    HRESULT Write(void *pvBin, DWORD cb);
    
    /**
     * Converts a wide string to the expected file encoding and appends it
     * to the file.
     */
    HRESULT WriteString(LPCWSTR szStr, DWORD cbInputStr);
    
    /**
     * Appends a formatted string to the file.
     */
    HRESULT Printf(LPCWSTR szFormatStr, ...);
};