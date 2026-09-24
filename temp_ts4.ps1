
$path = 'C:\Program Files\WindowsApps\ParadoxInteractive.CitiesSkylinesII-PCEdition_1.6.2.0_x64__zfnrdv2de78ny\Cities2_Data\Managed\Colossal.IO.AssetDatabase.dll'
$asm = [System.Reflection.Assembly]::LoadFrom($path)
$type = $asm.GetType('Colossal.IO.AssetDatabase.VirtualTexturing.TextureStreamingSystem')
$m = $type.GetMethod('Initialize', [System.Reflection.BindingFlags]'Public,NonPublic,Instance,Static', $null, @([int], [UnityEngine.Rendering.VirtualTexturing.FilterMode]), $null)
$body = $m.GetMethodBody()
$il = $body.GetILAsByteArray()

# Print IL opcodes
Write-Output "Total IL: $($il.Length)"
for ($i = 0; $i -lt $il.Length; $i++) {
    if ($i -ge 0x100 -and $i -le 0x140) {
        Write-Output ("{0:X4}: 0x{1:X2}" -f $i, $il[$i])
    }
}
