function Build-UnboundIcons {
    param([string]$repoRoot)
    $Source = Join-Path $repoRoot 'src\assets\filmhaus.png'
    $Destination = Join-Path $repoRoot 'src\assets'
    $Icon = Join-Path $repoRoot 'src\platforms\windows\resources\unbound.ico'
    Add-Type -AssemblyName System.Drawing
    New-Item -ItemType Directory -Force -Path $Destination | Out-Null
    $blue = [Drawing.ColorTranslator]::FromHtml('#00AEEF')
    $red = [Drawing.ColorTranslator]::FromHtml('#EF5350')
    $sourceImage = [Drawing.Bitmap]::new($Source)
    $master = [Drawing.Bitmap]::new($sourceImage.Width, $sourceImage.Height, [Drawing.Imaging.PixelFormat]::Format32bppArgb)
    try {
        if ($sourceImage.Width -ne $sourceImage.Height) { throw 'Expected a square source logo.' }
        for ($y=0; $y -lt $sourceImage.Height; $y++) {
            for ($x=0; $x -lt $sourceImage.Width; $x++) {
                $pixel = $sourceImage.GetPixel($x,$y)
                $fill = if ($pixel.B -gt $pixel.R) { $blue } else { $red }
                $master.SetPixel($x,$y,[Drawing.Color]::FromArgb($pixel.A,$fill.R,$fill.G,$fill.B))
            }
        }
        $master.Save((Join-Path $Destination 'filmhaus-resolve.png'),[Drawing.Imaging.ImageFormat]::Png)
        $sizes = @(16,20,24,32,40,48,64,96,128,256)
        $frames = @()
        foreach ($size in $sizes) {
            $bitmap = [Drawing.Bitmap]::new($size,$size,[Drawing.Imaging.PixelFormat]::Format32bppArgb)
            $graphics = [Drawing.Graphics]::FromImage($bitmap)
            $attributes = [Drawing.Imaging.ImageAttributes]::new()
            $stream = [IO.MemoryStream]::new()
            try {
                $graphics.Clear([Drawing.Color]::Transparent)
                $graphics.CompositingMode = [Drawing.Drawing2D.CompositingMode]::SourceCopy
                $graphics.CompositingQuality = [Drawing.Drawing2D.CompositingQuality]::HighQuality
                $graphics.InterpolationMode = [Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
                $graphics.PixelOffsetMode = [Drawing.Drawing2D.PixelOffsetMode]::HighQuality
                $attributes.SetWrapMode([Drawing.Drawing2D.WrapMode]::TileFlipXY)
                $graphics.DrawImage($master,[Drawing.Rectangle]::new(0,0,$size,$size),0,0,$master.Width,$master.Height,[Drawing.GraphicsUnit]::Pixel,$attributes)
                $bitmap.Save($stream,[Drawing.Imaging.ImageFormat]::Png)
                $bytes = $stream.ToArray()
                [IO.File]::WriteAllBytes((Join-Path $Destination "unbound-$size.png"),$bytes)
                $frames += ,$bytes
            } finally {
                $stream.Dispose(); $attributes.Dispose(); $graphics.Dispose(); $bitmap.Dispose()
            }
        }
        # Modern Windows ICO: PNG-compressed 32-bit RGBA frames, 256 encoded as 0.
        $output = [IO.File]::Create($Icon)
        $writer = [IO.BinaryWriter]::new($output)
        try {
            $writer.Write([uint16]0); $writer.Write([uint16]1); $writer.Write([uint16]$sizes.Count)
            $offset = 6 + 16*$sizes.Count
            for ($i=0; $i -lt $sizes.Count; $i++) {
                $dimension = if ($sizes[$i] -eq 256) { 0 } else { $sizes[$i] }
                $writer.Write([byte]$dimension); $writer.Write([byte]$dimension)
                $writer.Write([byte]0); $writer.Write([byte]0)
                $writer.Write([uint16]1); $writer.Write([uint16]32)
                $writer.Write([uint32]$frames[$i].Length); $writer.Write([uint32]$offset)
                $offset += $frames[$i].Length
            }
            foreach ($frame in $frames) { $writer.Write([byte[]]$frame) }
        } finally { $writer.Dispose(); $output.Dispose() }
        Write-Host "Created recolored master, PNG variants, in $Destination and Windows icon at $Icon"
    } finally { $master.Dispose(); $sourceImage.Dispose() }
}
