import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from scipy.interpolate import RegularGridInterpolator

# Chamber and grid setup
width, height = 3.5, 3.5  # cm
nx, ny = 70, 70
dx = width / nx
dy = height / ny

# Simulation parameters
alpha = 0.00134  # thermal diffusivity (cm²/s)
dt = 0.05        # time step (s)
steps = 1600      # total animation frames
n_substeps = 5   # substeps per frame

# Initialize temperature field
T = np.ones((ny, nx)) * 25.0

# Coordinates
x = np.linspace(0, width, nx)
y = np.linspace(0, height, ny)
X, Y = np.meshgrid(x, y)

# Heater/Cooler parameters
heater_temp_high = 50
heater_temp_low = 25
cooler_temp_low = 10
cooler_temp_high = 25
cycle_duration = 400  # frames before switching

# Define cooler region (1 cm × 1 cm square at center)
cooler_x_min = (width / 2) - 0.5
cooler_x_max = (width / 2) + 0.5
cooler_y_min = (height / 2) - 0.5
cooler_y_max = (height / 2) + 0.5

cooler_mask = (X >= cooler_x_min) & (X <= cooler_x_max) & (Y >= cooler_y_min) & (Y <= cooler_y_max)

def apply_boundary_conditions(timestep):
    phase = (timestep // cycle_duration) % 2

    if phase == 0:
        T[:, 0] = heater_temp_high
        T[:, -1] = heater_temp_low
        T[cooler_mask] = cooler_temp_low
    else:
        T[:, 0] = heater_temp_low
        T[:, -1] = heater_temp_high
        T[cooler_mask] = cooler_temp_high

# DNA particles
num_particles = 200
particles = np.zeros((num_particles, 2))
particles[:, 0] = np.random.uniform(0.1, width - 0.1, num_particles)
particles[:, 1] = np.random.uniform(0.1, height - 0.1, num_particles)

D_base = 0.0000097  # cm²/s at 25°C for 40bp DNA

# Interpolator
def interpolate_field(field, px, py):
    interp = RegularGridInterpolator((y, x), field, bounds_error=False, fill_value=None)
    coords = np.stack((py, px), axis=-1)
    return interp(coords)

# Plot setup
fig, ax = plt.subplots(figsize=(6, 6))
im = ax.imshow(T, cmap='inferno', vmin=10, vmax=50, origin='lower',
               extent=[0, width, 0, height])
scat = ax.scatter(particles[:, 0], particles[:, 1], s=10, c='cyan', edgecolor='black')
plt.colorbar(im, ax=ax, label='Temperature (°C)')
ax.set_title("Heaters & Cooler + Particle Simulation")
ax.set_xlabel("Width (cm)")
ax.set_ylabel("Height (cm)")

# Update function
def update(frame):
    global T, particles

    for _ in range(n_substeps):
        apply_boundary_conditions(frame)

        # Temperature diffusion (explicit finite difference)
        Tn = T.copy()
        Tn[1:-1, 1:-1] += alpha * dt * (
            (T[1:-1, 2:] - 2*T[1:-1, 1:-1] + T[1:-1, :-2]) / dx**2 +
            (T[2:, 1:-1] - 2*T[1:-1, 1:-1] + T[:-2, 1:-1]) / dy**2
        )
        T = Tn

        # Apply heaters/cooler again after diffusion to maintain boundary temps
        apply_boundary_conditions(frame)

        # Temperature gradients (∇T)
        dTdx = np.gradient(T, dx, axis=1)
        dTdy = np.gradient(T, dy, axis=0)

        grad_Tx = interpolate_field(dTdx, particles[:, 0], particles[:, 1])
        grad_Ty = interpolate_field(dTdy, particles[:, 0], particles[:, 1])

        # Drift velocity (thermophoresis)
        drift_coeff = 0.0001  # cm²/s/°C — tunable
        drift_x = -drift_coeff * grad_Tx
        drift_y = -drift_coeff * grad_Ty

        # Temperature-dependent diffusion
        T_p = interpolate_field(T, particles[:, 0], particles[:, 1])
        D = D_base * (T_p / 25.0)

        rand_dx = np.sqrt(2 * D * dt) * np.random.randn(num_particles)
        rand_dy = np.sqrt(2 * D * dt) * np.random.randn(num_particles)

        # Position update
        particles[:, 0] += drift_x * dt + rand_dx
        particles[:, 1] += drift_y * dt + rand_dy

        # Keep particles inside chamber
        particles[:, 0] = np.clip(particles[:, 0], 0, width)
        particles[:, 1] = np.clip(particles[:, 1], 0, height)

    im.set_array(T)
    scat.set_offsets(particles)
    return [im, scat]

# Animate
ani = animation.FuncAnimation(fig, update, frames=steps, interval=20, blit=True)

# Save animation
ani.save("alternating_heaters_with_central_cooler.gif", writer='pillow', fps=20)

plt.tight_layout()
plt.show()
