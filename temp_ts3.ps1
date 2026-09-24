
$path = 'C:\Program Files\WindowsApps\ParadoxInteractive.CitiesSkylinesII-PCEdition_1.6.2.0_x64__zfnrdv2de78ny\Cities2_Data\Managed\Colossal.IO.AssetDatabase.dll'
$asm = [System.Reflection.Assembly]::LoadFrom($path)
$type = $asm.GetType('Colossal.IO.AssetDatabase.VirtualTexturing.TextureStreamingSystem')
$m = $type.GetMethod('Initialize', [System.Reflection.BindingFlags]'Public,NonPublic,Instance,Static', $null, @([int], [UnityEngine.Rendering.VirtualTexturing.FilterMode]), $null)
$body = $m.GetMethodBody()
$il = $body.GetILAsByteArray()

# Print bytes around 0x100 to 0x140
for ($i = 0x100; $i -lt [Math]::Min($il.Length, 0x150); $i++) {
    Write-Host ("{0:X4}: {1:X2}" -f $i, $il[$i])
}
