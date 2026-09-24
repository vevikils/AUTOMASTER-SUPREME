import os
from render_hit_drum_loop import HitLoopRenderer

BASE_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT\09_Drum_Loops_64_Authentic_Hits_(Auto_Tempo)"

def generate_all():
    print("=== RENDERING 64 AUTHENTIC HIT-INSPIRED DRUM LOOPS (AUTO-TEMPO) ===")
    renderer = HitLoopRenderer()

    # =========================================================================
    # CATEGORY 1: TRAP ARGENTINO (16 LOOPS: 8 FULL BEATS + 8 TOP LOOPS)
    # References: Duki, YSY A, Bizarrap, Khea, Tiago PZK, Neo Pistea, Milo J
    # =========================================================================
    dir_arg = os.path.join(BASE_DIR, "01_Trap_Argentino_(Duki_YsyA_Bzrp_16)")
    
    # 1 & 2: Duki - Goteo (140 BPM) - Saturated Spinz, fast 1/32 rolls, sharp knock kick
    def duki_goteo(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            # Clap on beat 3
            ev.append(('arg_clap', b + 2.0, 0.95, 0.0))
            # Hi-hats: 8th notes with 1/32 roll in bar 2 & 4
            for s in range(8):
                ev.append(('arg_hat', b + s * 0.5, 0.65 if s % 2 == 0 else 0.45, -0.15 if s % 2 == 0 else 0.15))
            if bar in [1, 3]:
                for r in range(6):
                    ev.append(('arg_hat', b + 2.5 + r * (1.0 / 12.0), 0.55 + r * 0.05, 0.2))
                for r in range(8):
                    ev.append(('arg_hat', b + 3.5 + r * (1.0 / 16.0), 0.50 + r * 0.05, -0.2))
            ev.append(('arg_hat_open', b + 1.5, 0.5, -0.3))
        if full:
            # Goteo Kick bounce
            kicks = [0.0, 1.5, 2.75, 4.0, 5.5, 6.75, 7.25, 8.0, 9.5, 10.75, 12.0, 13.5, 14.5, 15.25]
            for kp in kicks: ev.append(('arg_kick', kp, 0.96, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('arg_808', bp, 0.90, 0.0))
        return ev
    renderer.render(140.0, 4, duki_goteo(full=True), "TrapArg_140BPM_Duki_Goteo_Full.wav", dir_arg, "Duki Goteo Full Beat (140 BPM)")
    renderer.render(140.0, 4, duki_goteo(full=False), "TrapArg_140BPM_Duki_Goteo_Top_Loop.wav", dir_arg, "Duki Goteo Top Loop (140 BPM)")

    # 3 & 4: Duki - Givenchy (144 BPM) - Aggressive hard trap bounce, rapid double-kicks
    def duki_givenchy(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('arg_clap', b + 2.0, 0.95, 0.0))
            for s in range(8):
                ev.append(('arg_hat', b + s * 0.5, 0.68 if s % 2 == 0 else 0.48, 0.1 if s % 2 == 0 else -0.1))
            # Givenchy triplet hat burst
            for r in range(6):
                ev.append(('arg_hat', b + 1.0 + r * (1.0 / 6.0), 0.60 + r * 0.04, -0.25))
            ev.append(('arg_hat_open', b + 0.5, 0.45, 0.3))
            ev.append(('arg_hat_open', b + 3.0, 0.45, -0.3))
        if full:
            kicks = [0.0, 0.75, 2.5, 3.25, 4.0, 4.75, 6.5, 8.0, 8.75, 10.5, 11.25, 12.0, 12.75, 14.5, 15.25]
            for kp in kicks: ev.append(('arg_kick', kp, 0.96, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('arg_808', bp, 0.92, 0.0))
        return ev
    renderer.render(144.0, 4, duki_givenchy(full=True), "TrapArg_144BPM_Duki_Givenchy_Full.wav", dir_arg, "Duki Givenchy Full Beat (144 BPM)")
    renderer.render(144.0, 4, duki_givenchy(full=False), "TrapArg_144BPM_Duki_Givenchy_Top_Loop.wav", dir_arg, "Duki Givenchy Top Loop (144 BPM)")

    # 5 & 6: YSY A - Traje Unos Trapitos (135 BPM) - Heavy swing, syncopated bounce
    def ysy_a(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('arg_clap', b + 2.0, 0.95, 0.0))
            for s in range(8):
                ev.append(('arg_hat', b + s * 0.5, 0.65 if s % 2 == 0 else 0.45, 0.15))
            if bar in [1, 3]:
                for r in range(4): ev.append(('arg_hat', b + 3.5 + r * 0.125, 0.7, -0.2))
            ev.append(('arg_hat_open', b + 1.5, 0.5, 0.25))
        if full:
            kicks = [0.0, 1.25, 2.5, 4.0, 5.25, 6.5, 7.25, 8.0, 9.25, 10.5, 12.0, 13.25, 14.5, 15.0]
            for kp in kicks: ev.append(('arg_kick', kp, 0.95, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('arg_808', bp, 0.90, 0.0))
        return ev
    renderer.render(135.0, 4, ysy_a(full=True), "TrapArg_135BPM_YsyA_Trapitos_Full.wav", dir_arg, "YSY A Trapitos Full Beat (135 BPM)")
    renderer.render(135.0, 4, ysy_a(full=False), "TrapArg_135BPM_YsyA_Trapitos_Top_Loop.wav", dir_arg, "YSY A Trapitos Top Loop (135 BPM)")

    # 7 & 8: Bizarrap & Duki - Session 50 (138 BPM) - Maximum 808 presence & sharp claps
    def bzrp_duki(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('arg_clap', b + 2.0, 0.95, 0.0))
            for s in range(8):
                ev.append(('arg_hat', b + s * 0.5, 0.66 if s % 2 == 0 else 0.46, -0.15))
            for r in range(6): ev.append(('arg_hat', b + 3.0 + r * (1.0/6.0), 0.62, 0.2))
            ev.append(('arg_hat_open', b + 1.5, 0.48, -0.25))
        if full:
            kicks = [0.0, 1.5, 2.75, 4.0, 5.5, 7.0, 8.0, 9.5, 10.75, 12.0, 13.5, 14.75, 15.25]
            for kp in kicks: ev.append(('arg_kick', kp, 0.96, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('arg_808', bp, 0.95, 0.0))
        return ev
    renderer.render(138.0, 4, bzrp_duki(full=True), "TrapArg_138BPM_Bzrp_Duki_Session50_Full.wav", dir_arg, "Bzrp x Duki Session 50 Full Beat (138 BPM)")
    renderer.render(138.0, 4, bzrp_duki(full=False), "TrapArg_138BPM_Bzrp_Duki_Session50_Top_Loop.wav", dir_arg, "Bzrp x Duki Session 50 Top Loop (138 BPM)")

    # 9 & 10: Khea & Duki - Loca (130 BPM) - The classic Argentine trap origin beat
    def khea_loca(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('arg_clap', b + 2.0, 0.92, 0.0))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.62 if s % 2 == 0 else 0.42, 0.1))
            ev.append(('arg_hat_open', b + 1.5, 0.45, -0.2))
        if full:
            kicks = [0.0, 1.5, 4.0, 5.5, 6.75, 8.0, 9.5, 12.0, 13.5, 14.75]
            for kp in kicks: ev.append(('arg_kick', kp, 0.95, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('arg_808', bp, 0.88, 0.0))
        return ev
    renderer.render(130.0, 4, khea_loca(full=True), "TrapArg_130BPM_Khea_Loca_Full.wav", dir_arg, "Khea Loca Full Beat (130 BPM)")
    renderer.render(130.0, 4, khea_loca(full=False), "TrapArg_130BPM_Khea_Loca_Top_Loop.wav", dir_arg, "Khea Loca Top Loop (130 BPM)")

    # 11 & 12: Neo Pistea - Tumbando el Club (142 BPM) - Stomp anthem trap
    def neo_pistea(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('arg_clap', b + 2.0, 0.95, 0.0))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.65 if s % 2 == 0 else 0.45, -0.15))
            for r in range(4): ev.append(('arg_hat', b + 3.5 + r * 0.125, 0.72, 0.25))
            ev.append(('arg_hat_open', b + 1.5, 0.5, 0.3))
        if full:
            kicks = [0.0, 1.0, 2.5, 4.0, 5.0, 6.5, 7.25, 8.0, 9.0, 10.5, 12.0, 13.0, 14.5, 15.25]
            for kp in kicks: ev.append(('arg_kick', kp, 0.96, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('arg_808', bp, 0.92, 0.0))
        return ev
    renderer.render(142.0, 4, neo_pistea(full=True), "TrapArg_142BPM_NeoPistea_TumbandoClub_Full.wav", dir_arg, "Neo Pistea Tumbando Club Full Beat (142 BPM)")
    renderer.render(142.0, 4, neo_pistea(full=False), "TrapArg_142BPM_NeoPistea_TumbandoClub_Top_Loop.wav", dir_arg, "Neo Pistea Tumbando Club Top Loop (142 BPM)")

    # 13 & 14: Tiago PZK - Sola (136 BPM) - Melodic trap rhythm
    def tiago_pzk(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('arg_clap', b + 2.0, 0.92, 0.0))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.63 if s % 2 == 0 else 0.43, 0.15))
            ev.append(('arg_hat_open', b + 1.5, 0.45, -0.2))
        if full:
            kicks = [0.0, 1.5, 2.75, 4.0, 5.5, 8.0, 9.5, 10.75, 12.0, 13.5]
            for kp in kicks: ev.append(('arg_kick', kp, 0.94, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('arg_808', bp, 0.88, 0.0))
        return ev
    renderer.render(136.0, 4, tiago_pzk(full=True), "TrapArg_136BPM_TiagoPZK_Sola_Full.wav", dir_arg, "Tiago PZK Sola Full Beat (136 BPM)")
    renderer.render(136.0, 4, tiago_pzk(full=False), "TrapArg_136BPM_TiagoPZK_Sola_Top_Loop.wav", dir_arg, "Tiago PZK Sola Top Loop (136 BPM)")

    # 15 & 16: Milo J - Rara Vez (125 BPM) - Boom-trap hybrid groove
    def milo_j(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('arg_clap', b + 2.0, 0.92, 0.0))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.60 if s % 2 == 0 else 0.40, -0.1))
            ev.append(('arg_hat_open', b + 3.5, 0.42, 0.25))
        if full:
            kicks = [0.0, 1.75, 4.0, 5.5, 7.0, 8.0, 9.75, 12.0, 13.5, 15.0]
            for kp in kicks: ev.append(('arg_kick', kp, 0.94, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('arg_808', bp, 0.86, 0.0))
        return ev
    renderer.render(125.0, 4, milo_j(full=True), "TrapArg_125BPM_MiloJ_RaraVez_Full.wav", dir_arg, "Milo J Rara Vez Full Beat (125 BPM)")
    renderer.render(125.0, 4, milo_j(full=False), "TrapArg_125BPM_MiloJ_RaraVez_Top_Loop.wav", dir_arg, "Milo J Rara Vez Top Loop (125 BPM)")

    # =========================================================================
    # CATEGORY 2: LATIN TRAP PUERTO RICO (16 LOOPS: 8 FULL + 8 TOP)
    # References: Bad Bunny, Anuel AA, Eladio Carrión, Myke Towers
    # =========================================================================
    dir_pr_trap = os.path.join(BASE_DIR, "02_Trap_Puerto_Rico_(BadBunny_Anuel_Eladio_16)")
    
    # 17 & 18: Bad Bunny - Chambea (130 BPM) - Iconic Latin Trap acoustic rim & deep 808
    def bb_chambea(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('pr_trap_snare', b + 2.0, 0.94, 0.0))
            ev.append(('pr_trap_rim', b + 3.75, 0.65, 0.2))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.62 if s % 2 == 0 else 0.44, 0.15 if s % 2 == 0 else -0.15))
            ev.append(('arg_hat_open', b + 1.5, 0.48, -0.3))
        if full:
            kicks = [0.0, 1.5, 2.75, 4.0, 5.5, 7.0, 8.0, 9.5, 10.75, 12.0, 13.5, 14.75, 15.25]
            for kp in kicks: ev.append(('pr_trap_kick', kp, 0.95, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('pr_trap_808', bp, 0.92, 0.0))
        return ev
    renderer.render(130.0, 4, bb_chambea(full=True), "TrapPR_130BPM_BadBunny_Chambea_Full.wav", dir_pr_trap, "Bad Bunny Chambea Full Beat (130 BPM)")
    renderer.render(130.0, 4, bb_chambea(full=False), "TrapPR_130BPM_BadBunny_Chambea_Top_Loop.wav", dir_pr_trap, "Bad Bunny Chambea Top Loop (130 BPM)")

    # 19 & 20: Bad Bunny - Soy Peor (128 BPM) - Dark, slow, heavy sub
    def bb_soypeor(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('pr_trap_snare', b + 2.0, 0.92, 0.0))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.60 if s % 2 == 0 else 0.40, -0.1))
            ev.append(('arg_hat_open', b + 1.5, 0.45, 0.25))
        if full:
            kicks = [0.0, 1.5, 4.0, 5.75, 8.0, 9.5, 12.0, 13.5, 14.5]
            for kp in kicks: ev.append(('pr_trap_kick', kp, 0.95, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('pr_trap_808', bp, 0.94, 0.0))
        return ev
    renderer.render(128.0, 4, bb_soypeor(full=True), "TrapPR_128BPM_BadBunny_SoyPeor_Full.wav", dir_pr_trap, "Bad Bunny Soy Peor Full Beat (128 BPM)")
    renderer.render(128.0, 4, bb_soypeor(full=False), "TrapPR_128BPM_BadBunny_SoyPeor_Top_Loop.wav", dir_pr_trap, "Bad Bunny Soy Peor Top Loop (128 BPM)")

    # 21 & 22: Anuel AA - Sola (138 BPM) - Street Anthem Trap
    def anuel_sola(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('pr_trap_snare', b + 2.0, 0.95, 0.0))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.65 if s % 2 == 0 else 0.45, 0.15))
            for r in range(4): ev.append(('arg_hat', b + 3.5 + r * 0.125, 0.68, -0.2))
            ev.append(('arg_hat_open', b + 1.5, 0.48, -0.25))
        if full:
            kicks = [0.0, 1.5, 2.5, 4.0, 5.5, 6.75, 8.0, 9.5, 10.5, 12.0, 13.5, 14.75, 15.25]
            for kp in kicks: ev.append(('pr_trap_kick', kp, 0.95, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('pr_trap_808', bp, 0.90, 0.0))
        return ev
    renderer.render(138.0, 4, anuel_sola(full=True), "TrapPR_138BPM_Anuel_Sola_Full.wav", dir_pr_trap, "Anuel AA Sola Full Beat (138 BPM)")
    renderer.render(138.0, 4, anuel_sola(full=False), "TrapPR_138BPM_Anuel_Sola_Top_Loop.wav", dir_pr_trap, "Anuel AA Sola Top Loop (138 BPM)")

    # 23 & 24: Anuel AA & Ozuna - Brindemos (134 BPM) - Deep sub, punchy rims
    def anuel_brindemos(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('pr_trap_rim', b + 2.0, 0.95, 0.0))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.64 if s % 2 == 0 else 0.44, -0.15))
            ev.append(('arg_hat_open', b + 3.5, 0.45, 0.3))
        if full:
            kicks = [0.0, 1.5, 4.0, 5.5, 7.25, 8.0, 9.5, 12.0, 13.5, 15.0]
            for kp in kicks: ev.append(('pr_trap_kick', kp, 0.95, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('pr_trap_808', bp, 0.92, 0.0))
        return ev
    renderer.render(134.0, 4, anuel_brindemos(full=True), "TrapPR_134BPM_Anuel_Brindemos_Full.wav", dir_pr_trap, "Anuel Brindemos Full Beat (134 BPM)")
    renderer.render(134.0, 4, anuel_brindemos(full=False), "TrapPR_134BPM_Anuel_Brindemos_Top_Loop.wav", dir_pr_trap, "Anuel Brindemos Top Loop (134 BPM)")

    # 25 & 26: Eladio Carrion - Kemba Walker (140 BPM) - Technical rapid rolls & hard knock
    def eladio_kemba(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('pr_trap_snare', b + 2.0, 0.95, 0.0))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.66 if s % 2 == 0 else 0.46, 0.15))
            for r in range(6): ev.append(('arg_hat', b + 2.5 + r * (1.0/6.0), 0.65, -0.2))
            for r in range(6): ev.append(('arg_hat', b + 3.5 + r * (1.0/6.0), 0.70, 0.2))
            ev.append(('arg_hat_open', b + 1.5, 0.48, -0.25))
        if full:
            kicks = [0.0, 1.25, 2.75, 4.0, 5.25, 6.75, 8.0, 9.25, 10.75, 12.0, 13.25, 14.75, 15.25]
            for kp in kicks: ev.append(('pr_trap_kick', kp, 0.96, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('pr_trap_808', bp, 0.94, 0.0))
        return ev
    renderer.render(140.0, 4, eladio_kemba(full=True), "TrapPR_140BPM_EladioCarrion_KembaWalker_Full.wav", dir_pr_trap, "Eladio Carrion Kemba Walker Full Beat (140 BPM)")
    renderer.render(140.0, 4, eladio_kemba(full=False), "TrapPR_140BPM_EladioCarrion_KembaWalker_Top_Loop.wav", dir_pr_trap, "Eladio Carrion Kemba Walker Top Loop (140 BPM)")

    # 27 & 28: Eladio Carrion - Sauce Boy (136 BPM) - Smooth, clean bounce
    def eladio_sauce(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('pr_trap_rim', b + 2.0, 0.95, 0.0))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.64 if s % 2 == 0 else 0.44, -0.15))
            ev.append(('arg_hat_open', b + 1.5, 0.45, 0.25))
        if full:
            kicks = [0.0, 1.5, 2.75, 4.0, 5.5, 8.0, 9.5, 10.75, 12.0, 13.5, 14.75]
            for kp in kicks: ev.append(('pr_trap_kick', kp, 0.95, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('pr_trap_808', bp, 0.90, 0.0))
        return ev
    renderer.render(136.0, 4, eladio_sauce(full=True), "TrapPR_136BPM_EladioCarrion_SauceBoy_Full.wav", dir_pr_trap, "Eladio Carrion Sauce Boy Full Beat (136 BPM)")
    renderer.render(136.0, 4, eladio_sauce(full=False), "TrapPR_136BPM_EladioCarrion_SauceBoy_Top_Loop.wav", dir_pr_trap, "Eladio Carrion Sauce Boy Top Loop (136 BPM)")

    # 29 & 30: Bad Bunny - Monaco (142 BPM) - Orchestral heavy trap bounce
    def bb_monaco(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('pr_trap_snare', b + 2.0, 0.95, 0.0))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.65 if s % 2 == 0 else 0.45, 0.15))
            for r in range(4): ev.append(('arg_hat', b + 3.5 + r * 0.125, 0.70, -0.2))
            ev.append(('arg_hat_open', b + 1.5, 0.48, -0.3))
        if full:
            kicks = [0.0, 1.5, 2.5, 4.0, 5.5, 7.0, 8.0, 9.5, 10.5, 12.0, 13.5, 14.5, 15.25]
            for kp in kicks: ev.append(('pr_trap_kick', kp, 0.96, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('pr_trap_808', bp, 0.95, 0.0))
        return ev
    renderer.render(142.0, 4, bb_monaco(full=True), "TrapPR_142BPM_BadBunny_Monaco_Full.wav", dir_pr_trap, "Bad Bunny Monaco Full Beat (142 BPM)")
    renderer.render(142.0, 4, bb_monaco(full=False), "TrapPR_142BPM_BadBunny_Monaco_Top_Loop.wav", dir_pr_trap, "Bad Bunny Monaco Top Loop (142 BPM)")

    # 31 & 32: Myke Towers - Easy Money (135 BPM) - Crisp Puerto Rico swagger
    def myke_towers(full=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            ev.append(('pr_trap_snare', b + 2.0, 0.94, 0.0))
            for s in range(8): ev.append(('arg_hat', b + s * 0.5, 0.64 if s % 2 == 0 else 0.44, -0.15))
            ev.append(('arg_hat_open', b + 3.5, 0.45, 0.25))
        if full:
            kicks = [0.0, 1.5, 4.0, 5.5, 6.75, 8.0, 9.5, 12.0, 13.5, 14.75]
            for kp in kicks: ev.append(('pr_trap_kick', kp, 0.95, 0.0))
            for bp in [0.0, 4.0, 8.0, 12.0]: ev.append(('pr_trap_808', bp, 0.90, 0.0))
        return ev
    renderer.render(135.0, 4, myke_towers(full=True), "TrapPR_135BPM_MykeTowers_EasyMoney_Full.wav", dir_pr_trap, "Myke Towers Easy Money Full Beat (135 BPM)")
    renderer.render(135.0, 4, myke_towers(full=False), "TrapPR_135BPM_MykeTowers_EasyMoney_Top_Loop.wav", dir_pr_trap, "Myke Towers Easy Money Top Loop (135 BPM)")

    # =========================================================================
    # CATEGORY 3: REGGAETÓN CLÁSICO PUERTO RICO (16 LOOPS: 8 FULL + 8 TOP)
    # References: Daddy Yankee, Don Omar, Wisin & Yandel, Plan B, Luny Tunes
    # =========================================================================
    dir_pr_classic = os.path.join(BASE_DIR, "03_Reggaeton_Clasico_PR_(DaddyYankee_DonOmar_16)")

    # Helper for authentic Puerto Rico Dembow with Turnaround Fill in Bar 4
    def build_classic_pr_dembow(bpm, full=True, has_guira=True, has_timbal_fill=True):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            is_turnaround = (bar == 3)
            for beat in range(4):
                pos = b + beat
                if full:
                    # 4-on-the-floor kick
                    ev.append(('classic_kick', pos, 0.96, 0.0))
                
                # Bar 4 turnaround fill
                if is_turnaround and beat >= 2:
                    # Snare roll fill on beats 3 & 4
                    for r in range(4):
                        ev.append(('classic_snare', pos + r * 0.25, 0.85 + r * 0.03, 0.1 if r % 2 == 0 else -0.1))
                    if has_timbal_fill and beat == 3:
                        ev.append(('classic_timbal', pos + 0.75, 0.95, 0.3))
                else:
                    # Classic Luny Tunes Dembow: snare on beat + 0.75
                    ev.append(('classic_snare', pos + 0.75, 0.92, 0.0))
                    # Extra snare on beats 2 and 4 (beat index 1 and 3) + 0.5
                    if beat % 2 == 1:
                        ev.append(('classic_snare', pos - 0.5, 0.85, -0.1))
                
                # Güira scraping
                if has_guira:
                    ev.append(('classic_guira', pos + 0.0, 0.55, 0.25))
                    ev.append(('classic_guira', pos + 0.25, 0.45, -0.25))
                    ev.append(('classic_guira', pos + 0.5, 0.55, 0.25))
                    ev.append(('classic_guira', pos + 0.75, 0.45, -0.25))
        return ev

    # 33 & 34: Daddy Yankee - Gasolina (96 BPM)
    renderer.render(96.0, 4, build_classic_pr_dembow(96.0, full=True), "ReggaetonPR_96BPM_DaddyYankee_Gasolina_Full.wav", dir_pr_classic, "Daddy Yankee Gasolina Full Dembow (96 BPM)")
    renderer.render(96.0, 4, build_classic_pr_dembow(96.0, full=False), "ReggaetonPR_96BPM_DaddyYankee_Gasolina_Top_Loop.wav", dir_pr_classic, "Daddy Yankee Gasolina Top Loop (96 BPM)")

    # 35 & 36: Don Omar - Pobre Diabla (94 BPM)
    renderer.render(94.0, 4, build_classic_pr_dembow(94.0, full=True, has_guira=True), "ReggaetonPR_94BPM_DonOmar_PobreDiabla_Full.wav", dir_pr_classic, "Don Omar Pobre Diabla Full Dembow (94 BPM)")
    renderer.render(94.0, 4, build_classic_pr_dembow(94.0, full=False, has_guira=True), "ReggaetonPR_94BPM_DonOmar_PobreDiabla_Top_Loop.wav", dir_pr_classic, "Don Omar Pobre Diabla Top Loop (94 BPM)")

    # 37 & 38: Wisin & Yandel - Rakata (95 BPM)
    renderer.render(95.0, 4, build_classic_pr_dembow(95.0, full=True), "ReggaetonPR_95BPM_WisinYandel_Rakata_Full.wav", dir_pr_classic, "Wisin & Yandel Rakata Full Dembow (95 BPM)")
    renderer.render(95.0, 4, build_classic_pr_dembow(95.0, full=False), "ReggaetonPR_95BPM_WisinYandel_Rakata_Top_Loop.wav", dir_pr_classic, "Wisin & Yandel Rakata Top Loop (95 BPM)")

    # 39 & 40: Plan B - Frikitona (98 BPM) - Fast Perreo Pesado
    renderer.render(98.0, 4, build_classic_pr_dembow(98.0, full=True), "ReggaetonPR_98BPM_PlanB_Frikitona_Full.wav", dir_pr_classic, "Plan B Frikitona Full Dembow (98 BPM)")
    renderer.render(98.0, 4, build_classic_pr_dembow(98.0, full=False), "ReggaetonPR_98BPM_PlanB_Frikitona_Top_Loop.wav", dir_pr_classic, "Plan B Frikitona Top Loop (98 BPM)")

    # 41 & 42: Daddy Yankee - Rompe (92 BPM) - Stomp heavy kick
    renderer.render(92.0, 4, build_classic_pr_dembow(92.0, full=True), "ReggaetonPR_92BPM_DaddyYankee_Rompe_Full.wav", dir_pr_classic, "Daddy Yankee Rompe Full Dembow (92 BPM)")
    renderer.render(92.0, 4, build_classic_pr_dembow(92.0, full=False), "ReggaetonPR_92BPM_DaddyYankee_Rompe_Top_Loop.wav", dir_pr_classic, "Daddy Yankee Rompe Top Loop (92 BPM)")

    # 43 & 44: Don Omar - Dile (100 BPM) - High energy classic
    renderer.render(100.0, 4, build_classic_pr_dembow(100.0, full=True), "ReggaetonPR_100BPM_DonOmar_Dile_Full.wav", dir_pr_classic, "Don Omar Dile Full Dembow (100 BPM)")
    renderer.render(100.0, 4, build_classic_pr_dembow(100.0, full=False), "ReggaetonPR_100BPM_DonOmar_Dile_Top_Loop.wav", dir_pr_classic, "Don Omar Dile Top Loop (100 BPM)")

    # 45 & 46: Zion & Lennox - Yo Voy (94 BPM)
    renderer.render(94.0, 4, build_classic_pr_dembow(94.0, full=True), "ReggaetonPR_94BPM_ZionLennox_YoVoy_Full.wav", dir_pr_classic, "Zion & Lennox Yo Voy Full Dembow (94 BPM)")
    renderer.render(94.0, 4, build_classic_pr_dembow(94.0, full=False), "ReggaetonPR_94BPM_ZionLennox_YoVoy_Top_Loop.wav", dir_pr_classic, "Zion & Lennox Yo Voy Top Loop (94 BPM)")

    # 47 & 48: Tego Calderon - Pa Que Retozen (96 BPM)
    renderer.render(96.0, 4, build_classic_pr_dembow(96.0, full=True), "ReggaetonPR_96BPM_TegoCalderon_PaQueRetozen_Full.wav", dir_pr_classic, "Tego Calderon Pa Que Retozen Full Dembow (96 BPM)")
    renderer.render(96.0, 4, build_classic_pr_dembow(96.0, full=False), "ReggaetonPR_96BPM_TegoCalderon_PaQueRetozen_Top_Loop.wav", dir_pr_classic, "Tego Calderon Pa Que Retozen Top Loop (96 BPM)")

    # =========================================================================
    # CATEGORY 4: REGGAETÓN MODERNO & TAINY STYLE (16 LOOPS: 8 FULL + 8 TOP)
    # References: Tainy / Data, Bad Bunny / Un Verano Sin Ti, Feid, Mora, Rauw
    # =========================================================================
    dir_pr_modern = os.path.join(BASE_DIR, "04_Reggaeton_Moderno_PR_(Tainy_Feid_Mora_16)")

    def build_modern_tainy_dembow(bpm, full=True, syncopated_kick=False):
        ev = []
        for bar in range(4):
            b = bar * 4.0
            is_turnaround = (bar == 3)
            for beat in range(4):
                pos = b + beat
                if full:
                    if syncopated_kick and beat == 2:
                        # Tainy syncopated kick drop on beat 3
                        ev.append(('tainy_kick', pos + 0.5, 0.95, 0.0))
                    else:
                        ev.append(('tainy_kick', pos, 0.95, 0.0))
                
                # Tainy hybrid clap-snare
                ev.append(('tainy_snare', pos + 0.75, 0.90, 0.0))
                if beat % 2 == 1:
                    ev.append(('tainy_snare', pos - 0.5, 0.84, -0.1))
                
                # Shaker groove
                for s in range(4):
                    ev.append(('tainy_shaker', pos + s * 0.25, 0.50 if s % 2 == 0 else 0.35, 0.2 if s % 2 == 0 else -0.2))
            
            # Subtle wood/rim ghost perc
            ev.append(('tainy_perc', b + 1.25, 0.6, -0.35))
            if is_turnaround:
                # Modern ghost fill on bar 4
                ev.append(('tainy_perc', b + 3.25, 0.7, 0.35))
                ev.append(('tainy_perc', b + 3.5, 0.75, -0.35))
                ev.append(('tainy_snare', b + 3.75, 0.95, 0.0))
        return ev

    # 49 & 50: Bad Bunny - Titi Me Pregunto (96 BPM)
    renderer.render(96.0, 4, build_modern_tainy_dembow(96.0, full=True), "ReggaetonModern_96BPM_BadBunny_Titi_Full.wav", dir_pr_modern, "Bad Bunny Titi Modern Dembow (96 BPM)")
    renderer.render(96.0, 4, build_modern_tainy_dembow(96.0, full=False), "ReggaetonModern_96BPM_BadBunny_Titi_Top_Loop.wav", dir_pr_modern, "Bad Bunny Titi Top Loop (96 BPM)")

    # 51 & 52: Feid - Ferxxo 100 (92 BPM)
    renderer.render(92.0, 4, build_modern_tainy_dembow(92.0, full=True), "ReggaetonModern_92BPM_Feid_Ferxxo100_Full.wav", dir_pr_modern, "Feid Ferxxo 100 Full Dembow (92 BPM)")
    renderer.render(92.0, 4, build_modern_tainy_dembow(92.0, full=False), "ReggaetonModern_92BPM_Feid_Ferxxo100_Top_Loop.wav", dir_pr_modern, "Feid Ferxxo 100 Top Loop (92 BPM)")

    # 53 & 54: Tainy & Bad Bunny - Lo Siento BB / Data (95 BPM) - Syncopated sub kick
    renderer.render(95.0, 4, build_modern_tainy_dembow(95.0, full=True, syncopated_kick=True), "ReggaetonModern_95BPM_Tainy_Data_LoSientoBB_Full.wav", dir_pr_modern, "Tainy Data Lo Siento BB Full (95 BPM)")
    renderer.render(95.0, 4, build_modern_tainy_dembow(95.0, full=False), "ReggaetonModern_95BPM_Tainy_Data_Top_Loop.wav", dir_pr_modern, "Tainy Data Top Loop (95 BPM)")

    # 55 & 56: Rauw Alejandro - Desesperados (98 BPM)
    renderer.render(98.0, 4, build_modern_tainy_dembow(98.0, full=True), "ReggaetonModern_98BPM_Rauw_Desesperados_Full.wav", dir_pr_modern, "Rauw Alejandro Desesperados Full (98 BPM)")
    renderer.render(98.0, 4, build_modern_tainy_dembow(98.0, full=False), "ReggaetonModern_98BPM_Rauw_Desesperados_Top_Loop.wav", dir_pr_modern, "Rauw Alejandro Desesperados Top Loop (98 BPM)")

    # 57 & 58: Bad Bunny & Jhayco - Dakiti (90 BPM) - Atmospheric laidback groove
    renderer.render(90.0, 4, build_modern_tainy_dembow(90.0, full=True), "ReggaetonModern_90BPM_BadBunny_Dakiti_Full.wav", dir_pr_modern, "Bad Bunny Dakiti Full Dembow (90 BPM)")
    renderer.render(90.0, 4, build_modern_tainy_dembow(90.0, full=False), "ReggaetonModern_90BPM_BadBunny_Dakiti_Top_Loop.wav", dir_pr_modern, "Bad Bunny Dakiti Top Loop (90 BPM)")

    # 59 & 60: Mora & Jhayco - Memorias (94 BPM) - Afro-Dembow swing
    renderer.render(94.0, 4, build_modern_tainy_dembow(94.0, full=True), "ReggaetonModern_94BPM_Mora_Memorias_Full.wav", dir_pr_modern, "Mora Memorias Full Dembow (94 BPM)")
    renderer.render(94.0, 4, build_modern_tainy_dembow(94.0, full=False), "ReggaetonModern_94BPM_Mora_Memorias_Top_Loop.wav", dir_pr_modern, "Mora Memorias Top Loop (94 BPM)")

    # 61 & 62: Feid & Young Miko - Classy 101 (93 BPM)
    renderer.render(93.0, 4, build_modern_tainy_dembow(93.0, full=True), "ReggaetonModern_93BPM_Feid_Classy101_Full.wav", dir_pr_modern, "Feid Classy 101 Full Dembow (93 BPM)")
    renderer.render(93.0, 4, build_modern_tainy_dembow(93.0, full=False), "ReggaetonModern_93BPM_Feid_Classy101_Top_Loop.wav", dir_pr_modern, "Feid Classy 101 Top Loop (93 BPM)")

    # 63 & 64: Bad Bunny & Tainy - Callaita (95 BPM) - Deep sub-bass dembow
    renderer.render(95.0, 4, build_modern_tainy_dembow(95.0, full=True, syncopated_kick=True), "ReggaetonModern_95BPM_Tainy_Callaita_Full.wav", dir_pr_modern, "Bad Bunny Callaita Full Dembow (95 BPM)")
    renderer.render(95.0, 4, build_modern_tainy_dembow(95.0, full=False), "ReggaetonModern_95BPM_Tainy_Callaita_Top_Loop.wav", dir_pr_modern, "Bad Bunny Callaita Top Loop (95 BPM)")

    print("[SUCCESS] All 64 Authentic Drum Loops Rendered Across 4 Hit Categories!")

if __name__ == "__main__":
    generate_all()
