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

# Initialize temperature field (uniform 37°C)
T = np.ones((ny, nx)) * 37.0

# Coordinates
x = np.linspace(0, width, nx)
y = np.linspace(0, height, ny)
X, Y = np.meshgrid(x, y)

# DNA particles
num_particles = 200
particles = np.zeros((num_particles, 2))
particles[:, 0] = np.random.uniform(0.1, width - 0.1, num_particles)
particles[:, 1] = np.random.uniform(0.1, height - 0.1, num_particles)

D_base = 0.0000114  # cm²/s at 25°C for 40bp DNA

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
ax.set_title("Uniform Temperature + Particle Simulation")
ax.set_xlabel("Width (cm)")
ax.set_ylabel("Height (cm)")

# Update function
def update(frame):
    global T, particles

    for _ in range(n_substeps):
        # Temperature stays uniform
        T_p = interpolate_field(T, particles[:, 0], particles[:, 1])
        D = D_base * (T_p / 25.0)  # Scaling diffusion by temperature

        rand_dx = np.sqrt(2 * D * dt) * np.random.randn(num_particles)
        rand_dy = np.sqrt(2 * D * dt) * np.random.randn(num_particles)

        particles[:, 0] += rand_dx
        particles[:, 1] += rand_dy

        particles[:, 0] = np.clip(particles[:, 0], 0, width)
        particles[:, 1] = np.clip(particles[:, 1], 0, height)

    im.set_array(T)
    scat.set_offsets(particles)
    return [im, scat]

# Animate
ani = animation.FuncAnimation(fig, update, frames=steps, interval=20, blit=True)

# Save animation
ani.save("uniform_temperature_control_37C.gif", writer='pillow', fps=20)

plt.tight_layout()
plt.show()
