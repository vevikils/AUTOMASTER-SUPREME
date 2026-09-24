
$path = 'C:\Program Files\WindowsApps\ParadoxInteractive.CitiesSkylinesII-PCEdition_1.6.2.0_x64__zfnrdv2de78ny\Cities2_Data\Managed\Colossal.IO.AssetDatabase.dll'
$asm = [System.Reflection.Assembly]::LoadFrom($path)
$type = $asm.GetType('Colossal.IO.AssetDatabase.VirtualTexturing.AtlasMaterialsGrouper')

Write-Output "=== FIELDS ==="
$type.GetFields([System.Reflection.BindingFlags]'Public,NonPublic,Instance,Static') | ForEach-Object {
    Write-Output "$($_.Name) : $($_.FieldType.Name)"
}

Write-Output "`n=== METHODS ==="
$type.GetMethods([System.Reflection.BindingFlags]'Public,NonPublic,Instance,Static') | ForEach-Object {
    Write-Output "$($_.Name)"
}
