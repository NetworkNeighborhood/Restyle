#include "File.h"
#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <new>
#include <strsafe.h>
#include <stringapiset.h>
#include <memory>
#include <winerror.h>

CSimpleFile::~CSimpleFile()
{
    Close();
}

bool CSimpleFile::IsOpen()
{
    return _hFile != INVALID_HANDLE_VALUE;
}

void CSimpleFile::Close()
{
    if (_hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(_hFile);
        _hFile = INVALID_HANDLE_VALUE;
    }
}

HRESULT CSimpleFile::Create(LPCWSTR szPath, ESimpleFileEncodingType eEncoding)
{
    _eEncoding = eEncoding;
    
    _hFile = CreateFileW(szPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
    
    if (_hFile == INVALID_HANDLE_VALUE)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    return S_OK;
}

HRESULT CSimpleFile::Open(LPCWSTR szPath, ESimpleFileEncodingType eEncoding, bool fExclusive)
{
    _eEncoding = eEncoding;
    
    DWORD dwShare = 0;
    if (!fExclusive)
    {
        dwShare = FILE_SHARE_READ;
    }
    
    _hFile = CreateFileW(szPath, GENERIC_READ, dwShare, nullptr, OPEN_EXISTING, 0, nullptr);
    
    if (_hFile == INVALID_HANDLE_VALUE)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    return S_OK;
}

HRESULT CSimpleFile::Append(LPCWSTR szPath, ESimpleFileEncodingType eEncoding)
{
    _eEncoding = eEncoding;
    
    _hFile = CreateFileW(szPath, GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, 0, nullptr);
    
    if (_hFile == INVALID_HANDLE_VALUE)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    SetLastError(0);
    
    SetFilePointer(_hFile, 0, nullptr, FILE_END);
    
    if (GetLastError() != 0)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    return S_OK;
}

DWORD CSimpleFile::GetFileSize(DWORD *pdwSizeHigh)
{
    return ::GetFileSize(_hFile, pdwSizeHigh);
}

HRESULT CSimpleFile::Read(void *pBuffer, DWORD nBytes, DWORD *pcb)
{
    switch (_eEncoding)
    {
        // Non-WTF-16 files (i.e. UTF-8) need to be converted to WTF-16 so that we can
        // parse them correctly.
        case ESimpleFileEncodingType::Utf8:
        {
            UINT uCodePage = GetWindowsIntlCodePage();
            
            if (!uCodePage)
            {
                return E_INVALIDARG;
            }
            
            // We will temporarily allocate a new string to hold the raw contents
            // of the file while we convert it:
            std::unique_ptr<BYTE[]> pszOriginal;
            try
            {
                pszOriginal = std::make_unique<BYTE[]>(nBytes);
            }
            catch (std::bad_alloc)
            {
                return E_OUTOFMEMORY;
            }

            if (!ReadFile(_hFile, pszOriginal.get(), nBytes, pcb, nullptr))
            {
                return HRESULT_FROM_WIN32(GetLastError());
            }
            
            int cchConv = MultiByteToWideChar(uCodePage, 0, (LPCCH)pszOriginal.get(), -1, nullptr, 0);
            
            if (cchConv * sizeof(WCHAR) > nBytes)
            {
                return E_OUTOFMEMORY;
            }
            
            MultiByteToWideChar(uCodePage, 0, (LPCCH)pszOriginal.get(), -1, (LPWSTR)pBuffer, cchConv);
            
            break;
        }
        
        default:
        case ESimpleFileEncodingType::Wtf16:
        {
            if (!ReadFile(_hFile, pBuffer, nBytes, pcb, nullptr))
            {
                return HRESULT_FROM_WIN32(GetLastError());
            }
            
            break;
        }
    }
    
    return S_OK;
}

HRESULT CSimpleFile::Write(void *pvBin, DWORD cb)
{
    DWORD dw;
    if (!WriteFile(_hFile, pvBin, cb, &dw, nullptr))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    return S_OK;
}

HRESULT CSimpleFile::WriteString(LPCWSTR szStr, DWORD cbInputStr)
{
    switch (_eEncoding)
    {
        case ESimpleFileEncodingType::Utf8:
        {
            UINT uCodePage = GetWindowsIntlCodePage();
            
            if (!uCodePage)
            {
                return E_INVALIDARG;
            }
            
            int cbBuffer = WideCharToMultiByte(uCodePage, 0, szStr, wcslen(szStr), nullptr, -1, nullptr, nullptr);
            
            std::unique_ptr<BYTE[]> pszNewEncoding;
            try
            {
                pszNewEncoding = std::make_unique<BYTE[]>(cbBuffer);
            }
            catch (std::bad_alloc)
            {
                return E_OUTOFMEMORY;
            }
            
            WideCharToMultiByte(uCodePage, 0, szStr, wcslen(szStr), (LPSTR)pszNewEncoding.get(), cbBuffer, nullptr, nullptr);
            
            return Write((void *)pszNewEncoding.get(), cbBuffer);
            
            break;
        }
        
        default:
        case ESimpleFileEncodingType::Wtf16:
        {
            return Write((void *)szStr, cbInputStr);
        }
    }
    
    return E_FAIL;
}

HRESULT CSimpleFile::Printf(LPCWSTR szFormatStr, ...)
{
    va_list args;
    va_start(args, szFormatStr);
    
    WCHAR szBuffer[2048];
    
    StringCchVPrintfW(szBuffer, ARRAYSIZE(szBuffer), szFormatStr, args);
    va_end(args);
    
    StringCchCatW(szBuffer, ARRAYSIZE(szBuffer), L"\r\n");
    
    return WriteString(szBuffer, lstrlenW(szBuffer) * sizeof(WCHAR));
}

inline UINT CSimpleFile::GetWindowsIntlCodePage()
{
    const struct
    {
        ESimpleFileEncodingType eEncoding;
        UINT uCodePage;
    } kConversionTable[] = {
        { ESimpleFileEncodingType::Utf8, CP_UTF8 },
    };
    
    for (int i = 0; i < ARRAYSIZE(kConversionTable); i++)
    {
        if (kConversionTable[i].eEncoding == _eEncoding)
        {
            return kConversionTable[i].uCodePage;
        }
    }
    
    return 0;
}