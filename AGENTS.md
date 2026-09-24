# Workspace Guidelines & Instructions

## Regla de Oro: Compilación y Versionado de Plugins VST3 para FL Studio
- **Cada vez que se haga una nueva compilación de un plugin o programa:**
  1. **SIEMPRE incrementar la versión** (ej. V.2.3 -> V.2.4).
  2. **Actualizar el nombre y versión en CMakeLists.txt, PluginProcessor y PluginEditor.**
  3. **Borrar las versiones viejas de `C:\Program Files\Common Files\VST3\`.**
  4. **Instalar la nueva versión recién compilada en la carpeta VST3.**
  5. Esto es crucial porque FL Studio cachea los plugins abiertos y no detecta cambios a menos que el nombre de versión cambie y se limpien las versiones obsoletas.
