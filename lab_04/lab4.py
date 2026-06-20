import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, CheckButtons
from scipy.signal import butter, filtfilt

class HarmonicApp:
    def __init__(self):
        self.init_amp = 1.0
        self.init_freq = 1.0
        self.init_phase = 0.0
        
        self.init_noise_mean = 0.0
        self.init_noise_cov = 0.1
        
        self.init_cutoff = 5.0
        
        self.t = np.linspace(0, 10, 1000)
        self.fs = 100.0
        
        self.current_noise = None
        self.last_noise_params = (None, None)
        self.generate_noise(self.init_noise_mean, self.init_noise_cov)
        
        self.setup_gui()

    def generate_noise(self, mean, cov):
        if self.last_noise_params != (mean, cov):
            std_dev = np.sqrt(max(cov, 0.0001)) 
            self.current_noise = np.random.normal(mean, std_dev, len(self.t))
            self.last_noise_params = (mean, cov)

    def harmonic_with_noise(self, amplitude, frequency, phase, noise_mean, noise_covariance, show_noise):
        clean_y = amplitude * np.sin(2 * np.pi * frequency * self.t + phase)
        
        self.generate_noise(noise_mean, noise_covariance)
        
        noisy_y = clean_y + self.current_noise
        
        return clean_y, noisy_y if show_noise else clean_y, noisy_y

    def apply_filter(self, data, cutoff):
        """Застосування низькочастотного фільтру Баттерворта"""
        nyq = 0.5 * self.fs
        normal_cutoff = cutoff / nyq
        if normal_cutoff >= 1.0:
            normal_cutoff = 0.99
        if normal_cutoff <= 0:
            normal_cutoff = 0.01
            
        b, a = butter(3, normal_cutoff, btype='low', analog=False)
        y = filtfilt(b, a, data)
        return y

    def setup_gui(self):
        self.fig, self.ax = plt.subplots(figsize=(10, 7))
        plt.subplots_adjust(left=0.1, bottom=0.5)

        clean_y, display_y, noisy_y = self.harmonic_with_noise(
            self.init_amp, self.init_freq, self.init_phase, 
            self.init_noise_mean, self.init_noise_cov, True
        )
        filtered_y = self.apply_filter(noisy_y, self.init_cutoff)

        self.line_display, = self.ax.plot(self.t, display_y, lw=2, label='Signal (Noisy/Clean)', color='orange')
        self.line_filtered, = self.ax.plot(self.t, filtered_y, lw=2, label='Filtered Signal', color='blue', linestyle='--')
        self.ax.legend()
        self.ax.set_ylim(-3, 3)
        self.ax.set_title("Гармоніка з шумом та фільтрація")

        axcolor = 'lightgoldenrodyellow'
        
        self.ax_amp = plt.axes([0.15, 0.40, 0.65, 0.03], facecolor=axcolor)
        self.ax_freq = plt.axes([0.15, 0.35, 0.65, 0.03], facecolor=axcolor)
        self.ax_phase = plt.axes([0.15, 0.30, 0.65, 0.03], facecolor=axcolor)
        self.ax_nmean = plt.axes([0.15, 0.25, 0.65, 0.03], facecolor=axcolor)
        self.ax_ncov = plt.axes([0.15, 0.20, 0.65, 0.03], facecolor=axcolor)
        self.ax_cutoff = plt.axes([0.15, 0.15, 0.65, 0.03], facecolor=axcolor)

        self.s_amp = Slider(self.ax_amp, 'Amplitude', 0.1, 5.0, valinit=self.init_amp)
        self.s_freq = Slider(self.ax_freq, 'Frequency', 0.1, 10.0, valinit=self.init_freq)
        self.s_phase = Slider(self.ax_phase, 'Phase', 0.0, 2*np.pi, valinit=self.init_phase)
        self.s_nmean = Slider(self.ax_nmean, 'Noise Mean', -1.0, 1.0, valinit=self.init_noise_mean)
        self.s_ncov = Slider(self.ax_ncov, 'Noise Cov (Var)', 0.0, 1.0, valinit=self.init_noise_cov)
        self.s_cutoff = Slider(self.ax_cutoff, 'Filter Cutoff', 0.1, 20.0, valinit=self.init_cutoff)

        self.ax_check = plt.axes([0.85, 0.25, 0.12, 0.15], facecolor=axcolor)
        self.check = CheckButtons(self.ax_check, ('Show Noise', 'Show Filtered'), (True, True))

        self.ax_reset = plt.axes([0.85, 0.15, 0.1, 0.04])
        self.b_reset = Button(self.ax_reset, 'Reset', color=axcolor, hovercolor='0.975')

        self.s_amp.on_changed(self.update)
        self.s_freq.on_changed(self.update)
        self.s_phase.on_changed(self.update)
        self.s_nmean.on_changed(self.update)
        self.s_ncov.on_changed(self.update)
        self.s_cutoff.on_changed(self.update)
        self.check.on_clicked(self.update)
        self.b_reset.on_clicked(self.reset)

        plt.show()

    def update(self, val=None):
        show_noise = self.check.get_status()[0]
        show_filtered = self.check.get_status()[1]

        clean_y, display_y, noisy_y = self.harmonic_with_noise(
            self.s_amp.val, self.s_freq.val, self.s_phase.val,
            self.s_nmean.val, self.s_ncov.val, show_noise
        )

        filtered_y = self.apply_filter(noisy_y, self.s_cutoff.val)

        self.line_display.set_ydata(display_y)
        
        if show_filtered:
            self.line_filtered.set_ydata(filtered_y)
            self.line_filtered.set_alpha(1.0)
        else:
            self.line_filtered.set_alpha(0.0)

        self.fig.canvas.draw_idle()

    def reset(self, event):
        self.s_amp.reset()
        self.s_freq.reset()
        self.s_phase.reset()
        self.s_nmean.reset()
        self.s_ncov.reset()
        self.s_cutoff.reset()
        if not self.check.get_status()[0]: self.check.set_active(0)
        if not self.check.get_status()[1]: self.check.set_active(1)

if __name__ == '__main__':
    app = HarmonicApp()
