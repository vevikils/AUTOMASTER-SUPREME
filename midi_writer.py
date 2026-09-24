import os
import struct

class MidiWriter:
    def __init__(self, ticks_per_beat=480):
        self.tpb = ticks_per_beat
        self.tracks = []

    def create_track(self):
        t = []
        self.tracks.append(t)
        return len(self.tracks) - 1

    def add_tempo(self, track_idx, tick, bpm):
        mpqn = int(60_000_000 / bpm)
        data = bytes([0xFF, 0x51, 0x03, (mpqn >> 16) & 0xFF, (mpqn >> 8) & 0xFF, mpqn & 0xFF])
        self.tracks[track_idx].append((tick, data))

    def add_note(self, track_idx, start_tick, duration_ticks, note, velocity=100, channel=0):
        on = bytes([0x90 | (channel & 0x0F), note & 0x7F, velocity & 0x7F])
        off = bytes([0x80 | (channel & 0x0F), note & 0x7F, 0])
        self.tracks[track_idx].append((start_tick, on))
        self.tracks[track_idx].append((start_tick + duration_ticks, off))

    def save(self, filepath):
        os.makedirs(os.path.dirname(filepath), exist_ok=True)
        # Format 1 if multiple tracks, else Format 0
        fmt = 1 if len(self.tracks) > 1 else 0
        num_tracks = len(self.tracks)
        
        mthd = b'MThd' + struct.pack('>IHHH', 6, fmt, num_tracks, self.tpb)
        all_mtrks = bytearray()
        
        for events in self.tracks:
            events.sort(key=lambda x: x[0])
            track_bytes = bytearray()
            last_tick = 0
            for tick, ev in events:
                delta = tick - last_tick
                val = delta
                buf = [val & 0x7F]
                while val > 0x7F:
                    val >>= 7
                    buf.insert(0, (val & 0x7F) | 0x80)
                track_bytes.extend(buf)
                track_bytes.extend(ev)
                last_tick = tick
            track_bytes.extend(b'\x00\xFF\x2F\x00')
            all_mtrks.extend(b'MTrk' + struct.pack('>I', len(track_bytes)) + bytes(track_bytes))
            
        with open(filepath, 'wb') as f:
            f.write(mthd + bytes(all_mtrks))

print("MIDI writer ready.")
