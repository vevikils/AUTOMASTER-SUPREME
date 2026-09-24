
$path = 'C:\Program Files\WindowsApps\ParadoxInteractive.CitiesSkylinesII-PCEdition_1.6.2.0_x64__zfnrdv2de78ny\Cities2_Data\Managed\Colossal.IO.AssetDatabase.dll'
$asm = [System.Reflection.Assembly]::LoadFrom($path)
$type = $asm.GetType('Colossal.IO.AssetDatabase.VirtualTexturing.TextureStreamingSystem')
$methods = $type.GetMethods([System.Reflection.BindingFlags]'Public,NonPublic,Instance,Static') | Where-Object { $_.Name -eq 'Initialize' }
foreach ($m in $methods) {
    $params = ($m.GetParameters() | ForEach-Object { $_.ParameterType.Name }) -join ", "
    Write-Output "Method: Initialize($params)"
    $body = $m.GetMethodBody()
    if ($body) {
        $il = $body.GetILAsByteArray()
        Write-Output "IL length: $($il.Length)"
    }
}
