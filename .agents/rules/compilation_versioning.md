# REGLA OBLIGATORIA: INCREMENTO DE VERSIÓN Y LIMPIEZA EN CADA COMPILACIÓN

## Contexto y Razón Crítica
FL Studio almacena en caché los metadatos y punteros de archivos de los plugins VST3 cargados en memoria. Si se compila sobre el mismo nombre de archivo o misma versión sin incrementar el número de versión, FL Studio continuará detectando y ejecutando la versión antigua en lugar de la nueva compilación.

## Instrucciones Permanentes
1. **SIEMPRE incrementar la versión en CADA nueva compilación:**
   - Si la versión actual es `V.2.3`, la siguiente compilación DEBE ser obligatoriamente `V.2.4`.
   - Modificar tanto el archivo de configuración de construcción (`CMakeLists.txt` o similar):
     - `set(PROJECT_VERSION X.Y.Z)`
     - Nombre del proyecto y target: e.g. `Supreme Tuner BPM V.X.Y`
   - Actualizar en el código fuente:
     - `PluginProcessor.cpp`: `getName()` retornando `"Supreme Tuner BPM V.X.Y"`.
     - `PluginEditor.cpp`: Badge `vX.Y.0 PRO STUDIO`, footer watermark, y cualquier etiqueta visual de versión.
     - Pruebas unitarias (`DSPTest.cpp`).

2. **SIEMPRE borrar las versiones viejas de la carpeta VST3:**
   - Ruta estándar: `C:\Program Files\Common Files\VST3\`
   - Antes o después de instalar la nueva versión, eliminar por completo los bundles anteriores (ej. `Supreme Tuner BPM V.2.2.vst3`, `Supreme Tuner BPM V.2.3.vst3`, etc.).
   - Si FL Studio tiene abierto un archivo anterior y bloquea el borrado directo, renombrar el archivo bloqueado a `.old` o notificar al usuario para liberar el slot.

3. **Copiar e instalar siempre la nueva versión limpia:**
   - Instalar el nuevo bundle recién compilado (ej. `Supreme Tuner BPM V.2.4.vst3`) directamente en `C:\Program Files\Common Files\VST3\`.
   - De esta manera, el plugin manager de FL Studio detecta automáticamente el nuevo archivo como plugin independiente y fresco sin problemas de caché.
