## Requirements

- A C++ compiler (g++ or MSVC)
- **SFML 2.6.1** — [Download here](https://www.sfml-dev.org/download.php)

**Note:** To change the optical setup (mirrors, lenses, positions, radius),
edit `main.cpp` — that is where the scene is configured.
`surface.h`, `math.h`, and `visualization.h` do not need to be modified.

## Controls

| Key | Action |
| `Z` | Zoom in |
| `X` | Zoom out |
| `↑` `↓` `←` `→` | Move camera |
## Authors

-Mariam Mohsen
-Mina John
-Farah Ehab
# C++ 3D Vector & Ray Optics Engine

A header-only C++ foundation library for 3D vector algebra, optical surface interactions, material properties, and ray tracing primitives. 

The library provides coordinate-free 3D vector operations, exact boundary hit tracking, and physical surface reflections/refractions driven by geometric optics.

---

## Technical Features

* **3D Vector Mathematics (`Vector3D`)**: Vector space operations, Euclidean norm calculations, normalized unit vectors, dot products ($\mathbf{u} \cdot \mathbf{v}$), and cross products ($\mathbf{u} \times \mathbf{v}$).
* **Parametric Ray Model (`Ray`)**: Parameterized directed ray formulation $\mathbf{r}(t) = \mathbf{o} + t\,\hat{\mathbf{d}}$ with unit direction enforcement.
* **Vectorial Optical Physics (`OpticsMath`)**:
  * Exact specular reflection vector derivation.
  * Vectorial Snell's law refraction with total internal reflection (TIR) detection.
  * Angular measurements and degree-radian conversions.
* **Material Property Database (`Material`)**: Physical refractive index ($n$) lookup across dielectric, biological, and semiconductor media (e.g., Silicon $n = 3.48$, Germanium $n = 4.05$, Crown Glass $n = 1.52$).
* **Boundary Tracking (`HitRecord`)**: Surface intersection record storing parametric distance $t$, interface normals, front-face orientation, and adjacent media properties ($n_1 \to n_2$).

---

## Mathematical Formulation

### 1. Vector Specular Reflection
Given an incident unit direction vector $\hat{\mathbf{d}}$ and a surface unit normal $\hat{\mathbf{n}}$ pointing outward:

$$\mathbf{r}_{\text{refl}} = \hat{\mathbf{d}} - 2(\hat{\mathbf{d}} \cdot \hat{\mathbf{n}})\hat{\mathbf{n}}$$

### 2. Vectorial Snell's Law & Refraction
Let $\eta = \frac{n_1}{n_2}$ be the ratio of refractive indices, and $\cos\theta_1 = -\hat{\mathbf{d}} \cdot \hat{\mathbf{n}}$.

The refracted vector is split into perpendicular ($\mathbf{r}_{\perp}$) and parallel ($\mathbf{r}_{\parallel}$) components relative to $\hat{\mathbf{n}}$:

$$\mathbf{r}_{\perp} = \eta \left( \hat{\mathbf{d}} + \cos\theta_1 \hat{\mathbf{n}} \right)$$

$$\mathbf{r}_{\parallel} = -\sqrt{1 - |\mathbf{r}_{\perp}|^2} \; \hat{\mathbf{n}}$$

$$\mathbf{r}_{\text{refr}} = \mathbf{r}_{\perp} + \mathbf{r}_{\parallel}$$

* **Total Internal Reflection (TIR):** Occurs when the discriminant $1 - |\mathbf{r}_{\perp}|^2 < 0$. In this case, `OpticsMath::refract` returns the zero vector $\mathbf{0}$.

---

## Core API Reference

| Component | Class / Namespace | Description |
| :--- | :--- | :--- |
| **Vector3D** | `class Vector3D` | 3D vector class supporting $+$, $-$, $*$, $/$, `dot()`, `cross()`, `length()`, and `normalized()`. |
| **Ray** | `class Ray` | Ray class defined by origin $\mathbf{o}$ and normalized direction $\hat{\mathbf{d}}$. Evaluates $\mathbf{r}(t)$ via `at(t)`. |
| **OpticsMath** | `namespace OpticsMath` | Mathematical functions: `reflect()`, `refract()`, `angleBetween()`, `degToRad()`, `radToDeg()`. |
| **Material** | `class Material` | Refractive index table lookup ($n$) and material property management. |
| **HitRecord** | `struct HitRecord` | Surface hit point, parametric distance $t$, oriented normal vector, and interface media ($n_1$, $n_2$). |

---

## Usage Example

```cpp
#include <iostream>
#include "OpticsEngine.hpp"

int main() {
    // Media definition
    Material air("Air");
    Material silicon("Silicon"); // n = 3.480

    // Incident ray setup
    Vector3D rayOrigin(0.0, 1.0, -2.0);
    Vector3D rayDir(0.0, -1.0, 1.0);
    Ray incidentRay(rayOrigin, rayDir);

    // Surface interface normal (pointing out of the silicon surface)
    Vector3D normal(0.0, 1.0, 0.0);

    // Compute Refraction (Air -> Silicon interface)
    Vector3D refractedDir = OpticsMath::refract(
        incidentRay.direction(),
        normal,
        air.refractiveindex,
        silicon.refractiveindex
    );

    std::cout << "Incident Direction:  " << incidentRay.direction() << "\n";
    std::cout << "Refracted Direction: " << refractedDir << "\n";

    return 0;
}

Future Roadmap & Physics Extensions1. Inhomogeneous Media & Variable Refractive Index $n(\mathbf{r})$GRIN Optics Simulation: Extend the ray tracer to inhomogeneous media where $n = n(x, y, z)$.Eikonal Equation Solver: Numerically integrate ray trajectories through continuous refractive index gradients using the differential ray equation:$$\frac{d}{ds} \left( n(\mathbf{r}) \frac{d\mathbf{r}}{ds} \right) = \nabla n(\mathbf{r})$$2. Full Wave Optics & ElectromagneticsFresnel Equations & Polarization: Compute power reflection ($R$) and transmission ($T$) coefficients for $s$- and $p$-polarized light fields.Phase Tracking: Accumulate optical path length (OPL) $\int n(\mathbf{r}) \, ds$ along rays to model interference and phase retardation.3. Integrated Photonics & Neuromorphic Optical ComputingWaveguide Optics & Mode Propagation: Extend geometric ray modeling toward waveguided structures (e.g., Silicon-on-Insulator ($\text{SOI}$) strip waveguides).Photonic Brain-on-a-Chip Architecture: Model integrated photonic mesh networks (such as arrays of Mach-Zehnder Interferometers or micro-ring resonators) to simulate optical neural networks ($\text{ONNs}$) and neuromorphic hardware implementations.

