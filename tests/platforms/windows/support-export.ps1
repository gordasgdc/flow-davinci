$ErrorActionPreference='Stop'
Set-StrictMode -Version Latest
$repo=(Resolve-Path (Join-Path $PSScriptRoot '..\..\..')).Path
$root=Join-Path ([IO.Path]::GetTempPath()) ('opengate-export-test-'+[Guid]::NewGuid().ToString('N'))
$logs=Join-Path $root 'logs';$install=Join-Path $root 'install';$output=Join-Path $root 'output'
$null=New-Item -ItemType Directory -Path $logs,$install,$output
'{"component":"OpenGate","event":"input.click"}' | Set-Content (Join-Path $logs 'events.jsonl')
'{"clean_shutdown":true}' | Set-Content (Join-Path $logs 'active-session.json')
'PRIVATE_UNRELATED_CONTENT' | Set-Content (Join-Path $logs 'do-not-export.txt')
'{"SourceCommit":"fixture"}' | Set-Content (Join-Path $install 'release-manifest.json')
$before=(Get-FileHash (Join-Path $logs 'events.jsonl')).Hash
& (Join-Path $repo 'infrastructure/windows/export-support.ps1') -LogDirectory $logs -InstallDirectory $install -OutputDirectory $output
$zip=@(Get-ChildItem $output -Filter '*.zip')
if($zip.Count -ne 1){throw 'Expected one support ZIP'}
$expanded=Join-Path $root 'expanded';Expand-Archive -LiteralPath $zip[0].FullName -DestinationPath $expanded
$manifest=Get-ChildItem $expanded -Filter 'support-manifest.json' -Recurse
$data=Get-Content -LiteralPath $manifest.FullName -Raw | ConvertFrom-Json
if($data.product -ne 'OpenGate' -or $data.errors.Count -ne 0 -or $data.files.Count -ne 3){throw 'Incomplete export receipt'}
if((Get-FileHash (Join-Path $logs 'events.jsonl')).Hash -ne $before){throw 'Export altered live logs'}
if(Get-ChildItem $expanded -Filter 'do-not-export.txt' -Recurse){throw 'Unrecognized content exported'}
foreach($file in $data.files){
    $path=Join-Path $manifest.DirectoryName $file.file
    if((Get-FileHash -LiteralPath $path).Hash -ne $file.sha256){throw 'Evidence checksum mismatch'}
}
$empty=Join-Path $root 'empty';$null=New-Item -ItemType Directory -Path $empty
& (Join-Path $repo 'infrastructure/windows/export-support.ps1') -LogDirectory $empty -InstallDirectory $empty -OutputDirectory $output
if(@(Get-ChildItem $output -Filter '*.zip').Count -ne 2){throw 'Missing-log export failed'}
Write-Host "PASS: exported logs/provenance, checksums, no mutation, allowlist and missing-helper case. Evidence: $root"
