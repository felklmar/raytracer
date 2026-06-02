# 3D Triangle Mesh Ray Tracer

![Ray Tracer Example](example.jpeg)

A lightweight physically based rendering (PBR) engine built from scratch in C++17. This project utilizes standard ray tracing models alongside advanced spatial acceleration to read, compute, and render photorealistic 3D imagery directly from Wavefront `.obj` files.

---

## Key Features

* **Wavefront OBJ Mesh Support**: Integrates `tiny_obj_loader.h` to parse and render arbitrary 3D triangular geometry, including individual vertex normals and associated material properties (`.mtl`).
* **Octree Spatial Acceleration Tree**: Optimizes ray-scene geometric intersection testing by dynamically building an axis-aligned bounding box (AABB) hierarchy. Uses smart traversal order logic to trace complex meshes in seconds instead of hours.
* **Area Light Source Simulation**: Simulates true soft shadows by utilizing multi-sample random sampling across polygonal area lights instead of calculating simple point light sources.
* **Whitted-Style Recursive Path Splitting**:
    * **Phong Shading Architecture**: Computes diffuse, specular highlights, and emissive properties.
    * **True Specular Reflections**: Traces recursive mirror bounces.
    * **Refraction & Transparency**: Implements Snell's Law and Fresnel equations with customizable Index of Refraction (IOR) thresholds for glass and water rendering.
* **Direct PPM Format Output**: Exports standard high-fidelity `P3` Netpbm/PPM image files directly upon task execution.

---

## Project Architecture

```text
├── scenes/                 # Wavefront .obj and .mtl scene data assets
└── src/                    # Primary implementation directory
    ├── BoundingBox.[cpp/h] # AABB implementations & Separating Axis Theorem (SAT) tests
    ├── Camera.[cpp/h]      # Field of view (FOV), target vectors, and frame math
    ├── IntersectionData.[cpp/h]# Tracks ray collision results (Hit status, T-value, normal points)
    ├── main.cpp            # Entry point, configuration handling, and frame processing
    ├── Makefile            # Automated multi-target GCC build layout
    ├── Material.[cpp/h]    # Stores Diffuse, Specular, Emissive, Transparency, and IOR metrics
    ├── Octree.[cpp/h]      # Hierarchical division nodes for accelerated tracing
    ├── Options.[cpp/h]     # Runtime rendering arguments and depth values
    ├── Ray.[cpp/h]         # Ray generation and Möller–Trumbore intersection math
    ├── Raytracer.[cpp/h]   # Primary Phong shading calculation loop and recursive tracing
    ├── Scene.[cpp/h]       # Handles object file loading and image color grids
    ├── tiny_obj_loader.h   # Header library dependency for .obj parsing
    ├── Triangle.[cpp/h]    # Handles basic primitive properties, surface normals, and areas
    └── vec3.[cpp/h]        # Performance-optimized 3D Vector mathematical toolkit
```

---

## Getting Started

### Prerequisites

You need a terminal environment with a standard C++ compiler supporting **C++17** or higher.
* `g++` / GCC compiler
* `make` build utility

### Compilation

Navigate inside the `src/` directory and execute the local `Makefile` rules to build the release configuration:

```bash
cd src
make
```

This generates a compiled binary executable file named `raytracer` one directory level higher (`../raytracer`).

To wipe object files and binary compilation assets clean, use:

```bash
make clean
```

---

## Usage Guide

The compiled engine relies on specific command-line arguments to load scenes and adjust precision parameters:

```bash
./raytracer <scene-config-file> <output-name.ppm> <width> <height> <recursion-depth> <light-samples> <octree-depth> <min-aabb-triangles>
```

### Argument Parameters breakdown:

| Parameter | Type | Purpose / Description | Example Value |
| :--- | :--- | :--- | :--- |
| `scene-config-file` | `string` | Relative address path pointing to your scene loader specification | `scenes/cornell.txt` |
| `output-name.ppm` | `string` | Target name for the generated Netpbm image output mapping | `output.ppm` |
| `width` | `integer` | Pixel resolution width size for the final rendered view frame | `1280` |
| `height` | `integer` | Pixel resolution height size for the final rendered view frame | `720` |
| `recursion-depth` | `integer` | Maximum threshold constraint allowed for reflective/refractive ray bounces | `5` |
| `light-samples` | `integer` | Number of rays cast per area light to sample for soft shadow generation | `32` |
| `octree-depth` | `integer` | Max allowed structural subdivide depth limits for node grouping partitions | `6` |
| `min-aabb-triangles`| `integer` | Minimum collection size of polygon data allowed before making a new leaf cell | `10` |

### Execution Example:

```bash
./raytracer scenes/cornell.txt output.ppm 1920 1080 5 64 6 10
```

### Scene Configuration File Format

**Note on Scene Configuration**: The `<scene-config-file>` (e.g., `scenes/CornellBox-Sphere.txt`) should be a simple plain-text file where the **first line** dictates the relative filename of the target `.obj` mesh situated inside the `scenes/` directory. The subsequent lines must specify the camera position, up vector, view direction, and FOV parameters respectively.

The scene configuration parameter targets a flat text file that coordinates the underlying 3D model asset with its structural camera placement variables. 

Below is an example of how your scene file (e.g., `scenes/CornellBox-Sphere.txt`) should be formatted:

```text
CornellBox-Sphere.obj
pos 0 1 4.42 
up 0 1 0 
dir 0 0 -1
fov 0.6
```

#### Line Breakdown:
1. **Line 1**: The filename of the actual Wavefront 3D mesh model (automatically loaded relative to the `scenes/` folder directory).
2. **Line 2 (`pos`)**: The 3D global coordinate space position vector $\mathbf{O} = (x, y, z)$ of the camera.
3. **Line 3 (`up`)**: The directional alignment vector identifying which orientation axis points upward in world coordinates.
4. **Line 4 (`dir`)**: The vector defining the exact target line-of-sight tracking path forward from the lens origin.
5. **Line 5 (`fov`)**: The horizontal/vertical angle threshold limit constraint (expressed cleanly in radians) to define frame zoom projection behavior.

---

## 🛠️ Technical Details & Math Underlying

The engine relies on classic ray tracing physics coupled with strict spatial partition trees to calculate lighting values efficiently across complex geometric inputs:

### 1. Triangle Intersections (Möller–Trumbore Algorithm)

Instead of calculating explicit plane equations for every triangle in a mesh, `Ray::triangleIntersection` implements the **Möller–Trumbore intersection algorithm**. This projects the ray origin into barycentric coordinates $(u, v)$ relative to the triangle vertices $A$, $B$, and $C$. 

Given a ray $R(t) = \mathbf{O} + t\mathbf{D}$ and edge vectors $\mathbf{E_1} = B - A$ and $\mathbf{E_2} = C - A$, the intersection point is solved via Cramer's Rule:

$$\begin{bmatrix} t \\ u \\ v \end{bmatrix} = \frac{1}{\mathbf{P} \cdot \mathbf{E_1}} \begin{bmatrix} \mathbf{Q} \cdot \mathbf{E_2} \\ \mathbf{P} \cdot \mathbf{T} \\ \mathbf{Q} \cdot \mathbf{D} \end{bmatrix}$$

Where:
* $\mathbf{P} = \mathbf{D} \times \mathbf{E_2}$
* $\mathbf{T} = \mathbf{O} - A$
* $\mathbf{Q} = \mathbf{T} \times \mathbf{E_1}$

A valid intersection occurs only if $u \ge 0$, $v \ge 0$, and $u + v \le 1$.

### 2. Bounding Box Intersections (Separating Axis Theorem)

To safely register triangle meshes inside spatial boundaries, `BoundingBox::intersects` checks overlap using the **Separating Axis Theorem (SAT)**. It tests a total of **13 separating axes** to confirm whether an intersection exists:
* **3 Principal Axes**: Box face normals (X, Y, and Z unit vectors).
* **1 Triangle Normal**: Face normal vector of the target triangle.
* **9 Edge Cross Products**: Cross products combining the 3 box edges with the 3 triangle edges.

If the projections of the box and triangle onto *any* of these 13 axes fail to overlap, the theorem guarantees no geometric intersection exists, allowing the engine to safely skip testing that leaf node.

### 3. Accelerated Octree Navigation

Rather than stepping through child boxes at random, `Octree::getTraversalOrder` analyzes the 3D directional signs (positive/negative) of the incoming ray vector $\mathbf{D}$. 

It calculates the intersection entry time ($t_{near}$) for all 8 subdivided child nodes simultaneously and sorts them using `std::sort`. The ray then steps through child voxels along its immediate line of sight:

```text
Ray Direction (X+, Y+, Z+) ---> Traces Octant [0, 1, 2, 3] before checking [4, 5, 6,7]
```