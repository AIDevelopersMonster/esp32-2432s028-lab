param(
    [string]$Port = "COM12",
    [string]$Output = "esp32-2432s028-full.bin",
    [int]$FlashSize = 0x400000,
    [int]$ChunkSize = 0x1000,
    [int]$Baud = 115200,
    [int]$Retries = 5
)

$ErrorActionPreference = "Stop"

if ($FlashSize -le 0 -or $ChunkSize -le 0) {
    throw "FlashSize and ChunkSize must be positive."
}

if (($FlashSize % $ChunkSize) -ne 0) {
    throw "FlashSize must be divisible by ChunkSize."
}

$finalPath = [System.IO.Path]::GetFullPath($Output)
$partialPath = "$finalPath.partial"
$tempDir = Join-Path ([System.IO.Path]::GetDirectoryName($finalPath)) ".flash-chunks"

New-Item -ItemType Directory -Force -Path $tempDir | Out-Null
Remove-Item $partialPath -Force -ErrorAction SilentlyContinue

$stream = [System.IO.File]::Open(
    $partialPath,
    [System.IO.FileMode]::Create,
    [System.IO.FileAccess]::Write,
    [System.IO.FileShare]::None
)

$firstChunk = $true

try {
    for ($offset = 0; $offset -lt $FlashSize; $offset += $ChunkSize) {
        $offsetHex = "0x{0:X}" -f $offset
        $sizeHex = "0x{0:X}" -f $ChunkSize
        $chunkName = "chunk-{0:X8}.bin" -f $offset
        $chunkPath = Join-Path $tempDir $chunkName
        $success = $false
        $forceReset = $firstChunk

        for ($attempt = 1; $attempt -le $Retries; $attempt++) {
            Remove-Item $chunkPath -Force -ErrorAction SilentlyContinue

            $beforeMode = if ($forceReset) { "default-reset" } else { "no-reset" }

            Write-Host ("Reading {0}/{1}: offset={2}, size={3}, attempt={4}, before={5}" -f `
                ($offset / $ChunkSize + 1), ($FlashSize / $ChunkSize), $offsetHex, $sizeHex, $attempt, $beforeMode)

            & python -m esptool `
                --chip esp32 `
                --port $Port `
                --baud $Baud `
                --before $beforeMode `
                --after no-reset `
                --no-stub `
                read-flash `
                --flash-size 4MB `
                $offsetHex `
                $sizeHex `
                $chunkPath

            $exitCode = $LASTEXITCODE
            $validChunk = Test-Path $chunkPath

            if ($validChunk) {
                $validChunk = ((Get-Item $chunkPath).Length -eq $ChunkSize)
            }

            if ($exitCode -eq 0 -and $validChunk) {
                $success = $true
                break
            }

            Write-Warning "Chunk read failed. Retrying after reconnect/reset."
            $forceReset = $true
            Start-Sleep -Seconds 2
        }

        if (-not $success) {
            throw "Unable to read chunk at $offsetHex after $Retries attempts. Partial image: $partialPath"
        }

        $bytes = [System.IO.File]::ReadAllBytes($chunkPath)
        $stream.Write($bytes, 0, $bytes.Length)
        $stream.Flush()

        Remove-Item $chunkPath -Force
        $firstChunk = $false
    }
}
finally {
    $stream.Dispose()
}

$actualSize = (Get-Item $partialPath).Length
if ($actualSize -ne $FlashSize) {
    throw "Unexpected output size: $actualSize bytes; expected $FlashSize bytes."
}

Remove-Item $finalPath -Force -ErrorAction SilentlyContinue
Move-Item $partialPath $finalPath
Remove-Item $tempDir -Force -Recurse -ErrorAction SilentlyContinue

$hash = (Get-FileHash $finalPath -Algorithm SHA256).Hash

Write-Host ""
Write-Host "FLASH_READ_COMPLETE"
Write-Host "File: $finalPath"
Write-Host "Size: $actualSize"
Write-Host "SHA256: $hash"
Write-Host ""
Write-Host "The ESP32 was deliberately left in the serial bootloader. Press RESET/EN or power-cycle the board."
