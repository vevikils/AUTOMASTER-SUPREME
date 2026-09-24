import os
import math
import struct
import numpy as np
from scipy import signal

SAMPLE_RATE = 44100

def write_wav(filepath, audio, sample_rate=44100, tempo=None, num_beats=None, root_note=60, title="", artist="Supreme Producer"):
    """
    Writes a high-quality 24-bit or 16-bit WAV file.
    If tempo and num_beats are provided, embeds an ACID chunk for 100% automatic tempo matching in FL Studio / DAWs!
    """
    # Ensure float in range [-1, 1]
    audio = np.nan_to_num(audio)
    max_val = np.max(np.abs(audio))
    if max_val > 0.99:
        audio = audio / max_val * 0.98

    # Convert to 16-bit PCM
    int_data = (audio * 32767).astype(np.int16)
    
    is_stereo = len(int_data.shape) > 1 and int_data.shape[1] == 2
    num_channels = 2 if is_stereo else 1
    bytes_per_sample = 2
    byte_rate = sample_rate * num_channels * bytes_per_sample
    block_align = num_channels * bytes_per_sample
    raw_data = int_data.tobytes()
    data_size = len(raw_data)
    
    # 1. fmt chunk
    fmt_chunk = b'fmt ' + struct.pack('<IHHIIHH', 16, 1, num_channels, sample_rate, byte_rate, block_align, 16)
    
    # 2. acid chunk (if tempo provided)
    acid_chunk = b''
    if tempo is not None and num_beats is not None:
        # flag 0x37 = loop, stretch, rooted
        acid_data = struct.pack('<IHHfIHHf', 0x00000037, int(root_note), 0, 0.0, int(num_beats), 4, 4, float(tempo))
        acid_chunk = b'acid' + struct.pack('<I', len(acid_data)) + acid_data

    # 3. INFO chunk
    def make_subchunk(tag, text):
        raw = text.encode('utf-8') + b'\x00'
        if len(raw) % 2 != 0:
            raw += b'\x00'
        return tag + struct.pack('<I', len(raw)) + raw

    info_body = make_subchunk(b'INAM', title or os.path.basename(filepath))
    info_body += make_subchunk(b'IART', artist)
    if tempo:
        info_body += make_subchunk(b'ICMT', f'{tempo:.1f} BPM - Auto-Tempo Synced')
    info_chunk = b'LIST' + struct.pack('<I', 4 + len(info_body)) + b'INFO' + info_body
    
    # 4. data chunk
    data_chunk = b'data' + struct.pack('<I', data_size) + raw_data
    
    # RIFF header
    total_size = 4 + len(fmt_chunk) + len(acid_chunk) + len(info_chunk) + len(data_chunk)
    riff_header = b'RIFF' + struct.pack('<I', total_size) + b'WAVE'
    
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    with open(filepath, 'wb') as f:
        f.write(riff_header + fmt_chunk + acid_chunk + info_chunk + data_chunk)

print("WAV writer helper ready.")
