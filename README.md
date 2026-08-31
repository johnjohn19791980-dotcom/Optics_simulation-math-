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
# Optics_math

A mathematical foundation for optical ray-tracing and simulation, developed as a fork of the original `svgOptics_simulation` project.

## Overview

This repository focuses on the **mathematical backbone of the optical simulation**.

My contribution is primarily contained in `math.h`, where I implemented the core mathematical and geometric structures required for the simulation to represent rays, perform vector calculations, model optical interactions, and handle materials.

The goal was to provide a reusable mathematical layer that allows the higher-level simulation components to operate on a consistent geometric and optical foundation.

---

## My Contribution

I focused on building the mathematical infrastructure underlying the simulation rather than the graphical representation.

The main components implemented in `math.h` are:

### 1. `Vector3D`

A three-dimensional vector class providing the fundamental operations required for geometric and optical calculations.

Implemented operations include:

* Vector addition and subtraction
* Scalar multiplication and division
* Negation
* Dot product
* Cross product
* Vector magnitude
* Vector normalization
* Stream output for debugging and visualization

These operations form the basis for representing positions, directions, surface normals, and other geometric quantities.

---

### 2. `Ray`

A ray representation based on an origin and normalized direction.

The class provides:

* Ray origin
* Ray direction
* Automatic direction normalization
* Position evaluation along the ray

A point along a ray is represented mathematically as:

$$
\mathbf{P}(t)=\mathbf{O}+t\mathbf{D}
$$

where:

* \(\mathbf{O}\) is the ray origin
* \(\mathbf{D}\) is the normalized direction
* \(t\) determines the position along the ray

This provides the fundamental geometric representation used by the optical simulation.

---

### 3. `OpticsMath`

A collection of mathematical operations specifically required for optical interactions.

#### Reflection

The reflection direction is calculated using:

$$
\mathbf{R}
=
\mathbf{I}
-
2(\mathbf{I}\cdot\mathbf{N})\mathbf{N}
$$

where \(\mathbf{I}\) is the incident direction and \(\mathbf{N}\) is the surface normal.

#### Refraction

The refraction calculation uses the refractive-index ratio between two media and computes the transmitted direction using the vector form of Snell's law.

The implementation also checks the refraction discriminant and handles the case where no transmitted ray exists.

#### Angle Calculations

The mathematical angle between two vectors is calculated from their normalized dot product:

$$
\theta =
\cos^{-1}
\left(
\frac{\mathbf{a}\cdot\mathbf{b}}
{|\mathbf{a}||\mathbf{b}|}
\right)
$$

The module also provides degree/radian conversion utilities.

---

### 4. `Material`

A material representation containing:

* Material name
* Refractive index
* Optical color

I also implemented a built-in refractive-index table containing several optical and biological materials, including:

* Vacuum
* Air
* Ice
* Water
* Ethyl Alcohol
* Fused Quartz
* Acrylic
* Crown Glass
* Flint Glass
* Diamond
* Silicon
* Germanium
* Cornea
* Blood
* Vitreous Humour

The refractive index is used by the optical calculations to determine how a ray behaves when transitioning between different media.

---

### 5. `HitRecord`

`HitRecord` stores the mathematical information associated with a ray-surface interaction.

It contains:

* Intersection parameter \(t\)
* Intersection point
* Surface normal
* Front/back-face information
* Material information on the two sides of the interface

The `setFaceNormal()` function determines whether the ray is hitting the front or back side of a surface and ensures that the working normal is oriented consistently relative to the incoming ray.

---

## Mathematical Architecture

The mathematical layer can be viewed as a pipeline:

```text
Vector3D
   │
   ├── Geometry
   │
   └── Directions / Normals
           │
           ▼
         Ray
           │
           ▼
   Surface Intersection
           │
           ▼
      HitRecord
           │
           ├── Surface Normal
           └── Materials
                    │
                    ▼
             Optical Interaction
              ┌─────┴─────┐
              ▼           ▼
          Reflection   Refraction
```

This structure separates the mathematical representation of the optical system from the higher-level simulation and visualization logic.

---

## Design Focus

The main focus of this contribution was to translate the mathematical requirements of ray-based optical simulation into computational structures that can be directly used by the rest of the project.

The implementation therefore combines:

* 3D vector algebra
* Euclidean geometry
* Ray parametrization
* Surface-normal calculations
* Reflection geometry
* Vector-form Snell's law
* Refractive-index modeling
* Coordinate and angle calculations
* Ray-surface interaction data structures

---

## File

The primary implementation is contained in:

```text
math.h
```

This header provides the mathematical and optical primitives required by the simulation.

---

## Relationship to the Original Project

This repository is a fork of the original `svgOptics_simulation` project.

The upstream project provides the broader optical simulation framework, while this fork documents and develops my contribution to its **mathematical foundation**.

The original project should be credited for the overall framework and project concept.

---

## Future Development

Potential extensions to the mathematical layer include:

* More comprehensive material databases
* Wavelength-dependent refractive indices
* Fresnel reflection/transmission calculations
* Total internal reflection handling with explicit optical-state representation
* More advanced geometric primitives
* Matrix and transformation utilities
* Additional numerical and computational geometry tools

---

## Author

**Mina John**

Undergraduate Nano Engineering student interested in computational physics, photonics, and quantum engineering.

