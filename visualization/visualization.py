import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from scipy.io import loadmat
from matplotlib.gridspec import GridSpec

def add_axis(filename, ax, label):
    data = loadmat(filename)

    tensor = data['density']
    tensor = np.transpose(tensor, axes=(1, 0, 2))

    Nx, Ny, Nz = 2048, 256, 256
    X, Y, Z = np.meshgrid(np.arange(Nx), np.arange(Ny), np.arange(Nz))

    kw = {
        'vmin': tensor.min(),
        'vmax': tensor.max(),
        'levels': np.linspace(tensor.min(), tensor.max(), 10),
    }

    # Plot the surfaces
    _ = ax.contourf(
        X[:, :, -1], Y[:, :, -1], tensor[:, :, -1], 
        zdir='z', offset=Z.max(), **kw
    )

    _ = ax.contourf(
        X[0, :, :], tensor[0, :, :], Z[0, :, :],
        zdir='y', offset=Y.min(), **kw
    )

    C = ax.contourf(
        tensor[:, -1, :], Y[:, -1, :], Z[:, -1, :], 
        zdir='x', offset=X.max(), **kw
    )

    # Set limits to match the box dimensions
    xmin, xmax = X.min(), X.max()
    ymin, ymax = Y.min(), Y.max()
    zmin, zmax = Z.min(), Z.max()

    # Plot edges to enhance the box appearance
    edges_kw = dict(color='0.4', linewidth=1, zorder=1e3)
    ax.plot([xmax, xmax], [ymin, ymax], [zmax, zmax], **edges_kw)
    ax.plot([xmin, xmax], [ymin, ymin], [zmax, zmax], **edges_kw)
    ax.plot([xmax, xmax], [ymin, ymin], [zmin, zmax], **edges_kw)
    ax.plot([xmin, xmin], [ymin, ymin], [zmin, zmax], **edges_kw)
    ax.plot([xmin, xmax], [ymin, ymin], [zmin, zmin], **edges_kw)
    ax.plot([xmax, xmax], [ymin, ymax], [zmin, zmin], **edges_kw)
    ax.plot([xmin, xmin], [ymin, ymax], [zmax, zmax], **edges_kw)
    ax.plot([xmin, xmax], [ymax, ymax], [zmax, zmax], **edges_kw)
    ax.plot([xmax, xmax], [ymax, ymax], [zmin, zmax], **edges_kw)

    # Remove grid ticks and numbers
    ax.set_axis_off()
    ax.text(0.5, -0.1, 0, label, fontsize=10, transform=ax.transAxes, zorder=100)

    # Set the viewpoint and aspect ratio
    ax.view_init(30, -40)  # Adjust the angle for better visualization
    ax.set_box_aspect([Nx, Ny, Nz], zoom=1.4)  # Adjust zoom to fit proportions

    return C

# Create a figure with 3D axes
fig = plt.figure(figsize=(12, 6))
gs = GridSpec(2, 3, figure=fig)

axes = []
ax1 = fig.add_subplot(gs[0, 0], projection='3d')
add_axis("compressionResults/compressedTensor_e-1.mat", ax1, "thr = 1e-1")
axes.append(ax1)
ax2 = fig.add_subplot(gs[0, 1], projection='3d')
add_axis("compressionResults/compressedTensor_e-2.mat", ax2, "thr = 1e-2")
axes.append(ax2)
ax3 = fig.add_subplot(gs[0, 2], projection='3d')
add_axis("compressionResults/compressedTensor_e-3.mat", ax3, "thr = 1e-3")
axes.append(ax3)
ax4 = fig.add_subplot(gs[1, 0], projection='3d')
add_axis("compressionResults/compressedTensor_e-4.mat", ax4, "thr = 1e-4")
axes.append(ax4)
ax5 = fig.add_subplot(gs[1, 1], projection='3d')
add_axis("compressionResults/compressedTensor_e-5.mat", ax5, "thr = 1e-5")
axes.append(ax5)
ax6 = fig.add_subplot(gs[1, 2], projection='3d')
C = add_axis("../density.mat", ax6, "original")
axes.append(ax6)

cbar_ax = fig.add_axes([0.91, 0.25, 0.015, 0.5])  # Adjust [left, bottom, width, height] for position
cbar = fig.colorbar(C, cax=cbar_ax, label='Density')  # Use collections[0] for the first contourf plot

plt.savefig('visualize_compression_3d.png', format='png')
