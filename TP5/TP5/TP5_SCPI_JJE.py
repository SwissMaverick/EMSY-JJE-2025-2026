"""
bode_measurement.py
Acquisition automatique d'un diagramme de Bode via VISA (SCPI).
Instruments : oscilloscope RTB2004 + generateur Keysight 33500B
"""

import csv
import time
from typing import Optional, List, Dict, Tuple
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import pyvisa

# ===========================================================
# PARAMETRES GLOBAUX
# ===========================================================
CFG = {
    "osc_ip":   "10.228.134.39",
    "gen_ip":   "10.228.134.53",
    "csv_out":  "mesures_bode.csv",
    "plot_out": "diagramme_bode.png",
    "amplitude_vpp": 5,
    "offset_v":      0,
    "freq_start":    10,
    "freq_stop":     1_000_000,
}

CSV_HEADER = [
    "Frequence_Hz", "Vpp_IN_V", "Vpp_OUT_V",
    "Periode_s",    "Gain",     "Gain_dB",
]


# ===========================================================
# GENERATION DE LA GRILLE DE FREQUENCES  (sequence 1-2-5)
# ===========================================================
def build_frequency_grid(f_min: float, f_max: float) -> List[float]:
    """
    Construit une liste logarithmique selon la sequence 1-2-5.
    Exemple : 10, 20, 50, 100, 200, 500, 1k, 2k, 5k …
    """
    result, f = [], f_min
    while f <= f_max:
        result.append(f)
        first_digit = int(str(f)[0])
        if first_digit == 1:
            f = f * 2
        elif first_digit == 2:
            f = f * 5 // 2
        else:
            f = f * 2
    return result


# ===========================================================
# GESTION DES INSTRUMENTS
# ===========================================================
class InstrumentManager:
    """Ouvre et configure les deux instruments VISA."""

    def __init__(self, osc_ip: str, gen_ip: str):
        self._rm  = pyvisa.ResourceManager("@py")
        self.osc  = self._rm.open_resource(f"TCPIP::{osc_ip}::INSTR")
        self.gen  = self._rm.open_resource(f"TCPIP::{gen_ip}::5025::SOCKET")

        self.gen.write_termination = "\n"
        self.gen.read_termination  = "\n"
        self.osc.timeout = 15_000   # ms
        self.gen.timeout = 10_000   # ms

    def identify(self) -> None:
        sep = "=" * 55
        print(sep)
        print("Oscilloscope :", self.osc.query("*IDN?").strip())
        print("Generateur   :", self.gen.query("*IDN?").strip())
        print(sep)

    def setup_generator(self, amplitude: float, offset: float, init_freq: float = 1_000) -> None:
        cmds = [
            "SOUR1:FUNC SIN",
            f"SOUR1:VOLT {amplitude}",
            f"SOUR1:VOLT:OFFS {offset}",
            f"SOUR1:FREQ {init_freq}",
            "OUTP1 ON",
        ]
        for cmd in cmds:
            self.gen.write(cmd)
        state = self.gen.query("OUTP1?").strip()
        print(f"Etat sortie generateur : {state}")   # attend "1" ou "ON"

    def setup_oscilloscope(self) -> None:
        settings = [
            "CHAN1:STAT ON",  "CHAN2:STAT ON",
            "CHAN1:SCAL 1",   "CHAN2:SCAL 1",
            "CHAN1:COUP DC",  "CHAN2:COUP DC",
            "TRIG:A:SOUR CH1",
            "TRIG:A:LEV 0",
        ]
        for cmd in settings:
            self.osc.write(cmd)

    def set_frequency(self, freq: float) -> None:
        self.gen.write(f"SOUR1:FREQ {freq}")
        # ~3 periodes sur 10 divisions
        tscal = max(1e-7, (3.0 / freq) / 10.0)
        self.osc.write(f"TIM:SCAL {tscal:.6e}")

    def read_measurement(self, slot: int, source: str, mtype: str,
                         freq: float, retries: int = 3) -> Optional[float]:
        """
        Configure un slot de mesure SCPI et lit le resultat.
        Retente jusqu'a `retries` fois en cas de timeout.
        """
        self.osc.write(f"MEAS{slot}:SOUR {source}")
        self.osc.write(f"MEAS{slot}:TYPE {mtype}")
        self.osc.write(f"MEAS{slot}:ENAB ON")
        self.osc.query("*OPC?")

        for attempt in range(1, retries + 1):
            try:
                return float(self.osc.query(f"MEAS{slot}:RES?"))
            except pyvisa.errors.VisaIOError:
                print(f"  [WARN] Timeout MEAS{slot} ({source}/{mtype}) "
                      f"@ {freq:.0f} Hz — essai {attempt}/{retries}")
                time.sleep(0.5)

        print(f"  [ERR]  MEAS{slot} abandonne apres {retries} essais.")
        return None

    def shutdown(self) -> None:
        self.gen.write("OUTP1 OFF")
        self.osc.close()
        self.gen.close()
        self._rm.close()


# ===========================================================
# ACQUISITION
# ===========================================================
def compute_gain(vpp_in: float, vpp_out: float) -> Tuple[float, float]:
    """Retourne (gain_lineaire, gain_dB). Retourne (0, -999) si entree nulle."""
    if vpp_in > 0:
        g = vpp_out / vpp_in
        return g, 20.0 * np.log10(g) if g > 0 else -999.0
    return 0.0, -999.0


def acquire_bode_data(instr: InstrumentManager,
                      frequencies: List[float],
                      csv_path: str) -> None:
    """Balaye les frequences, mesure Vpp et periode, ecrit le CSV."""

    with open(csv_path, "w", newline="") as fh:
        writer = csv.writer(fh, delimiter=";")
        writer.writerow(CSV_HEADER)

        for freq in frequencies:
            print(f"\n{'─'*45}")
            print(f"  Frequence : {freq:>10.0f} Hz")

            instr.set_frequency(freq)

            # Attente de stabilisation (10 periodes minimum, 2 s plancher)
            wait_time = max(2.0, 20.0 / freq)
            print(f"  Stabilisation : {wait_time:.1f} s ...")
            time.sleep(wait_time)

            vpp_in  = instr.read_measurement(1, "CH1", "VPP",    freq) or 0.0
            vpp_out = instr.read_measurement(2, "CH2", "VPP",    freq) or 0.0
            period  = instr.read_measurement(3, "CH1", "PERIOD", freq) or (1.0 / freq)

            gain, gain_db = compute_gain(vpp_in, vpp_out)

            print(f"  Vpp_IN  = {vpp_in:.4f} V")
            print(f"  Vpp_OUT = {vpp_out:.4f} V")
            print(f"  Periode = {period:.6e} s")
            print(f"  Gain    = {gain_db:.2f} dB")

            writer.writerow([
                f"{freq:.0f}",
                f"{vpp_in:.6f}",
                f"{vpp_out:.6f}",
                f"{period:.9f}",
                f"{gain:.6f}",
                f"{gain_db:.4f}",
            ])


# ===========================================================
# TRACÉ DU DIAGRAMME DE BODE
# ===========================================================
def load_csv(csv_path: str) -> Dict[str, np.ndarray]:
    """Charge le CSV et retourne un dict de tableaux numpy."""
    rows = {"Frequence_Hz": [], "Vpp_IN_V": [], "Vpp_OUT_V": [], "Gain_dB": []}
    with open(csv_path, "r", newline="") as fh:
        for row in csv.DictReader(fh, delimiter=";"):
            rows["Frequence_Hz"].append(float(row["Frequence_Hz"]))
            rows["Vpp_IN_V"].append(float(row["Vpp_IN_V"]))
            rows["Vpp_OUT_V"].append(float(row["Vpp_OUT_V"]))
            rows["Gain_dB"].append(float(row["Gain_dB"]))
    return {k: np.array(v) for k, v in rows.items()}


def freq_label(val: float, _pos) -> str:
    """Formatteur d'axe X : Hz / kHz / MHz."""
    if val >= 1_000_000:
        return f"{val / 1_000_000:.0f} MHz"
    if val >= 1_000:
        return f"{val / 1_000:.0f} kHz"
    return f"{val:.0f} Hz"


def plot_bode(csv_path: str, plot_path: str) -> None:
    """Lit le CSV et genere le diagramme de Bode (2 sous-graphes)."""
    print("\nGeneration du diagramme de Bode ...")
    data = load_csv(csv_path)

    # Remplace les sentinelles -999 par NaN pour eviter les artefacts
    gain_db = np.where(data["Gain_dB"] <= -998, np.nan, data["Gain_dB"])

    fig, (ax_amp, ax_gain) = plt.subplots(
        2, 1, figsize=(10, 8), sharex=True
    )
    fig.suptitle("Diagramme de Bode — Mesures TP5",
                 fontsize=14, fontweight="bold")

    # --- Sous-graphe 1 : amplitudes ---
    for arr, color, label in [
        (data["Vpp_IN_V"],  "b", "Vpp IN  (CH1)"),
        (data["Vpp_OUT_V"], "r", "Vpp OUT (CH2)"),
    ]:
        ax_amp.semilogx(data["Frequence_Hz"], arr,
                        f"{color}-o", markersize=5, label=label)

    ax_amp.set_ylabel("Amplitude Vpp (V)")
    ax_amp.set_title("Amplitude en fonction de la frequence")
    ax_amp.legend(loc="best")
    for ax in (ax_amp,):
        ax.grid(True, which="both", linestyle="--", linewidth=0.5)
        ax.grid(True, which="minor", linestyle=":", linewidth=0.3, alpha=0.5)

    # --- Sous-graphe 2 : gain ---
    ax_gain.semilogx(data["Frequence_Hz"], gain_db,
                     "g-o", markersize=5, label="Gain (dB)")
    ax_gain.axhline(y=-3, color="orange", linestyle="--",
                    linewidth=1.2, label="-3 dB")
    ax_gain.set_xlabel("Frequence (Hz)")
    ax_gain.set_ylabel("Gain (dB)")
    ax_gain.set_title("Gain en fonction de la frequence")
    ax_gain.legend(loc="best")
    ax_gain.grid(True, which="both", linestyle="--", linewidth=0.5)
    ax_gain.grid(True, which="minor", linestyle=":", linewidth=0.3, alpha=0.5)

    ax_gain.xaxis.set_major_formatter(ticker.FuncFormatter(freq_label))
    ax_gain.xaxis.set_minor_locator(ticker.LogLocator(subs="all"))

    plt.tight_layout()
    plt.savefig(plot_path, dpi=150, bbox_inches="tight")
    plt.show()
    print(f"  Diagramme sauvegarde dans : {plot_path}")


# ===========================================================
# POINT D'ENTREE
# ===========================================================
def main() -> None:
    frequencies = build_frequency_grid(CFG["freq_start"], CFG["freq_stop"])

    instr = InstrumentManager(CFG["osc_ip"], CFG["gen_ip"])
    instr.identify()
    instr.setup_generator(CFG["amplitude_vpp"], CFG["offset_v"])
    instr.setup_oscilloscope()

    try:
        acquire_bode_data(instr, frequencies, CFG["csv_out"])
    finally:
        instr.shutdown()

    sep = "=" * 55
    print(f"\n{sep}")
    print(f"  Mesures enregistrees dans : {CFG['csv_out']}")
    print(f"  Points mesures            : {len(frequencies)}")
    print(f"  Plage                     : {frequencies[0]} Hz — {frequencies[-1]} Hz")
    print(f"{sep}")

    plot_bode(CFG["csv_out"], CFG["plot_out"])


if __name__ == "__main__":
    main()