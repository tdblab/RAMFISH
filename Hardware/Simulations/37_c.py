import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from scipy.interpolate import RegularGridInterpolator

# Circular chamber parameters
diameter = 1.8  # cm
radius = diameter / 2

# Simulation grid
nx, ny = 70, 70
width, height = diameter, diameter
dx = width / nx
dy = height / ny

# Simulation parameters
alpha = 0.00143  # thermal diffusivity (cm²/s)
dt = 0.05        # time step (s)
steps = 1600      # total animation frames
n_substeps = 5   # substeps per frame

# Coordinates
x = np.linspace(0, width, nx)
y = np.linspace(0, height, ny)
X, Y = np.meshgrid(x, y)

# Circle center
cx, cy = radius, radius

# Create circular mask
mask = (X - cx)**2 + (Y - cy)**2 <= radius**2

# Temperature field
T = np.ones((ny, nx)) * 37.0
T[~mask] = np.nan

# DNA particles
num_particles = 200
particles = np.zeros((num_particles, 2))

# Initialize particles INSIDE circle
for i in range(num_particles):
    while True:
        px = np.random.uniform(0, width)
        py = np.random.uniform(0, height)
        if (px - cx)**2 + (py - cy)**2 <= radius**2:
            particles[i] = [px, py]
            break

D_base = 0.0000085  # cm²/s at 25°C for 40bp DNA

# Interpolator
def interpolate_field(field, px, py):
    interp = RegularGridInterpolator((y, x), field, bounds_error=False, fill_value=None)
    coords = np.stack((py, px), axis=-1)
    return interp(coords)

# Plot setup
fig, ax = plt.subplots(figsize=(6,6))

im = ax.imshow(T, cmap='inferno', vmin=10, vmax=50,
               origin='lower',
               extent=[0,width,0,height])

scat = ax.scatter(particles[:,0], particles[:,1],
                  s=10, c='cyan', edgecolor='black')

circle = plt.Circle((cx,cy), radius, color='white', fill=False, linewidth=2)
ax.add_patch(circle)

plt.colorbar(im, ax=ax, label="Temperature (°C)")
ax.set_title("37°C Particle Simulation")
ax.set_xlabel("cm")
ax.set_ylabel("cm")

# Update function
def update(frame):
    global particles

    for _ in range(n_substeps):

        T_p = interpolate_field(T, particles[:,0], particles[:,1])
        D = D_base * (T_p / 25.0)

        rand_dx = np.sqrt(2 * D * dt) * np.random.randn(num_particles)
        rand_dy = np.sqrt(2 * D * dt) * np.random.randn(num_particles)

        particles[:,0] += rand_dx
        particles[:,1] += rand_dy

        # Reflect particles if they hit circular wall
        for i in range(num_particles):
            dxp = particles[i,0] - cx
            dyp = particles[i,1] - cy
            dist = np.sqrt(dxp**2 + dyp**2)

            if dist > radius:
                particles[i,0] = cx + dxp/dist * radius
                particles[i,1] = cy + dyp/dist * radius

    scat.set_offsets(particles)

    return [scat]

# Animate
ani = animation.FuncAnimation(fig, update,
                              frames=steps,
                              interval=20,
                              blit=True)

# Save animation
ani.save("circular_chamber_37C.gif", writer='pillow', fps=20)

plt.tight_layout()
plt.show()