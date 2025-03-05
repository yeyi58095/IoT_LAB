import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.signal import hilbert
from scipy.fftpack import fft

# 讀取 Excel 檔案
file_path = "../data_3.xlsx"
df = pd.read_excel(file_path, header=None)

# 手動命名欄位（基於先前的數據結構）
df.columns = ["Time", "ECG", "NaN1", "NaN2", "NaN3", "Time_Duplicated", "PPG"]

# 選擇 ECG 或 PPG 訊號
signal = df["ECG"].dropna().values  # 去除 NaN 以避免 Hilbert 變換錯誤

# **計算 Hilbert 變換（求解析信號）**
analytic_signal = hilbert(signal)
envelope = np.abs(analytic_signal)  # 取振幅作為包絡線

# **FFT 分析包絡線頻率**
N = len(envelope)  # 取樣點數
fs = 1000  # 取樣率（請確認你的數據的實際取樣率）

# 計算 FFT 並只取正頻率部分
frequencies = np.fft.fftfreq(N, d=1/fs)  # 計算頻率軸
fft_values = np.abs(fft(envelope)) / N  # 正規化 FFT 振幅

# 只保留正頻率部分
pos_mask = frequencies > 0
frequencies = frequencies[pos_mask]
fft_values = fft_values[pos_mask]

# **繪圖**
plt.figure(figsize=(12, 6))

# 時域信號與包絡線
plt.subplot(2, 1, 1)
plt.plot(signal, label="Original Signal")
plt.plot(envelope, label="Envelope", linewidth=2)
plt.legend()
plt.title("Signal & Envelope")

# 頻譜分析（只取正頻率部分）
plt.subplot(2, 1, 2)
plt.plot(frequencies, fft_values)
plt.title("Envelope Frequency Spectrum")
plt.xlabel("Frequency (Hz)")
plt.ylabel("Amplitude")
plt.grid()

plt.show()
