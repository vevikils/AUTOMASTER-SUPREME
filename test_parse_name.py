import sys, os, clr

managed = r'C:\Program Files\WindowsApps\ParadoxInteractive.CitiesSkylinesII-PCEdition_1.6.2.0_x64__zfnrdv2de78ny\Cities2_Data\Managed'
sys.path.append(managed)
clr.AddReference('Colossal.AssetPipeline')
clr.AddReference('System')

from Microsoft.CSharp import CSharpCodeProvider
from System.CodeDom.Compiler import CompilerParameters

code = """
using System;
using Colossal.AssetPipeline;
using Unity.Mathematics;

public class Helper {
    public static string Test(string name) {
        try {
            string theme, assetName, material, suffix;
            int level, lod;
            int2 lotSize;
            Module module;
            AssetUtils.ParseName(name, out theme, out assetName, out level, out lotSize, out module, out lod, out material, out suffix);
            return "OK: theme=" + theme + " asset=" + assetName + " suffix=" + suffix + " lod=" + lod;
        } catch (Exception ex) {
            return "FAIL: " + ex.Message;
        }
    }
}
"""

prov = CSharpCodeProvider()
cp = CompilerParameters()
cp.ReferencedAssemblies.Add(os.path.join(managed, 'Colossal.AssetPipeline.dll'))
cp.ReferencedAssemblies.Add(os.path.join(managed, 'Unity.Mathematics.dll'))
cp.GenerateInMemory = True
res = prov.CompileAssemblyFromSource(cp, code)
if res.Errors.HasErrors:
    for err in res.Errors:
        print('Compile error:', err.ErrorText)
else:
    t = res.CompiledAssembly.GetType('Helper')
    meth = t.GetMethod('Test')
    names = [
        'Catedral_Oviedo',
        'Ayuntamiento_Oviedo',
        'Estadio_Carlos_Tartiere',
        'Estadio_CarlosTartiere',
        'Estadio_Tartiere',
        'EstadioTartiere',
        'Santa_Maria_Naranco',
        'SantaMaria_Naranco',
        'Santa_Naranco'
    ]
    for n in names:
        ret = meth.Invoke(None, [n])
        print(f'{n:30} -> {ret}')
