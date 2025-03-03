
# 🚀 3D Spaceship Simulation

This project is a **3D graphics rendering engine** implementing the **projective graphics pipeline** using C++ and OpenGL, featuring:  
- **Real-time terrain rendering**  
- **3D spaceship and launchpad models**  
- **Animation, camera tracking, and lighting effects**  

It utilizes computer graphics algorithms for rendering, including **Bresenham’s line drawing, triangle rasterization, and instancing**.  

## 🎮 Features  
✅ **3D Graphics Rendering** – Implements the **projective graphics pipeline** for realistic rendering.  
✅ **Custom Terrain Generation** – Procedural terrain rendering with **textures and shading**.  
✅ **Spaceship & Launchpad Models** – Designed with **animation and dynamic camera tracking**.  
✅ **Optimized Rendering** – Uses **Bresenham’s line algorithm, triangle rasterization**, and instancing for performance.  
✅ **Interactive Camera** – Allows **movement, zooming, and rotation** for enhanced user experience.    

## 🚀 How It Works  
The **spaceship simulation** follows a structured graphics pipeline:  
1. **Object Modeling** – Spaceship and terrain are modeled using **vertex and fragment shaders**.  
2. **Rasterization** – Uses **Bresenham’s algorithm and triangle rasterization** for rendering.  
3. **Lighting & Texturing** – Implements **Phong lighting** and texture mapping for realism.  
4. **Camera & Animation** – Features **dynamic camera tracking** and **object instancing** for performance.  

## 📊 Performance Optimizations  
- **Triangle Rasterization** – Efficiently handles large 3D models with minimal processing overhead.  
- **Instancing** – Reduces the number of draw calls for complex objects.  
- **Texture Compression** – Optimized texture loading for faster rendering.  
