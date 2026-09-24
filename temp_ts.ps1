
$path = 'C:\Program Files\WindowsApps\ParadoxInteractive.CitiesSkylinesII-PCEdition_1.6.2.0_x64__zfnrdv2de78ny\Cities2_Data\Managed\Colossal.IO.AssetDatabase.dll'
$asm = [System.Reflection.Assembly]::LoadFrom($path)
$type = $asm.GetType('Colossal.IO.AssetDatabase.VirtualTexturing.TextureStreamingSystem')
$method = $type.GetMethod('Initialize')
$body = $method.GetMethodBody()
$il = $body.GetILAsByteArray()

Write-Output "IL length: $($il.Length)"

# Let's inspect all types in VirtualTexturing namespace
$asm.GetTypes() | Where-Object { $_.Namespace -eq 'Colossal.IO.AssetDatabase.VirtualTexturing' } | ForEach-Object {
    Write-Output "Type: $($_.Name)"
}
